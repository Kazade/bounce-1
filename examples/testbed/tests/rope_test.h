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

#ifndef ROPE_H
#define ROPE_H

#include <bounce/rope/rope.h>

class Rope : public Test
{
public:
	enum 
	{
		e_count = 10
	};

	Rope()
	{
		b3Vec3 vertices[e_count];
		scalar masses[e_count];
		
		vertices[0].SetZero();
		masses[0] = 0.0f;

		for (int i = 1; i < e_count; ++i)
		{
			vertices[i].x = scalar(i);
			vertices[i].y = 0.0f;
			vertices[i].z = 0.0f;

			masses[i] = 1.0f;
		}

		b3RopeDef def;
		def.count = e_count;
		def.vertices = vertices;
		def.masses = masses;
		def.linearDamping = 0.1f;
		def.angularDamping = 0.1f;

		m_rope = new b3Rope(def);

		m_rope->SetGravity(b3Vec3(0.0f, -10.0f, 0.0f));
	}

	~Rope()
	{
		delete m_rope;
	}

	void Step()
	{
		Test::Step();

		m_rope->Step(g_testSettings->inv_hertz);

		m_rope->Draw(&m_draw);
	}

	static Test* Create()
	{
		return new Rope();
	}

	void KeyDown(int button)
	{
		if (button == GLFW_KEY_UP)
		{
			m_rope->SetGravity(b3Vec3(0.0f, 10.0f, 0.0f));
		}
		
		if (button == GLFW_KEY_DOWN)
		{
			m_rope->SetGravity(b3Vec3(0.0f, -10.0f, 0.0f));
		}

		if (button == GLFW_KEY_LEFT)
		{
			m_rope->SetGravity(b3Vec3(-10.0f, 0.0f, 0.0f));
		}

		if (button == GLFW_KEY_RIGHT)
		{
			m_rope->SetGravity(b3Vec3(10.0f, 0.0f, 0.0f));
		}
	}

	b3Rope* m_rope;
};

#endif
