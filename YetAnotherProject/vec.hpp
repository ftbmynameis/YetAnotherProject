#ifndef VEC_INCLUDED
#define VEC_INCLUDED

#pragma once

template <typename Type, unsigned int Dimension>
union vec
{
	Type data[Dimension];
};

template <typename T>
union vec<T, 2>
{
	T data[2];
	struct { T x, y; };

	vec()
		: x(T()), y(T())
	{
	}

	vec(const vec& rhs)
		: x(rhs.x), y(rhs.y)
	{
	}

	vec(T _x, T _y)
		: x(_x), y(_y)
	{
	}
};

template <typename T>
union vec<T, 3>
{
	T data[3];
	struct { T x, y, z; };

	vec()
		: x(T()), y(T()), z(T())
	{
	}

	vec(const vec& rhs)
		: x(rhs.x), y(rhs.y), z(rhs.z)
	{
	}

	vec(T _x, T _y, T _z)
		: x(_x), y(_y), z(_z)
	{
	}
};

template <typename Type, unsigned int Dimension>
vec<Type, Dimension>& operator += (vec<Type, Dimension>& lhs, const vec<Type, Dimension>& rhs)
{
	for (unsigned int i = 0; i < Dimension; ++i)
		lhs.data[i] = lhs.data[i] + rhs.data[i];
	return lhs;
}

template <typename Type, unsigned int Dimension>
vec<Type, Dimension>& operator -= (vec<Type, Dimension>& lhs, const vec<Type, Dimension>& rhs)
{
	for (unsigned int i = 0; i < Dimension; ++i)
		lhs.data[i] = lhs.data[i] - rhs.data[i];
	return lhs;
}

template <typename Type, unsigned int Dimension>
vec<Type, Dimension>& operator *= (vec<Type, Dimension>& lhs, const float rhs)
{
	for (unsigned int i = 0; i < Dimension; ++i)
		lhs.data[i] = lhs.data[i] * rhs;
	return lhs;
}

template <typename Type, unsigned int Dimension>
bool operator == (const vec<Type, Dimension>& a, const vec<Type, Dimension>& b)
{
	for (unsigned int i = 0; i < Dimension; ++i)
		if (a.data[i] != b.data[i])
			return false;
	return true;
}

template <typename Type, unsigned int Dimension>
bool operator != (const vec<Type, Dimension>& a, const vec<Type, Dimension>& b)
{
	return !(a == b);
}

template <typename Type, unsigned int Dimension>
vec<Type, Dimension> operator + (const vec<Type, Dimension>& a, const vec<Type, Dimension>& b)
{
	vec<Type, Dimension> c;
	for (unsigned int i = 0; i < Dimension; ++i)
		c.data[i] = a.data[i] + b.data[i];
	return c;
}

template <typename Type, unsigned int Dimension>
vec<Type, Dimension> operator - (const vec<Type, Dimension>& a, const vec<Type, Dimension>& b)
{
	vec<Type, Dimension> c;
	for (unsigned int i = 0; i < Dimension; ++i)
		c.data[i] = a.data[i] - b.data[i];
	return c;
}

template <typename Type, unsigned int Dimension>
vec<Type, Dimension> operator - (const vec<Type, Dimension>& a)
{
	vec<Type, Dimension> c;
	for (unsigned int i = 0; i < Dimension; ++i)
		c.data[i] = -a.data[i];
	return c;
}

template <typename Type, unsigned int Dimension>
vec<Type, Dimension> operator * (const vec<Type, Dimension>& v, float f)
{
	vec<Type, Dimension> c;
	for (unsigned int i = 0; i < Dimension; ++i)
		c.data[i] = v.data[i] * f;
	return c;
}

template <typename Type, unsigned int Dimension>
vec<Type, Dimension> operator * (float f, const vec<Type, Dimension>& v)
{
	return v * f;
}

//
// common vector functions
//

template <typename Type, unsigned int Dimension>
float LengthSquare(const vec<Type, Dimension>& v)
{
	Type result = 0;
	for (unsigned int i = 0; i < Dimension; ++i)
		result += v.data[i] * v.data[i];
	return result;
}

template <typename Type, unsigned int Dimension>
float Length(const vec<Type, Dimension>& v)
{
	return pow(LengthSquare(v), 0.5f);
}

template <typename Type, unsigned int Dimension>
vec<Type, Dimension> Normalice(const vec<Type, Dimension>& v)
{
	vec<Type, Dimension> result = v;
	float length = Length(v);
	for (unsigned int i = 0; i < Dimension; ++i)
		result.data[i] /= length;
	return result;
}

template <typename Type, unsigned int Dimension>
Type DotProduct(const vec<Type, Dimension>& a, const vec<Type, Dimension>& b)
{
	Type result = 0;
	for (unsigned int i = 0; i < Dimension; ++i)
		result += a[i] * b[i];
	return result;
}

// vec2

template <typename Type>
Type CrossProduct(const vec<Type, 2>& a, const vec<Type, 2>& b)
{
	return a.x * b.y - a.y * b.x;
}

typedef vec<float, 2> vec2f;
typedef vec<double, 2> vec2d;

// vec3
template <typename Type>
vec<Type, 3> CrossProduct(const vec<Type, 3>& a, const vec<Type, 3>& b)
{
	vec<Type, 3> result;
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
	return result;
}

typedef vec<float, 3> vec3f;
typedef vec<double, 3> vec3d;

#endif