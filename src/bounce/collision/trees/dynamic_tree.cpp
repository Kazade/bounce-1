/*
* Copyright (c) 2016-2019 Irlan Robson
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include <bounce/collision/trees/dynamic_tree.h>
#include <bounce/common/draw.h>
#include <string.h>

b3DynamicTree::b3DynamicTree()
{
	m_root = B3_NULL_NODE_D;

	// Preallocate 32 nodes.
	m_nodeCapacity = 32;
	m_nodes = (b3Node*)b3Alloc(m_nodeCapacity * sizeof(b3Node));
	memset(m_nodes, 0, m_nodeCapacity * sizeof(b3Node));
	m_nodeCount = 0;

	// Link the allocated nodes and make the first node 
	// available the the next allocation.
	AddToFreeList(m_nodeCount);
}

b3DynamicTree::~b3DynamicTree()
{
	b3Free(m_nodes);
}

// Return a node from the pool.
u32 b3DynamicTree::AllocateNode()
{
	B3_ASSERT(m_nodeCapacity > 0);

	if (m_freeList == B3_NULL_NODE_D)
	{
		B3_ASSERT(m_nodeCount == m_nodeCapacity);

		// Duplicate capacity.
		m_nodeCapacity *= 2;

		b3Node* oldNodes = m_nodes;
		m_nodes = (b3Node*)b3Alloc(m_nodeCapacity * sizeof(b3Node));;
		memcpy(m_nodes, oldNodes, m_nodeCount * sizeof(b3Node));
		b3Free(oldNodes);

		// Link the (allocated) nodes starting from the new 
		// node and make the new nodes available the the next allocation.
		AddToFreeList(m_nodeCount);
	}

	// Grab the free node.
	u32 node = m_freeList;

	m_freeList = m_nodes[node].next;

	m_nodes[node].parent = B3_NULL_NODE_D;
	m_nodes[node].child1 = B3_NULL_NODE_D;
	m_nodes[node].child2 = B3_NULL_NODE_D;
	m_nodes[node].height = 0;
	m_nodes[node].userData = nullptr;

	++m_nodeCount;

	return node;
}

void b3DynamicTree::FreeNode(u32 node)
{
	B3_ASSERT(node != B3_NULL_NODE_D && node < m_nodeCapacity);
	m_nodes[node].next = m_freeList;
	m_nodes[node].height = -1;
	m_freeList = node;
	--m_nodeCount;
}

void b3DynamicTree::AddToFreeList(u32 node)
{
	B3_ASSERT(m_nodeCapacity > 0);

	// Starting from the given node, relink the linked list of nodes.
	for (u32 i = node; i < m_nodeCapacity - 1; ++i)
	{
		m_nodes[i].next = i + 1;
		m_nodes[i].height = -1;
	}

	m_nodes[m_nodeCapacity - 1].next = B3_NULL_NODE_D;
	m_nodes[m_nodeCapacity - 1].height = -1;

	// Make the node available for the next allocation.
	m_freeList = node;
}

u32 b3DynamicTree::CreateProxy(const b3AABB& aabb, void* userData)
{
	// Insert into the array.
	u32 proxyId = AllocateNode();
	m_nodes[proxyId].aabb = aabb;
	m_nodes[proxyId].userData = userData;
	m_nodes[proxyId].height = 0;

	// Fatten the aabb.
	b3Vec3 r(B3_AABB_EXTENSION, B3_AABB_EXTENSION, B3_AABB_EXTENSION);
	m_nodes[proxyId].aabb.lowerBound = aabb.lowerBound - r;
	m_nodes[proxyId].aabb.upperBound = aabb.upperBound + r;
	m_nodes[proxyId].userData = userData;
	m_nodes[proxyId].height = 0;
	
	// Insert into the tree.
	InsertLeaf(proxyId);

	// Return the proxy ID.
	return proxyId;
}

void b3DynamicTree::DestroyProxy(u32 proxyId)
{
	// Remove from the tree.
	RemoveLeaf(proxyId);

	// Remove from the node array and make it available.
	FreeNode(proxyId);
}

bool b3DynamicTree::MoveProxy(u32 proxyId, const b3AABB& aabb, const b3Vec3& displacement)
{
	B3_ASSERT(0 <= proxyId && proxyId < m_nodeCapacity);
	B3_ASSERT(m_nodes[proxyId].IsLeaf());

	// Extend the AABB.
	b3AABB fatAABB = aabb;
	fatAABB.Extend(B3_AABB_EXTENSION);

	// Predict AABB displacement.
	b3Vec3 d = B3_AABB_MULTIPLIER * displacement;

	if (d.x < scalar(0))
	{
		fatAABB.lowerBound.x += d.x;
	}
	else
	{
		fatAABB.upperBound.x += d.x;
	}

	if (d.y < scalar(0))
	{
		fatAABB.lowerBound.y += d.y;
	}
	else
	{
		fatAABB.upperBound.y += d.y;
	}

	if (d.z < scalar(0))
	{
		fatAABB.lowerBound.z += d.z;
	}
	else
	{
		fatAABB.upperBound.z += d.z;
	}

	const b3AABB& treeAABB = GetAABB(proxyId);
	if (treeAABB.Contains(aabb))
	{
		// The tree AABB still contains the object, but it might be too large.
		// Perhaps the object was moving fast but has since gone to sleep.
		// The huge AABB is larger than the new fat AABB.
		b3AABB hugeAABB = fatAABB;
		hugeAABB.Extend(scalar(4) * B3_AABB_EXTENSION);
		
		if (hugeAABB.Contains(treeAABB))
		{
			// The tree AABB contains the object AABB and the tree AABB is
			// not too large. No tree update needed.
			return false;
		}

		// Otherwise the tree AABB is huge and needs to be shrunk
	}

	// Remove old AABB from the tree.
	RemoveLeaf(proxyId);

	// Insert the new AABB to the tree.
	m_nodes[proxyId].aabb = fatAABB;
	
	InsertLeaf(proxyId);

	// Notify the proxy has moved.
	return true;
}

u32 b3DynamicTree::PickBest(const b3AABB& leafAABB) const
{
	u32 index = m_root;
	while (!m_nodes[index].IsLeaf())
	{
		scalar branchArea = m_nodes[index].aabb.GetSurfaceArea();

		// Minumum cost of pushing the leaf down the tree.
		b3AABB combinedAABB = b3Combine(leafAABB, m_nodes[index].aabb);
		scalar combinedArea = combinedAABB.GetSurfaceArea();

		// Cost for creating a new parent node.
		scalar branchCost = scalar(2) * combinedArea;

		scalar inheritanceCost = scalar(2) * (combinedArea - branchArea);

		// The branch node child nodes cost.
		u32 child1 = m_nodes[index].child1;
		u32 child2 = m_nodes[index].child2;

		// Cost of descending onto child1.
		scalar childCost1 = scalar(0);
		if (m_nodes[child1].IsLeaf())
		{
			b3AABB aabb = b3Combine(leafAABB, m_nodes[child1].aabb);
			childCost1 = aabb.GetSurfaceArea();
		}
		else
		{
			b3AABB aabb = b3Combine(leafAABB, m_nodes[child1].aabb);
			scalar oldArea = m_nodes[child1].aabb.GetSurfaceArea();
			scalar newArea = aabb.GetSurfaceArea();
			childCost1 = (newArea - oldArea) + inheritanceCost;
		}

		// Cost of descending onto child1.
		scalar childCost2 = scalar(0);
		if (m_nodes[child2].IsLeaf())
		{
			b3AABB aabb = b3Combine(leafAABB, m_nodes[child2].aabb);
			childCost2 = aabb.GetSurfaceArea();
		}
		else
		{
			b3AABB aabb = b3Combine(leafAABB, m_nodes[child2].aabb);
			scalar oldArea = m_nodes[child2].aabb.GetSurfaceArea();
			scalar newArea = aabb.GetSurfaceArea();
			childCost2 = (newArea - oldArea) + inheritanceCost;
		}

		// Choose the node that has the minimum cost.
		if (branchCost < childCost1 && branchCost < childCost2)
		{
			// The current branch node is the best node and it will be used.
			break;
		}

		// Visit the node that has the minimum cost.
		index = childCost1 < childCost2 ? child1 : child2;
	}
	return index;
}

void b3DynamicTree::InsertLeaf(u32 leaf)
{
	if (m_root == B3_NULL_NODE_D)
	{
		// If this tree root node is empty then just set the leaf
		// node to it.
		m_root = leaf;
		m_nodes[m_root].parent = B3_NULL_NODE_D;
		return;
	}

	// Get the inserted leaf AABB.
	b3AABB leafAabb = m_nodes[leaf].aabb;

	// Search for the best branch node of this tree starting from the tree root node.
	u32 sibling = PickBest(leafAabb);

	u32 oldParent = m_nodes[sibling].parent;

	// Create and setup new parent. 
	u32 newParent = AllocateNode();
	m_nodes[newParent].parent = oldParent;
	m_nodes[newParent].child1 = sibling;
	m_nodes[sibling].parent = newParent;
	m_nodes[newParent].child2 = leaf;
	m_nodes[leaf].parent = newParent;
	m_nodes[newParent].userData = nullptr;
	m_nodes[newParent].aabb = b3Combine(leafAabb, m_nodes[sibling].aabb);
	m_nodes[newParent].height = m_nodes[sibling].height + 1;

	if (oldParent != B3_NULL_NODE_D)
	{
		// The sibling was not the root.
		// Find which child node of the old parent is the sibling
		// and link the new parent to it.
		if (m_nodes[oldParent].child1 == sibling)
		{
			m_nodes[oldParent].child1 = newParent;
		}
		else
		{
			m_nodes[oldParent].child2 = newParent;
		}
	}
	else
	{
		// If the sibling was the root then the root becomes the created
		// node.
		m_root = newParent;
	}

	// If we have ancestor nodes then adjust its AABBs.
	Refit(m_nodes[leaf].parent);
}

void b3DynamicTree::RemoveLeaf(u32 leaf)
{
	if (leaf == m_root)
	{
		m_root = B3_NULL_NODE_D;
		return;
	}

	u32 parent = m_nodes[leaf].parent;
	u32 grandParent = m_nodes[parent].parent;
	u32 sibling;
	if (m_nodes[parent].child1 == leaf)
	{
		sibling = m_nodes[parent].child2;
	}
	else
	{
		sibling = m_nodes[parent].child1;
	}

	if (grandParent != B3_NULL_NODE_D)
	{
		if (m_nodes[grandParent].child1 == parent)
		{
			m_nodes[grandParent].child1 = sibling;
		}
		else
		{
			m_nodes[grandParent].child2 = sibling;
		}
		m_nodes[sibling].parent = grandParent;

		// Remove parent node.
		FreeNode(parent);

		// If we have ancestor then nodes adjust its AABBs.
		Refit(grandParent);
	}
	else
	{
		m_root = sibling;
		m_nodes[sibling].parent = B3_NULL_NODE_D;
		// Remove parent node.
		FreeNode(parent);
	}
}

void b3DynamicTree::Refit(u32 node)
{
	while (node != B3_NULL_NODE_D)
	{
		node = Balance(node);

		u32 child1 = m_nodes[node].child1;
		u32 child2 = m_nodes[node].child2;

		B3_ASSERT(child1 != B3_NULL_NODE_D);
		B3_ASSERT(child2 != B3_NULL_NODE_D);

		m_nodes[node].height = 1 + b3Max(m_nodes[child1].height, m_nodes[child2].height);
		m_nodes[node].aabb = b3Combine(m_nodes[child1].aabb, m_nodes[child2].aabb);

		node = m_nodes[node].parent;
	}
}

// Perform a left or right rotation if node A is imbalanced.
// Returns the new root index.
u32 b3DynamicTree::Balance(u32 iA)
{
	B3_ASSERT(iA != B3_NULL_NODE_D);

	b3Node* A = m_nodes + iA;
	if (A->IsLeaf() || A->height < 2)
	{
		return iA;
	}

	u32 iB = A->child1;
	u32 iC = A->child2;
	B3_ASSERT(0 <= iB && iB < m_nodeCapacity);
	B3_ASSERT(0 <= iC && iC < m_nodeCapacity);

	b3Node* B = m_nodes + iB;
	b3Node* C = m_nodes + iC;

	i32 balance = i32(C->height) - i32(B->height);

	// Rotate C up
	if (balance > 1)
	{
		u32 iF = C->child1;
		u32 iG = C->child2;
		b3Node* F = m_nodes + iF;
		b3Node* G = m_nodes + iG;
		B3_ASSERT(0 <= iF && iF < m_nodeCapacity);
		B3_ASSERT(0 <= iG && iG < m_nodeCapacity);

		// Swap A and C
		C->child1 = iA;
		C->parent = A->parent;
		A->parent = iC;

		// A's old parent should point to C
		if (C->parent != B3_NULL_NODE_D)
		{
			if (m_nodes[C->parent].child1 == iA)
			{
				m_nodes[C->parent].child1 = iC;
			}
			else
			{
				B3_ASSERT(m_nodes[C->parent].child2 == iA);
				m_nodes[C->parent].child2 = iC;
			}
		}
		else
		{
			m_root = iC;
		}

		// Rotate
		if (F->height > G->height)
		{
			C->child2 = iF;
			A->child2 = iG;
			G->parent = iA;
			A->aabb.Combine(B->aabb, G->aabb);
			C->aabb.Combine(A->aabb, F->aabb);

			A->height = 1 + b3Max(B->height, G->height);
			C->height = 1 + b3Max(A->height, F->height);
		}
		else
		{
			C->child2 = iG;
			A->child2 = iF;
			F->parent = iA;
			A->aabb.Combine(B->aabb, F->aabb);
			C->aabb.Combine(A->aabb, G->aabb);

			A->height = 1 + b3Max(B->height, F->height);
			C->height = 1 + b3Max(A->height, G->height);
		}

		return iC;
	}

	// Rotate B up
	if (balance < -1)
	{
		u32 iD = B->child1;
		u32 iE = B->child2;
		b3Node* D = m_nodes + iD;
		b3Node* E = m_nodes + iE;
		B3_ASSERT(0 <= iD && iD < m_nodeCapacity);
		B3_ASSERT(0 <= iE && iE < m_nodeCapacity);

		// Swap A and B
		B->child1 = iA;
		B->parent = A->parent;
		A->parent = iB;

		// A's old parent should point to B
		if (B->parent != B3_NULL_NODE_D)
		{
			if (m_nodes[B->parent].child1 == iA)
			{
				m_nodes[B->parent].child1 = iB;
			}
			else
			{
				B3_ASSERT(m_nodes[B->parent].child2 == iA);
				m_nodes[B->parent].child2 = iB;
			}
		}
		else
		{
			m_root = iB;
		}

		// Rotate
		if (D->height > E->height)
		{
			B->child2 = iD;
			A->child1 = iE;
			E->parent = iA;
			A->aabb.Combine(C->aabb, E->aabb);
			B->aabb.Combine(A->aabb, D->aabb);

			A->height = 1 + b3Max(C->height, E->height);
			B->height = 1 + b3Max(A->height, D->height);
		}
		else
		{
			B->child2 = iE;
			A->child1 = iD;
			D->parent = iA;
			A->aabb.Combine(C->aabb, D->aabb);
			B->aabb.Combine(A->aabb, E->aabb);

			A->height = 1 + b3Max(C->height, D->height);
			B->height = 1 + b3Max(A->height, E->height);
		}

		return iB;
	}

	return iA;
}
void b3DynamicTree::Validate(u32 nodeID) const
{
	if (nodeID == B3_NULL_NODE_D)
	{
		return;
	}

	// The root node has no parent.
	if (nodeID == m_root)
	{
		B3_ASSERT(m_nodes[nodeID].parent == B3_NULL_NODE_D);
	}

	const b3Node* node = m_nodes + nodeID;

	u32 child1 = node->child1;
	u32 child2 = node->child2;

	if (node->IsLeaf())
	{
		// Leaf nodes has no children and its height is zero.
		B3_ASSERT(child1 == B3_NULL_NODE_D);
		B3_ASSERT(child2 == B3_NULL_NODE_D);
		B3_ASSERT(node->height == 0);
	}
	else
	{
		B3_ASSERT(0 <= child1 && child1 < m_nodeCapacity);
		B3_ASSERT(0 <= child2 && child2 < m_nodeCapacity);

		// The parent of its children is its parent (really?!).

		B3_ASSERT(m_nodes[child1].parent == nodeID);
		B3_ASSERT(m_nodes[child2].parent == nodeID);

		// Walk down the tree.
		Validate(child1);
		Validate(child2);
	}
}

void b3DynamicTree::Draw(b3Draw* draw) const
{
	if (m_nodeCount == 0)
	{
		return;
	}

	b3Stack<u32, 256> stack;
	stack.Push(m_root);

	while (!stack.IsEmpty())
	{
		u32 nodeIndex = stack.Top();
		stack.Pop();

		if (nodeIndex == B3_NULL_NODE_D)
		{
			continue;
		}

		const b3Node* node = m_nodes + nodeIndex;
		if (node->IsLeaf())
		{
			draw->DrawAABB(node->aabb, b3Color_pink);
		}
		else
		{
			draw->DrawAABB(node->aabb, b3Color_red);

			stack.Push(node->child1);
			stack.Push(node->child2);
		}
	}
}
