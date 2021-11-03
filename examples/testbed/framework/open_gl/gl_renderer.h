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

#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include <bounce/common/graphics/debug_primitives.h>
#include "gl_render_points.h"
#include "gl_render_lines.h"
#include "gl_render_triangles.h"

class b3Camera;

class GLRenderer : public b3DebugRenderer
{
public:
	GLRenderer(int pointCapacity, int lineCapacity, int triangleCapacity);

	void SetClearColor(float r, float g, float b, float a);
	void SynchronizeViewport();
	void ClearBuffers();
	
	void AddPoint(const b3Vec3& position, const b3Color& color, scalar size) override;
	void AddLine(const b3Vec3& p1, const b3Vec3& p2, const b3Color& color) override;
	void AddTriangle(const b3Vec3& p1, const b3Vec3& p2, const b3Vec3& p3, const b3Color& color, const b3Vec3& normal) override;
	
	void FlushPoints(bool depthEnabled) override;
	void FlushLines(bool depthEnabled) override;
	void FlushTriangles(bool depthEnabled) override;

	void SetCamera(b3Camera* camera) { m_camera = camera; }
	b3Camera* GetCamera() { return m_camera; };
protected:
	GLRenderPoints m_points;
	GLRenderLines m_lines;
	GLRenderTriangles m_triangles;
	b3Camera* m_camera;
};

#endif
