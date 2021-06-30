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

#ifndef JENGA_H
#define JENGA_H

class Jenga : public Test
{
public:
	enum
	{
		e_layerCount = 20,
		e_depthCount = 3,
	};

	Jenga()
	{
		{
			b3BodyDef bd;
			b3Body* body = m_world.CreateBody(bd);

			b3HullShape hs;
			hs.m_hull = &m_groundHull;

			b3FixtureDef sd;
			sd.shape = &hs;

			body->CreateFixture(sd);
		}

		b3Vec3 boxScale(1.0f, 0.5f, 3.0f);

		static b3BoxHull boxHull(boxScale.x, boxScale.y, boxScale.z);

		scalar y = 2.0f;

		for (u32 i = 0; i < e_layerCount / 2; ++i)
		{
			for (u32 j = 0; j < e_depthCount; ++j)
			{
				b3BodyDef bd;
				bd.type = b3BodyType::e_dynamicBody;

				bd.position.x = 2.0f * scalar(j) * boxScale.x;
				bd.position.y = y;
				bd.position.z = 0.0f;
				
				b3Body* body = m_world.CreateBody(bd);

				b3HullShape hs;
				hs.m_hull = &boxHull;

				b3FixtureDef sd;
				sd.shape = &hs;
				sd.density = 0.1f;
				sd.friction = 0.3f;

				body->CreateFixture(sd);
			}

			y += 2.05f * boxScale.y;

			for (u32 j = 0; j < e_depthCount; ++j)
			{
				b3BodyDef bd;
				bd.type = b3BodyType::e_dynamicBody;

				bd.orientation = b3QuatRotationY(0.5f * B3_PI);

				bd.position.x = 2.0f * boxScale.x;
				bd.position.y = y;
				bd.position.z = -2.0f * boxScale.x + 2.0f * scalar(j) * boxScale.x;
				
				b3Body* body = m_world.CreateBody(bd);

				b3HullShape hs;
				hs.m_hull = &boxHull;

				b3FixtureDef sd;
				sd.shape = &hs;
				sd.density = 0.1f;
				sd.friction = 0.3f;

				body->CreateFixture(sd);
			}

			y += 2.05f * boxScale.y;
		}
	}

	static Test* Create()
	{
		return new Jenga();
	}
};

#endif