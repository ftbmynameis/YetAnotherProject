#include "mat4.hpp"

#include <cmath>
#include "vec.hpp"
#include "utility.hpp"

using namespace vec;

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

	mat4f multiply(const mat4f& lhs, const mat4f& rhs) {
		mat4f resultMatrix;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				resultMatrix[i][j] = 0;
				for (int k = 0; k < 4; k++)
				{
					resultMatrix[i][j] += lhs[i][k] * rhs[k][j];
				}
			}
		}

		return resultMatrix;
	}

	vec4f multiply(const mat4f& mat, const vec4f& vec) {
		vec4f result;
		result.x = mat[0][0] * vec.x + mat[0][1] * vec.y + mat[0][2] * vec.z + mat[0][3] * vec.w;
		result.y = mat[1][0] * vec.x + mat[1][1] * vec.y + mat[1][2] * vec.z + mat[1][3] * vec.w;
		result.z = mat[2][0] * vec.x + mat[2][1] * vec.y + mat[2][2] * vec.z + mat[2][3] * vec.w;
		result.w = mat[3][0] * vec.x + mat[3][1] * vec.y + mat[3][2] * vec.z + mat[3][3] * vec.w;
		return result;
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

	mat4f rotate_z(float angle_in_rad)
	{
		mat4f result = identity();

		float cos = std::cosf(angle_in_rad);
		float sin = std::sinf(angle_in_rad);
		result[0][0] = cos;
		result[0][1] = -sin;
		result[1][0] = sin;
		result[1][1] = cos;
		return result;
	}

	mat4f proj(float fov, float aspect, float near_plane, float far_plane)
	{
		float y_scale = util::math::cot(fov / 2.0f);
		float x_scale = y_scale / aspect;

		auto result = zeroed();
		result[0][0] = x_scale;
		result[1][1] = y_scale;
		result[2][2] = far_plane / (far_plane - near_plane);
		result[2][3] = 1.0f;
		result[3][2] = -near_plane * far_plane / (far_plane - near_plane);

		return result;
	}

	mat4f look_at(vec3f eye, vec3f at, vec3f up)
	{
		auto az = normalice(at - eye);
		auto ax = normalice(cross(up, az));
		auto ay = cross(az, ax);

		mat4f m = identity();

		m[0][0] = ax.x; m[0][1] = ay.x; m[0][2] = az.x;
		m[1][0] = ax.y; m[1][1] = ay.y; m[1][2] = az.y;
		m[2][0] = ax.z; m[2][1] = ay.z; m[2][2] = az.z;
		m[3][0] = -dot(ax, eye); m[3][1] = -dot(ay, eye); m[3][2] = -dot(az, eye);

		return m;
	}
}

mat4f operator * (const mat4f& lhs, const mat4f& rhs)
{
	return mat::multiply(lhs, rhs);
}

vec4f operator * (const mat4f& m, const vec4f& v)
{
	return mat::multiply(m, v);
}