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

#ifndef B3_COLOR_H
#define B3_COLOR_H

#include <bounce/common/settings.h>

// Color channels.
struct b3Color
{
	// Default constructor does nothing for performance.
	b3Color() { }
	
	// Construct this color from four components.
	b3Color(scalar R, scalar G, scalar B, scalar A = scalar(1)) : r(R), g(G), b(B), a(A) { }
	
	// Set this color from four components.
	void Set(scalar R, scalar G, scalar B, scalar A)
	{
		r = R;
		g = G;
		b = B;
		a = A;
	}
	
	scalar r, g, b, a;
};

// Color pallete.
extern const b3Color b3Color_black;
extern const b3Color b3Color_white;
extern const b3Color b3Color_red;
extern const b3Color b3Color_green;
extern const b3Color b3Color_blue;
extern const b3Color b3Color_yellow;
extern const b3Color b3Color_pink;
extern const b3Color b3Color_gray;

#endif
