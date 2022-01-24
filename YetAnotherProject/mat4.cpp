#include "mat4.hpp"

#include <cmath>

namespace mat {
	mat4f zeroed() {
		mat4f m;
		m[0][0] = 0; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
		m[1][0] = 0; m[1][1] = 0; m[1][2] = 0; m[1][3] = 0;
		m[2][0] = 0; m[2][1] = 0; m[2][2] = 0; m[2][3] = 0;
		m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 0;
		return m;
	}

	mat4f identity() {
		mat4f m;
		m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
		m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
		m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
		m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
		return m;
	}

	mat4f multiply(const mat4f& a, const mat4f& b) {
		mat4f resultMatrix;

		for (int x = 0; x < 4; x++) {
			for (int j = 0; j < 4; j++) {
				int result = 0;
				for (int y = 0; y < 4; y++) {
					result += a[x][y] * b[y][j];
				}
				resultMatrix[x][j] = result;
			}
		}

		return resultMatrix;
	}

	mat4f ortho(float width, float height, float nearPlane, float farPlane) {
		mat4f result = identity();

		result[0][0] = 2.0f / width;
		result[1][1] = 2.0f / height;
		result[2][2] = 1.0f / (farPlane - nearPlane);
		result[2][3] = nearPlane / (nearPlane - farPlane);
		return result;
	}

	mat4f ortho(const vec2f& dimension, const vec2f& near_far_planes) {
		return ortho(dimension.x, dimension.y, near_far_planes.x, near_far_planes.y);
	}

	mat4f rotate_z(float angle_rad)
	{
		mat4f result = identity();

		float cos = std::cosf(angle_rad);
		float sin = std::sinf(angle_rad);
		result[0][0] = cos;
		result[0][1] = -sin;
		result[1][0] = sin;
		result[1][1] = cos;
		return result;
	}
}