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

#ifndef B3_DEBUGDRAW_H
#define B3_DEBUGDRAW_H

#include <bounce/common/graphics/debug_primitives.h>

// Implement this interface and pass it into b3DebugDraw so it can render the debug primitives.
class b3DebugDrawCallback
{
public:
	virtual void AddPoint(const b3Vec3& position, const b3Color& color, scalar size) = 0;
	virtual void AddLine(const b3Vec3& p1, const b3Vec3& p2, const b3Color& color) = 0;
	virtual void AddTriangle(const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color, const b3Vec3& normal) = 0;

	virtual void FlushPoints(bool depthEnabled) = 0;
	virtual void FlushLines(bool depthEnabled) = 0;
	virtual void FlushTriangles(bool depthEnabled) = 0;
};

// Draw primitives with batching and depth test controlling support.
class b3DebugDraw
{
public:
	b3DebugDraw(u32 pointCapacity, u32 lineCapacity, u32 triangleCapacity, b3DebugDrawCallback* callback) :
		m_points(pointCapacity, callback),
		m_lines(lineCapacity, callback),
		m_triangles(triangleCapacity, callback)
	{
		m_drawPoints = true;
		m_drawLines = true;
		m_drawTriangles = true;
		m_callback = callback;
	}

	~b3DebugDraw()
	{
	}

	// Call this function before issuing any debug draw call.
	void Begin()
	{
	}

	// Call this function to render the primitives.
	void End()
	{
		// Order: Points over lines and lines over triangles.
		m_triangles.Flush();
		m_lines.Flush();
		m_points.Flush();
	}

	// Should points be rendered at the end?
	void EnableDrawPoints(bool flag)
	{
		m_drawPoints = flag;
	}

	// Should lines be rendered at the end?
	void EnableDrawLines(bool flag)
	{
		m_drawLines = flag;
	}

	// Should triangles be rendered at the end?
	void EnableDrawTriangles(bool flag)
	{
		m_drawTriangles = flag;
	}

	// Draw a point.
	void DrawPoint(const b3Vec3& p, scalar size, const b3Color& color, bool depthEnabled = true)
	{
		if (!m_drawPoints)
		{
			return;
		}

		m_points.Draw(p, size, color, depthEnabled);
	}

	// Draw a line segment.
	void DrawLine(const b3Vec3& p1, const b3Vec3& p2, const b3Color& color, bool depthEnabled = true)
	{
		if (!m_drawLines)
		{
			return;
		}

		m_lines.Draw(p1, p2, color, depthEnabled);
	}

	// Draw a solid triangle with vertices ordered CCW.
	void DrawTriangle(const b3Vec3& normal, const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color, bool depthEnabled = true)
	{
		if (!m_drawTriangles)
		{
			return;
		}

		m_triangles.Draw(normal, p1, p2, p3, color, depthEnabled);
	}
private:
	bool m_drawPoints;
	b3DebugPoints<b3DebugDrawCallback> m_points;

	bool m_drawLines;
	b3DebugLines<b3DebugDrawCallback> m_lines;

	bool m_drawTriangles;
	b3DebugTriangles<b3DebugDrawCallback> m_triangles;

	b3DebugDrawCallback* m_callback;
};

// The debug draw utility section.

// Rotation between two normal vectors.
static inline b3Quat b3RotationBetween(const b3Vec3& n1, const b3Vec3& n2)
{
	// |n1 x n2| = sin(theta)
	// n1 . n2 = cos(theta)
	// sin(theta / 2) = +/- sqrt([1 - cos(theta)] / 2)
	// cos(theta / 2) = +/- sqrt([1 + cos(theta)] / 2)
	// q.v = sin(theta / 2) * (n1 x n2) / |n1 x n2|
	// q.v = cos(theta / 2)
	b3Quat rotation;
	
	b3Vec3 axis = b3Cross(n1, n2);
	scalar s = b3Length(axis);
	scalar c = b3Dot(n1, n2);
	if (s > B3_EPSILON)
	{
		rotation.v = b3Sqrt(scalar(0.5) * (scalar(1) - c)) * (axis / s);
		rotation.s = b3Sqrt(scalar(0.5) * (scalar(1) + c));
	}
	else
	{
		rotation.SetIdentity();
	}
	
	return rotation;	
}

// Draw a point.
inline void b3DrawPoint(b3DebugDraw* draw, const b3Vec3& p, scalar size, const b3Color& color, bool depthEnabled = true)
{
	draw->DrawPoint(p, size, color, depthEnabled);
}

// Draw a segment.
inline void b3DrawSegment(b3DebugDraw* draw, const b3Vec3& p1, const b3Vec3& p2, const b3Color& color, bool depthEnabled = true)
{
	draw->DrawLine(p1, p2, color, depthEnabled);
}

// Draw a triangle.
inline void b3DrawTriangle(b3DebugDraw* draw, const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color, bool depthEnabled = true)
{
	draw->DrawLine(p1, p2, color, depthEnabled);
	draw->DrawLine(p2, p3, color, depthEnabled);
	draw->DrawLine(p3, p1, color, depthEnabled);
}

// Draw a solid triangle.
inline void b3DrawSolidTriangle(b3DebugDraw* draw, const b3Vec3& normal, const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color, bool depthEnabled = true)
{
	draw->DrawTriangle(normal, p1, p2, p3, color, depthEnabled);
}

static inline b3Vec3 b3MakeVec3(int vertexStride, const void* vertexBase, int vtx)
{
	float* p = (float*)((char*)vertexBase + vertexStride * vtx);
	return b3Vec3(p[0], p[1], p[2]);
}

// Draw a polygon.
inline void b3DrawPolygon(b3DebugDraw* draw, const void* vertices, u32 vertexStride, u32 count, const b3Color& color, bool depthEnabled = true)
{
	b3Vec3 p1 = b3MakeVec3(vertexStride, vertices, count - 1);
	for (u32 i = 0; i < count; ++i)
	{
		b3Vec3 p2 = b3MakeVec3(vertexStride, vertices, i);

		draw->DrawLine(p1, p2, color, depthEnabled);

		p1 = p2;
	}
}

// Draw a solid polygon with vertices ordered CCW.
inline void b3DrawSolidPolygon(b3DebugDraw* draw, const b3Vec3& normal, const void* vertices, u32 vertexStride, u32 count, const b3Color& color, bool depthEnabled = true)
{
	b3Vec3 p1 = b3MakeVec3(vertexStride, vertices, 0);
	for (u32 i = 1; i < count - 1; ++i)
	{
		b3Vec3 p2 = b3MakeVec3(vertexStride, vertices, i);
		b3Vec3 p3 = b3MakeVec3(vertexStride, vertices, i + 1);

		draw->DrawTriangle(normal, p1, p2, p3, color, depthEnabled);
	}
}

// Draw a circle.
template <u32 E = 20>
inline void b3DrawCircle(b3DebugDraw* draw, const b3Vec3& normal, const b3Vec3& center, scalar radius, const b3Color& color, bool depthEnabled = true)
{
	b3Vec3 n1, n3;
	b3ComputeBasis(normal, n1, n3);
	
	scalar kAngleInc = 2.0f * B3_PI / scalar(E);
	
	b3Quat q;
	q.SetAxisAngle(normal, kAngleInc);

	b3Vec3 p1 = center + radius * n1;
	for (u32 i = 0; i < E; ++i)
	{
		b3Vec3 n2 = b3Mul(q, n1);
		b3Vec3 p2 = center + radius * n2;

		draw->DrawLine(p1, p2, color, depthEnabled);

		n1 = n2;
		p1 = p2;
	}
}

// Draw a solid circle.
template<u32 E = 20>
void b3DrawSolidCircle(b3DebugDraw* draw, const b3Vec3& normal, const b3Vec3& center, scalar radius, const b3Color& color, bool depthEnabled = true)
{
	b3Vec3 n1, n3;
	b3ComputeBasis(normal, n1, n3);

	scalar kAngleInc = 2.0f * B3_PI / scalar(E);
	
	b3Quat q;
	q.SetAxisAngle(normal, kAngleInc);

	b3Vec3 p1 = center + radius * n1;
	for (u32 i = 0; i < E; ++i)
	{
		b3Vec3 n2 = b3Mul(q, n1);
		b3Vec3 p2 = center + radius * n2;

		draw->DrawTriangle(normal, center, p1, p2, color, depthEnabled);

		n1 = n2;
		p1 = p2;
	}
}

// A (H + 1) x (W + 1) UV sphere mesh stored in row-major order.
// v(i, j) = i * (W + 1) + j
template<u32 H = 1, u32 W = 1>
struct b3SphereMesh
{
	u32 vertexCount;
	b3Vec3 vertices[(H + 1) * (W + 1)];
	u32 indexCount;
	u32 indices[3 * 2 * H * W];
	
	b3SphereMesh()
	{
		// Build vertices
		
		// Latitude increment in range [0, pi]
		scalar kThetaInc = B3_PI / scalar(H);
		
		// Longitude increment in range [0, 2*pi]
		scalar kPhiInc = 2.0f * B3_PI / scalar(W);
		
		vertexCount = 0;
		for (u32 i = 0; i < H + 1; ++i)
		{
			// Plane to spherical coordinates
			scalar theta = scalar(i) * kThetaInc;
			scalar cos_theta = cos(theta);
			scalar sin_theta = sin(theta);
				
			for (u32 j = 0; j < W + 1; ++j)
			{
				scalar phi = scalar(j) * kPhiInc;	
				scalar cos_phi = cos(phi);
				scalar sin_phi = sin(phi);
				
				// Spherical to Cartesian coordinates		
				b3Vec3 p;
				p.x = sin_theta * sin_phi;
				p.y = cos_theta;
				p.z = sin_theta * cos_phi;
				
				u32 vertex = GetVertex(i, j);
				vertices[vertex] = p;
				++vertexCount;
			}
		}
		
		B3_ASSERT(vertexCount == (H + 1) * (W + 1));
		
		// Build triangles
		indexCount = 0;
		for (u32 i = 0; i < H; ++i)
		{
			for (u32 j = 0; j < W; ++j)
			{
				// 1*|----|*4
				//   |----|
				// 2*|----|*3
				u32 v1 = GetVertex(i, j);
				u32 v2 = GetVertex(i + 1, j);
				u32 v3 = GetVertex(i + 1, j + 1);
				u32 v4 = GetVertex(i, j + 1);

				indices[indexCount++] = v1;
				indices[indexCount++] = v2;
				indices[indexCount++] = v3;

				indices[indexCount++] = v3;
				indices[indexCount++] = v4;
				indices[indexCount++] = v1;
			}
		}

		B3_ASSERT(indexCount == 3 * 2 * H * W);
	}

	u32 GetVertex(u32 i, u32 j)
	{
		B3_ASSERT(i < H + 1);
		B3_ASSERT(j < W + 1);
		return i * (W + 1) + j;
	}
};

// Draw a sphere.
template<u32 H = 20, u32 W = 20>
inline void b3DrawSphere(b3DebugDraw* draw, const b3Vec3& center, scalar radius, const b3Color& color, bool depthEnabled = true)
{
	b3SphereMesh<H, W> sphere;
	
	for (u32 i = 0; i < sphere.indexCount / 3; ++i)
	{
		u32 v1 = sphere.indices[3 * i];
		u32 v2 = sphere.indices[3 * i + 1];
		u32 v3 = sphere.indices[3 * i + 2];

		b3Vec3 p1 = sphere.vertices[v1];
		p1 *= radius;
		p1 += center;

		b3Vec3 p2 = sphere.vertices[v2];
		p2 *= radius;
		p2 += center;

		b3Vec3 p3 = sphere.vertices[v3];
		p3 *= radius;
		p3 += center;
		
		b3DrawTriangle(draw, p1, p2, p3, color, depthEnabled);
	}
}

// Draw a solid sphere.
template<u32 H = 20, u32 W = 20>
inline void b3DrawSolidSphere(b3DebugDraw* draw, const b3Vec3& axis, const b3Vec3& center, scalar radius, const b3Color& color, bool depthEnabled = true)
{
	b3SphereMesh<H, W> sphere;
	
	b3Transform xf;
	xf.rotation = b3RotationBetween(b3Vec3_y, axis);
	xf.translation = center;

	for (u32 i = 0; i < sphere.indexCount / 3; ++i)
	{
		u32 v1 = sphere.indices[3 * i];
		u32 v2 = sphere.indices[3 * i + 1];
		u32 v3 = sphere.indices[3 * i + 2];

		b3Vec3 p1 = sphere.vertices[v1];
		
		b3Vec3 n1 = p1;

		p1 *= radius;
		p1 = b3Mul(xf, p1);

		b3Vec3 p2 = sphere.vertices[v2];

		b3Vec3 n2 = p2;
		
		p2 *= radius;
		p2 = b3Mul(xf, p2);

		b3Vec3 p3 = sphere.vertices[v3];

		b3Vec3 n3 = p3;
		
		p3 *= radius;
		p3 = b3Mul(xf, p3);

		b3Vec3 n = (n1 + n2 + n3) / 3.0f;
		n = b3Mul(xf.rotation, n);

		draw->DrawTriangle(n, p1, p2, p3, color, depthEnabled);
	}
}

// A cylinder mesh.
template<u32 H = 1, u32 W = 1>
struct b3CylinderMesh
{
	u32 vertexCount;
	b3Vec3 vertices[(H + 1) * (W + 1)];
	u32 indexCount;
	u32 indices[3 * 2 * H * W + 3 * 2 * (((W + 1) - 1) - 1)];
	
	b3CylinderMesh()
	{
		// Build vertices
		
		// Angular increment in range [0, 2*pi]
		scalar kPhiInc = 2.0f * B3_PI / scalar(W);
		
		// Longitude increment in range [0, 1]
		scalar kYInc = 1.0f / scalar(H);
		
		vertexCount = 0;
		for (u32 i = 0; i < H + 1; ++i)
		{
			// Plane to cylindrical coordinates
			scalar y = scalar(i) * kYInc;
				
			for (u32 j = 0; j < W + 1; ++j)
			{
				// Plane to cylindrical coordinates
				scalar phi = scalar(j) * kPhiInc;	
				scalar cos_phi = cos(phi);
				scalar sin_phi = sin(phi);
				
				// Cylindrical to Cartesian coordinates		
				b3Vec3 p;
				p.x = cos_phi;
				p.y = y - 0.5f; // Centralize
				p.z = sin_phi;
				
				u32 vertex = GetVertex(i, j);
				vertices[vertex] = p;
				++vertexCount;
			}
		}
		
		B3_ASSERT(vertexCount == (H + 1) * (W + 1));
		
		// Build triangles
		indexCount = 0;
		for (u32 i = 0; i < H; ++i)
		{
			for (u32 j = 0; j < W; ++j)
			{
				// 1*|----|*4
				//   |----|
				// 2*|----|*3
				u32 v1 = GetVertex(i, j);
				u32 v2 = GetVertex(i + 1, j);
				u32 v3 = GetVertex(i + 1, j + 1);
				u32 v4 = GetVertex(i, j + 1);

				indices[indexCount++] = v1;
				indices[indexCount++] = v2;
				indices[indexCount++] = v3;

				indices[indexCount++] = v3;
				indices[indexCount++] = v4;
				indices[indexCount++] = v1;
			}
		}

		B3_ASSERT(indexCount == 3 * 2 * H * W);
		
		// Lower circle
		u32 i1 = 0;
		for (u32 i2 = i1 + 1; i2 < (W + 1) - 1; ++i2)
		{
			u32 i3 = i2 + 1;
			
			u32 v1 = GetVertex(0, i1);
			u32 v2 = GetVertex(0, i2);
			u32 v3 = GetVertex(0, i3);
			
			indices[indexCount++] = v1;
			indices[indexCount++] = v2;
			indices[indexCount++] = v3;
		}
		
		// Upper circle
		i1 = 0;
		for (u32 i2 = i1 + 1; i2 < (W + 1) - 1; ++i2)
		{
			u32 i3 = i2 + 1;
			
			u32 v1 = GetVertex(H, i1);
			u32 v2 = GetVertex(H, i2);
			u32 v3 = GetVertex(H, i3);
			
			// Flip order to ensure CCW
			indices[indexCount++] = v3;
			indices[indexCount++] = v2;
			indices[indexCount++] = v1;
		}
		
		B3_ASSERT(indexCount == 3 * 2 * H * W + 3 * 2 * (((W + 1) - 1) - 1));
	}
	
	u32 GetVertex(u32 i, u32 j)
	{
		B3_ASSERT(i < H + 1);
		B3_ASSERT(j < W + 1);
		return i * (W + 1) + j;
	}
};

// Draw a cylinder.
template<u32 H = 20, u32 W = 20>
inline void b3DrawCylinder(b3DebugDraw* draw, const b3Vec3& axis, const b3Vec3& center, scalar radius, scalar height, const b3Color& color, bool depthEnabled = true)
{
	b3CylinderMesh<H, W> cylinder;
	
	b3Transform xf;
	xf.rotation = b3RotationBetween(b3Vec3_y, axis);
	xf.translation = center;

	for (u32 i = 0; i < cylinder.indexCount / 3; ++i)
	{
		u32 v1 = cylinder.indices[3 * i];
		u32 v2 = cylinder.indices[3 * i + 1];
		u32 v3 = cylinder.indices[3 * i + 2];

		b3Vec3 p1 = cylinder.vertices[v1];
		
		p1.x *= radius;
		p1.y *= height;
		p1.z *= radius;
		p1 = b3Mul(xf, p1);
		
		b3Vec3 p2 = cylinder.vertices[v2];
		
		p2.x *= radius;
		p2.y *= height;
		p2.z *= radius;
		p2 = b3Mul(xf, p2);

		b3Vec3 p3 = cylinder.vertices[v3];
		
		p3.x *= radius;
		p3.y *= height;
		p3.z *= radius;
		p3 = b3Mul(xf, p3);

		b3DrawTriangle(draw, p1, p2, p3, color, depthEnabled);
	}
}

// Draw a solid cylinder.
template<u32 H = 20, u32 W = 20>
inline void b3DrawSolidCylinder(b3DebugDraw* draw, const b3Vec3& axis, const b3Vec3& center, scalar radius, scalar height, const b3Color& color, bool depthEnabled = true)
{
	b3CylinderMesh<H, W> cylinder;
	
	b3Transform xf;
	xf.rotation = b3RotationBetween(b3Vec3_y, axis);
	xf.translation = center;

	for (u32 i = 0; i < cylinder.indexCount / 3; ++i)
	{
		u32 v1 = cylinder.indices[3 * i];
		u32 v2 = cylinder.indices[3 * i + 1];
		u32 v3 = cylinder.indices[3 * i + 2];

		b3Vec3 p1 = cylinder.vertices[v1];
		b3Vec3 p2 = cylinder.vertices[v2];
		b3Vec3 p3 = cylinder.vertices[v3];
		
		b3Vec3 n = b3Cross(p2 - p1, p3 - p1);
		n.Normalize();
		
		n = b3Mul(xf.rotation, n);
		
		p1.x *= radius;
		p1.y *= height;
		p1.z *= radius;
		p1 = b3Mul(xf, p1);

		p2.x *= radius;
		p2.y *= height;
		p2.z *= radius;
		p2 = b3Mul(xf, p2);
		
		p3.x *= radius;
		p3.y *= height;
		p3.z *= radius;
		p3 = b3Mul(xf, p3);

		draw->DrawTriangle(n, p1, p2, p3, color, depthEnabled);
	}
}

// Draw a capsule.
template<u32 H = 20, u32 W = 20>
inline void b3DrawCapsule(b3DebugDraw* draw, const b3Vec3& c1, const b3Vec3& c2, scalar radius, const b3Color& color, bool depthEnabled = true)
{
	b3DrawSphere<H, W>(draw, c1, radius, color);
	if (b3LengthSquared(c1 - c2) > B3_EPSILON * B3_EPSILON)
	{
		draw->DrawLine(c1, c2, color, depthEnabled);
		
		b3DrawSphere<H, W>(draw, c2, radius, color, depthEnabled);
	}
}

// Draw a capsule in solid rendering mode.
template<u32 H = 20, u32 W = 20>
inline void b3DrawSolidCapsule(b3DebugDraw* draw, const b3Vec3& axis, const b3Vec3& c1, const b3Vec3& c2, scalar radius, const b3Color& color, bool depthEnabled = true)
{
	b3DrawSolidSphere<H, W>(draw, axis, c1, radius, color, depthEnabled);
	if (b3LengthSquared(c1 - c2) > B3_EPSILON * B3_EPSILON)
	{
		{
			scalar height = b3Length(c1 - c2);
			b3Vec3 axis = (c1 - c2) / height;
			b3Vec3 center = scalar(0.5) * (c1 + c2);
						
			b3DrawSolidCylinder<H, W>(draw, axis, center, radius, height, color, depthEnabled);
		}

		b3DrawSolidSphere<H, W>(draw, axis, c2, radius, color, depthEnabled);
	}
}

// Draw a transform.
inline void b3DrawTransform(b3DebugDraw* draw, const b3Transform& xf, bool depthEnabled = true)
{
	scalar lenght = scalar(1);

	b3Vec3 translation = xf.translation;
	b3Quat rotation = xf.rotation;
	
	b3Vec3 A = translation + lenght * rotation.GetXAxis();
	b3Vec3 B = translation + lenght * rotation.GetYAxis();
	b3Vec3 C = translation + lenght * rotation.GetZAxis();

	draw->DrawLine(translation, A, b3Color_red, depthEnabled);
	draw->DrawLine(translation, B, b3Color_green, depthEnabled);
	draw->DrawLine(translation, C, b3Color_blue, depthEnabled);
}

// Draw an AABB.
inline void b3DrawAABB(b3DebugDraw* draw, const b3Vec3& lowerBound, const b3Vec3& upperBound, const b3Color& color, bool depthEnabled = true)
{
	b3Vec3 vs[8];

	vs[0] = lowerBound;
	vs[1] = b3Vec3(lowerBound.x, upperBound.y, lowerBound.z);
	vs[2] = b3Vec3(upperBound.x, upperBound.y, lowerBound.z);
	vs[3] = b3Vec3(upperBound.x, lowerBound.y, lowerBound.z);

	vs[4] = upperBound;
	vs[5] = b3Vec3(upperBound.x, lowerBound.y, upperBound.z);
	vs[6] = b3Vec3(lowerBound.x, lowerBound.y, upperBound.z);
	vs[7] = b3Vec3(lowerBound.x, upperBound.y, upperBound.z);

	draw->DrawLine(vs[0], vs[1], color, depthEnabled);
	draw->DrawLine(vs[1], vs[2], color, depthEnabled);
	draw->DrawLine(vs[2], vs[3], color, depthEnabled);
	draw->DrawLine(vs[3], vs[0], color, depthEnabled);

	draw->DrawLine(vs[4], vs[5], color, depthEnabled);
	draw->DrawLine(vs[5], vs[6], color, depthEnabled);
	draw->DrawLine(vs[6], vs[7], color, depthEnabled);
	draw->DrawLine(vs[7], vs[4], color, depthEnabled);

	draw->DrawLine(vs[2], vs[4], color, depthEnabled);
	draw->DrawLine(vs[5], vs[3], color, depthEnabled);

	draw->DrawLine(vs[6], vs[0], color, depthEnabled);
	draw->DrawLine(vs[1], vs[7], color, depthEnabled);
}

// Draw a grid.
template<u32 H = 32, u32 W = 32>
inline void b3DrawGrid(b3DebugDraw* draw, const b3Vec3& normal, const b3Vec3& center, u32 width, u32 height, const b3Color& color, bool depthEnabled = true)
{
	b3Vec3 vs[(H + 1) * (W + 1)];
	
	u32 h = b3Min(H + 1, height + 1);
	u32 w = b3Min(W + 1, width + 1);
	
	b3Vec3 grid_center;
	grid_center.x = scalar(0.5) * scalar(width);
	grid_center.y = scalar(0);
	grid_center.z = scalar(0.5) * scalar(height);
	
	for (u32 i = 0; i < h; ++i)
	{
		for (u32 j = 0; j < w; ++j)
		{
			u32 index = i * w + j;

			b3Vec3 v;
			v.x = scalar(j);
			v.y = scalar(0);
			v.z = scalar(i);

			v -= grid_center;
			
			vs[index] = v;
		}
	}

	b3Quat q = b3RotationBetween(b3Vec3_y, normal);
	
	b3Color borderColor(scalar(0), scalar(0), scalar(0), scalar(1));
	b3Color centerColor(scalar(0.8), scalar(0.8), scalar(0.8), scalar(1));
	
	// Left to right lines
	for (u32 i = 0; i < h; ++i)
	{
		u32 iv1 = i * w + 0;
		u32 iv2 = i * w + (w - 1);

		b3Vec3 v1 = b3Mul(q, vs[iv1]) + center;
		b3Vec3 v2 = b3Mul(q, vs[iv2]) + center;
		
		if (i == 0 || i == (h - 1))
		{
			draw->DrawLine(v1, v2, borderColor, depthEnabled);
			continue;
		}

		if (i == (h - 1) / 2)
		{
			draw->DrawLine(v1, v2, centerColor, depthEnabled);
			continue;
		}

		draw->DrawLine(v1, v2, color, depthEnabled);
	}

	// Up to bottom lines
	for (u32 j = 0; j < w; ++j)
	{
		u32 iv1 = 0 * w + j;
		u32 iv2 = (h - 1) * w + j;

		b3Vec3 v1 = b3Mul(q, vs[iv1]) + center;
		b3Vec3 v2 = b3Mul(q, vs[iv2]) + center;
		
		if (j == 0 || j == (w - 1))
		{
			draw->DrawLine(v1, v2, borderColor, depthEnabled);
			continue;
		}

		if (j == (w - 1) / 2)
		{
			draw->DrawLine(v1, v2, centerColor, depthEnabled);
			continue;
		}

		draw->DrawLine(v1, v2, color, depthEnabled);
	}
}

// Draw a plane.
inline void b3DrawPlane(b3DebugDraw* draw, const b3Vec3& normal, const b3Vec3& center, scalar radius, const b3Color& color, bool depthEnabled = true)
{
	b3Vec3 n1, n2;
	b3ComputeBasis(normal, n1, n2);

	scalar scale = scalar(2) * radius;

	b3Vec3 v1 = center - scale * (n1 - n2);
	b3Vec3 v2 = center + scale * (n1 - n2);
	b3Vec3 v3 = center + scale * (n1 + n2);
	b3Vec3 v4 = center - scale * (n1 + n2);

	draw->DrawLine(v1, v2, color, depthEnabled);
	draw->DrawLine(v2, v3, color, depthEnabled);
	draw->DrawLine(v3, v4, color, depthEnabled);
	draw->DrawLine(v4, v1, color, depthEnabled);
}

// Draw a solid plane.
inline void b3DrawSolidPlane(b3DebugDraw* draw, const b3Vec3& normal, const b3Vec3& center, scalar radius, const b3Color& color, bool depthEnabled = true)
{
	b3Vec3 n1, n2;
	b3ComputeBasis(normal, n1, n2);

	scalar scale = scalar(2) * radius;

	b3Vec3 v1 = center - scale * (n1 - n2);
	b3Vec3 v2 = center + scale * (n1 - n2);
	b3Vec3 v3 = center + scale * (n1 + n2);
	b3Vec3 v4 = center - scale * (n1 + n2);

	draw->DrawTriangle(normal, v1, v2, v3, color, depthEnabled);
	draw->DrawTriangle(normal, v3, v4, v1, color, depthEnabled);
}

#endif
