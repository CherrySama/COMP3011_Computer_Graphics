#pragma once

void ClearColourBuffer(float col[4])
{
}

void ClearDepthBuffer()
{
}

void ApplyTransformationMatrix(glm::mat4 T, vector<triangle>& tris)
{
}

void ApplyPerspectiveDivision(vector<triangle>& tris)
{
}

void ApplyViewportTransformation(int w, int h, vector<triangle>& tris)
{
}


void ComputeBarycentricCoordinates(int px, int py, triangle t, float& alpha, float& beta, float& gamma)
{
}

void ShadeFragment(triangle tri, float& alpha, float& beta, float& gamma, glm::vec3& col, float& depth)
{
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
		}
	}
	std::clog << "\rFinish rendering.           \n";
}

void render(vector<triangle>& tris)
{
}
