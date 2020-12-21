#pragma once

#include <iostream>
#include <vector>
#define PI 3.14159265358979324
using namespace std;

struct Shape
{
	vector<GLshort> shape_indices;
	vector<GLfloat> shape_vertices;
	vector<GLfloat> shape_colors;
	~Shape()
	{
		shape_indices.clear();
		shape_indices.shrink_to_fit();
		shape_vertices.clear();
		shape_vertices.shrink_to_fit();
		shape_colors.clear();
		shape_colors.shrink_to_fit();
	}
	GLsizei NumIndices() { return shape_indices.size(); }
	void BufferShape(GLuint* ibo, GLuint* points_vbo, GLuint* colors_vbo)
	{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shape_indices[0]) * shape_indices.size(), &shape_indices.front(), GL_STATIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, *points_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(shape_vertices[0]) * shape_vertices.size(), &shape_vertices.front(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(shape_vertices[0]) * 3, 0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, *colors_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(shape_colors[0]) * shape_colors.size(), &shape_colors.front(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			glBindBuffer(GL_ARRAY_BUFFER, 0); 
	}
	void ColorShape(GLfloat r, GLfloat g, GLfloat b)
	{
		shape_colors.clear();
		shape_colors.shrink_to_fit();
		for (int i = 0; i < shape_vertices.size(); i+=3)
		{
			shape_colors.push_back(r);
			shape_colors.push_back(g);
			shape_colors.push_back(b);
		}
		shape_colors.shrink_to_fit(); // Good idea after a bunch of pushes.
	}
};

struct Plane : public Shape // Vertical plane of 1x1 units across.
{
	Plane()
	{
		shape_indices = {
			0, 1, 2,
			2, 3, 0
		};
		shape_vertices = {
			0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,		
			1.0f, 1.0f, 0.0f,		
			0.0f, 1.0f, 0.0f
		};
		ColorShape(1.0f, 1.0f, 1.0f);
	}
};

struct Grid : public Shape // Flat grid on ground. Starts at 0,0,0 and increases into positive.
{
	Grid(int quads)
	{
		for (int row = 0; row <= quads; row++)
		{
			for (int col = 0; col <= quads; col++)
			{
				shape_vertices.push_back(col);
				shape_vertices.push_back(row);
				shape_vertices.push_back(0.0f);
			}
		}
		int i = 0;
		for (int row = 0; row < (quads * (quads + 1)); row = i * (quads + 1))
		{
			if (i % 2 == 0) // Even row: l->r
			{
				for (int col = 0; col < quads; col++)
				{
					// Triangle one.
					shape_indices.push_back(row + col);
					shape_indices.push_back(row + (col + 1));
					shape_indices.push_back(row + (quads + 1) + (col + 1));
					// Triangle two.
					shape_indices.push_back(row + (quads + 1) + (col + 1));
					shape_indices.push_back(row + (quads + 1) + col);
					shape_indices.push_back(row + col);
				}
			}
			else // Odd row: r->l
			{
				for (int col = quads - 1; col >= 0; col--)
				{
					// Triangle one.
					shape_indices.push_back(row + col);
					shape_indices.push_back(row + (col + 1));
					shape_indices.push_back(row + (quads + 1) + (col + 1));
					// Triangle two.
					shape_indices.push_back(row + (quads + 1) + (col + 1));
					shape_indices.push_back(row + (quads + 1) + col);
					shape_indices.push_back(row + col);
				}
			}
			i++;
		}
		ColorShape(0.0f, 1.0f, 1.0f);
	}
};
struct Cube : public Shape
{
	Cube()
	{
		shape_indices = {
			// Front.
			0, 1, 2,
			2, 3, 0,
			// Left.
			4, 0, 3,
			3, 7, 4,
			// Bottom.
			5, 1, 0,
			0, 4, 5,
			// Right.
			6, 2, 1,
			1, 5, 6,
			// Back.
			7, 6, 5,
			5, 4, 7,
			// Top.
			3, 2, 6,
			6, 7, 3
		};
		shape_vertices = {
			0.0f, 0.0f, 1.0f,		// 0.
			1.0f, 0.0f, 1.0f,		// 1.
			1.0f, 1.0f, 1.0f,		// 2.
			0.0f, 1.0f, 1.0f,		// 3.
			0.0f, 0.0f, 0.0f,		// 4.
			1.0f, 0.0f, 0.0f,		// 5.
			1.0f, 1.0f, 0.0f,		// 6.
			0.0f, 1.0f, 0.0f,		// 7.
		};
		ColorShape(1.0f, 1.0f, 1.0f);
	}
};

struct Prism : public Shape
{
	Prism(int sides)
	{
		float theta = 0.0f;
		// Top face.
		shape_vertices.push_back(0.5f);
		shape_vertices.push_back(1.0f);
		shape_vertices.push_back(0.5f);
		for (int i = 0; i < sides; ++i)
		{
			shape_vertices.push_back(0.5f + 0.5f * cos(theta));
			shape_vertices.push_back(1.0f);
			shape_vertices.push_back(0.5f + 0.5f * sin(theta));
			theta += 2 * PI / sides;
		}
		// Bottom face.
		shape_vertices.push_back(0.5f);
		shape_vertices.push_back(0.0f);
		shape_vertices.push_back(0.5f);
		for (int i = 0; i < sides; ++i)
		{
			shape_vertices.push_back(0.5f + 0.5f * cos(theta));
			shape_vertices.push_back(0.0f);
			shape_vertices.push_back(0.5f + 0.5f * sin(theta));
			theta += 2 * PI / sides;
		}
		// Indices now.
		// Bottom face.
		for (int i = sides + 1; i < sides * 2; i++)
		{
			shape_indices.push_back(sides + 1);
			shape_indices.push_back(i + 1);
			shape_indices.push_back(i + 2);
		}
		shape_indices.push_back(sides + 1);
		shape_indices.push_back(sides * 2 + 1);
		shape_indices.push_back(sides + 2);
		// Middle faces.
		for (int i = 1; i < sides; i++)
		{
			// Triangle one.
			shape_indices.push_back(i);
			shape_indices.push_back(i+1);
			shape_indices.push_back(sides + i + 2);
			// Triangle two.
			shape_indices.push_back(sides + i + 2);
			shape_indices.push_back(sides + i + 1);
			shape_indices.push_back(i);
		}
		shape_indices.push_back(sides);
		shape_indices.push_back(1);
		shape_indices.push_back(sides + 2);
		shape_indices.push_back(sides + 2);
		shape_indices.push_back(sides * 2 + 1);
		shape_indices.push_back(sides);
		// Top face.
		for (int i = 1; i < sides; i++)
		{
			shape_indices.push_back(0);
			shape_indices.push_back(i + 1);
			shape_indices.push_back(i);
		}
		shape_indices.push_back(0);
		shape_indices.push_back(1);
		shape_indices.push_back(sides);
		ColorShape(1.0f, 1.0f, 1.0f);
	}
};

struct Cone : public Shape
{
	Cone(int sides)
	{
		float theta = 0.0f;
		// Bottom face.
		shape_vertices.push_back(0.5f);
		shape_vertices.push_back(0.0f);
		shape_vertices.push_back(0.5f);
		for (int i = 0; i < sides; ++i)
		{
			shape_vertices.push_back(0.5f + 0.5f * cos(theta));
			shape_vertices.push_back(0.0f);
			shape_vertices.push_back(0.5f + 0.5f * sin(theta));
			theta += 2 * PI / sides;
		}
		shape_vertices.push_back(0.5f);
		shape_vertices.push_back(1.0f);
		shape_vertices.push_back(0.5f);
		// Indices now. Bottom face.
		for (int i = 1; i < sides; i++)
		{
			shape_indices.push_back(0);
			shape_indices.push_back(i);
			shape_indices.push_back(i + 1);
		}
		shape_indices.push_back(0);
		shape_indices.push_back(sides);
		shape_indices.push_back(1);
		// Middle faces.
		for (int i = 1; i < sides; i++)
		{
			shape_indices.push_back(i);
			shape_indices.push_back(sides + 1);
			shape_indices.push_back(i + 1);
		}
		shape_indices.push_back(sides);
		shape_indices.push_back(sides + 1);
		shape_indices.push_back(1);
		ColorShape(1.0f, 1.0f, 1.0f);
	}
};