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

#ifndef TUMBLER_TEST_H
#define TUMBLER_TEST_H

class Tumbler : public Test
{
public:
	enum
	{
		e_count = 100
	};

	Tumbler()
	{
		{
			b3BodyDef bd;
			b3Body* ground = m_world.CreateBody(bd);

			bd.type = e_dynamicBody;
			b3Body* rotor = m_world.CreateBody(bd);

			{
				static b3BoxHull box;

				box.SetExtents(50.0f, 1.0f, 200.0f);
				
				b3Vec3 translation(0.0f, -45.0f, 0.0f);
				box.Translate(translation);

				b3HullShape hs;
				hs.m_hull = &box;

				b3FixtureDef sd;
				sd.density = 5.0f;
				sd.shape = &hs;

				rotor->CreateFixture(sd);
			}

			{
				static b3BoxHull box;

				box.SetExtents(50.0f, 1.0f, 200.0f);
				
				b3Vec3 translation(0.0f, 50.0f, 0.0f);
				box.Translate(translation);

				b3HullShape hs;
				hs.m_hull = &box;

				b3FixtureDef sd;
				sd.density = 5.0f;
				sd.shape = &hs;

				rotor->CreateFixture(sd);
			}

			{
				static b3BoxHull box;

				box.SetExtents(50.0f, 50.0f, 1.0f);
				
				b3Vec3 translation(0.0f, 5.0f, -200.0f);
				box.Translate(translation);

				b3HullShape hs;
				hs.m_hull = &box;

				b3FixtureDef sd;
				sd.density = 5.0f;
				sd.shape = &hs;

				rotor->CreateFixture(sd);
			}

			{
				static b3BoxHull box;

				box.SetExtents(50.0f, 50.0f, 1.0f);
				
				b3Vec3 translation(0.0f, 5.0f, 200.0f);
				box.Translate(translation);

				b3HullShape hs;
				hs.m_hull = &box;

				b3FixtureDef sd;
				sd.density = 5.0f;
				sd.shape = &hs;

				rotor->CreateFixture(sd);
			}

			{
				static b3BoxHull box;

				box.SetExtents(1.0f, 50.0f, 200.0f);
				
				b3Vec3 translation(-50.0f, 5.0f, 0.0f);
				box.Translate(translation);

				b3HullShape hs;
				hs.m_hull = &box;

				b3FixtureDef sd;
				sd.density = 5.0f;
				sd.shape = &hs;

				rotor->CreateFixture(sd);
			}

			{
				static b3BoxHull box;

				box.SetExtents(1.0f, 50.0f, 200.0f);
				
				b3Vec3 translation(50.0f, 5.0f, 0.0f);

				box.Translate(translation);

				b3HullShape hs;
				hs.m_hull = &box;

				b3FixtureDef sd;
				sd.density = 5.0f;
				sd.shape = &hs;

				rotor->CreateFixture(sd);
			}

			{
				b3RevoluteJointDef jd;
				jd.Initialize(ground, rotor, b3Vec3(0.0f, 0.0f, -1.0f), ground->GetPosition(), -B3_PI, B3_PI);
				jd.motorSpeed = 0.05f * B3_PI;
				jd.maxMotorTorque = 1000.0f * rotor->GetMass();
				jd.enableMotor = true;
				
				b3Joint* joint = (b3RevoluteJoint*)m_world.CreateJoint(jd);
			}
		}

		m_coneHull.SetExtents(1.0f, 1.0f);
		m_cylinderHull.SetExtents(1.0f, 1.0f);

		m_count = 0;
	}

	~Tumbler()
	{
	}

	void Step()
	{
		if(m_count < e_count)
		{
			++m_count;

			{
				b3BodyDef bd;
				bd.type = e_dynamicBody;
				bd.position.Set(-10.0f, 5.0f, 0.0f);

				b3Body* body = m_world.CreateBody(bd);

				b3SphereShape sphere;
				sphere.m_center.SetZero();
				sphere.m_radius = 1.0f;

				b3FixtureDef fd;
				fd.density = 1.0f;
				fd.friction = 0.3f;
				fd.shape = &sphere;

				body->CreateFixture(fd);
			}

			{
				b3BodyDef bd;
				bd.type = e_dynamicBody;
				bd.position.Set(-5.0f, 5.0f, 0.0f);

				b3Body* body = m_world.CreateBody(bd);

				b3CapsuleShape capsule;
				capsule.m_vertex1.Set(0.0f, 0.0f, -1.0f);
				capsule.m_vertex2.Set(0.0f, 0.0f, 1.0f);
				capsule.m_radius = 1.0f;

				b3FixtureDef fd;
				fd.density = 0.1f;
				fd.friction = 0.2f;
				fd.shape = &capsule;

				body->CreateFixture(fd);
			}

			{
				b3BodyDef bd;
				bd.type = e_dynamicBody;
				bd.position.Set(0.0f, 0.0f, 0.0f);
				bd.angularVelocity.Set(0.0f, 0.05f * B3_PI, 0.0f);
				b3Body* body = m_world.CreateBody(bd);

				b3HullShape hs;
				hs.m_hull = &b3BoxHull_identity;

				b3FixtureDef sd;
				sd.density = 0.05f;
				sd.shape = &hs;

				body->CreateFixture(sd);
			}

			{
				b3BodyDef bd;
				bd.type = e_dynamicBody;
				bd.position.Set(0.0f, 5.0f, 0.0f);

				b3Body* body = m_world.CreateBody(bd);

				b3HullShape hull;
				hull.m_hull = &m_coneHull;

				b3FixtureDef fd;
				fd.density = 1.0f;
				fd.friction = 0.3f;
				fd.shape = &hull;

				body->CreateFixture(fd);
			}

			{
				b3BodyDef bd;
				bd.type = e_dynamicBody;
				bd.position.Set(4.0f, 5.0f, 0.0f);

				b3Body* body = m_world.CreateBody(bd);

				b3HullShape hull;
				hull.m_hull = &m_cylinderHull;

				b3FixtureDef fd;
				fd.density = 1.0f;
				fd.friction = 0.2f;
				fd.shape = &hull;

				body->CreateFixture(fd);
			}
		}

		Test::Step();
	}

	static Test* Create()
	{
		return new Tumbler();
	}

	u32 m_count;
	b3ConeHull m_coneHull;
	b3CylinderHull m_cylinderHull;
};

#endif