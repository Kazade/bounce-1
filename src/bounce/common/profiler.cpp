/*
* Copyright (c) 2016-2019 Irlan Robson https://irlanrobson.github.io
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

#include <bounce/common/profiler.h>
#include <bounce/common/math/math.h>

b3Profiler* b3Profiler_profiler = nullptr;

b3ProfilerNode* b3ProfilerNode::FindChildNode(const char* name) 
{
	for (b3ProfilerNode* c = m_childHead; c != nullptr; c = c->m_childNext)
	{
		if (c->m_name == name)
		{
			return c;
		}
	}
	return nullptr;
}

b3Profiler::b3Profiler() : 
	m_nodePool(sizeof(b3ProfilerNode)),
	m_statsPool(sizeof(b3ProfilerNodeStats))
{
	m_root = nullptr;
	m_top = nullptr;
	m_statsHead = nullptr;
}

void b3Profiler::Begin()
{
	B3_ASSERT(m_top == nullptr);
}

b3ProfilerNodeStats* b3Profiler::FindStats(const char* name)
{
	for (b3ProfilerNodeStats* s = m_statsHead; s != nullptr; s = s->next)
	{
		if (s->name == name)
		{
			return s;
		}
	}
	return nullptr;
}

void b3Profiler::OpenScope(const char* name)
{
	if (m_top)
	{
		b3ProfilerNode* topChild = m_top->FindChildNode(name);
		if (topChild)
		{
			// Top child node becomes top node
			m_top = topChild;

			++topChild->m_callCount;

			if (topChild->m_recursionCallCount == 0)
			{
				m_time.Update();
				topChild->m_t0 = m_time.GetCurrentMilis();
			}

			++topChild->m_recursionCallCount;

			return;
		}
	}

	m_time.Update();
	
	// Create a new node
	b3ProfilerNode* newNode = (b3ProfilerNode*)m_nodePool.Allocate();
	newNode->m_name = name;
	newNode->m_t0 = m_time.GetCurrentMilis();
	newNode->m_elapsed = 0.0f;
	newNode->m_callCount = 1;
	newNode->m_recursionCallCount = 1;
	newNode->m_stats = nullptr;
	newNode->m_parent = m_top;
	newNode->m_childHead = nullptr;
	newNode->m_childNext = nullptr;

	if (m_root == nullptr)
	{
		// Insert into tree
		B3_ASSERT(m_top == nullptr);
		m_root = newNode;
		m_top = newNode;
		return;
	}

	if (m_top)
	{
		// Top node gets a new kid
		newNode->m_childNext = m_top->m_childHead;
		m_top->m_childHead = newNode;
	}

	// New node becomes top node
	m_top = newNode;
}

void b3Profiler::CloseScope()
{
	B3_ASSERT(m_top != nullptr);

	--m_top->m_recursionCallCount;
	if (m_top->m_recursionCallCount > 0)
	{
		return;
	}

	m_time.Update();
	m_top->m_t1 = m_time.GetCurrentMilis();

	double elapsed = m_top->m_t1 - m_top->m_t0;

	m_top->m_elapsed += elapsed;

	// Update top stats
	b3ProfilerNodeStats* topStats = FindStats(m_top->m_name);
	if (topStats == nullptr)
	{
		topStats = (b3ProfilerNodeStats*)m_statsPool.Allocate();
		topStats->name = m_top->m_name;
		topStats->minElapsed = elapsed;
		topStats->maxElapsed = elapsed;

		// Push stat to profiler list of stats
		topStats->next = m_statsHead;
		m_statsHead = topStats;
	}
	else
	{
		topStats->minElapsed = b3Min(topStats->minElapsed, elapsed);
		topStats->maxElapsed = b3Max(topStats->maxElapsed, elapsed);
	}

	if (m_top->m_stats == nullptr)
	{
		m_top->m_stats = topStats;
	}

	B3_ASSERT(m_top->m_stats == topStats);

	m_top = m_top->m_parent;
}

void b3Profiler::DestroyNodeRecursively(b3ProfilerNode* node)
{
	b3ProfilerNode* c = node->m_childHead;
	while (c)
	{
		b3ProfilerNode* boom = c;
		c = c->m_childNext;
		DestroyNodeRecursively(boom);
	}
	m_nodePool.Free(node);
}

void b3Profiler::End()
{
	B3_ASSERT(m_top == nullptr);
	if (m_root)
	{
		DestroyNodeRecursively(m_root);
		m_root = nullptr;
	}
}
