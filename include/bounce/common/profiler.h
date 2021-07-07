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
#include <bounce/common/settings.h>

struct b3ProfilerNodeStats;

// Profiler tree node
struct b3ProfilerNode
{
	const char* name; // unique name. used as identifier
	
	scalar elapsed; // total elapsed time
	u32 callCount; // number of calls inside the parent node
	u32 recursionCallCount; // recursion helper counter

	double t0; // internal
	double t1; // internal 

	b3ProfilerNode* parent; // parent node
	b3ProfilerNode* childHead; // list of children
	b3ProfilerNode* childNext; // link to the next node in the parent node list of children

	b3ProfilerNodeStats* stats; // global node statistics
};

// Profiler tree node permanent statistics 
struct b3ProfilerNodeStats
{
	const char* name; // name
	scalar minElapsed; // min elapsed time
	scalar maxElapsed; // max elapsed time
	b3ProfilerNodeStats* next; // list into profiler
};

// Immediate mode hierarchical profiler 
class b3Profiler
{
public:
	b3Profiler();
	~b3Profiler();

	// Return the root node.
	const b3ProfilerNode* GetRoot() const { return m_root; };
	
	// Begin profiling.
	void Begin();

	// Open a scope block.
	void BeginScope(const char* name);

	// Close the last scope block.
	void EndScope();

	// End profiling.
	void End();
private:
	b3ProfilerNode* FindTopChildNode(const char* name);
	b3ProfilerNodeStats* FindStats(const char* name);
	
	b3Time m_currentTime; // time
	b3BlockPool m_nodePool;
	b3ProfilerNode* m_root; // root node
	b3ProfilerNode* m_top; // top node
	b3ProfilerNodeStats* m_statsHead; // list of permanent statistics
};

// The profiler used by Bounce. 
extern b3Profiler* b3Profiler_profiler;

#define B3_JOIN(a, b) a##b
#define B3_CONCATENATE(a, b) B3_JOIN(a, b)
#define B3_UNIQUE_NAME(name) B3_CONCATENATE(name, __LINE__)

// A profiler block. 
struct b3ProfileScope
{
	b3ProfileScope(const char* name)
	{
		if (b3Profiler_profiler)
		{
			b3Profiler_profiler->BeginScope(name);
		}
	}

	~b3ProfileScope()
	{
		if (b3Profiler_profiler)
		{
			b3Profiler_profiler->EndScope();
		}
	}
};

#define B3_PROFILE(name) b3ProfileScope B3_UNIQUE_NAME(scope)(name)

#endif