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

#ifndef WHEEL_TEST_H
#define WHEEL_TEST_H

class WheelTest : public Test
{
public:
	WheelTest()
	{
		{
			b3BodyDef bd;
			b3Body* ground = m_world.CreateBody(bd);

			b3HullShape hs;
			hs.m_hull = &m_groundHull;

			b3FixtureDef sd;
			sd.shape = &hs;

			ground->CreateFixture(sd);
		}

		m_chassisHull.SetExtents(2.0f, 0.5f, 5.0f);

		b3HullShape chassisShape;
		chassisShape.m_hull = &m_chassisHull;

		m_wheelHull.SetExtents(1.0f, 0.5f);

		b3HullShape wheelShape;
		wheelShape.m_hull = &m_wheelHull;

		// Chassis
		b3Body* chassis;
		{
			b3BodyDef bd;
			bd.type = e_dynamicBody;
			bd.position.Set(0.0f, 10.0f, 0.0f);

			chassis = m_world.CreateBody(bd);

			b3FixtureDef fd;
			fd.density = 0.1f;
			fd.friction = 0.3f;
			fd.shape = &chassisShape;

			chassis->CreateFixture(fd);
		}

		b3Quat orientation = b3QuatRotationZ(0.5f * B3_PI);

		b3Body* wheelLF;
		{
			b3BodyDef bd;
			bd.type = e_dynamicBody;
			bd.position.Set(-1.0f, 7.0f, 4.5f);
			bd.orientation = orientation;

			wheelLF = m_world.CreateBody(bd);

			b3FixtureDef fd;
			fd.shape = &wheelShape;
			fd.density = 0.1f;
			fd.friction = 1.0f;

			wheelLF->CreateFixture(fd);
		}

		{
			b3WheelJointDef def;
			def.Initialize(chassis, wheelLF, wheelLF->GetPosition(), b3Vec3_y, b3Vec3_x);
			def.motorSpeed = 0.25f * B3_PI;
			def.maxMotorTorque = 1000.0f;

			m_joint1 = (b3WheelJoint*)m_world.CreateJoint(def);
		}

		b3Body* wheelRF;
		{
			b3BodyDef bd;
			bd.type = e_dynamicBody;
			bd.position.Set(1.0f, 7.0, 4.5f);
			bd.orientation = orientation;

			wheelRF = m_world.CreateBody(bd);

			b3FixtureDef fd;
			fd.density = 0.1f;
			fd.friction = 1.0f;
			fd.shape = &wheelShape;

			wheelRF->CreateFixture(fd);
		}

		{
			b3WheelJointDef def;
			def.Initialize(chassis, wheelRF, wheelRF->GetPosition(), b3Vec3_y, b3Vec3_x);
			def.motorSpeed = 0.25f * B3_PI;
			def.maxMotorTorque = 1000.0f;

			m_joint2 = (b3WheelJoint*)m_world.CreateJoint(def);
		}

		b3Body* wheelLB;
		{
			b3BodyDef bd;
			bd.type = e_dynamicBody;
			bd.position.Set(-1.0f, 7.0f, -4.5f);
			bd.orientation = orientation;

			wheelLB = m_world.CreateBody(bd);

			b3FixtureDef fd;
			fd.shape = &wheelShape;
			fd.density = 0.1f;
			fd.friction = 1.0f;

			wheelLB->CreateFixture(fd);
		}

		{
			b3WheelJointDef def;
			def.Initialize(chassis, wheelLB, wheelLB->GetPosition(), b3Vec3_y, b3Vec3_x);

			m_world.CreateJoint(def);
		}

		b3Body* wheelRB;
		{
			b3BodyDef bd;
			bd.type = e_dynamicBody;
			bd.position.Set(1.0f, 7.0f, -4.5f);
			bd.orientation = orientation;

			wheelRB = m_world.CreateBody(bd);

			b3FixtureDef fd;
			fd.density = 0.1f;
			fd.friction = 1.0f;
			fd.shape = &wheelShape;

			wheelRB->CreateFixture(fd);
		}

		{
			b3WheelJointDef def;
			def.Initialize(chassis, wheelRB, wheelRB->GetPosition(), b3Vec3_y, b3Vec3_x);

			m_world.CreateJoint(def);
		}
	}

	void Step()
	{
		Test::Step();

		DrawString(b3Color_white, "M - Enable Motor");
		DrawString(b3Color_white, "S - Flip Motor Speed");
	}

	void KeyDown(int button)
	{
		if (button == GLFW_KEY_M)
		{
			m_joint1->EnableMotor(!m_joint1->IsMotorEnabled());
			m_joint2->EnableMotor(!m_joint2->IsMotorEnabled());
		}

		if (button == GLFW_KEY_S)
		{
			m_joint1->SetMotorSpeed(-m_joint1->GetMotorSpeed());
			m_joint2->SetMotorSpeed(-m_joint2->GetMotorSpeed());
		}
	}

	static Test* Create()
	{
		return new WheelTest();
	}

	b3BoxHull m_chassisHull;
	b3CylinderHull m_wheelHull;
	b3WheelJoint* m_joint1;
	b3WheelJoint* m_joint2;
};

#endif
