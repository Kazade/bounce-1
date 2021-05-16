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

#include <bounce/common/math/vec2.h>
#include <bounce/common/math/vec3.h>
#include <bounce/common/math/mat33.h>
#include <bounce/common/math/mat44.h>
#include <bounce/common/math/transform.h>
#include <bounce/common/graphics/color.h>

struct b3DebugPoint
{
	b3Vec3 position;
	b3Color color;
	scalar size;
	bool depthEnabled;
};

// Draw points with batching and depth test controlling support.
template<class T>
class b3DebugPoints
{
public:
	b3DebugPoints(u32 pointCapacity, T* callback)
	{
		m_debugPointCapacity = pointCapacity;
		m_debugPointCount = 0;
		m_debugPoints = (b3DebugPoint*)b3Alloc(m_debugPointCapacity * sizeof(b3DebugPoint));
		m_callback = callback;
	}
	
	~b3DebugPoints()
	{
		b3Free(m_debugPoints);
	}
	
	// Draw a point.
	void Draw(const b3Vec3& p, scalar size, const b3Color& color, bool depthEnabled = true)
	{
		if (m_debugPointCount == m_debugPointCapacity)
		{
			Flush();
		}

		B3_ASSERT(m_debugPointCount < m_debugPointCapacity);
		
		b3DebugPoint point;
		point.position = p;
		point.size = size;
		point.color = color;
		point.depthEnabled = depthEnabled;
		
		m_debugPoints[m_debugPointCount++] = point;
	}
	
	// Flush the points.
	void Flush()
	{
		// First pass: Depth test enabled.
		for (int i = 0; i < m_debugPointCount; ++i)
		{
			b3DebugPoint point = m_debugPoints[i];	
			if (point.depthEnabled == true)
			{
				m_callback->AddPoint(point.position, point.color, point.size);
			}
		}
		
		m_callback->FlushPoints(true);
		
		// Second pass: Depth test disabled.
		for (int i = 0; i < m_debugPointCount; ++i)
		{
			b3DebugPoint point = m_debugPoints[i];	
			if (point.depthEnabled == false)
			{
				m_callback->AddPoint(point.position, point.color, point.size);
			}
		}
		
		m_callback->FlushPoints(false);
		
		m_debugPointCount = 0;
	}
private:
	T* m_callback;
	u32 m_debugPointCapacity;
	u32 m_debugPointCount;
	b3DebugPoint* m_debugPoints;
};

struct b3DebugLine
{
	b3Vec3 p1, p2;
	b3Color color;
	bool depthEnabled;
};

// Draw lines with batching and depth test controlling support.
template<class T>
class b3DebugLines
{
public:
	b3DebugLines(u32 lineCapacity, T* callback)
	{
		m_debugLineCapacity = lineCapacity;
		m_debugLineCount = 0;
		m_debugLines = (b3DebugLine*)b3Alloc(m_debugLineCapacity * sizeof(b3DebugLine));
		m_callback = callback;
	}
	
	~b3DebugLines()
	{
		b3Free(m_debugLines);
	}
	
	// Draw a line.
	void Draw(const b3Vec3& p1, const b3Vec3& p2, const b3Color& color, bool depthEnabled = true)
	{		
		if (m_debugLineCount == m_debugLineCapacity)
		{
			Flush();
		}

		B3_ASSERT(m_debugLineCount < m_debugLineCapacity);
		
		b3DebugLine line;
		line.p1 = p1;
		line.p2 = p2;
		line.color = color;
		line.depthEnabled = depthEnabled;
		
		m_debugLines[m_debugLineCount++] = line;	
	}
	
	// Flush the lines.
	void Flush()
	{
		// First pass: Depth test enabled.
		for (int i = 0; i < m_debugLineCount; ++i)
		{
			b3DebugLine line = m_debugLines[i];	
			if (line.depthEnabled == true)
			{
				m_callback->AddLine(line.p1, line.p2, line.color);
			}
		}
		
		m_callback->FlushLines(true);
		
		// Second pass: Depth test disabled.
		for (int i = 0; i < m_debugLineCount; ++i)
		{
			b3DebugLine line = m_debugLines[i];	
			if (line.depthEnabled == false)
			{	
				m_callback->AddLine(line.p1, line.p2, line.color);
			}
		}
		
		m_callback->FlushLines(false);
		
		m_debugLineCount = 0;	
	}
private:
	T* m_callback;
	u32 m_debugLineCapacity;
	u32 m_debugLineCount;
	b3DebugLine* m_debugLines;
};

struct b3DebugTriangle
{
	b3Vec3 p1, p2, p3;
	b3Color color;
	b3Vec3 normal;
	bool depthEnabled;
};

// Draw triangles with batching and depth test controlling support.
template<class T>
class b3DebugTriangles
{
public:
	b3DebugTriangles(u32 triangleCapacity, T* callback)
	{
		m_debugTriangleCapacity = triangleCapacity;
		m_debugTriangleCount = 0;
		m_debugTriangles = (b3DebugTriangle*)b3Alloc(m_debugTriangleCapacity * sizeof(b3DebugTriangle));
		m_callback = callback;
	}
	
	~b3DebugTriangles()
	{		
		b3Free(m_debugTriangles);
	}
	
	// Draw a triangle.
	void Draw(const b3Vec3& normal, const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color, bool depthEnabled = true)
	{		
		if (m_debugTriangleCount == m_debugTriangleCapacity)
		{
			Flush();
		}

		B3_ASSERT(m_debugTriangleCount < m_debugTriangleCapacity);
		
		b3DebugTriangle triangle;
		triangle.p1 = p1;
		triangle.p2 = p2;
		triangle.p3 = p3;
		triangle.color = color;
		triangle.normal = normal;
		triangle.depthEnabled = depthEnabled;
		
		m_debugTriangles[m_debugTriangleCount++] = triangle;
	}
	
	// Flush the triangles.
	void Flush()
	{
		// First pass: Depth test enabled.
		for (int i = 0; i < m_debugTriangleCount; ++i)
		{
			b3DebugTriangle triangle = m_debugTriangles[i];	
			if (triangle.depthEnabled == true)
			{
				m_callback->AddTriangle(triangle.p1, triangle.p2, triangle.p3, triangle.color, triangle.normal);
			}
		}
		
		m_callback->FlushTriangles(true);
		
		// Second pass: Depth test disabled.
		for (int i = 0; i < m_debugTriangleCount; ++i)
		{
			b3DebugTriangle triangle = m_debugTriangles[i];	
			if (triangle.depthEnabled == false)
			{
				m_callback->AddTriangle(triangle.p1, triangle.p2, triangle.p3, triangle.color, triangle.normal);
			}
		}
		
		m_callback->FlushTriangles(false);
		
		m_debugTriangleCount = 0;	
	}
private:
	T* m_callback;
	u32 m_debugTriangleCapacity;
	u32 m_debugTriangleCount;
	b3DebugTriangle* m_debugTriangles;
};

#endif
