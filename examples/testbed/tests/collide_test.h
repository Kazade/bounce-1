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

#ifndef COLLIDE_H
#define COLLIDE_H

class Collide : public Test
{
public:
	Collide()
	{

	}

	void Step()
	{
		b3ConvexCache cache;
		cache.simplexCache.count = 0;
		cache.featureCache.m_featurePair.state = b3SATCacheType::e_empty;

		b3Manifold manifold;
		manifold.Initialize();

		b3CollideShapeAndShape(manifold, m_xfA, m_shapeA, m_xfB, m_shapeB, &cache);
		
		for (u32 i = 0; i < manifold.pointCount; ++i)
		{
			b3WorldManifold wm;
			wm.Initialize(&manifold, m_shapeA->m_radius, m_xfA, m_shapeB->m_radius, m_xfB);

			b3Vec3 pw = wm.points[i].point;
			
			b3DrawPoint(g_debugDraw, pw, 4.0f, b3Color_green, false);
			b3DrawSegment(g_debugDraw, pw, pw + wm.points[i].normal, b3Color_white, false);
		}

		m_shapeA->Draw(m_xfA, b3Color_black);
		m_shapeB->Draw(m_xfB, b3Color_black);

		m_shapeA->DrawSolid(m_xfA, b3Color(1.0f, 1.0f, 1.0f, 0.25f));
		m_shapeB->DrawSolid(m_xfB, b3Color(1.0f, 1.0f, 1.0f, 0.25f));

		DrawString(b3Color_white, "Left/Right/Up/Down Arrow - Translate shape");
		DrawString(b3Color_white, "X/Y/Z - Rotate shape");
	}

	virtual void KeyDown(int key)
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

	b3Shape* m_shapeA;
	b3Transform m_xfA;
	
	b3Shape* m_shapeB;
	b3Transform m_xfB;
	
	b3SimplexCache m_cache;
};

#endif
