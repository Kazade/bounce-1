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

#ifndef B3_TRANSFORM_H
#define B3_TRANSFORM_H

#include <bounce/common/math/mat33.h>
#include <bounce/common/math/mat44.h>
#include <bounce/common/math/quat.h>

// A transform represents a rigid frame. 
// It has a translation representing a position 
// and a rotation quaternion representing an orientation 
// relative to some reference frame.
struct b3Transform 
{
	// Default ctor does nothing for performance.
	b3Transform() { }

	// Set this transform from a translation vector and a rotation quaternion.
	b3Transform(const b3Vec3& _translation, const b3Quat& _rotation) : translation(_translation), rotation(_rotation) { }

	// Set this transform to the identity transform.
	void SetIdentity() 
	{
		translation.SetZero();
		rotation.SetIdentity();
	}

	// Convert this transform to a 4-by-4 transformation matrix.
	b3Mat44 GetTransformMatrix() const
	{
		b3Vec3 t = translation;
		b3Mat33 R = rotation.GetRotationMatrix();

		return b3Mat44(
			b3Vec4(R.x.x, R.x.y, R.x.z, scalar(0)),
			b3Vec4(R.y.x, R.y.y, R.y.z, scalar(0)),
			b3Vec4(R.z.x, R.z.y, R.z.z, scalar(0)),
			b3Vec4(t.x, t.y, t.z, scalar(1)));
	}

	b3Vec3 translation;
	b3Quat rotation;
};

// Identity transformation
extern const b3Transform b3Transform_identity;

// Multiply a transform times a vector.
inline b3Vec3 b3Mul(const b3Transform& T, const b3Vec3& v)
{
	return b3Mul(T.rotation, v) + T.translation;
}

// Multiply a transform times another transform.
inline b3Transform b3Mul(const b3Transform& A, const b3Transform& B)
{
	// [A y][B x] = [AB Ax+y]
	// [0 1][0 1]   [0  1   ]
	b3Transform C;
	C.rotation = b3Mul(A.rotation, B.rotation);
	C.translation = b3Mul(A.rotation, B.translation) + A.translation;
	return C;
}

// Multiply the transpose of one transform (inverse 
// transform) times another transform (composed transform).
inline b3Transform b3MulT(const b3Transform& A, const b3Transform& B) 
{
	//[A^-1  -A^-1*y][B x] = [A^-1*B A^-1(x-y)]
	//[0      1     ][0 1]   [0      1        ]
	b3Transform C;
	C.rotation = b3MulC(A.rotation, B.rotation);
	C.translation = b3MulC(A.rotation, B.translation - A.translation);
	return C;
}

// Multiply the transpose of a transform times a vector.
// If the transform represents a frame then this transforms
// the vector from one frame to another (inverse transform).
inline b3Vec3 b3MulT(const b3Transform& A, const b3Vec3& v)
{
	//[A^-1  -A^-1*y][x] = A^-1*x - A^-1*y = A^-1 * (x - y)
	//[0     1      ][1]   
	return b3MulC(A.rotation, v - A.translation);
}

// Inverse transform.
inline b3Transform b3Inverse(const b3Transform& T)
{
	b3Transform B;
	B.rotation = b3Conjugate(T.rotation);
	B.translation = b3MulC(T.rotation, -T.translation);
	return B;
}

// Multiply a transform times a vector. If the transform 
// represents a frame this returns the vector in terms 
// of the frame.
inline b3Vec3 operator*(const b3Transform& T, const b3Vec3& v)
{
	return b3Mul(T, v);
}

// Multiply a transform times another transform (composed transform).
inline b3Transform operator*(const b3Transform& A, const b3Transform& B)
{
	return b3Mul(A, B);
}

#endif
