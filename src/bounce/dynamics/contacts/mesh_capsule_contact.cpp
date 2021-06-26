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

#include <bounce/dynamics/contacts/mesh_capsule_contact.h>
#include <bounce/dynamics/shapes/triangle_shape.h>
#include <bounce/dynamics/shapes/mesh_shape.h>
#include <bounce/dynamics/shapes/capsule_shape.h>
#include <bounce/common/memory/block_allocator.h>

b3Contact* b3MeshAndCapsuleContact::Create(b3Shape* shapeA, b3Shape* shapeB, b3BlockAllocator* allocator)
{
	void* mem = allocator->Allocate(sizeof(b3MeshAndCapsuleContact));
	return new (mem) b3MeshAndCapsuleContact(shapeA, shapeB);
}

void b3MeshAndCapsuleContact::Destroy(b3Contact* contact, b3BlockAllocator* allocator)
{
	((b3MeshAndCapsuleContact*)contact)->~b3MeshAndCapsuleContact();
	allocator->Free(contact, sizeof(b3MeshAndCapsuleContact));
}

b3MeshAndCapsuleContact::b3MeshAndCapsuleContact(b3Shape* shapeA, b3Shape* shapeB) : b3MeshContact(shapeA, shapeB)
{
	B3_ASSERT(shapeA->GetType() == e_meshShape);
	B3_ASSERT(shapeB->GetType() == e_capsuleShape);
}

void b3MeshAndCapsuleContact::Evaluate(b3Manifold& manifold, const b3Transform& xfA, const b3Transform& xfB, u32 cacheIndex)
{
	B3_ASSERT(cacheIndex < m_triangleCount);
	
	b3MeshShape* mesh = (b3MeshShape*)GetShapeA();
	b3TriangleShape triangle;
	mesh->GetChildTriangle(&triangle, m_triangles[cacheIndex].index);
	b3CollideTriangleAndCapsule(manifold, xfA, &triangle, xfB, (b3CapsuleShape*)GetShapeB());
} 