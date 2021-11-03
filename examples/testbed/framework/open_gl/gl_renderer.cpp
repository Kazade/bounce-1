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

#include "gl_renderer.h"
#include "glad/glad.h"
#include <bounce/common/graphics/camera.h>

GLRenderer::GLRenderer(int pointCapacity, int lineCapacity, int triangleCapacity) : 
	m_points(pointCapacity), 
	m_lines(lineCapacity), 
	m_triangles(triangleCapacity)
{
	m_camera = nullptr;
}

void GLRenderer::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void GLRenderer::SynchronizeViewport()
{
	glViewport(0, 0, GLsizei(m_camera->GetWidth()), GLsizei(m_camera->GetHeight()));
}

void GLRenderer::ClearBuffers()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderer::AddPoint(const b3Vec3& position, const b3Color& color, scalar size)
{
	m_points.PushVertex(position.x, position.y, position.z, color.r, color.g, color.b, color.a, size);
}

void GLRenderer::AddLine(const b3Vec3& p1, const b3Vec3& p2, const b3Color& color)
{
	m_lines.PushVertex(p1.x, p1.y, p1.z, color.r, color.g, color.b, color.a);
	m_lines.PushVertex(p2.x, p2.y, p2.z, color.r, color.g, color.b, color.a);
}

void GLRenderer::AddTriangle(const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color, const b3Vec3& normal)
{
	m_triangles.PushVertex(p1.x, p1.y, p1.z, color.r, color.g, color.b, color.a, normal.x, normal.y, normal.z);
	m_triangles.PushVertex(p2.x, p2.y, p2.z, color.r, color.g, color.b, color.a, normal.x, normal.y, normal.z);
	m_triangles.PushVertex(p3.x, p3.y, p3.z, color.r, color.g, color.b, color.a, normal.x, normal.y, normal.z);
}

void GLRenderer::FlushPoints(bool depthEnabled)
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

	m_points.SetMVP(&MVP.x.x);
	m_points.Flush();
}

void GLRenderer::FlushLines(bool depthEnabled)
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

	m_lines.SetMVP(&MVP.x.x);
	m_lines.Flush();
}

void GLRenderer::FlushTriangles(bool depthEnabled)
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

	m_triangles.SetMVP(&MVP.x.x);
	m_triangles.Flush();
}