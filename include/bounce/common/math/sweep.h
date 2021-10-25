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

#ifndef B3_SWEEP_H
#define B3_SWEEP_H

#include <bounce/common/math/transform.h>

// This is essentially a motion proxy for TOI computation.
// It is used to represent rigid body transforms at the previous 
// and current time step.
struct b3Sweep
{
	// Get this sweep transform at a given time in the interval [0, 1]
	b3Transform GetTransform(scalar t) const;

	b3Vec3 localCenter; // local center

	b3Quat orientation0; // last orientation
	b3Vec3 worldCenter0; // last world center

	scalar t0; // last fraction between [0, 1]

	b3Quat orientation; // world orientation
	b3Vec3 worldCenter; // world center
};

inline b3Transform b3Sweep::GetTransform(scalar t) const
{
	b3Vec3 c = (scalar(1) - t) * worldCenter0 + t * worldCenter;

	b3Quat q1 = orientation0;
	b3Quat q2 = orientation;

	if (b3Dot(q1, q2) < scalar(0))
	{
		q1 = -q1;
	}

	b3Quat q = (scalar(1) - t) * q1 + t * q2;
	q.Normalize();

	b3Transform xf;
	xf.translation = c - b3Mul(q, localCenter);
	xf.rotation = q;

	return xf;
}

#endif