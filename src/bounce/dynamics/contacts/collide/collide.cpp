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

#include <bounce/dynamics/contacts/collide/collide.h>
#include <bounce/dynamics/contacts/convex_contact.h>
#include <bounce/dynamics/contacts/mesh_contact.h>
#include <bounce/dynamics/contacts/contact_cluster.h>
#include <bounce/dynamics/shapes/sphere_shape.h>
#include <bounce/dynamics/shapes/capsule_shape.h>
#include <bounce/dynamics/shapes/triangle_shape.h>
#include <bounce/dynamics/shapes/hull_shape.h>
#include <bounce/dynamics/shapes/mesh_shape.h>
#include <bounce/dynamics/body.h>
#include <bounce/dynamics/world.h>
#include <bounce/collision/shapes/sphere.h>
#include <bounce/collision/shapes/capsule.h>
#include <bounce/collision/shapes/hull.h>
#include <bounce/collision/shapes/mesh.h>
#include <bounce/collision/collision.h>

void b3ShapeGJKProxy::Set(const b3Shape* shape, u32 index)
{
	switch (shape->GetType())
	{
	case e_sphereShape:
	{
		const b3SphereShape* sphere = (b3SphereShape*)shape;
		vertexCount = 1;
		vertices = &sphere->m_center;
		radius = sphere->m_radius;
		break;
	}
	case e_capsuleShape:
	{
		const b3CapsuleShape* capsule = (b3CapsuleShape*)shape;
		vertexCount = 2;
		vertices = &capsule->m_vertex1;
		radius = capsule->m_radius;
		break;
	}
	case e_triangleShape:
	{
		const b3TriangleShape* triangle = (b3TriangleShape*)shape;
		vertexCount = 3;
		vertices = &triangle->m_vertex1;
		radius = triangle->m_radius;
		break;
	}
	case e_hullShape:
	{
		const b3HullShape* hull = (b3HullShape*)shape;
		vertexCount = hull->m_hull->vertexCount;
		vertices = hull->m_hull->vertices;
		radius = hull->m_radius;
		break;
	}
	case e_meshShape:
	{
		const b3MeshShape* mesh = (b3MeshShape*)shape;

		B3_ASSERT(index >= 0);
		B3_ASSERT(index < mesh->m_mesh->triangleCount);

		const b3MeshTriangle* triangle = mesh->m_mesh->GetTriangle(index);

		vertexBuffer[0] = b3Mul(mesh->m_scale, mesh->m_mesh->vertices[triangle->v1]);
		vertexBuffer[1] = b3Mul(mesh->m_scale, mesh->m_mesh->vertices[triangle->v2]);
		vertexBuffer[2] = b3Mul(mesh->m_scale, mesh->m_mesh->vertices[triangle->v3]);

		vertexCount = 3;
		vertices = vertexBuffer;
		radius = mesh->m_radius;
		break;
	}
	default:
	{
		B3_ASSERT(false);
		break;
	}
	}
}

bool b3TestOverlap(const b3Transform& xfA, u32 indexA, const b3Shape* shapeA,
	const b3Transform& xfB, u32 indexB, const b3Shape* shapeB,
	b3ConvexCache* cache)
{
	b3ShapeGJKProxy proxyA(shapeA, indexA);
	b3ShapeGJKProxy proxyB(shapeB, indexB);
	
	b3GJKOutput distance = b3GJK(xfA, proxyA, xfB, proxyB, true, &cache->simplexCache);

	const scalar kTol = scalar(10) * B3_EPSILON;
	return distance.distance <= kTol;
}

void b3Contact::CollideSphereAndSphereShapes(b3Contact* contact)
{
	b3ConvexContact* convexContact = (b3ConvexContact*)contact;

	b3Shape* shapeA = contact->GetShapeA();
	b3Shape* shapeB = contact->GetShapeB();

	b3Transform xfA = shapeA->GetBody()->GetTransform();
	b3Transform xfB = shapeB->GetBody()->GetTransform();

	b3SphereShape* sphereA = (b3SphereShape*)shapeA;
	b3SphereShape* sphereB = (b3SphereShape*)shapeB;
	
	B3_ASSERT(contact->m_manifoldCount == 0);
	b3CollideSphereAndSphere(convexContact->m_stackManifold, xfA, sphereA, xfB, sphereB);
	contact->m_manifoldCount = 1;
}

void b3Contact::CollideCapsuleAndSphereShapes(b3Contact* contact)
{
	b3ConvexContact* convexContact = (b3ConvexContact*)contact;

	b3Shape* shapeA = contact->GetShapeA();
	b3Shape* shapeB = contact->GetShapeB();

	b3Transform xfA = shapeA->GetBody()->GetTransform();
	b3Transform xfB = shapeB->GetBody()->GetTransform();
	
	b3CapsuleShape* capsuleA = (b3CapsuleShape*)shapeA;
	b3SphereShape* sphereB = (b3SphereShape*)shapeB;
	
	B3_ASSERT(contact->m_manifoldCount == 0);
	b3CollideCapsuleAndSphere(convexContact->m_stackManifold, xfA, capsuleA, xfB, sphereB);
	contact->m_manifoldCount = 1;
}

void b3Contact::CollideCapsuleAndCapsuleShapes(b3Contact* contact)
{
	b3ConvexContact* convexContact = (b3ConvexContact*)contact;

	b3Shape* shapeA = contact->GetShapeA();
	b3Shape* shapeB = contact->GetShapeB();

	b3Transform xfA = shapeA->GetBody()->GetTransform();
	b3Transform xfB = shapeB->GetBody()->GetTransform();

	b3CapsuleShape* capsuleA = (b3CapsuleShape*)shapeA;
	b3CapsuleShape* capsuleB = (b3CapsuleShape*)shapeB;
	
	B3_ASSERT(contact->m_manifoldCount == 0);
	b3CollideCapsuleAndCapsule(convexContact->m_stackManifold, xfA, capsuleA, xfB, capsuleB);
	contact->m_manifoldCount = 1;
}

void b3Contact::CollideTriangleAndSphereShapes(b3Contact* contact)
{
	b3ConvexContact* convexContact = (b3ConvexContact*)contact;

	b3Shape* shapeA = contact->GetShapeA();
	b3Shape* shapeB = contact->GetShapeB();

	b3Transform xfA = shapeA->GetBody()->GetTransform();
	b3Transform xfB = shapeB->GetBody()->GetTransform();
	
	b3TriangleShape* triangleA = (b3TriangleShape*)shapeA;
	b3SphereShape* sphereB = (b3SphereShape*)shapeB;

	B3_ASSERT(contact->m_manifoldCount == 0);
	b3CollideTriangleAndSphere(convexContact->m_stackManifold, xfA, triangleA, xfB, sphereB);
	contact->m_manifoldCount = 1;
}

void b3Contact::CollideTriangleAndCapsuleShapes(b3Contact* contact)
{
	b3ConvexContact* convexContact = (b3ConvexContact*)contact;

	b3Shape* shapeA = contact->GetShapeA();
	b3Shape* shapeB = contact->GetShapeB();

	b3Transform xfA = shapeA->GetBody()->GetTransform();
	b3Transform xfB = shapeB->GetBody()->GetTransform();
	
	b3TriangleShape* triangleA = (b3TriangleShape*)shapeA;
	b3CapsuleShape* capsuleB = (b3CapsuleShape*)shapeB;
	
	B3_ASSERT(contact->m_manifoldCount == 0);
	b3CollideTriangleAndCapsule(convexContact->m_stackManifold, xfA, triangleA, xfB, capsuleB);
	contact->m_manifoldCount = 1;
}

void b3Contact::CollideTriangleAndHullShapes(b3Contact* contact)
{
	b3ConvexContact* convexContact = (b3ConvexContact*)contact;

	b3Shape* shapeA = contact->GetShapeA();
	b3Shape* shapeB = contact->GetShapeB();

	b3Transform xfA = shapeA->GetBody()->GetTransform();
	b3Transform xfB = shapeB->GetBody()->GetTransform();

	b3TriangleShape* triangleA = (b3TriangleShape*)shapeA;
	b3HullShape* hullB = (b3HullShape*)shapeB;
	
	B3_ASSERT(contact->m_manifoldCount == 0);
	b3CollideTriangleAndHull(convexContact->m_stackManifold, xfA, triangleA, xfB, hullB, &convexContact->m_cache);
	contact->m_manifoldCount = 1;
}

void b3Contact::CollideHullAndSphereShapes(b3Contact* contact)
{
	b3ConvexContact* convexContact = (b3ConvexContact*)contact;

	b3Shape* shapeA = contact->GetShapeA();
	b3Shape* shapeB = contact->GetShapeB();

	b3Transform xfA = shapeA->GetBody()->GetTransform();
	b3Transform xfB = shapeB->GetBody()->GetTransform();
	
	b3HullShape* hullA = (b3HullShape*)shapeA;
	b3SphereShape* sphereB = (b3SphereShape*)shapeB;
	
	B3_ASSERT(contact->m_manifoldCount == 0);
	b3CollideHullAndSphere(convexContact->m_stackManifold, xfA, hullA, xfB, sphereB);
	contact->m_manifoldCount = 1;
}

void b3Contact::CollideHullAndCapsuleShapes(b3Contact* contact)
{
	b3ConvexContact* convexContact = (b3ConvexContact*)contact;

	b3Shape* shapeA = contact->GetShapeA();
	b3Shape* shapeB = contact->GetShapeB();

	b3Transform xfA = shapeA->GetBody()->GetTransform();
	b3Transform xfB = shapeB->GetBody()->GetTransform();
	
	b3HullShape* hullA = (b3HullShape*)shapeA;
	b3CapsuleShape* capsuleB = (b3CapsuleShape*)shapeB;
	
	B3_ASSERT(contact->m_manifoldCount == 0);
	b3CollideHullAndCapsule(convexContact->m_stackManifold, xfA, hullA, xfB, capsuleB);
	contact->m_manifoldCount = 1;
}

void b3Contact::CollideHullAndHullShapes(b3Contact* contact)
{
	b3ConvexContact* convexContact = (b3ConvexContact*)contact;

	b3Shape* shapeA = contact->GetShapeA();
	b3Shape* shapeB = contact->GetShapeB();

	b3Transform xfA = shapeA->GetBody()->GetTransform();
	b3Transform xfB = shapeB->GetBody()->GetTransform();

	b3HullShape* hullA = (b3HullShape*)shapeA;
	b3HullShape* hullB = (b3HullShape*)shapeB;
	
	B3_ASSERT(contact->m_manifoldCount == 0);
	b3CollideHullAndHull(convexContact->m_stackManifold, xfA, hullA, xfB, hullB, &convexContact->m_cache);
	contact->m_manifoldCount = 1;
}

void b3Contact::CollideMeshAndSphereShapes(b3Contact* contact)
{
	b3MeshContact* meshContact = (b3MeshContact*)contact;
	
	b3Shape* shapeA = contact->GetShapeA();
	b3Shape* shapeB = contact->GetShapeB();

	b3Transform xfA = shapeA->GetBody()->GetTransform();
	b3Transform xfB = shapeB->GetBody()->GetTransform();

	b3MeshShape* meshShapeA = (b3MeshShape*)shapeA;
	b3SphereShape* sphereB = (b3SphereShape*)shapeB;

	b3StackAllocator* allocator = &shapeA->GetBody()->GetWorld()->m_stackAllocator;

	// Create one temporary manifold per overlapping triangle.
	b3Manifold* tempManifolds = (b3Manifold*)allocator->Allocate(meshContact->m_triangleCount * sizeof(b3Manifold));
	u32 tempCount = 0;

	const b3Mesh* meshA = meshShapeA->m_mesh;
	for (u32 i = 0; i < meshContact->m_triangleCount; ++i)
	{
		b3TriangleCache* triangleCache = meshContact->m_triangles + i;
		u32 triangleIndex = triangleCache->index;

		b3TriangleShape triangleShapeA;
		meshShapeA->GetChildTriangle(&triangleShapeA, triangleIndex);

		b3Manifold* manifold = tempManifolds + tempCount;
		manifold->Initialize();

		b3CollideTriangleAndSphere(*manifold, xfA, &triangleShapeA, xfB, sphereB);

		for (u32 j = 0; j < manifold->pointCount; ++j)
		{
			manifold->points[j].key.triangleKey = triangleIndex;
		}

		++tempCount;
	}

	// Send contact manifolds for clustering. This is an important optimization.
	B3_ASSERT(contact->m_manifoldCount == 0);

	b3ClusterSolver clusterSolver;
	clusterSolver.Run(meshContact->m_stackManifolds, contact->m_manifoldCount, tempManifolds, tempCount, xfA, B3_HULL_RADIUS, xfB, shapeB->m_radius);

	allocator->Free(tempManifolds);
}

void b3Contact::CollideMeshAndCapsuleShapes(b3Contact* contact)
{
	b3MeshContact* meshContact = (b3MeshContact*)contact;

	b3Shape* shapeA = contact->GetShapeA();
	b3Shape* shapeB = contact->GetShapeB();

	b3Transform xfA = shapeA->GetBody()->GetTransform();
	b3Transform xfB = shapeB->GetBody()->GetTransform();

	b3MeshShape* meshShapeA = (b3MeshShape*)shapeA;
	b3CapsuleShape* capsuleB = (b3CapsuleShape*)shapeB;

	b3StackAllocator* allocator = &shapeA->GetBody()->GetWorld()->m_stackAllocator;

	// Create one temporary manifold per overlapping triangle.
	b3Manifold* tempManifolds = (b3Manifold*)allocator->Allocate(meshContact->m_triangleCount * sizeof(b3Manifold));
	u32 tempCount = 0;

	const b3Mesh* meshA = meshShapeA->m_mesh;
	for (u32 i = 0; i < meshContact->m_triangleCount; ++i)
	{
		b3TriangleCache* triangleCache = meshContact->m_triangles + i;
		u32 triangleIndex = triangleCache->index;

		b3TriangleShape triangleShapeA;
		meshShapeA->GetChildTriangle(&triangleShapeA, triangleIndex);

		b3Manifold* manifold = tempManifolds + tempCount;
		manifold->Initialize();

		b3CollideTriangleAndCapsule(*manifold, xfA, &triangleShapeA, xfB, capsuleB);

		for (u32 j = 0; j < manifold->pointCount; ++j)
		{
			manifold->points[j].key.triangleKey = triangleIndex;
		}

		++tempCount;
	}

	// Send contact manifolds for clustering. This is an important optimization.
	B3_ASSERT(contact->m_manifoldCount == 0);

	b3ClusterSolver clusterSolver;
	clusterSolver.Run(meshContact->m_stackManifolds, contact->m_manifoldCount, tempManifolds, tempCount, xfA, B3_HULL_RADIUS, xfB, shapeB->m_radius);

	allocator->Free(tempManifolds);
}

void b3Contact::CollideMeshAndHullShapes(b3Contact* contact)
{
	b3MeshContact* meshContact = (b3MeshContact*)contact;

	b3Shape* shapeA = contact->GetShapeA();
	b3Shape* shapeB = contact->GetShapeB();

	b3Transform xfA = shapeA->GetBody()->GetTransform();
	b3Transform xfB = shapeB->GetBody()->GetTransform();

	b3MeshShape* meshShapeA = (b3MeshShape*)shapeA;
	b3HullShape* hullB = (b3HullShape*)shapeB;

	b3StackAllocator* allocator = &shapeA->GetBody()->GetWorld()->m_stackAllocator;

	// Create one temporary manifold per overlapping triangle.
	b3Manifold* tempManifolds = (b3Manifold*)allocator->Allocate(meshContact->m_triangleCount * sizeof(b3Manifold));
	u32 tempCount = 0;

	const b3Mesh* meshA = meshShapeA->m_mesh;
	for (u32 i = 0; i < meshContact->m_triangleCount; ++i)
	{
		b3TriangleCache* triangleCache = meshContact->m_triangles + i;
		u32 triangleIndex = triangleCache->index;

		b3TriangleShape triangleShapeA;
		meshShapeA->GetChildTriangle(&triangleShapeA, triangleIndex);

		b3Manifold* manifold = tempManifolds + tempCount;
		manifold->Initialize();

		b3CollideTriangleAndHull(*manifold, xfA, &triangleShapeA, xfB, hullB, &triangleCache->cache);

		for (u32 j = 0; j < manifold->pointCount; ++j)
		{
			manifold->points[j].key.triangleKey = triangleIndex;
		}

		++tempCount;
	}

	// Send contact manifolds for clustering. This is an important optimization.
	B3_ASSERT(contact->m_manifoldCount == 0);

	b3ClusterSolver clusterSolver;
	clusterSolver.Run(meshContact->m_stackManifolds, contact->m_manifoldCount, tempManifolds, tempCount, xfA, B3_HULL_RADIUS, xfB, shapeB->m_radius);

	allocator->Free(tempManifolds);
}