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

#ifndef B3_WORLD_CALLBACKS_H
#define B3_WORLD_CALLBACKS_H

#include <bounce/common/math/math.h>

class b3Contact;
class b3Fixture;

class b3QueryListener 
{
public:
	virtual ~b3QueryListener() {}

	// Report to the contact listener that a fixture is overlapping 
	// the queried AABB.
	virtual bool ReportFixture(b3Fixture* fixture) = 0;
};

// Implement this class to provide query filtering.
class b3QueryFilter
{
public:
	virtual ~b3QueryFilter() { }

	// Return true if this fixture should be reported to the listener.
	virtual bool ShouldReport(b3Fixture* fixture) = 0;
};

class b3RayCastListener 
{
public:	
	// The user must return the new ray cast fraction.
	// If fraction equals zero then the ray cast query will be canceled immediately.
	virtual ~b3RayCastListener() { }

	// Report that a fixture was hit by the ray to this contact listener.
	// The reported information are the fixture hit by the ray,
	// the intersection point on the fixture, the surface normal associated with the point, and the 
	// intersection fraction for the ray.
	virtual scalar ReportFixture(b3Fixture* fixture, const b3Vec3& point, const b3Vec3& normal, scalar fraction) = 0;
};

// Implement this class to provide ray-cast filtering.
class b3RayCastFilter
{
public:
	virtual ~b3RayCastFilter() { }

	// Return true if ray-cast calculations should be performed on this fixture.
	virtual bool ShouldRayCast(b3Fixture* fixture) = 0;
};

// Implement this class to provide shape-cast filtering.
class b3ShapeCastFilter
{
public:
	virtual ~b3ShapeCastFilter() { }

	// Return true if shape-cast calculations should be performed on this fixture.
	virtual bool ShouldShapeCast(b3Fixture* fixture) = 0;
};

class b3ShapeCastListener
{
public:
	// The user must return the new shape cast fraction.
	// If fraction equals zero then the convex cast query will be canceled immediately.
	virtual ~b3ShapeCastListener() { }

	// Report that a fixture was hit by the ray to this contact listener.
	// The reported information are the fixture hit by the convex shape,
	// the intersection point on the fixture, the surface normal associated with the point, and the 
	// intersection fraction for the displacement vector.
	virtual scalar ReportFixture(b3Fixture* fixture, const b3Vec3& point, const b3Vec3& normal, scalar fraction) = 0;
};

// Inherit from this class and set it in the world to listen for collision events.	
// Call the functions below to inspect when a shape start/end colliding with another shape.
// @warning You cannot create/destroy Bounce objects inside these callbacks.
class b3ContactListener
{
public:
	virtual ~b3ContactListener() { }

	// Called when two shapes begin to overlap.
	virtual void BeginContact(b3Contact* contact) 
	{
		B3_NOT_USED(contact);
	}

	// Called when two shapes cease to overlap.
	virtual void EndContact(b3Contact* contact)
	{
		B3_NOT_USED(contact);
	}
	
	// Called after a dynamic contact is updated.
	virtual void PreSolve(b3Contact* contact)
	{
		B3_NOT_USED(contact);
	}

	// Called after a contact is solved.
	// This is usefull for inspecting impulses.
	virtual void PostSolve(b3Contact* contact)
	{
		B3_NOT_USED(contact);
	}
};

// By implementing this interface the contact filter will 
// be notified before a contact between two fixtures is created, updated, and solved.
class b3ContactFilter
{
public:
	virtual ~b3ContactFilter() { }

	// Should the two fixtures collide with each other?
	virtual bool ShouldCollide(b3Fixture* fixtureA, b3Fixture* fixtureB) = 0;

	// Should one or both fixtures respond to a contact with each other?
	virtual bool ShouldRespond(b3Fixture* fixtureA, b3Fixture* fixtureB)
	{
		B3_NOT_USED(fixtureA);
		B3_NOT_USED(fixtureB);
		return true;
	}
};

#endif