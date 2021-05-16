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

#if !defined(quickhull_h)

typedef struct qh_vec3_t
{
	float x, y, z;
} qh_vec3_t;

typedef struct qh_plane_t
{
	qh_vec3_t n;
	float d;
} qh_plane_t;

typedef struct qh_output_face_t
{
	int edge;
} qh_output_face_t;

typedef struct qh_output_half_edge_t
{
	int origin;
	int twin;
	int face;
	int prev;
	int next;
} qh_output_half_edge_t;

typedef struct qh_output_t
{
	int vertex_count;
	qh_vec3_t* vertices;
	int edge_count;
	qh_output_half_edge_t* edges;
	int face_count;
	qh_output_face_t* faces;
	qh_plane_t* planes;
	qh_vec3_t centroid;
} qh_output_t;

// Create a convex hull given list of points.
qh_output_t* qh_create_hull(int vertex_stride, const void* vertex_base, int vertex_count, float coincidence_tolerance);

// Create a simplified convex hull.
qh_output_t* qh_create_simplified_hull(int vertex_stride, const void* vertex_base, int vertex_count, float coincidence_tolerance, float angle_tolerance);

// If you called qh_create_hull/qh_create_simplified_hull then you must call this function for destroying the convex hull.
void qh_destroy_hull(qh_output_t* hull);

#define quickhull_h // #if !defined(qh_hull_h)

#endif // #define qh_hull_h
