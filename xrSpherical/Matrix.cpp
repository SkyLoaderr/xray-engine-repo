#include "stdafx.h"

#include "Matrix.h"

#include "Point.h"
#include "Quaternion.h"

#include <math.h>

namespace MiniBall
{
	Matrix Matrix::diag(float m11, float m22, float m33, float m44)
	{
		return Matrix(m11, 0,   0,   0,
		              0,   m22, 0,   0,
		              0,   0,   m33, 0,
		              0,   0,   0,   m44);
	}

	Matrix::operator float*()
	{
		return &(*this)(1, 1);
	}

	Matrix Matrix::operator+() const
	{
		return *this;
	}

	Matrix Matrix::operator-() const
	{
		const Matrix &M = *this;

		return Matrix(-M(1, 1), -M(1, 2), -M(1, 3), -M(1, 4), 
		              -M(2, 1), -M(2, 2), -M(2, 3), -M(2, 4), 
		              -M(3, 1), -M(3, 2), -M(3, 3), -M(3, 4), 
		              -M(4, 1), -M(4, 2), -M(4, 3), -M(4, 4));
	}

	Matrix Matrix::operator!() const
	{
		const Matrix &M = *this;
		Matrix I;

		float M3344 = M(3, 3) * M(4, 4) - M(4, 3) * M(3, 4);
		float M2344 = M(2, 3) * M(4, 4) - M(4, 3) * M(2, 4);
		float M2334 = M(2, 3) * M(3, 4) - M(3, 3) * M(2, 4);
		float M3244 = M(3, 2) * M(4, 4) - M(4, 2) * M(3, 4);
		float M2244 = M(2, 2) * M(4, 4) - M(4, 2) * M(2, 4);
		float M2234 = M(2, 2) * M(3, 4) - M(3, 2) * M(2, 4);
		float M3243 = M(3, 2) * M(4, 3) - M(4, 2) * M(3, 3);
		float M2243 = M(2, 2) * M(4, 3) - M(4, 2) * M(2, 3);
		float M2233 = M(2, 2) * M(3, 3) - M(3, 2) * M(2, 3);
		float M1344 = M(1, 3) * M(4, 4) - M(4, 3) * M(1, 4);
		float M1334 = M(1, 3) * M(3, 4) - M(3, 3) * M(1, 4);
		float M1244 = M(1, 2) * M(4, 4) - M(4, 2) * M(1, 4);
		float M1234 = M(1, 2) * M(3, 4) - M(3, 2) * M(1, 4);
		float M1243 = M(1, 2) * M(4, 3) - M(4, 2) * M(1, 3);
		float M1233 = M(1, 2) * M(3, 3) - M(3, 2) * M(1, 3);
		float M1324 = M(1, 3) * M(2, 4) - M(2, 3) * M(1, 4);
		float M1224 = M(1, 2) * M(2, 4) - M(2, 2) * M(1, 4);
		float M1223 = M(1, 2) * M(2, 3) - M(2, 2) * M(1, 3);

		// Adjoint Matrix
		I(1, 1) =  M(2, 2) * M3344 - M(3, 2) * M2344 + M(4, 2) * M2334;
		I(2, 1) = -M(2, 1) * M3344 + M(3, 1) * M2344 - M(4, 1) * M2334;
		I(3, 1) =  M(2, 1) * M3244 - M(3, 1) * M2244 + M(4, 1) * M2234;
		I(4, 1) = -M(2, 1) * M3243 + M(3, 1) * M2243 - M(4, 1) * M2233;

		I(1, 2) = -M(1, 2) * M3344 + M(3, 2) * M1344 - M(4, 2) * M1334;
		I(2, 2) =  M(1, 1) * M3344 - M(3, 1) * M1344 + M(4, 1) * M1334;
		I(3, 2) = -M(1, 1) * M3244 + M(3, 1) * M1244 - M(4, 1) * M1234;
		I(4, 2) =  M(1, 1) * M3243 - M(3, 1) * M1243 + M(4, 1) * M1233;

		I(1, 3) =  M(1, 2) * M2344 - M(2, 2) * M1344 + M(4, 2) * M1324;
		I(2, 3) = -M(1, 1) * M2344 + M(2, 1) * M1344 - M(4, 1) * M1324;
		I(3, 3) =  M(1, 1) * M2244 - M(2, 1) * M1244 + M(4, 1) * M1224;
		I(4, 3) = -M(1, 1) * M2243 + M(2, 1) * M1243 - M(4, 1) * M1223;

		I(1, 4) = -M(1, 2) * M2334 + M(2, 2) * M1334 - M(3, 2) * M1324;
		I(2, 4) =  M(1, 1) * M2334 - M(2, 1) * M1334 + M(3, 1) * M1324;
		I(3, 4) = -M(1, 1) * M2234 + M(2, 1) * M1234 - M(3, 1) * M1224;
		I(4, 4) =  M(1, 1) * M2233 - M(2, 1) * M1233 + M(3, 1) * M1223;

		// Division by determinant
		I /= M(1, 1) * I(1, 1) +
		     M(2, 1) * I(1, 2) +
		     M(3, 1) * I(1, 3) +
		     M(4, 1) * I(1, 4);

		return I;
	}

	Matrix Matrix::operator~() const
	{
		const Matrix &M = *this;

		return Matrix(M(1, 1), M(2, 1), M(3, 1), M(4, 1), 
		              M(1, 2), M(2, 2), M(3, 2), M(4, 2), 
		              M(1, 3), M(2, 3), M(3, 3), M(4, 3), 
		              M(1, 4), M(2, 4), M(3, 4), M(4, 4));
	}

	Matrix &Matrix::operator+=(const Matrix &N)
	{
		Matrix &M = *this;

		M(1, 1) += N(1, 1); M(1, 2) += N(1, 2); M(1, 3) += N(1, 3); M(1, 4) += N(1, 4);
		M(2, 1) += N(2, 1); M(2, 2) += N(2, 2); M(2, 3) += N(2, 3); M(2, 4) += N(2, 4);
		M(3, 1) += N(3, 1); M(3, 2) += N(3, 2); M(3, 3) += N(3, 3); M(3, 4) += N(3, 4);
		M(4, 1) += N(4, 1); M(4, 2) += N(4, 2); M(4, 3) += N(4, 3); M(4, 4) += N(4, 4);

		return M;
	}

	Matrix &Matrix::operator-=(const Matrix &N)
	{
		Matrix &M = *this;

		M(1, 1) -= N(1, 1); M(1, 2) -= N(1, 2); M(1, 3) -= N(1, 3); M(1, 4) -= N(1, 4);
		M(2, 1) -= N(2, 1); M(2, 2) -= N(2, 2); M(2, 3) -= N(2, 3); M(2, 4) -= N(2, 4);
		M(3, 1) -= N(3, 1); M(3, 2) -= N(3, 2); M(3, 3) -= N(3, 3); M(3, 4) -= N(3, 4);
		M(4, 1) -= N(4, 1); M(4, 2) -= N(4, 2); M(4, 3) -= N(4, 3); M(4, 4) -= N(4, 4);

		return M;
	}

	Matrix &Matrix::operator*=(float s)
	{
		Matrix &M = *this;

		M(1, 1) *= s; M(1, 2) *= s; M(1, 3) *= s; M(1, 4) *= s;
		M(2, 1) *= s; M(2, 2) *= s; M(2, 3) *= s; M(2, 4) *= s;
		M(3, 1) *= s; M(3, 2) *= s; M(3, 3) *= s; M(3, 4) *= s;
		M(4, 1) *= s; M(4, 2) *= s; M(4, 3) *= s; M(4, 4) *= s;

		return M;
	}

	Matrix &Matrix::operator*=(const Matrix &M)
	{
		return *this = *this * M;
	}

	Matrix &Matrix::operator/=(float s)
	{
		float r = 1.0f / s;

		return *this *= r;
	}

	bool operator==(const Matrix &M, const Matrix &N)
	{
		if(M(1, 1) == N(1, 1) && M(1, 2) == N(1, 2) && M(1, 3) == N(1, 3) && M(1, 4) == N(1, 4) &&
		   M(2, 1) == N(2, 1) && M(2, 2) == N(2, 2) && M(2, 3) == N(2, 3) && M(2, 4) == N(2, 4) &&
		   M(3, 1) == N(3, 1) && M(3, 2) == N(3, 2) && M(3, 3) == N(3, 3) && M(3, 4) == N(3, 4) &&
		   M(4, 1) == N(4, 1) && M(4, 2) == N(4, 2) && M(4, 3) == N(4, 3) && M(4, 4) == N(4, 4))
			return true;
		else
			return false;
	}

	bool operator!=(const Matrix &M, const Matrix &N)
	{
		if(M(1, 1) != N(1, 1) || M(1, 2) != N(1, 2) || M(1, 3) != N(1, 3) || M(1, 4) != N(1, 4) ||
		   M(2, 1) != N(2, 1) || M(2, 2) != N(2, 2) || M(2, 3) != N(2, 3) || M(2, 4) != N(2, 4) ||
		   M(3, 1) != N(3, 1) || M(3, 2) != N(3, 2) || M(3, 3) != N(3, 3) || M(3, 4) != N(3, 4) ||
		   M(4, 1) != N(4, 1) || M(4, 2) != N(4, 2) || M(4, 3) != N(4, 3) || M(4, 4) != N(4, 4))
			return true;
		else
			return false;
	}

	Matrix operator+(const Matrix &M, const Matrix &N)
	{
		return Matrix(M(1, 1) + N(1, 1), M(1, 2) + N(1, 2), M(1, 3) + N(1, 3), M(1, 4) + N(1, 4), 
		              M(2, 1) + N(2, 1), M(2, 2) + N(2, 2), M(2, 3) + N(2, 3), M(2, 4) + N(2, 4), 
		              M(3, 1) + N(3, 1), M(3, 2) + N(3, 2), M(3, 3) + N(3, 3), M(3, 4) + N(3, 4), 
		              M(4, 1) + N(4, 1), M(4, 2) + N(4, 2), M(4, 3) + N(4, 3), M(4, 4) + N(4, 4));
	}

	Matrix operator-(const Matrix &M, const Matrix &N)
	{
		return Matrix(M(1, 1) - N(1, 1), M(1, 2) - N(1, 2), M(1, 3) - N(1, 3), M(1, 4) - N(1, 4), 
		              M(2, 1) - N(2, 1), M(2, 2) - N(2, 2), M(2, 3) - N(2, 3), M(2, 4) - N(2, 4), 
		              M(3, 1) - N(3, 1), M(3, 2) - N(3, 2), M(3, 3) - N(3, 3), M(3, 4) - N(3, 4), 
		              M(4, 1) - N(4, 1), M(4, 2) - N(4, 2), M(4, 3) - N(4, 3), M(4, 4) - N(4, 4));
	}

	Matrix operator*(float s, const Matrix &M)
	{
		return Matrix(s * M(1, 1), s * M(1, 2), s * M(1, 3), s * M(1, 4), 
		              s * M(2, 1), s * M(2, 2), s * M(2, 3), s * M(2, 4), 
		              s * M(3, 1), s * M(3, 2), s * M(3, 3), s * M(3, 4), 
		              s * M(4, 1), s * M(4, 2), s * M(4, 3), s * M(4, 4));
	}

	Matrix operator*(const Matrix &M, float s)
	{
		return Matrix(M(1, 1) * s, M(1, 2) * s, M(1, 3) * s, M(1, 4) * s, 
		              M(2, 1) * s, M(2, 2) * s, M(2, 3) * s, M(2, 4) * s, 
		              M(3, 1) * s, M(3, 2) * s, M(3, 3) * s, M(3, 4) * s, 
		              M(4, 1) * s, M(4, 2) * s, M(4, 3) * s, M(4, 4) * s);
	}

	Matrix operator*(const Matrix &M, const Matrix &N)
	{
		return Matrix(M(1, 1) * N(1, 1) + M(1, 2) * N(2, 1) + M(1, 3) * N(3, 1) + M(1, 4) * N(4, 1), M(1, 1) * N(1, 2) + M(1, 2) * N(2, 2) + M(1, 3) * N(3, 2) + M(1, 4) * N(4, 2), M(1, 1) * N(1, 3) + M(1, 2) * N(2, 3) + M(1, 3) * N(3, 3) + M(1, 4) * N(4, 3), M(1, 1) * N(1, 4) + M(1, 2) * N(2, 4) + M(1, 3) * N(3, 4) + M(1, 4) * N(4, 4), 
		              M(2, 1) * N(1, 1) + M(2, 2) * N(2, 1) + M(2, 3) * N(3, 1) + M(2, 4) * N(4, 1), M(2, 1) * N(1, 2) + M(2, 2) * N(2, 2) + M(2, 3) * N(3, 2) + M(2, 4) * N(4, 2), M(2, 1) * N(1, 3) + M(2, 2) * N(2, 3) + M(2, 3) * N(3, 3) + M(2, 4) * N(4, 3), M(2, 1) * N(1, 4) + M(2, 2) * N(2, 4) + M(2, 3) * N(3, 4) + M(2, 4) * N(4, 4), 
		              M(3, 1) * N(1, 1) + M(3, 2) * N(2, 1) + M(3, 3) * N(3, 1) + M(3, 4) * N(4, 1), M(3, 1) * N(1, 2) + M(3, 2) * N(2, 2) + M(3, 3) * N(3, 2) + M(3, 4) * N(4, 2), M(3, 1) * N(1, 3) + M(3, 2) * N(2, 3) + M(3, 3) * N(3, 3) + M(3, 4) * N(4, 3), M(3, 1) * N(1, 4) + M(3, 2) * N(2, 4) + M(3, 3) * N(3, 4) + M(3, 4) * N(4, 4), 
		              M(4, 1) * N(1, 1) + M(4, 2) * N(2, 1) + M(4, 3) * N(3, 1) + M(4, 4) * N(4, 1), M(4, 1) * N(1, 2) + M(4, 2) * N(2, 2) + M(4, 3) * N(3, 2) + M(4, 4) * N(4, 2), M(4, 1) * N(1, 3) + M(4, 2) * N(2, 3) + M(4, 3) * N(3, 3) + M(4, 4) * N(4, 3), M(4, 1) * N(1, 4) + M(4, 2) * N(2, 4) + M(4, 3) * N(3, 4) + M(4, 4) * N(4, 4));
	}

	Matrix operator/(const Matrix &M, float s)
	{
		float r = 1.0f / s;

		return M * s;
	}

	Vector Matrix::operator*(const Vector &v) const
	{
		const Matrix &M = *this;

		return Vector(M(1, 1) * v.x + M(1, 2) * v.y + M(1, 3) * v.z,
		              M(2, 1) * v.x + M(2, 2) * v.y + M(2, 3) * v.z,
		              M(3, 1) * v.x + M(3, 2) * v.y + M(3, 3) * v.z);
	}

	Point Matrix::operator*(const Point &P) const
	{
		const Matrix &M = *this;

		return Point(M(1, 1) * P.x + M(1, 2) * P.y + M(1, 3) * P.z + M(1, 4),
		             M(2, 1) * P.x + M(2, 2) * P.y + M(2, 3) * P.z + M(2, 4),
		             M(3, 1) * P.x + M(3, 2) * P.y + M(3, 3) * P.z + M(3, 4));
	}

	Quaternion Matrix::operator*(const Quaternion &Q) const
	{
		const Matrix &M = *this;

		return Quaternion(M(1, 1) * Q.x + M(1, 2) * Q.y + M(1, 3) * Q.z + M(1, 4) * Q.w,
		                  M(2, 1) * Q.x + M(2, 2) * Q.y + M(2, 3) * Q.z + M(2, 4) * Q.w,
		                  M(3, 1) * Q.x + M(3, 2) * Q.y + M(3, 3) * Q.z + M(3, 4) * Q.w,
		                  M(4, 1) * Q.x + M(4, 2) * Q.y + M(4, 3) * Q.z + M(4, 4) * Q.w);
	}

	Vector operator*(const Vector &v, const Matrix &M)
	{
		return Vector(v.x * M(1, 1) + v.y * M(2, 1) + v.z * M(3, 1) + M(4, 1),
		              v.x * M(1, 2) + v.y * M(2, 2) + v.z * M(3, 2) + M(4, 2),
		              v.x * M(1, 3) + v.y * M(2, 3) + v.z * M(3, 3) + M(4, 3));
	}

	Point operator*(const Point &P, const Matrix &M)
	{
		return Point(P.x * M(1, 1) + P.y * M(2, 1) + P.z * M(3, 1),
		             P.x * M(1, 2) + P.y * M(2, 2) + P.z * M(3, 2),
		             P.x * M(1, 3) + P.y * M(2, 3) + P.z * M(3, 3));
	}

	Quaternion operator*(const Quaternion &Q, const Matrix &M)
	{
		return Quaternion(Q.x * M(1, 1) + Q.y * M(2, 1) + Q.z + M(3, 1) + Q.w * M(4, 1),
		                  Q.x * M(1, 2) + Q.y * M(2, 2) + Q.z + M(3, 2) + Q.w * M(4, 2),
		                  Q.x * M(1, 3) + Q.y * M(2, 3) + Q.z + M(3, 3) + Q.w * M(4, 3),
		                  Q.x * M(1, 4) + Q.y * M(2, 4) + Q.z + M(3, 4) + Q.w * M(4, 4));
	}

	Vector &operator*=(Vector &v, const Matrix &M)
	{
		return v = v * M;
	}

	Point &operator*=(Point &P, const Matrix &M)
	{
		return P = P * M;
	}

	Quaternion &operator*=(Quaternion &Q, const Matrix &M)
	{
		return Q = Q * M;
	}

	float Matrix::det(const Matrix &M)
	{
		float M3344 = M(3, 3) * M(4, 4) - M(4, 3) * M(3, 4);
		float M2344 = M(2, 3) * M(4, 4) - M(4, 3) * M(2, 4);
		float M2334 = M(2, 3) * M(3, 4) - M(3, 3) * M(2, 4);
		float M1344 = M(1, 3) * M(4, 4) - M(4, 3) * M(1, 4);
		float M1334 = M(1, 3) * M(3, 4) - M(3, 3) * M(1, 4);
		float M1324 = M(1, 3) * M(2, 4) - M(2, 3) * M(1, 4);

		return M(1, 1) * (M(2, 2) * M3344 - M(3, 2) * M2344 + M(4, 2) * M2334) -
		       M(2, 1) * (M(1, 2) * M3344 - M(3, 2) * M1344 + M(4, 2) * M1334) +
		       M(3, 1) * (M(1, 2) * M2344 - M(2, 2) * M1344 + M(4, 2) * M1324) -
		       M(4, 1) * (M(1, 2) * M2334 - M(2, 2) * M1334 + M(3, 2) * M1324);
	}

	float Matrix::det(float m11)
	{
		return m11;
	}

	float Matrix::det(float m11, float m12, 
	                  float m21, float m22)
	{
		return m11 * m22 - m12 * m21; 
	}

	float Matrix::det(float m11, float m12, float m13, 
	                  float m21, float m22, float m23, 
	                  float m31, float m32, float m33)
	{
		return m11 * (m22 * m33 - m32 * m23) -
		       m21 * (m12 * m33 - m32 * m13) +
		       m31 * (m12 * m23 - m22 * m13);
	}

	float Matrix::det(float m11, float m12, float m13, float m14, 
	                  float m21, float m22, float m23, float m24, 
	                  float m31, float m32, float m33, float m34, 
	                  float m41, float m42, float m43, float m44)
	{
		float M3344 = m33 * m44 - m43 * m34;
		float M2344 = m23 * m44 - m43 * m24;
		float M2334 = m23 * m34 - m33 * m24;
		float M1344 = m13 * m44 - m43 * m14;
		float M1334 = m13 * m34 - m33 * m14;
		float M1324 = m13 * m24 - m23 * m14;

		return m11 * (m22 * M3344 - m32 * M2344 + m42 * M2334) -
		       m21 * (m12 * M3344 - m32 * M1344 + m42 * M1334) +
		       m31 * (m12 * M2344 - m22 * M1344 + m42 * M1324) -
		       m41 * (m12 * M2334 - m22 * M1334 + m32 * M1324);
	}

	float Matrix::det(const Vector &v1, const Vector &v2, const Vector &v3)
	{
		return v1 * (v2 % v3);
	}

	float Matrix::tr(const Matrix &M)
	{
		return M(1, 1) + M(2, 2) + M(3, 3) + M(4, 4);
	}

	Matrix &Matrix::orthonormalise()
	{
		Matrix &M = *this;

		Vector v1(M(1, 1), M(2, 1), M(3, 1));
		Vector v2(M(1, 2), M(2, 2), M(3, 2));
		Vector v3(M(1, 3), M(2, 3), M(3, 3));

		v2 -= v1 * (v1 * v2) / (v1 * v1);
		v3 -= v1 * (v1 * v3) / (v1 * v1);
		v3 -= v2 * (v2 * v3) / (v2 * v2);

		v1 /= N(v1);
		v2 /= N(v2);
		v3 /= N(v3);

		M(1, 1) = v1.x;  M(1, 2) = v2.x;  M(1, 3) = v3.x;
		M(2, 1) = v1.y;  M(2, 2) = v2.y;  M(2, 3) = v3.y;
		M(3, 1) = v1.z;  M(3, 2) = v2.z;  M(3, 3) = v3.z;

		return *this;
	}
}