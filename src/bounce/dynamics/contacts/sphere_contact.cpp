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

#include <bounce/dynamics/contacts/sphere_contact.h>
#include <bounce/dynamics/shapes/sphere_shape.h>
#include <bounce/dynamics/body.h>
#include <bounce/common/memory/block_allocator.h>

b3Contact* b3SphereContact::Create(b3Shape* shapeA, b3Shape* shapeB, b3BlockAllocator* allocator)
{
	void* mem = allocator->Allocate(sizeof(b3SphereContact));
	return new (mem) b3SphereContact(shapeA, shapeB);
}

void b3SphereContact::Destroy(b3Contact* contact, b3BlockAllocator* allocator)
{
	((b3SphereContact*)contact)->~b3SphereContact();
	allocator->Free(contact, sizeof(b3SphereContact));
}

b3SphereContact::b3SphereContact(b3Shape* shapeA, b3Shape* shapeB) : b3ConvexContact(shapeA, shapeB)
{
	B3_ASSERT(shapeA->GetType() == e_sphereShape);
	B3_ASSERT(shapeB->GetType() == e_sphereShape);
}

void b3SphereContact::Collide()
{
	b3Shape* shapeA = GetShapeA();
	b3Shape* shapeB = GetShapeB();

	b3Transform xfA = shapeA->GetBody()->GetTransform();
	b3Transform xfB = shapeB->GetBody()->GetTransform();

	b3SphereShape* sphereA = (b3SphereShape*)shapeA;
	b3SphereShape* sphereB = (b3SphereShape*)shapeB;

	B3_ASSERT(m_manifoldCount == 0);
	b3CollideSphereAndSphere(m_stackManifold, xfA, sphereA, xfB, sphereB);
	m_manifoldCount = 1;
}