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

#ifndef TEST_H
#define TEST_H

#include "GLFW/glfw3.h"
#include "draw.h"
#include "view_model.h"
#include "body_dragger.h"
#include <bounce/bounce.h>
#include <bounce/common/profiler.h>
#include <bounce/common/graphics/camera.h>
#include <bounce/common/graphics/debugdraw.h>

extern b3Profiler* g_profiler;
extern b3Camera* g_camera;
extern b3DebugDrawData* g_debugDrawData;

b3Hull* CreateHull(const b3Vec3* vertices, int vertexCount);
void DestroyHull(b3Hull* hull);

float RandomFloat(float a, float b);

extern void DrawString(const b3Color& color, const b3Vec2& ps, const char* string, ...);
extern void DrawString(const b3Color& color, const b3Vec3& pw, const char* string, ...);
extern void DrawString(const b3Color& color, const char* string, ...);

class Test : public b3ContactListener
{
public:
	Test();
	virtual ~Test() { }
	
	virtual void Step();

	virtual void MouseMove(const b3Ray& pw);
	virtual void MouseLeftDown(const b3Ray& pw);
	virtual void MouseLeftUp(const b3Ray& pw);
	virtual void KeyDown(int button) { }
	virtual void KeyUp(int button) { }

	virtual void BeginDragging() { }
	virtual void EndDragging() { }

	void BeginContact(b3Contact* c) override { }
	void EndContact(b3Contact* c) override { }
	void PreSolve(b3Contact* c) override { }

	Draw m_draw;
	b3Ray m_ray;
	b3World m_world;
	b3BodyDragger m_bodyDragger;
	b3BoxHull m_groundHull;
	b3GridMesh<50, 50> m_groundMesh;
};

#endif
