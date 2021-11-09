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

#ifndef BOX_EDGE_CONTACT_H
#define BOX_EDGE_CONTACT_H

class BoxEdgeContact : public Test
{
public:
	BoxEdgeContact()
	{
		m_box.SetExtents(1.0f, 2.0f, 1.0f);

		m_sA.m_hull = &m_box;
		m_xfA.translation.Set(1.500000, 1.000000, 0.000000);
		m_xfA.rotation.SetIdentity();

		m_sB.m_hull = &m_box;
		m_xfB.translation.Set(-1.29999995, 1.34999979, 0.000000000);
		m_xfB.rotation.Set(0.810514629, 0.342624813, 0.334119707, 0.337692767);
	}
	
	void Step()
	{
		b3ConvexCache cache;
		cache.simplexCache.count = 0;
		cache.featureCache.featurePair.state = b3SATCacheType::e_empty;

		b3Manifold manifold;
		manifold.Initialize();

		b3CollideHullAndHull(manifold, m_xfA, &m_sA, m_xfB, &m_sB, &cache, m_xfA, m_xfB);

		for (u32 i = 0; i < manifold.pointCount; ++i)
		{
			b3WorldManifold wm;
			wm.Initialize(&manifold, m_sA.m_radius, m_xfA, m_sB.m_radius, m_xfB);

			b3Vec3 pw = wm.points[i].point;

			b3DrawPoint(g_debugDrawData, pw, 4.0f, b3Color_green, false);
			b3DrawSegment(g_debugDrawData, pw, pw + wm.points[i].normal, b3Color_white, false);
		}

		m_sA.Draw(&m_draw, m_xfA, b3Color_black);
		m_sB.Draw(&m_draw, m_xfB, b3Color_black);

		m_sA.DrawSolid(&m_draw, m_xfA, b3Color(1.0f, 1.0f, 1.0f, 0.25f));
		m_sB.DrawSolid(&m_draw, m_xfB, b3Color(1.0f, 1.0f, 1.0f, 0.25f));

		DrawString(b3Color_white, "Left/Right/Up/Down Arrow - Translate shape");
		DrawString(b3Color_white, "X/Y/Z - Rotate shape");
	}

	void KeyDown(int key)
	{
		if (key == GLFW_KEY_LEFT)
		{
			m_xfB.translation.x -= 0.05f;
		}

		if (key == GLFW_KEY_RIGHT)
		{
			m_xfB.translation.x += 0.05f;
		}

		if (key == GLFW_KEY_UP)
		{
			m_xfB.translation.y += 0.05f;
		}

		if (key == GLFW_KEY_DOWN)
		{
			m_xfB.translation.y -= 0.05f;
		}

		if (key == GLFW_KEY_X)
		{
			b3Quat qx = b3QuatRotationX(0.05f * B3_PI);

			m_xfB.rotation = m_xfB.rotation * qx;
		}

		if (key == GLFW_KEY_Y)
		{
			b3Quat qy = b3QuatRotationY(0.05f * B3_PI);

			m_xfB.rotation = m_xfB.rotation * qy;
		}

		if (key == GLFW_KEY_Z)
		{
			b3Quat qz = b3QuatRotationZ(0.05f * B3_PI);

			m_xfB.rotation = m_xfB.rotation * qz;
		}
	}
	
	static Test* Create()
	{
		return new BoxEdgeContact();
	}

	b3BoxHull m_box;
	
	b3Transform m_xfA;
	b3HullShape m_sA;
	
	b3Transform m_xfB;
	b3HullShape m_sB;
};

#endif
