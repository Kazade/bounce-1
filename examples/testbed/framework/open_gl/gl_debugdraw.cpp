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

#include "gl_debugdraw.h"

#include "glad/glad.h"

#include "gl_render_points.h"
#include "gl_render_lines.h"
#include "gl_render_triangles.h"

#include <bounce/common/graphics/camera.h>

GLDebugDraw::GLDebugDraw(int pointCapacity, int lineCapacity, int triangleCapacity)
{
	m_points = new GLRenderPoints(pointCapacity);
	m_lines = new GLRenderLines(lineCapacity);
	m_triangles = new GLRenderTriangles(triangleCapacity);

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glClearDepth(1.0f);
}

GLDebugDraw::~GLDebugDraw()
{
	delete m_points;
	delete m_lines;
	delete m_triangles;
}

void GLDebugDraw::Begin()
{
	glViewport(0, 0, GLsizei(m_camera->GetWidth()), GLsizei(m_camera->GetHeight()));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLDebugDraw::End()
{
}

void GLDebugDraw::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void GLDebugDraw::AddPoint(const b3Vec3& position, const b3Color& color, scalar size)
{
	m_points->PushVertex(position.x, position.y, position.z, color.r, color.g, color.b, color.a, size);
}

void GLDebugDraw::AddLine(const b3Vec3& p1, const b3Vec3& p2, const b3Color& color)
{
	m_lines->PushVertex(p1.x, p1.y, p1.z, color.r, color.g, color.b, color.a);
	m_lines->PushVertex(p2.x, p2.y, p2.z, color.r, color.g, color.b, color.a);
}

void GLDebugDraw::AddTriangle(const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color, const b3Vec3& normal)
{
	m_triangles->PushVertex(p1.x, p1.y, p1.z, color.r, color.g, color.b, color.a, normal.x, normal.y, normal.z);
	m_triangles->PushVertex(p2.x, p2.y, p2.z, color.r, color.g, color.b, color.a, normal.x, normal.y, normal.z);
	m_triangles->PushVertex(p3.x, p3.y, p3.z, color.r, color.g, color.b, color.a, normal.x, normal.y, normal.z);
}

void GLDebugDraw::FlushPoints(bool depthEnabled)
{
	if (depthEnabled)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	b3Mat44 P = m_camera->BuildProjectionMatrix();
	b3Mat44 V = m_camera->BuildViewMatrix();

	b3Mat44 MVP = P * V;

	m_points->SetMVP(&MVP.x.x);
	m_points->Flush();
}

void GLDebugDraw::FlushLines(bool depthEnabled)
{
	if (depthEnabled)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	b3Mat44 P = m_camera->BuildProjectionMatrix();
	b3Mat44 V = m_camera->BuildViewMatrix();

	b3Mat44 MVP = P * V;

	m_lines->SetMVP(&MVP.x.x);
	m_lines->Flush();
}

void GLDebugDraw::FlushTriangles(bool depthEnabled)
{
	if (depthEnabled)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	b3Mat44 P = m_camera->BuildProjectionMatrix();
	b3Mat44 V = m_camera->BuildViewMatrix();

	b3Mat44 MVP = P * V;

	m_triangles->SetMVP(&MVP.x.x);
	m_triangles->Flush();
}