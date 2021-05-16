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

#include <bounce/dynamics/shapes/shape.h>
#include <bounce/dynamics/shapes/sphere_shape.h>
#include <bounce/dynamics/shapes/capsule_shape.h>
#include <bounce/dynamics/shapes/triangle_shape.h>
#include <bounce/dynamics/shapes/hull_shape.h>
#include <bounce/dynamics/shapes/mesh_shape.h>
#include <bounce/dynamics/body.h>
#include <bounce/dynamics/world.h>
#include <bounce/dynamics/contacts/contact.h>
#include <bounce/collision/shapes/sphere.h>
#include <bounce/collision/shapes/capsule.h>
#include <bounce/collision/shapes/hull.h>
#include <bounce/collision/shapes/mesh.h>

b3Shape::b3Shape() 
{
	m_density = scalar(0);
	m_friction = scalar(0);
	m_restitution = scalar(0);
	
	m_isSensor = false;
	m_userData = nullptr;

	m_body = nullptr;
}

void b3Shape::SetSensor(bool flag)
{
	if (flag != m_isSensor)
	{
		if (m_body)
		{
			m_body->SetAwake(true);
		}
		m_isSensor = flag;
	}
}

void b3Shape::DestroyContacts()
{
	b3World* world = m_body->GetWorld();
	b3ContactEdge* ce = m_contactEdges.m_head;
	while (ce)
	{
		b3ContactEdge* tmp = ce;
		ce = ce->m_next;
		world->m_contactMan.Destroy(tmp->contact);
	}
}

const b3AABB& b3Shape::GetAABB() const
{
	return m_body->GetWorld()->m_contactMan.m_broadPhase.GetAABB(m_broadPhaseID);
}

void b3Shape::SetShape(b3Shape* shape)
{
	m_body = shape->GetBody();
}

void b3Shape::Dump(u32 bodyIndex) const
{
	switch (m_type)
	{
	case e_sphereShape:
	{
		b3SphereShape* sphere = (b3SphereShape*) this;
		b3Log("		b3SphereShape shape;\n");
		b3Log("		shape.m_center.Set(%f, %f, %f);\n", sphere->m_center.x, sphere->m_center.y, sphere->m_center.z);
		b3Log("		shape.m_radius = %f;\n", sphere->m_radius);
		break;
	}
	case e_capsuleShape:
	{
		b3CapsuleShape* capsule = (b3CapsuleShape*) this;
		b3Log("		b3CapsuleShape shape;\n");
		b3Log("		shape.m_centers[0].Set(%f, %f, %f);\n", capsule->m_vertex1.x, capsule->m_vertex1.y, capsule->m_vertex1.z);
		b3Log("		shape.m_centers[1].Set(%f, %f, %f);\n", capsule->m_vertex2.x, capsule->m_vertex2.y, capsule->m_vertex2.z);
		b3Log("		shape.m_radius = %f;\n", capsule->m_radius);
		break;
	}
	case e_triangleShape:
	{
		b3TriangleShape* triangle = (b3TriangleShape*)this;
		b3Log("		b3TriangleShape shape;\n");
		b3Log("		shape.m_vertex1.Set(%f, %f, %f);\n", triangle->m_vertex1.x, triangle->m_vertex1.y, triangle->m_vertex1.z);
		b3Log("		shape.m_vertex2.Set(%f, %f, %f);\n", triangle->m_vertex2.x, triangle->m_vertex2.y, triangle->m_vertex2.z);
		b3Log("		shape.m_vertex3.Set(%f, %f, %f);\n", triangle->m_vertex3.x, triangle->m_vertex3.y, triangle->m_vertex3.z);
		b3Log("		shape.m_radius = %f;\n", triangle->m_radius);
		break;
	}
	case e_hullShape:
	{
		b3HullShape* hs = (b3HullShape*) this;
		const b3Hull* h = hs->m_hull;
		
		b3Log("		u8* marker = (u8*) b3Alloc(%d);\n", h->GetSize());
		b3Log("		\n");
		b3Log("		b3Hull* h = (b3Hull*)marker;\n");
		b3Log("		marker += 1 * sizeof(b3Hull);\n");
		b3Log("		h->vertices = (b3Vec3*)marker;\n");
		b3Log("		marker += %d * sizeof(b3Vec3);\n", h->vertexCount);
		b3Log("		h->edges = (b3HalfEdge*)marker;\n");
		b3Log("		marker += %d * sizeof(b3HalfEdge);\n", h->edgeCount);
		b3Log("		h->faces = (b3Face*)marker;\n");
		b3Log("		marker += %d * sizeof(b3Face);\n", h->faceCount);
		b3Log("		h->planes = (b3Plane*)marker;\n");
		b3Log("		marker += %d * sizeof(b3Plane);\n", h->faceCount);
		b3Log("		\n");
		b3Log("		h->centroid.Set(%f, %f, %f);\n", h->centroid.x, h->centroid.y, h->centroid.z);
		b3Log("		\n");
		b3Log("		h->vertexCount = %d;\n", h->vertexCount);
		for (u32 i = 0; i < h->vertexCount; ++i)
		{
			const b3Vec3* v = h->vertices + i;
			b3Log("		h->vertices[%d].Set(%f, %f, %f);\n", i, v->x, v->y, v->z);
		}
		b3Log("		\n");
		b3Log("		h->edgeCount = %d;\n", h->edgeCount);
		for (u32 i = 0; i < h->edgeCount; ++i)
		{
			const b3HalfEdge* e = h->edges + i;
			b3Log("		h->edges[%d].origin = %d;\n", i, e->origin);
			b3Log("		h->edges[%d].twin = %d;\n", i, e->twin);
			b3Log("		h->edges[%d].face = %d;\n", i, e->face);
			b3Log("		h->edges[%d].prev = %d;\n", i, e->prev);
			b3Log("		h->edges[%d].next = %d;\n", i, e->next);
		}
		b3Log("		\n");
		b3Log("		h->faceCount = %d;\n", h->faceCount);
		for (u32 i = 0; i < h->faceCount; ++i)
		{
			const b3Face* f = h->faces + i;
			b3Log("		h->faces[%d].edge = %d;\n", i, f->edge);
		}
		b3Log("		\n");
		for (u32 i = 0; i < h->faceCount; ++i)
		{
			const b3Plane* p = h->planes + i;
			b3Log("		h->planes[%d].normal.Set(%f, %f, %f);\n", i, p->normal.x, p->normal.y, p->normal.z);
			b3Log("		h->planes[%d].offset = %f;\n", i, p->offset);
		}
		b3Log("		\n");
		b3Log("		h->Validate();\n");
		b3Log("		\n");
		b3Log("		b3HullShape shape;\n");
		b3Log("		shape.m_hull = h;\n");
		b3Log("		shape.m_radius = %f;\n", m_radius);
		break;
	}
	case e_meshShape:
	{
		b3MeshShape* ms = (b3MeshShape*) this;
		const b3Mesh* m = ms->m_mesh;
		
		b3Log("		u8* marker = (u8*) b3Alloc(%d);\n", m->GetSize());
		b3Log("		\n");
		b3Log("		b3Mesh* m = (b3Hull*)marker;\n");
		b3Log("		marker += 1 * sizeof(b3Mesh);\n");
		b3Log("		m->vertices = (b3Vec3*)marker;\n");
		b3Log("		marker += %d * sizeof(b3Vec3);\n", m->vertexCount);
		b3Log("		m->triangles = (b3MeshTriangle*)marker;\n");
		b3Log("		marker += %d * sizeof(b3MeshTriangle);\n", m->triangleCount);
		b3Log("		m->planes = (b3Plane*)marker;\n");
		b3Log("		marker += %d * sizeof(b3Plane);\n", 2 * m->triangleCount);
		b3Log("		\n");
		for (u32 i = 0; i < m->vertexCount; ++i)
		{
			const b3Vec3* v = m->vertices + i;
			b3Log("		m->vertices[%d].Set(%f, %f, %f);\n", i, v->x, v->y, v->z);
		}
		b3Log("		\n");
		for (u32 i = 0; i < m->triangleCount; ++i)
		{
			const b3MeshTriangle* t = m->triangles + i;
			b3Log("		m->triangles[%d].v1 = %d;\n", i, t->v1);
			b3Log("		m->triangles[%d].v2 = %d;\n", i, t->v2);
			b3Log("		m->triangles[%d].v3 = %d;\n", i, t->v3);
		}
		b3Log("		\n");
		b3Log("		\n");
		b3Log("		m->BuildTree();\n");		
		b3Log("		\n");
		b3Log("		b3MeshShape shape;\n");
		b3Log("		shape.m_mesh = m;\n");
		b3Log("		shape.m_radius = %f;\n", m_radius);
		break;
	}
	default:
	{
		B3_ASSERT(false);
		break;
	}
	};

	b3Log("		\n");
	b3Log("		b3ShapeDef sd;\n");
	b3Log("		sd.shape = &shape;\n");
	b3Log("		sd.density = %f;\n", m_density);
	b3Log("		sd.restitution = %f;\n", m_restitution);
	b3Log("		sd.friction = %f;\n", m_friction);
	b3Log("		sd.sensor = %d;\n", m_isSensor);
	b3Log("		\n");
	b3Log("		bodies[%d]->CreateShape(sd);\n", bodyIndex);
}

void b3Shape::Draw(const b3Transform& xf, const b3Color& color)
{
	switch (m_type)
	{
	case e_sphereShape:
	{
		const b3SphereShape* sphere = (b3SphereShape*)this;
		b3Vec3 p = xf * sphere->m_center;
		b3Draw_draw->DrawPoint(p, scalar(4), color);
		break;
	}
	case e_capsuleShape:
	{
		const b3CapsuleShape* capsule = (b3CapsuleShape*)this;
		b3Vec3 p1 = xf * capsule->m_vertex1;
		b3Vec3 p2 = xf * capsule->m_vertex2;
		b3Draw_draw->DrawPoint(p1, scalar(4), color);
		b3Draw_draw->DrawPoint(p2, scalar(4), color);
		b3Draw_draw->DrawSegment(p1, p2, color);
		break;
	}
	case e_triangleShape:
	{
		const b3TriangleShape* triangle = (b3TriangleShape*)this;
		b3Vec3 v1 = xf * triangle->m_vertex1;
		b3Vec3 v2 = xf * triangle->m_vertex2;
		b3Vec3 v3 = xf * triangle->m_vertex3;
		b3Vec3 n = b3Cross(v2 - v1, v3 - v1);
		n.Normalize();
		b3Draw_draw->DrawTriangle(v1, v2, v3, color);
		break;
	}
	case e_hullShape:
	{
		const b3HullShape* hs = (b3HullShape*)this;
		const b3Hull* hull = hs->m_hull;
		for (u32 i = 0; i < hull->edgeCount; i += 2)
		{
			const b3HalfEdge* edge = hull->GetEdge(i);
			const b3HalfEdge* twin = hull->GetEdge(i + 1);

			b3Vec3 p1 = xf * hull->vertices[edge->origin];
			b3Vec3 p2 = xf * hull->vertices[twin->origin];

			b3Draw_draw->DrawSegment(p1, p2, color);
		}
		break;
	}
	case e_meshShape:
	{
		const b3MeshShape* ms = (b3MeshShape*)this;
		const b3Mesh* mesh = ms->m_mesh;
		for (u32 i = 0; i < mesh->triangleCount; ++i)
		{
			const b3MeshTriangle* t = mesh->triangles + i;

			b3Vec3 p1 = xf * b3Mul(ms->m_scale, mesh->vertices[t->v1]);
			b3Vec3 p2 = xf * b3Mul(ms->m_scale, mesh->vertices[t->v2]);
			b3Vec3 p3 = xf * b3Mul(ms->m_scale, mesh->vertices[t->v3]);

			b3Draw_draw->DrawTriangle(p1, p2, p3, color);
		}
		break;
	}
	default:
	{
		break;
	}
	};
}

void b3Shape::DrawSolid(const b3Transform& xf, const b3Color& color)
{
	switch (m_type)
	{
	case e_sphereShape:
	{
		const b3SphereShape* sphere = (b3SphereShape*)this;

		b3Vec3 center = xf * sphere->m_center;
		
		b3Draw_draw->DrawSolidSphere(xf.rotation.GetYAxis(), center, sphere->m_radius, color);

		break;
	}
	case e_capsuleShape:
	{
		const b3CapsuleShape* capsule = (b3CapsuleShape*)this;

		b3Vec3 c1 = xf * capsule->m_vertex1;
		b3Vec3 c2 = xf * capsule->m_vertex2;

		b3Draw_draw->DrawSolidCapsule(xf.rotation.GetYAxis(), c1, c2, capsule->m_radius, color);
		
		break;
	}
	case e_triangleShape:
	{
		const b3TriangleShape* triangle = (b3TriangleShape*)this;
		
		b3Vec3 v1 = xf * triangle->m_vertex1;
		b3Vec3 v2 = xf * triangle->m_vertex2;
		b3Vec3 v3 = xf * triangle->m_vertex3;
		
		b3Vec3 n = b3Cross(v2 - v1, v3 - v1);
		n.Normalize();
		
		b3Draw_draw->DrawSolidTriangle(-n, v3, v2, v1, color);
		b3Draw_draw->DrawSolidTriangle(n, v1, v2, v3, color);

		break;
	}
	case e_hullShape:
	{
		const b3HullShape* hullShape = (b3HullShape*)this;

		const b3Hull* hull = hullShape->m_hull;

		for (u32 i = 0; i < hull->faceCount; ++i)
		{
			const b3Face* face = hull->GetFace(i);
			const b3HalfEdge* begin = hull->GetEdge(face->edge);

			b3Vec3 n = b3Mul(xf.rotation, hull->planes[i].normal);

			const b3HalfEdge* edge = hull->GetEdge(begin->next);
			do
			{
				u32 i1 = begin->origin;
				u32 i2 = edge->origin;
				const b3HalfEdge* next = hull->GetEdge(edge->next);
				u32 i3 = next->origin;

				b3Vec3 p1 = xf * hull->vertices[i1];
				b3Vec3 p2 = xf * hull->vertices[i2];
				b3Vec3 p3 = xf * hull->vertices[i3];

				b3Draw_draw->DrawSolidTriangle(n, p1, p2, p3, color);

				edge = next;
			} while (hull->GetEdge(edge->next) != begin);
		}

		break;
	}
	case e_meshShape:
	{
		const b3MeshShape* meshShape = (b3MeshShape*)this;

		const b3Mesh* mesh = meshShape->m_mesh;
		for (u32 i = 0; i < mesh->triangleCount; ++i)
		{
			const b3MeshTriangle* t = mesh->triangles + i;

			b3Vec3 p1 = xf * b3Mul(meshShape->m_scale, mesh->vertices[t->v1]);
			b3Vec3 p2 = xf * b3Mul(meshShape->m_scale, mesh->vertices[t->v2]);
			b3Vec3 p3 = xf * b3Mul(meshShape->m_scale, mesh->vertices[t->v3]);

			b3Vec3 n1 = b3Cross(p2 - p1, p3 - p1);
			n1.Normalize();
			b3Draw_draw->DrawSolidTriangle(n1, p1, p2, p3, color);

			b3Vec3 n2 = -n1;
			b3Draw_draw->DrawSolidTriangle(n2, p3, p2, p1, color);
		}

		break;
	}
	default:
	{
		break;
	}
	};

}

b3Shape* b3Shape::Create(const b3ShapeDef& def)
{
	b3Shape* shape = nullptr;
	switch (def.shape->GetType())
	{
	case e_sphereShape:
	{
		// Grab pointer to the specific memory.
		b3SphereShape* sphere1 = (b3SphereShape*)def.shape;
		void* mem = b3Alloc(sizeof(b3SphereShape));
		b3SphereShape* sphere2 = new (mem)b3SphereShape();
		// Clone the polyhedra.
		sphere2->Clone(*sphere1);
		shape = sphere2;
		break;
	}
	case e_capsuleShape:
	{
		// Grab pointer to the specific memory.
		b3CapsuleShape* caps1 = (b3CapsuleShape*)def.shape;
		void* block = b3Alloc(sizeof(b3CapsuleShape));
		b3CapsuleShape* caps2 = new (block)b3CapsuleShape();
		caps2->Clone(*caps1);
		shape = caps2;
		break;
	}
	case e_triangleShape:
	{
		// Grab pointer to the specific memory.
		b3TriangleShape* triangle1 = (b3TriangleShape*)def.shape;
		void* block = b3Alloc(sizeof(b3TriangleShape));
		b3TriangleShape* triangle2 = new (block)b3TriangleShape();
		triangle2->Clone(*triangle1);
		shape = triangle2;
		break;
	}
	case e_hullShape:
	{
		// Grab pointer to the specific memory.
		b3HullShape* hull1 = (b3HullShape*)def.shape;
		void* block = b3Alloc(sizeof(b3HullShape));
		b3HullShape* hull2 = new (block)b3HullShape();
		hull2->Clone(*hull1);
		shape = hull2;
		break;
	}
	case e_meshShape:
	{
		// Grab pointer to the specific memory.
		b3MeshShape* mesh1 = (b3MeshShape*)def.shape;
		void* block = b3Alloc(sizeof(b3MeshShape));
		b3MeshShape* mesh2 = new (block) b3MeshShape();
		// Clone the mesh.
		mesh2->Clone(*mesh1);
		shape = mesh2;
		break;
	}
	default:
	{
		B3_ASSERT(false);
		break;
	}
	}

	return shape;
}

void b3Shape::Destroy(b3Shape* shape)
{
	// Free the shape from the memory.
	switch (shape->GetType())
	{
	case e_sphereShape:
	{
		b3SphereShape* sphere = (b3SphereShape*)shape;
		sphere->~b3SphereShape();
		b3Free(shape);
		break;
	}
	case e_capsuleShape:
	{
		b3CapsuleShape* caps = (b3CapsuleShape*)shape;
		caps->~b3CapsuleShape();
		b3Free(shape);
		break;
	}
	case e_triangleShape:
	{
		b3TriangleShape* triangle = (b3TriangleShape*)shape;
		triangle->~b3TriangleShape();
		b3Free(shape);
		break;
	}
	case e_hullShape:
	{
		b3HullShape* hull = (b3HullShape*)shape;
		hull->~b3HullShape();
		b3Free(shape);
		break;
	}
	case e_meshShape:
	{
		b3MeshShape* mesh = (b3MeshShape*)shape;
		mesh->~b3MeshShape();
		b3Free(shape);
		break;
	}
	default:
	{
		B3_ASSERT(false);
	}
	}
}
