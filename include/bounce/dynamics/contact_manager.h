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

#ifndef B3_CONTACT_MANAGER_H
#define B3_CONTACT_MANAGER_H

#include <bounce/common/template/list.h>
#include <bounce/collision/broad_phase.h>
#include <bounce/dynamics/contacts/contact.h>

class b3Shape;
class b3ContactFilter;
class b3ContactListener;
class b3BlockAllocator;

// Contact delegator for b3World.
class b3ContactManager 
{
public:
	b3ContactManager();

	// The broad-phase callback.
	void AddPair(void* proxyDataA, void* proxyDataB);

	// Reference AABBs in some contacts need to be synchronized with the 
	// synchronized body transforms.
	void SynchronizeShapes();

	// Perform broad-phase collision detection.
	void FindNewContacts();
	
	// Perform narrow-phase collision detection.
	void UpdateContacts();

	b3Contact* Create(b3Shape* shapeA, b3Shape* shapeB);
	void Destroy(b3Contact* c);

	b3BroadPhase m_broadPhase;	
	b3List<b3Contact> m_contactList;
	b3ContactFilter* m_contactFilter;
	b3ContactListener* m_contactListener;
	b3BlockAllocator* m_allocator;
};

#endif