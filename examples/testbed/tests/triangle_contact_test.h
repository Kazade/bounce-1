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

#ifndef TRIANGLE_CONTACT_TEST_H
#define TRIANGLE_CONTACT_TEST_H

class TriangleContactTest : public Test
{
public:
	TriangleContactTest()
	{
		{
			b3BodyDef bd;
			bd.type = b3BodyType::e_staticBody;

			b3Body* body = m_world.CreateBody(bd);

			b3TriangleShape ts;
			ts.m_vertex1.Set(-5.0f, 0.0f, 5.0f);
			ts.m_vertex2.Set(5.0f, 0.0f, 5.0f);
			ts.m_vertex3.Set(0.0f, 0.0f, -5.0f);

			b3FixtureDef fd;
			fd.shape = &ts;
			fd.friction = 1.0f;

			body->CreateFixture(fd);
		}

		{
			b3BodyDef bd;
			bd.type = b3BodyType::e_dynamicBody;
			bd.position.Set(0.0f, 5.0f, 0.0f);

			b3Body* body = m_world.CreateBody(bd);

			b3HullShape hs;
			hs.m_hull = &b3BoxHull_identity;

			b3FixtureDef fd;
			fd.density = 0.1f;
			fd.friction = 0.1f;
			fd.shape = &hs;

			body->CreateFixture(fd);
		}
	}

	static Test* Create()
	{
		return new TriangleContactTest();
	}
};

#endif