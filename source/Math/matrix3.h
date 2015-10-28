#ifndef MATRIX3_CLASS_H
#define MATRIX3_CLASS_H

#include "..\Core\stdafx.h"

#include <algorithm>
#include <cmath>
#include "vec3.h"

template <typename T>
struct matrix3
{
	typedef T element_type;
	typedef matrix3 &reference;
	typedef matrix3 const &const_reference;
	typedef matrix3 blas_class;

	union
	{
		element_type m[3][3];
		element_type ma[9];
		struct
		{
			element_type xx, xy, xz;
			element_type yx, yy, yz;
			element_type zx, zy, zz;
		};
		struct
		{
			vec3<element_type> axis_x;
			vec3<element_type> axis_y;
			vec3<element_type> axis_z;
		};
	};

	matrix3(void) { }
	explicit matrix3(element_type n);

	template < typename VT >
	matrix3(const vec3<VT> &v0,
			  const vec3<VT> &v1,
			  const vec3<VT> &v2);

	matrix3(element_type m00, element_type m01, element_type m02,
			  element_type m10, element_type m11, element_type m12,
			  element_type m20, element_type m21, element_type m22);

	// TODO: I'm only pretty sure this should be explicit
	// I think this counts as a one argument constructor, not as a copy constructor.
	// The question is, does this replace the copy constructor when T == MT ?
	template <typename MT>
	explicit matrix3(const matrix3<MT> &m);

	template < typename VT >
	matrix3(const vec3<VT> &diag, const vec3<VT> &sym);

	// TODO: is this any faster than the built in one?
	// TODO: does this work with UDTs that are POD and if so,
	// is that desired and defined behavior?
	template <typename MT>
	reference operator=(const matrix3<MT> &m);

   inline T operator[](size_t index)
   {
	   return ma[index];
   }

   inline const T operator[](size_t index) const
   {
	   return ma[index];
   }

	template <typename MT>
   matrix3<T> operator*(const matrix3<MT> &rhs);

	template <typename MT>
	reference operator*=(const matrix3<MT> &rhs);

	reference make_zero(void);
	reference make_identity(void);

	// "this" BECOMES the specified rotation matrix
	// make_rotation_ does NOT rotate "this", it constructs
	// a rotation matrix of 'radians' on the specified axis.
	reference make_rotation_x(element_type radians);
	reference make_rotation_y(element_type radians);
	reference make_rotation_z(element_type radians);

	// pre is rotation * "this"
	reference rotate_x_pre(element_type radians);
	reference rotate_y_pre(element_type radians);
	reference rotate_z_pre(element_type radians);

	// post is "this" * rotation
	reference rotate_x_post(element_type radians);
	reference rotate_y_post(element_type radians);
	reference rotate_z_post(element_type radians);

	reference invert(void);
	reference transpose(void);
	reference negate(void);

	template <typename VT>
	reference skew(const vec3<VT> &v);
};

template <typename T>
template <typename VT>
matrix3<T>::matrix3(const vec3<VT> &v0, const vec3<VT> &v1, const vec3<VT> &v2)
:	xx(v0.x), xy(v0.y), xz(v0.z),
	yx(v1.x), yy(v1.y), yz(v1.z),
	zx(v2.x), zy(v2.y), zz(v2.z)
{
}

template <typename T>
matrix3<T>::matrix3(element_type n)
:	xx(n), xy(n), xz(n),
	yx(n), yy(n), yz(n),
	zx(n), zy(n), zz(n)
{
}

template <typename T>
matrix3<T>::matrix3(element_type m00, element_type m01, element_type m02,
						  element_type m10, element_type m11, element_type m12,
						  element_type m20, element_type m21, element_type m22)
:	xx(m00), xy(m01), xz(m02),
	yx(m10), yy(m11), yz(m12),
	zx(m20), zy(m21), zz(m22)
{
}

template <typename T>
template <typename MT>
matrix3<T>::matrix3(const matrix3<MT> &m)
:	xx(m.xx), xy(m.xy), xz(m.xz),
	yx(m.yx), yy(m.yy), yz(m.yz),
	zx(m.zx), zy(m.zy), zz(m.zz)
{
}

template <typename T>
template <typename VT>
matrix3<T>::matrix3(const vec3<VT> &diag, const vec3<VT> &sym)
{
	make_symmetric(this, diag, sym);
}

template <typename T>
template <typename MT>
typename matrix3<T>::reference matrix3<T>::operator=(const matrix3<MT> &m)
{
	if (static_cast<void *>(this) != static_cast<const void *>(&m))
	{
		xx = m.xx;	xy = m.xy;	xz = m.xz;
		yx = m.yx;	yy = m.yy;	yz = m.yz;
		zx = m.zx;	zy = m.zy;	zz = m.zz;
	}
	return *this;
}

template <typename T>
typename matrix3<T>::reference matrix3<T>::make_zero(void)
{
	xx = 0;	xy = 0;	xz = 0;
	yx = 0;	yy = 0;	yz = 0;
	zx = 0;	zy = 0;	zz = 0;
	return *this;
}

template <typename T>
typename matrix3<T>::reference matrix3<T>::make_identity(void)
{
	xx = 1;	xy = 0;	xz = 0;
	yx = 0;	yy = 1;	yz = 0;
	zx = 0;	zy = 0;	zz = 1;
	return *this;
}

template <typename T>
typename matrix3<T>::reference matrix3<T>::negate(void)
{
	*this *= -1;
	return *this;
}

template <typename T>
typename matrix3<T>::reference matrix3<T>::make_rotation_x(element_type radians)
{
	element_type sin_angle, cos_angle;
	sin_angle = std::sin(radians);
	cos_angle = std::cos(radians);

	yy = cos_angle;
	yz = sin_angle;
	zy = -sin_angle;
	zz = cos_angle;
	xx = 1;
	yx = zx = xy = xz = 0;
	return *this;
}

template <typename T>
typename matrix3<T>::reference matrix3<T>::make_rotation_y(element_type radians)
{
	element_type sin_angle, cos_angle;
	sin_angle = std::sin(radians);
	cos_angle = std::cos(radians);

	xx = cos_angle;
	xz = -sin_angle;
	zx = sin_angle;
	zz = cos_angle;
	yy = 1;
	xy = zy = yx = yz = 0;
	return *this;
}

template <typename T>
typename matrix3<T>::reference matrix3<T>::make_rotation_z(element_type radians)
{
	element_type sin_angle, cos_angle;
	sin_angle = std::sin(radians);
	cos_angle = std::cos(radians);

	xx = cos_angle;
	xy = sin_angle;
	yx = -sin_angle;
	yy = cos_angle;
	zz = 1;
	xz = zz = zx = zz = 0;
	return *this;
}

template <typename T>
template <typename VT>
typename matrix3<T>::reference matrix3<T>::skew(const vec3<VT> &v)
{
	xx = yy = zz = 0;
	zy =  v.x;
	yz = -v.x;
	xz =  v.y;
	zx = -v.y;
	yx =  v.z;
	xy = -v.z;
	return *this;
}

template <typename T>
typename matrix3<T>::reference matrix3<T>::transpose(void)
{
	std::swap(xy, yx);
	std::swap(yz, zy);
	std::swap(xz, zx);
	return *this;
}

// TODO: how is this different from transpose?
// Should it be different?
// Which implementation is faster?
template <typename T>
typename matrix3<T>::reference matrix3<T>::invert(void)
{
	operator =(xx, yx, zx,
				  xy, yy, zy,
				  xz, yz, zz);
	return *this;
}

template <typename T>
typename matrix3<T>::reference matrix3<T>::rotate_x_pre(element_type radians)
{
	*this = matrix3<T>().make_rotation_x(radians) * (*this);
	return *this;
}

template <typename T>
typename matrix3<T>::reference matrix3<T>::rotate_y_pre(element_type radians)
{
	*this = matrix3<T>().make_rotation_y(radians) * (*this);
	return *this;
}

template <typename T>
typename matrix3<T>::reference matrix3<T>::rotate_z_pre(element_type radians)
{
	*this = matrix3<T>().make_rotation_z(radians) * (*this);
	return *this;
}

template <typename T>
typename matrix3<T>::reference matrix3<T>::rotate_x_post(element_type radians)
{
	*this *= matrix3<T>().make_rotation_x(radians);
	return *this;
}

template <typename T>
typename matrix3<T>::reference matrix3<T>::rotate_y_post(element_type radians)
{
	*this *= matrix3<T>().make_rotation_y(radians);
	return *this;
}

template <typename T>
typename matrix3<T>::reference matrix3<T>::rotate_z_post(element_type radians)
{
	*this *= matrix3<T>().make_rotation_z(radians);
	return *this;
}

template <typename T>
template <typename MT>
typename matrix3<T> matrix3<T>::operator*(const matrix3<MT> &rhs)
{
	matrix3<T> temp(xx * rhs.xx + yx * rhs.xy + zx * rhs.xz,
						 xy * rhs.xx + yy * rhs.xy + zy * rhs.xz,
						 xz * rhs.xx + yz * rhs.xy + zz * rhs.xz,

						 xx * rhs.yx + yx * rhs.yy + zx * rhs.yz,
						 xy * rhs.yx + yy * rhs.yy + zy * rhs.yz,
						 xz * rhs.yx + yz * rhs.yy + zz * rhs.yz,

						 xx * rhs.zx + yx * rhs.zy + zx * rhs.zz,
						 xy * rhs.zx + yy * rhs.zy + zy * rhs.zz,
						 xz * rhs.zx + yz * rhs.zy + zz * rhs.zz);
   return(temp);
}

template <typename T>
template <typename MT>
typename matrix3<T>::reference matrix3<T>::operator*=(const matrix3<MT> &rhs)
{
   *this = *this * rhs;
	return *this;
}

typedef matrix3<float> matrix3f;
typedef matrix3<double> matrix3d;
typedef matrix3f float3x3;
typedef matrix3d double3x3;

#endif
