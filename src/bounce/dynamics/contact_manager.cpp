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

#include <bounce/dynamics/contact_manager.h>
#include <bounce/dynamics/body.h>
#include <bounce/dynamics/fixture.h>
#include <bounce/dynamics/world_listeners.h>
#include <bounce/common/profiler.h>

b3ContactManager::b3ContactManager()
{
	m_contactListener = nullptr;
	m_contactFilter = nullptr;
}

void b3ContactManager::AddPair(void* dataA, void* dataB)
{
	b3Fixture* fixtureA = (b3Fixture*)dataA;
	b3Fixture* fixtureB = (b3Fixture*)dataB;

	b3Body* bodyA = fixtureA->GetBody();
	b3Body* bodyB = fixtureB->GetBody();

	if (bodyA == bodyB)
	{
		// Two fixtures that belong to the same body cannot collide.
		return;
	}

	// Check if there is a contact between the two fixtures.
	// Search the list A or B. The shorter if possible.
	for (b3ContactEdge* ce = fixtureB->m_contactEdges.m_head; ce; ce = ce->m_next)
	{
		if (ce->other == fixtureA)
		{
			b3Contact* c = ce->contact;

			b3Fixture* fA = c->GetFixtureA();
			b3Fixture* fB = c->GetFixtureB();

			if (fA == fixtureA && fB == fixtureB)
			{
				// A contact already exists.
				return;
			}

			if (fB == fixtureA && fA == fixtureB)
			{
				// A contact already exists.
				return;
			}
		}
	}

	// Is at least one of the bodies kinematic or dynamic? 
	// Does a joint prevent the collision?
	if (bodyA->ShouldCollide(bodyB) == false)
	{
		// The bodies must not collide with each other.
		return;
	}

	// Check if the contact filter prevents the collision.
	if (m_contactFilter)
	{
		if (m_contactFilter->ShouldCollide(fixtureA, fixtureB) == false)
		{
			return;
		}
	}

	// Create contact.
	b3Contact* c = Create(fixtureA, fixtureB);
	if (c == nullptr)
	{
		return;
	}

	// Get the fixtures from the contact again because contact creation can swap the fixtures.
	fixtureA = c->GetFixtureA();
	fixtureB = c->GetFixtureB();
	bodyA = fixtureA->GetBody();
	bodyB = fixtureB->GetBody();

	c->m_flags = 0;
	b3OverlappingPair* pair = &c->m_pair;

	// Initialize edge A
	pair->edgeA.contact = c;
	pair->edgeA.other = fixtureB;

	// Add edge A to fixture A's contact list.
	fixtureA->m_contactEdges.PushFront(&pair->edgeA);

	// Initialize edge B
	pair->edgeB.contact = c;
	pair->edgeB.other = fixtureA;

	// Add edge B to fixture B's contact list.
	fixtureB->m_contactEdges.PushFront(&pair->edgeB);

	// Awake the bodies if both are not sensors.
	if (!fixtureA->IsSensor() && !fixtureB->IsSensor())
	{
		bodyA->SetAwake(true);
		bodyB->SetAwake(true);
	}

	// Add the contact to the world contact list.
	m_contactList.PushFront(c);
}

void b3ContactManager::SynchronizeFixtures()
{
	b3Contact* c = m_contactList.m_head;
	while (c)
	{
		c->SynchronizeFixture();
		c = c->m_next;
	}
}

void b3ContactManager::FindNewContacts()
{
	m_broadPhase.FindPairs(this);

	b3Contact* c = m_contactList.m_head;
	while (c)
	{
		c->FindPairs();
		c = c->m_next;
	}
}

void b3ContactManager::UpdateContacts()
{
	B3_PROFILE(m_profiler, "Update Contacts");

	// Update the state of all contacts.
	b3Contact* c = m_contactList.m_head;
	while (c)
	{
		b3OverlappingPair* pair = &c->m_pair;

		b3Fixture* fixtureA = pair->fixtureA;
		u32 proxyA = fixtureA->m_broadPhaseID;
		b3Body* bodyA = fixtureA->m_body;

		b3Fixture* fixtureB = pair->fixtureB;
		u32 proxyB = fixtureB->m_broadPhaseID;
		b3Body* bodyB = fixtureB->m_body;

		// Check if the bodies must not collide with each other.
		if (bodyA->ShouldCollide(bodyB) == false)
		{
			b3Contact* quack = c;
			c = c->m_next;
			Destroy(quack);
			continue;
		}

		// Check for external filtering.
		if (m_contactFilter)
		{
			if (m_contactFilter->ShouldCollide(fixtureA, fixtureB) == false)
			{
				// The user has stopped the contact.
				b3Contact* quack = c;
				c = c->m_next;
				Destroy(quack);
				continue;
			}
		}

		// At least one body must be dynamic or kinematic.
		bool activeA = bodyA->IsAwake() && bodyA->m_type != e_staticBody;
		bool activeB = bodyB->IsAwake() && bodyB->m_type != e_staticBody;
		if (activeA == false && activeB == false)
		{
			c = c->m_next;
			continue;
		}

		// Destroy the contact if the shape AABBs are not overlapping.
		bool overlap = m_broadPhase.TestOverlap(proxyA, proxyB);
		if (overlap == false)
		{
			b3Contact* quack = c;
			c = c->m_next;
			Destroy(quack);
			continue;
		}

		// The contact persists.
		c->Update(m_contactListener);

		c = c->m_next;
	}
}

b3Contact* b3ContactManager::Create(b3Fixture* fixtureA, b3Fixture* fixtureB)
{
	return b3Contact::Create(fixtureA, fixtureB, m_allocator);
}

void b3ContactManager::Destroy(b3Contact* c)
{
	// Report to the contact listener the contact will be destroyed.
	if (m_contactListener)
	{
		if (c->IsOverlapping())
		{
			m_contactListener->EndContact(c);
		}
	}

	b3OverlappingPair* pair = &c->m_pair;

	b3Fixture* fixtureA = c->GetFixtureA();
	b3Fixture* fixtureB = c->GetFixtureB();

	fixtureA->m_contactEdges.Remove(&pair->edgeA);
	fixtureB->m_contactEdges.Remove(&pair->edgeB);

	// Remove the contact from the world contact list.
	m_contactList.Remove(c);

	// Free the contact.
	b3Contact::Destroy(c, m_allocator);
}