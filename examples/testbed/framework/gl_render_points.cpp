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

#include "gl_render_points.h"
#include "gl_shader.h"

#include <stdlib.h>
#include <assert.h>

GLRenderPoints::GLRenderPoints(uint32_t point_capacity)
{
	const char* vs = \
		"#version 120\n"
		"uniform mat4 m_projection;\n"
		"attribute vec3 v_position;\n"
		"attribute vec4 v_color;\n"
		"attribute float v_size;\n"
		"varying vec4 f_color;\n"
		"void main()\n"
		"{\n"
		"	f_color = v_color;\n"
		"	gl_Position = m_projection * vec4(v_position, 1.0f);\n"
		"   gl_PointSize = v_size;\n"
		"}\n";

	const char* fs = \
		"#version 120\n"
		"varying vec4 f_color;\n"
		"void main(void)\n"
		"{\n"
		"	gl_FragColor = f_color;\n"
		"}\n";

	m_program = GLCreateShaderProgram(vs, fs);
	m_position_attribute = glGetAttribLocation(m_program, "v_position");
	m_color_attribute = glGetAttribLocation(m_program, "v_color");
	m_size_attribute = glGetAttribLocation(m_program, "v_size");
	m_projection_uniform = glGetUniformLocation(m_program, "m_projection");

	m_vertex_capacity = point_capacity;
	m_positions = (float*)malloc(point_capacity * 3 * sizeof(float));
	m_colors = (float*)malloc(point_capacity * 4 * sizeof(float));
	m_sizes = (float*)malloc(point_capacity * sizeof(float));
	m_vertex_count = 0;

	glGenBuffers(3, m_vbos);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, point_capacity * 3 * sizeof(float), m_positions, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, point_capacity * 4 * sizeof(float), m_colors, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[2]);
	glBufferData(GL_ARRAY_BUFFER, point_capacity * sizeof(float), m_sizes, GL_DYNAMIC_DRAW);

	GLAssert();

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (i == j) 
			{ 
				m_mvp[i + 4 * j] = 1.0f; 
			}
			else 
			{ 
				m_mvp[i + 4 * j] = 0.0f; 
			}
		}
	}
}

GLRenderPoints::~GLRenderPoints()
{
	glDeleteProgram(m_program);
	glDeleteBuffers(3, m_vbos);

	free(m_positions);
	free(m_colors);
	free(m_sizes);
}

void GLRenderPoints::PushVertex(float x, float y, float z, float r, float g, float b, float a, float point_size)
{
	if (m_vertex_count == m_vertex_capacity)
	{
		Flush();
	}

	m_positions[3 * m_vertex_count] = x;
	m_positions[3 * m_vertex_count + 1] = y;
	m_positions[3 * m_vertex_count + 2] = z;
	m_colors[4 * m_vertex_count] = r;
	m_colors[4 * m_vertex_count + 1] = g;
	m_colors[4 * m_vertex_count + 2] = b;
	m_colors[4 * m_vertex_count + 3] = a;
	m_sizes[m_vertex_count] = point_size;
	++m_vertex_count;
}

void GLRenderPoints::SetMVP(float* mvp)
{
	for (int i = 0; i < 16; ++i)
	{
		m_mvp[i] = mvp[i];
	}
}

void GLRenderPoints::Flush()
{
	if (m_vertex_count == 0)
	{
		return;
	}

	glUseProgram(m_program);

	glUniformMatrix4fv(m_projection_uniform, 1, GL_FALSE, m_mvp);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertex_count * 3 * sizeof(float), m_positions);
	glVertexAttribPointer(m_position_attribute, 3, GL_FLOAT, GL_FALSE, 0, GL_PTR_ADD(NULL, 0));
	glEnableVertexAttribArray(m_position_attribute);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertex_count * 4 * sizeof(float), m_colors);
	glEnableVertexAttribArray(m_color_attribute);
	glVertexAttribPointer(m_color_attribute, 4, GL_FLOAT, GL_FALSE, 0, GL_PTR_ADD(NULL, 0));

	glBindBuffer(GL_ARRAY_BUFFER, m_vbos[2]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertex_count * sizeof(float), m_sizes);
	glEnableVertexAttribArray(m_size_attribute);
	glVertexAttribPointer(m_size_attribute, 1, GL_FLOAT, GL_FALSE, 0, GL_PTR_ADD(NULL, 0));

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDrawArrays(GL_POINTS, 0, m_vertex_count);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

	m_vertex_count = 0;

	glDisableVertexAttribArray(m_size_attribute);
	glDisableVertexAttribArray(m_color_attribute);
	glDisableVertexAttribArray(m_position_attribute);

	GLAssert();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
}