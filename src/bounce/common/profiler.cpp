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

b3Profiler::b3Profiler() : m_nodePool(sizeof(b3ProfilerNode))
{
	m_root = nullptr;
	m_top = nullptr;
	m_statsHead = nullptr;
}

b3Profiler::~b3Profiler()
{
	b3ProfilerNodeStats* s = m_statsHead;
	while (s)
	{
		b3ProfilerNodeStats* boom = s;
		s = s->next;
		b3Free(boom);
	}
}

void b3Profiler::Begin()
{
	B3_ASSERT(m_top == nullptr);
}

b3ProfilerNode* b3Profiler::FindTopChildNode(const char* name)
{
	if (m_top)
	{
		for (b3ProfilerNode* c = m_top->childHead; c != nullptr; c = c->childNext)
		{
			if (c->name == name)
			{
				return c;
			}
		}
	}
	return nullptr;
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

void b3Profiler::BeginScope(const char* name)
{
	b3ProfilerNode* foundTopChildNode = FindTopChildNode(name);
	if (foundTopChildNode)
	{
		// Top child node becomes m_top node
		m_top = foundTopChildNode;

		++foundTopChildNode->callCount;

		if (foundTopChildNode->recursionCallCount == 0)
		{
			m_currentTime.Update();
			foundTopChildNode->t0 = m_currentTime.GetCurrentMilis();
		}

		++foundTopChildNode->recursionCallCount;
		
		return;
	}

	m_currentTime.Update();

	// Create a new node
	b3ProfilerNode* newNode = (b3ProfilerNode*)m_nodePool.Allocate();
	newNode->name = name;
	newNode->t0 = m_currentTime.GetCurrentMilis();
	newNode->elapsed = 0.0f;
	newNode->callCount = 1;
	newNode->recursionCallCount = 1;
	newNode->stats = nullptr;
	newNode->parent = m_top;
	newNode->childHead = nullptr;
	newNode->childNext = nullptr;

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
		newNode->childNext = m_top->childHead;
		m_top->childHead = newNode;
	}

	// New node becomes m_top node
	m_top = newNode;
}

void b3Profiler::EndScope()
{
	B3_ASSERT(m_top != nullptr);

	--m_top->recursionCallCount;
	if (m_top->recursionCallCount > 0)
	{
		return;
	}

	m_currentTime.Update();
	m_top->t1 = m_currentTime.GetCurrentMilis();

	scalar elapsedTime = (scalar)(m_top->t1 - m_top->t0);

	m_top->elapsed += elapsedTime;

	// Permanent statistics
	b3ProfilerNodeStats* topStats = FindStats(m_top->name);
	if (topStats == NULL)
	{
		topStats = (b3ProfilerNodeStats*)b3Alloc(sizeof(b3ProfilerNodeStats));
		topStats->name = m_top->name;
		topStats->minElapsed = elapsedTime;
		topStats->maxElapsed = elapsedTime;

		// Push stat to tree list of stats
		topStats->next = m_statsHead;
		m_statsHead = topStats;
	}
	else
	{
		topStats->minElapsed = b3Min(topStats->minElapsed, elapsedTime);
		topStats->maxElapsed = b3Max(topStats->maxElapsed, elapsedTime);
	}

	if (m_top->stats == nullptr)
	{
		m_top->stats = topStats;
	}

	B3_ASSERT(m_top->stats == topStats);

	m_top = m_top->parent;
}

static void b3DestroyNodeRecursively(b3ProfilerNode* node, b3BlockPool* allocator)
{
	b3ProfilerNode* c = node->childHead;
	while (c)
	{
		b3ProfilerNode* boom = c;
		c = c->childNext;
		b3DestroyNodeRecursively(boom, allocator);
	}
	allocator->Free(node);
}

void b3Profiler::End()
{
	B3_ASSERT(m_top == nullptr);
	if (m_root)
	{
		b3DestroyNodeRecursively(m_root, &m_nodePool);
		m_root = nullptr;
	}
}
