#pragma once
#include "do_not_edit.h"


glm::vec3 DoNothing(triangle* tri, int depth, glm::vec3 p, glm::vec3 dir)
{
    return vec3(0);
}

glm::vec3 Shade(triangle* tri, int depth, glm::vec3 p, glm::vec3 dir)
{
    vec3 col = vec3(0);
	vec3 surface_col = tri->v1.col;
	vec3 n = normalize(tri->v1.nor);
	//vec3 surface_col = (tri->v1.col + tri->v2.col + tri->v3.col) * 0.33f;
	//vec3 n = normalize((tri->v1.nor + tri->v2.nor + tri->v3.nor) * 0.33f);

	// ambient
	float ambient = 0.1f;
	col += ambient * surface_col;

	// diffuse
	vec3 light_dir = normalize(light_pos - p);
	float diffuse = max(dot(n, light_dir), 0.0f);

	// shadow
	float shadow;
	vec3 shadow_col;
	trace(p + 0.001f * light_dir, light_dir, shadow, shadow_col, depth + 1, DoNothing);
	if (shadow == FLT_MAX)
		col += diffuse * surface_col;

	// reflection
	if (tri->reflect && depth < max_recursion_depth) 
	{
		vec3 reflect_dir = reflect(dir, n);
		float reflect_t;
		vec3 reflect_col;
		trace(p + 0.001f * reflect_dir, reflect_dir, reflect_t, reflect_col, depth + 1, Shade);
		col += reflect_col;
	}

    return col;
}

bool PointInTriangle(glm::vec3 pt, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
	glm::vec3 v1v2 = v2 - v1;
	glm::vec3 v1pt = pt - v1;
	glm::vec3 v2v3 = v3 - v2;
	glm::vec3 v2pt = pt - v2;
	glm::vec3 v3v1 = v1 - v3;
	glm::vec3 v3pt = pt - v3;

	glm::vec3 normal = glm::cross(v1v2, v2v3);
	glm::vec3 cross1 = glm::cross(v1v2, v1pt);
	glm::vec3 cross2 = glm::cross(v2v3, v2pt);
	glm::vec3 cross3 = glm::cross(v3v1, v3pt);

	float dot1 = glm::dot(cross1, normal);
	float dot2 = glm::dot(cross2, normal);
	float dot3 = glm::dot(cross3, normal);

	if ((dot1 >= 0 && dot2 >= 0 && dot3 >= 0) || (dot1 <= 0 && dot2 <= 0 && dot3 <= 0))
		return true;

    return false;
}

// this func already check whether the point is inside the triangle
float RayTriangleIntersection(glm::vec3 o, glm::vec3 dir, triangle* tri, glm::vec3& point)
{
	glm::vec3 v0 = tri->v1.pos;
	glm::vec3 v1 = tri->v2.pos;
	glm::vec3 v2 = tri->v3.pos;

	glm::vec3 v0v1 = v1 - v0;
	glm::vec3 v0v2 = v2 - v0;
	glm::vec3 N = glm::normalize(glm::cross(v0v1, v0v2));

	float dN = glm::dot(dir, N);
	// check whether the ray is parallel to the triangle
	if (std::abs(dN) < 1e-6f)
		return FLT_MAX;

	float dN_inv = 1.0f / dN;
	float t = glm::dot(v0 - o, N) * dN_inv;
	// check whether the intersection point is behind the orig of the light
	if (t < 1e-6f)  
		return FLT_MAX;

	point = o + t * dir;
	if (PointInTriangle(point, v0, v1, v2))
		return t;

	return FLT_MAX;
}

void trace(glm::vec3 o, glm::vec3 dir, float& t, glm::vec3& io_col, int depth, closest_hit p_hit)
{
	t = FLT_MAX;
	triangle* closest_tri = nullptr;
	glm::vec3 closest_point;

	// find the triangle
	for (auto& tri : tris)
	{
		glm::vec3 curr_point;
		float curr_T = RayTriangleIntersection(o, dir, &tri, curr_point);

		if (curr_T < t)
		{
			t = curr_T;
			closest_tri = &tri;
			closest_point = curr_point;
		}
	}

	if (closest_tri != nullptr) 
		io_col = p_hit(closest_tri, depth, closest_point, dir);
	else 
		io_col = bkgd;
}

// calculate the light direction for each pixel on the screen, 
vec3 GetRayDirection(float px, float py, int W, int H, float aspect_ratio, float fov)
{
	glm::vec3 R(1, 0, 0);
	glm::vec3 U(0, -1, 0);
	glm::vec3 F(0, 0, -1);

	float tan_theta = tan(glm::radians(fov / 2.0f));
	float screen_x = (2 * (px + 0.5f) / W - 1) * aspect_ratio * tan_theta;
	float screen_y = (1 - 2 * (py + 0.5f) / H) * tan_theta;

    return vec3(normalize(screen_x * R + screen_y * U + F));
}

void raytrace()
{
	float aspect_ratio = (float)PIXEL_W / (float)PIXEL_H;
	float fov = 90.0f; 

    for (int pixel_y = 0; pixel_y < PIXEL_H; ++pixel_y)
    {
        float percf = (float)pixel_y / (float)PIXEL_H;
        int perci = percf * 100;
        std::clog << "\rScanlines done: " << perci << "%" << ' ' << std::flush;

        for (int pixel_x = 0; pixel_x < PIXEL_W; ++pixel_x)
        {
			vec3 dir = GetRayDirection(pixel_x, pixel_y, PIXEL_W, PIXEL_H, aspect_ratio, fov);
			float t;
			vec3 col;
			trace(eye, dir, t, col, 0, Shade);
			writeCol(col, pixel_x, PIXEL_H - 1 - pixel_y);
        }
    }
    std::clog << "\rFinish rendering.           \n";
}
