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

#ifndef B3_CONTACT_H
#define B3_CONTACT_H

#include <bounce/common/math/math.h>
#include <bounce/common/template/list.h>
#include <bounce/common/template/array.h>
#include <bounce/dynamics/shapes/shape.h>
#include <bounce/dynamics/contacts/manifold.h>

class b3BlockPool;

class b3Shape;
class b3Body;
class b3Contact;
class b3ContactListener;
class b3BlockAllocator;
struct b3ConvexCache;

// A contact edge for the contact graph, 
// where a shape is a vertex and a contact 
// an edge.
struct b3ContactEdge
{
	b3Shape* other;
	b3Contact* contact;
	// Links to the contact edge list.
	b3ContactEdge* m_prev;
	b3ContactEdge* m_next;
};

// This goes inside a contact.
// It holds two shapes that are overlapping.
struct b3OverlappingPair
{
	// To the shape A edge list.
	b3Shape* shapeA;
	b3ContactEdge edgeA;
	// To the shape B edge list.
	b3Shape* shapeB;
	b3ContactEdge edgeB;
};

enum b3ContactType
{
	e_convexContact,
	e_meshContact,
	e_maxContact
};

typedef b3Contact* b3ContactCreateFcn(b3Shape* shapeA, b3Shape* shapeB, b3BlockAllocator* allocator);
typedef void b3ContactDestroyFcn(b3Contact* contact, b3BlockAllocator* allocator);
typedef void b3ContactCollideFcn(b3Contact* contact);

struct b3ContactRegister
{
	b3ContactCreateFcn* createFcn = nullptr;
	b3ContactDestroyFcn* destroyFcn = nullptr;
	b3ContactCollideFcn* collideFcn = nullptr;
	bool primary;
};

class b3Contact
{
public:
	// Get the contact type.
	b3ContactType GetType() const;

	// Get the shape A in this contact.
	const b3Shape* GetShapeA() const;
	b3Shape* GetShapeA();

	// Get the shape B in this contact.
	const b3Shape* GetShapeB() const;
	b3Shape* GetShapeB();

	// Get the manifold capacity from this contact.
	u32 GetManifoldCapacity() const;
	
	// Get a contact manifold from this contact.
	const b3Manifold* GetManifold(u32 index) const;
	b3Manifold* GetManifold(u32 index);

	// Get the number of manifolds from this contact.
	u32 GetManifoldCount() const;

	// Get a world contact manifold from this contact.
	void GetWorldManifold(b3WorldManifold* out, u32 index) const;
	
	// Are the shapes in this contact overlapping?
	bool IsOverlapping() const;

	// Has this contact at least one sensor shape?
	bool IsSensorContact() const;

	// Has this contact at least one dynamic body?
	bool HasDynamicBody() const;

	// Get the next contact in the world contact list.
	const b3Contact* GetNext() const;
	b3Contact* GetNext();
protected:
	friend class b3World;
	friend class b3Island;
	friend class b3Shape;
	friend class b3ContactManager;
	friend class b3ContactSolver;
	friend class b3List<b3Contact>;

	enum b3ContactFlags 
	{
		e_overlapFlag = 0x0001,
		e_islandFlag = 0x0002,
	};

	b3Contact(b3Shape* shapeA, b3Shape* shapeB);
	virtual ~b3Contact() { }

	static b3ContactRegister s_registers[e_maxShapes][e_maxShapes];
	static bool s_initialized;
	
	static void AddType(b3ContactCreateFcn* createFcn, b3ContactDestroyFcn* destoryFcn, b3ContactCollideFcn* collideFcn,
		b3ShapeType type1, b3ShapeType type2);
	
	static void InitializeRegisters();

	// Factory create.
	static b3Contact* Create(b3Shape* shapeA, b3Shape* shapeB, b3BlockAllocator* allocator);
	
	// Factory destroy.
	static void Destroy(b3Contact* contact, b3BlockAllocator* allocator);

	// Collide function.
	static void Collide(b3Contact* contact);

	// Primary collide functions.
	static void CollideSphereAndSphereShapes(b3Contact* contact);
	static void CollideCapsuleAndSphereShapes(b3Contact* contact);
	static void CollideCapsuleAndCapsuleShapes(b3Contact* contact);
	static void CollideTriangleAndSphereShapes(b3Contact* contact);
	static void CollideTriangleAndCapsuleShapes(b3Contact* contact);
	static void CollideTriangleAndHullShapes(b3Contact* contact);
	static void CollideHullAndSphereShapes(b3Contact* contact);
	static void CollideHullAndCapsuleShapes(b3Contact* contact);
	static void CollideHullAndHullShapes(b3Contact* contact);
	static void CollideMeshAndSphereShapes(b3Contact* contact);
	static void CollideMeshAndCapsuleShapes(b3Contact* contact);
	static void CollideMeshAndHullShapes(b3Contact* contact);

	// Update the contact state.
	void Update(b3ContactListener* listener);

	// Test if the shapes in this contact are overlapping.
	virtual bool TestOverlap() = 0;

	// Some contacts store reference AABBs for internal queries and therefore 
	// need to synchronize with body transforms.
	virtual void SynchronizeShape() = 0;

	// Some contacts act like a midphase and therefore need to find 
	// new internal overlapping pairs.
	virtual void FindPairs() = 0;

	b3ContactType m_type;
	u32 m_flags;
	b3OverlappingPair m_pair;

	// Contact manifolds.
	u32 m_manifoldCapacity;
	b3Manifold* m_manifolds;
	u32 m_manifoldCount;

	// Links to the world contact list.
	b3Contact* m_prev;
	b3Contact* m_next;
};

inline b3ContactType b3Contact::GetType() const
{
	return m_type;
}

inline b3Shape* b3Contact::GetShapeA() 
{
	return m_pair.shapeA;
}

inline const b3Shape* b3Contact::GetShapeA() const 
{
	return m_pair.shapeA;
}

inline b3Shape* b3Contact::GetShapeB() 
{
	return m_pair.shapeB;
}

inline const b3Shape* b3Contact::GetShapeB() const 
{
	return m_pair.shapeB;
}

inline u32 b3Contact::GetManifoldCapacity() const
{
	return m_manifoldCapacity;
}

inline const b3Manifold* b3Contact::GetManifold(u32 index) const
{
	B3_ASSERT(index < m_manifoldCount);
	return m_manifolds + index;
}

inline b3Manifold* b3Contact::GetManifold(u32 index)
{
	B3_ASSERT(index < m_manifoldCount);
	return m_manifolds + index;
}

inline u32 b3Contact::GetManifoldCount() const
{
	return m_manifoldCount;
}

inline bool b3Contact::IsOverlapping() const 
{
	return (m_flags & e_overlapFlag) != 0;
}

inline const b3Contact* b3Contact::GetNext() const
{
	return m_next;
}

inline b3Contact* b3Contact::GetNext()
{
	return m_next;
}

#endif