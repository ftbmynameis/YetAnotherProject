#ifndef VEC_INCLUDED
#define VEC_INCLUDED

#pragma once

template <typename Type, unsigned int Dimension>
union vec_type
{
	Type data[Dimension];
};

template <typename T>
union vec_type<T, 2>
{
	T data[2];
	struct { T x, y; };

	vec_type()
		: x(T()), y(T())
	{
	}

	vec_type(const vec_type& rhs)
		: x(rhs.x), y(rhs.y)
	{
	}

	vec_type(T _x, T _y)
		: x(_x), y(_y)
	{
	}
};

template <typename T>
union vec_type<T, 3>
{
	T data[3];
	struct { T x, y, z; };

	vec_type()
		: x(T()), y(T()), z(T())
	{
	}

	vec_type(const vec_type& rhs)
		: x(rhs.x), y(rhs.y), z(rhs.z)
	{
	}

	vec_type(T _x, T _y, T _z)
		: x(_x), y(_y), z(_z)
	{
	}
};

template <typename T>
union vec_type<T, 4>
{
	T data[4];
	struct { T x, y, z, w; };

	vec_type()
		: x(T()), y(T()), z(T()), w(T())
	{
	}

	vec_type(const vec_type& rhs)
		: x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
	{
	}

	vec_type(T _x, T _y, T _z, T _w)
		: x(_x), y(_y), z(_z), w(_w)
	{
	}
};

namespace vec {
	template <typename Type, unsigned int Dimension>
	float length_sq(const vec_type<Type, Dimension>& v)
	{
		Type result = 0;
		for (unsigned int i = 0; i < Dimension; ++i)
			result += v.data[i] * v.data[i];
		return result;
	}

	template <typename Type, unsigned int Dimension>
	float length(const vec_type<Type, Dimension>& v)
	{
		auto l = length_sq(v);
		return powf(l, 0.5f);
	}

	template <typename Type, unsigned int Dimension>
	vec_type<Type, Dimension> normalice(const vec_type<Type, Dimension>& v)
	{
		vec_type<Type, Dimension> result = v;
		float l = length(v);
		for (unsigned int i = 0; i < Dimension; ++i)
			result.data[i] /= l;
		return result;
	}

	template <typename Type, unsigned int Dimension>
	Type dot(const vec_type<Type, Dimension>& a, const vec_type<Type, Dimension>& b)
	{
		Type result = 0;
		for (unsigned int i = 0; i < Dimension; ++i)
			result += a.data[i] * b.data[i];
		return result;
	}

	// vec2

	template <typename Type>
	Type cross(const vec_type<Type, 2>& a, const vec_type<Type, 2>& b)
	{
		return a.x * b.y - a.y * b.x;
	}

	// vec3
	template <typename Type>
	vec_type<Type, 3> cross(const vec_type<Type, 3>& a, const vec_type<Type, 3>& b)
	{
		vec_type<Type, 3> result;
		result.x = a.y * b.z - a.z * b.y;
		result.y = a.z * b.x - a.x * b.z;
		result.z = a.x * b.y - a.y * b.x;
		return result;
	}
}

template <typename Type, unsigned int Dimension>
vec_type<Type, Dimension>& operator += (vec_type<Type, Dimension>& lhs, const vec_type<Type, Dimension>& rhs)
{
	for (unsigned int i = 0; i < Dimension; ++i)
		lhs.data[i] = lhs.data[i] + rhs.data[i];
	return lhs;
}

template <typename Type, unsigned int Dimension>
vec_type<Type, Dimension>& operator -= (vec_type<Type, Dimension>& lhs, const vec_type<Type, Dimension>& rhs)
{
	for (unsigned int i = 0; i < Dimension; ++i)
		lhs.data[i] = lhs.data[i] - rhs.data[i];
	return lhs;
}

template <typename Type, unsigned int Dimension>
vec_type<Type, Dimension>& operator *= (vec_type<Type, Dimension>& lhs, const float rhs)
{
	for (unsigned int i = 0; i < Dimension; ++i)
		lhs.data[i] = lhs.data[i] * rhs;
	return lhs;
}

template <typename Type, unsigned int Dimension>
bool operator == (const vec_type<Type, Dimension>& a, const vec_type<Type, Dimension>& b)
{
	for (unsigned int i = 0; i < Dimension; ++i)
		if (a.data[i] != b.data[i])
			return false;
	return true;
}

template <typename Type, unsigned int Dimension>
bool operator != (const vec_type<Type, Dimension>& a, const vec_type<Type, Dimension>& b)
{
	return !(a == b);
}

template <typename Type, unsigned int Dimension>
vec_type<Type, Dimension> operator + (const vec_type<Type, Dimension>& a, const vec_type<Type, Dimension>& b)
{
	vec_type<Type, Dimension> c;
	for (unsigned int i = 0; i < Dimension; ++i)
		c.data[i] = a.data[i] + b.data[i];
	return c;
}

template <typename Type, unsigned int Dimension>
vec_type<Type, Dimension> operator - (const vec_type<Type, Dimension>& a, const vec_type<Type, Dimension>& b)
{
	vec_type<Type, Dimension> c;
	for (unsigned int i = 0; i < Dimension; ++i)
		c.data[i] = a.data[i] - b.data[i];
	return c;
}

template <typename Type, unsigned int Dimension>
vec_type<Type, Dimension> operator - (const vec_type<Type, Dimension>& a)
{
	vec_type<Type, Dimension> c;
	for (unsigned int i = 0; i < Dimension; ++i)
		c.data[i] = -a.data[i];
	return c;
}

template <typename Type, unsigned int Dimension>
vec_type<Type, Dimension> operator * (const vec_type<Type, Dimension>& v, float f)
{
	vec_type<Type, Dimension> c;
	for (unsigned int i = 0; i < Dimension; ++i)
		c.data[i] = v.data[i] * f;
	return c;
}

template <typename Type, unsigned int Dimension>
vec_type<Type, Dimension> operator * (float f, const vec_type<Type, Dimension>& v)
{
	return v * f;
}

typedef vec_type<float, 2> vec2f;
typedef vec_type<double, 2> vec2d;

typedef vec_type<float, 3> vec3f;
typedef vec_type<double, 3> vec3d;
typedef vec_type<float, 4> vec4f;
typedef vec_type<double, 4> vec4d;

#endif