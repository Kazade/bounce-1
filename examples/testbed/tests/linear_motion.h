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

#ifndef LINEAR_MOTION_H
#define LINEAR_MOTION_H

class LinearMotion : public Test
{
public:
	LinearMotion()
	{
		b3BodyDef bd;
		bd.type = e_dynamicBody;
		bd.position.Set(0.0f, 0.0f, 0.0f);
		bd.fixedRotationX = true;
		bd.fixedRotationY = true;
		bd.fixedRotationZ = true;

		m_body = m_world.CreateBody(bd);

		b3CapsuleShape shape;
		shape.m_vertex1.Set(0.0f, 1.0f, 0.0f);
		shape.m_vertex2.Set(0.0f, -1.0f, 0.0f);
		shape.m_radius = 1.0f;

		b3FixtureDef fd;
		fd.shape = &shape;
		fd.density = 1.0f;

		m_body->CreateFixture(fd);

		b3Vec3 g(0.0f, 0.0f, 0.0f);
		m_world.SetGravity(g);

		b3Vec3 f(0.0f, 0.0f, -10000.0f);
		m_body->ApplyForceToCenter(f, true);
	}

	void Step()
	{
		Test::Step();

		b3Vec3 p(0.0f, 0.0f, 0.0f);
		if (b3Distance(m_body->GetPosition(), p) > 50.0f)
		{
			b3Quat q = m_body->GetOrientation();
			m_body->SetTransform(p, q);
		}
	}

	static Test* Create()
	{
		return new LinearMotion();
	}

	b3Body* m_body;
};

#endif