#pragma once
#include "do_not_edit.h"

// glClearColor()->Initialize the color buffer to the specified color
void ClearColourBuffer(float col[4])
{
	for (int i = 0; i < PIXEL_W * PIXEL_H * 3; i += 3)
	{
		colour_buffer[i] = col[0] * 255.0f;     // R
		colour_buffer[i + 1] = col[1] * 255.0f; // G
		colour_buffer[i + 2] = col[2] * 255.0f; // B
	}
}

// Initialize the depth buffer to the maximum value
void ClearDepthBuffer()
{
	for (int i = 0; i < PIXEL_W * PIXEL_H; i++) 
		depth_buffer[i] = FLT_MAX;
}

void ApplyTransformationMatrix(glm::mat4 T, vector<triangle>& tris)
{
	for (auto& tri : tris) 
	{
		tri.v1.pos = T * tri.v1.pos;
		tri.v2.pos = T * tri.v2.pos;
		tri.v3.pos = T * tri.v3.pos;
	}
}

// Apply perspective division to each vertex
// Homogeneous coordinate -> Cartesian coordinate
void ApplyPerspectiveDivision(vector<triangle>& tris)
{
	for (auto& tri : tris) 
	{
		tri.v1.pos.x /= tri.v1.pos.w;
		tri.v1.pos.y /= tri.v1.pos.w;
		tri.v1.pos.z /= tri.v1.pos.w;
		tri.v1.pos.w = 1.0f;

		tri.v2.pos.x /= tri.v2.pos.w;
		tri.v2.pos.y /= tri.v2.pos.w;
		tri.v2.pos.z /= tri.v2.pos.w;
		tri.v2.pos.w = 1.0f;

		tri.v3.pos.x /= tri.v3.pos.w;
		tri.v3.pos.y /= tri.v3.pos.w;
		tri.v3.pos.z /= tri.v3.pos.w;
		tri.v3.pos.w = 1.0f;
	}
}

// NDC -> Screen
// [-1,1] -> [0,w] or [0,h]
void ApplyViewportTransformation(int w, int h, vector<triangle>& tris)
{
	for (auto& tri : tris) 
	{
		tri.v1.pos.x = (tri.v1.pos.x + 1.0f) * 0.5f * w;
		tri.v1.pos.y = (tri.v1.pos.y + 1.0f) * 0.5f * h;

		tri.v2.pos.x = (tri.v2.pos.x + 1.0f) * 0.5f * w;
		tri.v2.pos.y = (tri.v2.pos.y + 1.0f) * 0.5f * h;

		tri.v3.pos.x = (tri.v3.pos.x + 1.0f) * 0.5f * w;
		tri.v3.pos.y = (tri.v3.pos.y + 1.0f) * 0.5f * h;
	}
}


void ComputeBarycentricCoordinates(int px, int py, triangle t, float& alpha, float& beta, float& gamma)
{
	glm::vec2 p(px, py);
	glm::vec2 a(t.v1.pos.x, t.v1.pos.y);
	glm::vec2 b(t.v2.pos.x, t.v2.pos.y);
	glm::vec2 c(t.v3.pos.x, t.v3.pos.y);

	float area_abc = 0.5f * abs((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y));

	float area_bcp = 0.5f * abs((b.x - p.x) * (c.y - p.y) - (c.x - p.x) * (b.y - p.y));
	float area_acp = 0.5f * abs((p.x - a.x) * (c.y - a.y) - (c.x - a.x) * (p.y - a.y));
	float area_abp = 0.5f * abs((b.x - a.x) * (p.y - a.y) - (p.x - a.x) * (b.y - a.y));

	alpha = area_bcp / area_abc;
	beta = area_acp / area_abc;
	gamma = area_abp / area_abc;
}

// Interpolate vertex color and depth by using barycentric coordinates.
void ShadeFragment(triangle tri, float& alpha, float& beta, float& gamma, glm::vec3& col, float& depth)
{
	col = alpha * tri.v1.col + beta * tri.v2.col + gamma * tri.v3.col;
	depth = alpha * tri.v1.pos.z + beta * tri.v2.pos.z + gamma * tri.v3.pos.z;
}


void Rasterise(vector<triangle> tris)
{
	for (int py = 0; py < PIXEL_H; py++)
	{
		float percf = (float)py / (float)PIXEL_H;
		int perci = percf * 100;
		std::clog << "\rScanlines done: " << perci << "%" << ' ' << std::flush;

		for (int px = 0; px < PIXEL_W; px++)
		{
			// Iterate all the triangles
			for (auto& tri : tris)
			{
				float alpha, beta, gamma;
				ComputeBarycentricCoordinates(px, py, tri, alpha, beta, gamma);

				// Check whether the point is inside the triangle
				if (alpha >= 0 && beta >= 0 && gamma >= 0 && (alpha + beta + gamma <= 1.0001f))
				{
					// calculate col and depth
					glm::vec3 color;
					float depth;
					ShadeFragment(tri, alpha, beta, gamma, color, depth);

					// depth test
					int bufferIndex = py * PIXEL_W + px;
					if (depth < depth_buffer[bufferIndex])
					{
						depth_buffer[bufferIndex] = depth;
						writeColToDisplayBuffer(color, px, PIXEL_H - py);
					}
				}
			}
		}
	}
	std::clog << "\rFinish rendering.           \n";
}

void render(vector<triangle>& tris)
{
}
