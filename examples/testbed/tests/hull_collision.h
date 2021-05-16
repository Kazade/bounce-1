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

#ifndef HULL_COLLISION_H
#define HULL_COLLISION_H

class HullCollision : public Collide
{
public:
	enum
	{
		e_count = 256
	};

	HullCollision()
	{
		m_xfA.translation.Set(0.0f, 1.5f, 0.0f);
		m_xfA.rotation.SetIdentity();

		m_xfB.SetIdentity();

		m_cache.count = 0;

		m_hull1 = nullptr;
		m_hull2 = nullptr;
		Generate();
	}

	~HullCollision()
	{
		if (m_hull1)
		{
			DestroyHull(m_hull1);
		}

		if (m_hull2)
		{
			DestroyHull(m_hull2);
		}
	}

	void KeyDown(int button)
	{
		if (button == GLFW_KEY_G)
		{
			Generate();
		}
		
		Collide::KeyDown(button);
	}

	void Generate()
	{
		if (m_hull1)
		{
			DestroyHull(m_hull1);
			m_hull1 = nullptr;
		}

		if (m_hull2)
		{
			DestroyHull(m_hull2);
			m_hull2 = nullptr;
		}

		b3Vec3 points1[e_count];
		for (u32 i = 0; i < e_count; ++i)
		{
			float x = 3.0f * RandomFloat(-1.0f, 1.0f);
			float y = 3.0f * RandomFloat(-1.0f, 1.0f);
			float z = 3.0f * RandomFloat(-1.0f, 1.0f);

			x = b3Clamp(x, -2.5f, 2.5f);
			y = b3Clamp(y, -2.5f, 2.5f);
			z = b3Clamp(z, -2.5f, 2.5f);

			b3Vec3 p(x, y, z);

			points1[i] = p;
		}

		m_hull1 = CreateHull(points1, e_count);
		
		b3Vec3 points2[e_count];
		for (u32 i = 0; i < e_count; ++i)
		{
			float x = 3.0f * RandomFloat(-1.0f, 1.0f);
			float y = 3.0f * RandomFloat(-1.0f, 1.0f);
			float z = 3.0f * RandomFloat(-1.0f, 1.0f);

			x = b3Clamp(x, -2.5f, 2.5f);
			y = b3Clamp(y, -2.5f, 2.5f);
			z = b3Clamp(z, -2.5f, 2.5f);

			b3Vec3 p(x, y, z);

			points2[i] = p;
		}

		m_hull2 = CreateHull(points2, e_count);
	}
	
	void Step()
	{
		DrawString(b3Color_white, "G - Generate random convex hulls");

		if (!m_hull1 || !m_hull2)
		{
			return;
		}

		b3HullShape sA;
		sA.m_hull = m_hull1;
		m_shapeA = &sA;

		b3HullShape sB;
		sB.m_hull = m_hull2;
		m_shapeB = &sB;

		Collide::Step();
	}

	static Test* Create()
	{
		return new HullCollision();
	}

	b3Hull* m_hull1;
	b3Hull* m_hull2;
};

#endif
