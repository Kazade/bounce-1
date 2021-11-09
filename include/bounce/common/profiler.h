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

#ifndef B3_PROFILER_H
#define B3_PROFILER_H

#include <bounce/common/memory/block_pool.h>
#include <bounce/common/time.h>

// Persistent statistics. 
struct b3ProfilerStats
{
	const char* name;
	double minElapsed;
	double maxElapsed;
	b3ProfilerStats* next;
};

// A profiler node.
class b3ProfilerNode
{
public:
	// Get the unique name of this node, used as the node identifier.
	const char* GetName() const { return m_name; }

	// Get the elapsed time of this node. Units are miliseconds.
	double GetElapsedTime() const { return m_elapsed; }

	// Get the total number of calls in this node.
	u32 GetCallCount() const { return m_callCount; }

	// Get the parent node of this node.
	const b3ProfilerNode* GetParent() const { return m_parent; }

	// Get the list of child nodes in this node.
	const b3ProfilerNode* GetChildList() const { return m_childHead; }

	// Get the next child node in this node's list of child nodes.
	const b3ProfilerNode* GetNextChild() const { return m_childNext; }

	// Get the statistics for this node.
	const b3ProfilerStats* GetStats() const { return m_stats; }
private:
	friend class b3Profiler;

	b3ProfilerNode* FindChildNode(const char* name);

	const char* m_name;
	double m_elapsed;
	u32 m_callCount;
	u32 m_recursionCount;
	double m_t1;
	double m_t2;
	b3ProfilerNode* m_parent;
	b3ProfilerNode* m_childHead;
	b3ProfilerNode* m_childNext;
	b3ProfilerStats* m_stats;
};

// Immediate mode hierarchical profiler. 
class b3Profiler
{
public:
	// Default ctor.
	b3Profiler();

	// Clear the profiler tree node buffer. 
	// This must be called before profiling usually at the 
	// beginning of each frame.
	void Clear();

	// Open a scope block.
	void OpenScope(const char* name);

	// Close the last scope block.
	void CloseScope();

	// Return the root node.
	const b3ProfilerNode* GetRoot() const { return m_root; };
private:
	b3ProfilerStats* FindStats(const char* name);
	void DestroyNodeRecursively(b3ProfilerNode* node);

	b3Time m_time; 
	b3BlockPool m_nodePool;
	b3BlockPool m_statsPool;
	b3ProfilerNode* m_root; 
	b3ProfilerNode* m_top; 
	b3ProfilerStats* m_statsHead; 
};

// A profiler scope. 
struct b3ProfileScope
{
	// Open a new scope.
	b3ProfileScope(b3Profiler* _profiler, const char* name)
	{
		profiler = _profiler;
		if (profiler)
		{
			profiler->OpenScope(name);
		}
	}

	// Close this scope.
	~b3ProfileScope()
	{
		if (profiler)
		{
			profiler->CloseScope();
		}
	}
	
	// This scope profiler.
	b3Profiler* profiler;
};

// Use this macro to start a block of scope.
#define B3_PROFILE(profiler, name) b3ProfileScope scope(profiler, name)

#endif