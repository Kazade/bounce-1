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

#ifndef B3_DEBUG_PRIMITIVES_H
#define B3_DEBUG_PRIMITIVES_H

#include <bounce/common/math/vec3.h>
#include <bounce/common/graphics/color.h>

// Implement this interface to render the debug primitives.
class b3DebugRenderer
{
public:
	virtual void AddPoint(const b3Vec3& position, const b3Color& color, scalar size) { }
	virtual void AddLine(const b3Vec3& p1, const b3Vec3& p2, const b3Color& color) { }
	virtual void AddTriangle(const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color, const b3Vec3& normal) { }

	virtual void FlushPoints(bool depthEnabled) { }
	virtual void FlushLines(bool depthEnabled) { }
	virtual void FlushTriangles(bool depthEnabled) { }
};

struct b3DebugPoint
{
	b3Vec3 position;
	b3Color color;
	scalar size;
	bool depthEnabled;
};

// Draw points with batching and depth test controlling support.
class b3DebugPoints
{
public:
	b3DebugPoints(u32 capacity)
	{
		m_capacity = capacity;
		m_count = 0;
		m_points = (b3DebugPoint*)b3Alloc(m_capacity * sizeof(b3DebugPoint));
		m_renderer = nullptr;
		m_drawEnabled = true;
	}
	
	~b3DebugPoints()
	{
		b3Free(m_points);
	}

	// Draw a point.
	void Draw(const b3Vec3& p, scalar size, const b3Color& color, bool depthEnabled = true)
	{
		if (m_drawEnabled == false)
		{
			return;
		}

		if (m_count == m_capacity)
		{
			Flush();
		}

		B3_ASSERT(m_count < m_capacity);
		
		b3DebugPoint point;
		point.position = p;
		point.size = size;
		point.color = color;
		point.depthEnabled = depthEnabled;
		
		m_points[m_count++] = point;
	}
	
	// Flush the points.
	void Flush()
	{
		if (m_renderer == nullptr)
		{
			m_count = 0;
			return;
		}

		// First pass: Depth test enabled.
		for (u32 i = 0; i < m_count; ++i)
		{
			b3DebugPoint point = m_points[i];	
			if (point.depthEnabled == true)
			{
				m_renderer->AddPoint(point.position, point.color, point.size);
			}
		}
		
		m_renderer->FlushPoints(true);
		
		// Second pass: Depth test disabled.
		for (u32 i = 0; i < m_count; ++i)
		{
			b3DebugPoint point = m_points[i];	
			if (point.depthEnabled == false)
			{
				m_renderer->AddPoint(point.position, point.color, point.size);
			}
		}
		
		m_renderer->FlushPoints(false);
		
		m_count = 0;
	}

	// Attach a renderer.
	void SetRenderer(b3DebugRenderer* renderer) { m_renderer = renderer; }

	// Return the attached renderer.
	b3DebugRenderer* GetRenderer() { return m_renderer; }
	const b3DebugRenderer* GetRenderer() const { return m_renderer; }

	// Enable drawing the points.
	void EnableDraw(bool flag) { m_drawEnabled = flag; }

	// Is drawing enabled.
	bool IsDrawEnabled() const { return m_drawEnabled; }
private:
	u32 m_capacity;
	u32 m_count;
	b3DebugPoint* m_points;
	b3DebugRenderer* m_renderer;
	bool m_drawEnabled;
};

struct b3DebugLine
{
	b3Vec3 p1, p2;
	b3Color color;
	bool depthEnabled;
};

// Draw lines with batching and depth test controlling support.
class b3DebugLines
{
public:
	b3DebugLines(u32 capacity)
	{
		m_capacity = capacity;
		m_count = 0;
		m_lines = (b3DebugLine*)b3Alloc(m_capacity * sizeof(b3DebugLine));
		m_renderer = nullptr;
		m_drawEnabled = true;
	}
	
	~b3DebugLines()
	{
		b3Free(m_lines);
	}
	
	// Draw a line.
	void Draw(const b3Vec3& p1, const b3Vec3& p2, const b3Color& color, bool depthEnabled = true)
	{
		if (m_drawEnabled == false)
		{
			return;
		}

		if (m_count == m_capacity)
		{
			Flush();
		}

		B3_ASSERT(m_count < m_capacity);
		
		b3DebugLine line;
		line.p1 = p1;
		line.p2 = p2;
		line.color = color;
		line.depthEnabled = depthEnabled;
		
		m_lines[m_count++] = line;	
	}
	
	// Flush the lines.
	void Flush()
	{
		if (m_renderer == nullptr)
		{
			m_count = 0;
			return;
		}

		// First pass: Depth test enabled.
		for (u32 i = 0; i < m_count; ++i)
		{
			b3DebugLine line = m_lines[i];	
			if (line.depthEnabled == true)
			{
				m_renderer->AddLine(line.p1, line.p2, line.color);
			}
		}
		
		m_renderer->FlushLines(true);
		
		// Second pass: Depth test disabled.
		for (u32 i = 0; i < m_count; ++i)
		{
			b3DebugLine line = m_lines[i];	
			if (line.depthEnabled == false)
			{	
				m_renderer->AddLine(line.p1, line.p2, line.color);
			}
		}
		
		m_renderer->FlushLines(false);
		
		m_count = 0;	
	}

	// Attach a renderer.
	void SetRenderer(b3DebugRenderer* renderer) { m_renderer = renderer; }

	// Return the attached renderer.
	b3DebugRenderer* GetRenderer() { return m_renderer; }
	const b3DebugRenderer* GetRenderer() const { return m_renderer; }

	// Enable drawing the points.
	void EnableDraw(bool flag) { m_drawEnabled = flag; }

	// Is drawing enabled.
	bool IsDrawEnabled() const { return m_drawEnabled; }
private:
	u32 m_capacity;
	u32 m_count;
	b3DebugLine* m_lines;
	b3DebugRenderer* m_renderer;
	bool m_drawEnabled;
};

struct b3DebugTriangle
{
	b3Vec3 p1, p2, p3;
	b3Color color;
	b3Vec3 normal;
	bool depthEnabled;
};

// Draw triangles with batching and depth test controlling support.
class b3DebugTriangles
{
public:
	b3DebugTriangles(u32 capacity)
	{
		m_capacity = capacity;
		m_count = 0;
		m_triangles = (b3DebugTriangle*)b3Alloc(m_capacity * sizeof(b3DebugTriangle));
		m_renderer = nullptr;
		m_drawEnabled = true;
	}
	
	~b3DebugTriangles()
	{		
		b3Free(m_triangles);
	}
	
	// Draw a triangle.
	void Draw(const b3Vec3& normal, const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color, bool depthEnabled = true)
	{	
		if (m_drawEnabled == false)
		{
			return;
		}

		if (m_count == m_capacity)
		{
			Flush();
		}

		B3_ASSERT(m_count < m_capacity);
		
		b3DebugTriangle triangle;
		triangle.p1 = p1;
		triangle.p2 = p2;
		triangle.p3 = p3;
		triangle.color = color;
		triangle.normal = normal;
		triangle.depthEnabled = depthEnabled;
		
		m_triangles[m_count++] = triangle;
	}
	
	// Flush the triangles.
	void Flush()
	{
		if (m_renderer == nullptr)
		{
			m_count = 0;
			return;
		}

		// First pass: Depth test enabled.
		for (u32 i = 0; i < m_count; ++i)
		{
			b3DebugTriangle triangle = m_triangles[i];	
			if (triangle.depthEnabled == true)
			{
				m_renderer->AddTriangle(triangle.p1, triangle.p2, triangle.p3, triangle.color, triangle.normal);
			}
		}
		
		m_renderer->FlushTriangles(true);
		
		// Second pass: Depth test disabled.
		for (u32 i = 0; i < m_count; ++i)
		{
			b3DebugTriangle triangle = m_triangles[i];	
			if (triangle.depthEnabled == false)
			{
				m_renderer->AddTriangle(triangle.p1, triangle.p2, triangle.p3, triangle.color, triangle.normal);
			}
		}
		
		m_renderer->FlushTriangles(false);
		
		m_count = 0;	
	}

	// Attach a renderer.
	void SetRenderer(b3DebugRenderer* renderer) { m_renderer = renderer; }

	// Return the attached renderer.
	b3DebugRenderer* GetRenderer() { return m_renderer; }
	const b3DebugRenderer* GetRenderer() const { return m_renderer; }

	// Enable drawing the points.
	void EnableDraw(bool flag) { m_drawEnabled = flag; }

	// Is drawing enabled.
	bool IsDrawEnabled() const { return m_drawEnabled; }
private:
	u32 m_capacity;
	u32 m_count;
	b3DebugTriangle* m_triangles;
	b3DebugRenderer* m_renderer;
	bool m_drawEnabled;
};

#endif