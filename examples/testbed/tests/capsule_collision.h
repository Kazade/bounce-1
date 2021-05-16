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

#ifndef CAPSULE_COLLISION_H
#define CAPSULE_COLLISION_H

class CapsuleCollision : public Collide
{
public:
	CapsuleCollision()
	{
		m_sA.m_vertex1.Set(0.0f, -5.0f, 0.0f);
		m_sA.m_vertex2.Set(0.0f, 5.0f, 0.0f);
		m_sA.m_radius = 1.0f;

		m_xfA.SetIdentity();
		
		m_sB.m_vertex1.Set(0.0f, -1.0f, 0.0f);
		m_sB.m_vertex2.Set(0.0f, 1.0f, 0.0f);
		m_sB.m_radius = 1.0f;

		m_xfB.SetIdentity();

		m_shapeA = &m_sA;
		m_shapeB = &m_sB;
		m_cache.count = 0;
	}

	static Test* Create()
	{
		return new CapsuleCollision();
	}

	b3CapsuleShape m_sA;
	b3CapsuleShape m_sB;
};

#endif
