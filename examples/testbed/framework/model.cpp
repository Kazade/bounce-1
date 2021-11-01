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


#include "model.h"
#include "view_model.h"
#include "test.h"

b3Camera* g_camera = nullptr;
b3DebugDraw* g_debugDraw = nullptr;
b3Profiler* g_profiler = nullptr;

Model::Model() :
	m_debugDraw(512, 512, 512, &m_glDebugDraw),
	m_glDebugDraw(512, 512, 512)
{
	m_glDebugDraw.SetCamera(&m_camera);
	m_glDebugDraw.SetClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	
	m_test = nullptr;
	m_viewModel = nullptr;
	
	Action_ResetCamera();

	m_setTest = true;
	m_pause = true;
	m_singlePlay = false;

	g_camera = &m_camera;
	g_debugDraw = &m_debugDraw;
	g_profiler = &m_profiler;
}

Model::~Model()
{
	delete m_test;
	g_debugDraw = nullptr;
	g_camera = nullptr;
	g_profiler = nullptr;
}

void Model::Command_Press_Key(int button)
{
	m_test->KeyDown(button);
}

void Model::Command_Release_Key(int button)
{
	m_test->KeyUp(button);
}

static inline b3Ray ConvertScreenToWorldRay(const b3Camera& camera, const b3Vec2& ps)
{
	b3Vec3 pw = camera.ConvertScreenToWorld(b3Vec2(ps.x, ps.y));
	b3Vec3 cp = camera.BuildPosition();
	
	b3Ray rw;
	rw.origin = b3Vec3(cp.x, cp.y, cp.z);
	rw.direction = b3Vec3(pw.x, pw.y, pw.z);
	rw.fraction = camera.GetZFar();
	return rw;	
}

void Model::Command_Press_Mouse_Left(const b3Vec2& ps)
{
	b3Ray rw = ConvertScreenToWorldRay(m_camera, ps);
	
	m_test->MouseLeftDown(rw);
}

void Model::Command_Release_Mouse_Left(const b3Vec2& ps)
{
	b3Ray rw = ConvertScreenToWorldRay(m_camera, ps);
	
	m_test->MouseLeftUp(rw);
}

void Model::Command_Move_Cursor(const b3Vec2& ps)
{
	b3Ray rw = ConvertScreenToWorldRay(m_camera, ps);
	
	m_test->MouseMove(rw);
}

void Model::Update()
{
	m_debugDraw.EnableDrawPoints(g_settings->drawPoints);
	m_debugDraw.EnableDrawLines(g_settings->drawLines);
	m_debugDraw.EnableDrawTriangles(g_settings->drawTriangles);

	m_glDebugDraw.Begin();
	
	if (m_setTest)
	{
		Action_ResetCamera();
		
		delete m_test;
		
		m_test = g_settings->tests[g_settings->testID].create();
		
		m_setTest = false;
		m_pause = true;
	}
	
	if (g_settings->drawGrid)
	{
		b3DrawGrid<20, 20>(&m_debugDraw, b3Vec3_y, b3Vec3_zero, 20, 20, b3Color(0.4f, 0.4f, 0.4f, 1.0f));
	}

	if (m_pause)
	{
		if (m_singlePlay)
		{
			g_testSettings->inv_hertz = g_testSettings->hertz > 0.0f ? 1.0f / g_testSettings->hertz : 0.0f;
			m_singlePlay = false;
		}
		else
		{
			g_testSettings->inv_hertz = 0.0f;
		}
	}
	else
	{
		g_testSettings->inv_hertz = g_testSettings->hertz > 0.0f ? 1.0f / g_testSettings->hertz : 0.0f;
	}

	m_test->Step();
	
	m_glDebugDraw.End();
	
	m_debugDraw.Flush();
}
