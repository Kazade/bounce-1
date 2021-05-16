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

#ifndef GL_RENDER_TRIANGLES_H
#define GL_RENDER_TRIANGLES_H

#include "glad/glad.h"
#include <stdint.h>

class GLRenderTriangles
{
public:
	GLRenderTriangles(uint32_t triangle_capacity);
	~GLRenderTriangles();
	
	uint32_t GetVertexCapacity() { return m_vertex_capacity; }

	uint32_t GetVertexCount() { return m_vertex_count; }

	void PushVertex(float x, float y, float z, float r, float g, float b, float a, float nx, float ny, float nz);

	void SetMVP(float* mvp);

	void Flush();
private:
	uint32_t m_vertex_capacity;
	float* m_positions;
	float* m_colors;
	float* m_normals;
	uint32_t m_vertex_count;
	float m_mvp[16];

	GLuint m_vbos[3];

	GLuint m_program;
	GLuint m_position_attribute;
	GLuint m_color_attribute;
	GLuint m_normal_attribute;
	GLuint m_projection_uniform;
};

#endif