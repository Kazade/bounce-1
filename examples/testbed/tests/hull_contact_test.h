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

#ifndef HULL_CONTACT_TEST_H
#define HULL_CONTACT_TEST_H

class HullContactTest : public Test
{
public:
	enum
	{
		e_count = 12,
		e_pointCount = 43
	};

	HullContactTest()
	{
		{
			b3BodyDef bd;
			bd.type = b3BodyType::e_staticBody;

			b3Body* body = m_world.CreateBody(bd);

			b3HullShape hs;
			hs.m_hull = &m_groundHull;

			b3FixtureDef fd;
			fd.shape = &hs;
			fd.friction = 1.0f;

			body->CreateFixture(fd);
		}

		m_count = 0;
		Generate();
	}

	~HullContactTest()
	{
		for (int i = 0; i < m_count; ++i)
		{
			DestroyHull(m_hulls[i]);
		}
	}

	void Generate()
	{
		if (m_bodyDragger.IsDragging())
		{
			m_bodyDragger.StopDragging();
		}

		for (int i = 0; i < m_count; ++i)
		{
			m_world.DestroyBody(m_bodies[i]);
			DestroyHull(m_hulls[i]);
		}
		m_count = 0;

		for (int i = 0; i < e_count; ++i)
		{
			b3Vec3 points[e_pointCount];
			for (int j = 0; j < e_pointCount; ++j)
			{
				// Clamp to force coplanarities.
				// This will stress the generation code.
				float x = 3.0f * RandomFloat(-1.0f, 1.0f);
				float y = 3.0f * RandomFloat(-1.0f, 1.0f);
				float z = 3.0f * RandomFloat(-1.0f, 1.0f);

				x = b3Clamp(x, -1.5f, 1.5f);
				y = b3Clamp(y, -1.5f, 1.5f);
				z = b3Clamp(z, -1.5f, 1.5f);

				b3Vec3 p(x, y, z);

				points[j] = p;
			}

			b3Hull* hull = CreateHull(points, e_pointCount);
			if (!hull)
			{
				continue;
			}
			m_hulls[m_count] = hull;

			b3BodyDef bd;
			bd.type = b3BodyType::e_dynamicBody;
			bd.position.Set(0.0f, 5.0f, 0.0f);

			b3Body* body = m_world.CreateBody(bd);
			m_bodies[m_count] = body;

			++m_count;

			b3HullShape hs;
			hs.m_hull = hull;

			b3FixtureDef fd;
			fd.density = 0.1f;
			fd.friction = 0.1f;
			fd.shape = &hs;

			body->CreateFixture(fd);
		}
	}

	void Step()
	{
		Test::Step();
		
		DrawString(b3Color_white, "G - Generate random convex hulls");
	}

	void KeyDown(int button)
	{
		if (button == GLFW_KEY_G)
		{
			Generate();
		}
	}

	static Test* Create()
	{
		return new HullContactTest();
	}

	int m_count;
	b3Hull* m_hulls[e_count];
	b3Body* m_bodies[e_count];
};

#endif
