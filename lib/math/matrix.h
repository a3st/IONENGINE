// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

template<typename Type>
struct Matrix {

	Type m00, m01, m02, m03;
	Type m10, m11, m12, m13;
	Type m20, m21, m22, m23;
	Type m30, m31, m32, m33;

	Matrix() = default;

	Matrix(
		Type const _m00, Type const _m01, Type const _m02, Type const _m03,
		Type const _m10, Type const _m11, Type const _m12, Type const _m13,
		Type const _m20, Type const _m21, Type const _m22, Type const _m23,
		Type const _m30, Type const _m31, Type const _m32, Type const _m33
	) :  
		m00(_m00), m01(_m01), m02(_m02), m03(_m03),
		m10(_m10), m11(_m11), m12(_m12), m13(_m13),
		m20(_m20), m21(_m21), m22(_m22), m23(_m23),
		m30(_m30), m31(_m31), m32(_m32), m33(_m33) { }

	Matrix(Matrix const& other) : 
		m00(other.m00), m01(other.m01), m02(other.m02), m03(other.m03),
		m10(other.m10), m11(other.m11), m12(other.m12), m13(other.m13),
		m20(other.m20), m21(other.m21), m22(other.m22), m23(other.m23),
		m30(other.m30), m31(other.m31), m32(other.m32), m33(other.m33) { }

	Matrix(Matrix&& other) noexcept :
		m00(other.m00), m01(other.m01), m02(other.m02), m03(other.m03),
		m10(other.m10), m11(other.m11), m12(other.m12), m13(other.m13),
		m20(other.m20), m21(other.m21), m22(other.m22), m23(other.m23),
		m30(other.m30), m31(other.m31), m32(other.m32), m33(other.m33) { }

	Matrix& operator=(Matrix const& other) {

		m00 = other.m00;
		m01 = other.m01;
		m02 = other.m02;
		m03 = other.m03;
		m10 = other.m10;
		m11 = other.m11;
		m12 = other.m12;
		m13 = other.m13;
		m20 = other.m20;
		m21 = other.m21;
		m22 = other.m22;
		m23 = other.m23;
		m30 = other.m30;
		m31 = other.m31;
		m32 = other.m32;
		m33 = other.m33;
		return *this;
	}

	Matrix& operator=(Matrix&& other) noexcept {

		m00 = other.m00;
		m01 = other.m01;
		m02 = other.m02;
		m03 = other.m03;
		m10 = other.m10;
		m11 = other.m11;
		m12 = other.m12;
		m13 = other.m13;
		m20 = other.m20;
		m21 = other.m21;
		m22 = other.m22;
		m23 = other.m23;
		m30 = other.m30;
		m31 = other.m31;
		m32 = other.m32;
		m33 = other.m33;
		return *this;
	}

	const Type* data() const { return &m00; }

	size_t size() const { return 16; }

	Matrix& transpose() {

		Matrix mat = *this;
		m00 = mat.m00;
		m01 = mat.m10;
		m02 = mat.m20;
		m03 = mat.m30;
		m10 = mat.m01;
		m11 = mat.m11;
		m12 = mat.m21;
		m13 = mat.m31;
		m20 = mat.m02;
		m21 = mat.m12;
		m22 = mat.m22;
		m23 = mat.m32;
		m30 = mat.m03;
		m31 = mat.m13;
		m32 = mat.m23;
		m33 = mat.m33;
		return *this;
	}

	Matrix operator*(Matrix const& other) const {

		auto mat = Matrix {};
		mat.m00 = m00 * rhs.m00 + m01 * rhs.m10 + m02 * rhs.m20 + m03 * rhs.m30;
		mat.m01 = m00 * rhs.m01 + m01 * rhs.m11 + m02 * rhs.m21 + m03 * rhs.m31;
		mat.m02 = m00 * rhs.m02 + m01 * rhs.m12 + m02 * rhs.m22 + m03 * rhs.m32;
		mat.m03 = m00 * rhs.m03 + m01 * rhs.m13 + m02 * rhs.m23 + m03 * rhs.m33;
		mat.m10 = m10 * rhs.m00 + m11 * rhs.m10 + m12 * rhs.m20 + m13 * rhs.m30;
		mat.m11 = m10 * rhs.m01 + m11 * rhs.m11 + m12 * rhs.m21 + m13 * rhs.m31;
		mat.m12 = m10 * rhs.m02 + m11 * rhs.m12 + m12 * rhs.m22 + m13 * rhs.m32;
		mat.m13 = m10 * rhs.m03 + m11 * rhs.m13 + m12 * rhs.m23 + m13 * rhs.m33;
		mat.m20 = m20 * rhs.m00 + m21 * rhs.m10 + m22 * rhs.m20 + m23 * rhs.m30;
		mat.m21 = m20 * rhs.m01 + m21 * rhs.m11 + m22 * rhs.m21 + m23 * rhs.m31;
		mat.m22 = m20 * rhs.m02 + m21 * rhs.m12 + m22 * rhs.m22 + m23 * rhs.m32;
		mat.m23 = m20 * rhs.m03 + m21 * rhs.m13 + m22 * rhs.m23 + m23 * rhs.m33;
		mat.m30 = m30 * rhs.m00 + m31 * rhs.m10 + m32 * rhs.m20 + m33 * rhs.m30;
		mat.m31 = m30 * rhs.m01 + m31 * rhs.m11 + m32 * rhs.m21 + m33 * rhs.m31;
		mat.m32 = m30 * rhs.m02 + m31 * rhs.m12 + m32 * rhs.m22 + m33 * rhs.m32;
		mat.m33 = m30 * rhs.m03 + m31 * rhs.m13 + m32 * rhs.m23 + m33 * rhs.m33;
		return mat;
	}

	/*Matrix operator*(const Vector4<T>& rhs) const {

		Matrix mat{};
		mat.m00 = m00 * rhs.x;
		mat.m01 = m01 * rhs.y;
		mat.m02 = m02 * rhs.z;
		mat.m03 = m03 * rhs.w;
		mat.m10 = m10 * rhs.x;
		mat.m11 = m11 * rhs.y;
		mat.m12 = m12 * rhs.z;
		mat.m13 = m13 * rhs.w;
		mat.m20 = m20 * rhs.x;
		mat.m21 = m21 * rhs.y;
		mat.m22 = m22 * rhs.z;
		mat.m23 = m23 * rhs.w;
		mat.m30 = m30 * rhs.x;
		mat.m31 = m31 * rhs.y;
		mat.m32 = m32 * rhs.z;
		mat.m33 = m33 * rhs.w;
		return mat;
	}*/

	Matrix operator*(Type const other) const {

		auto mat = Matrix {};
		mat.m00 = m00 * other;
		mat.m01 = m01 * other;
		mat.m02 = m02 * other;
		mat.m03 = m03 * other;
		mat.m10 = m10 * other;
		mat.m11 = m11 * other;
		mat.m12 = m12 * other;
		mat.m13 = m13 * other;
		mat.m20 = m20 * other;
		mat.m21 = m21 * other;
		mat.m22 = m22 * other;
		mat.m23 = m23 * other;
		mat.m30 = m30 * other;
		mat.m31 = m31 * other;
		mat.m32 = m32 * other;
		mat.m33 = m33 * other;
		return mat;
	}

	Matrix operator+(Matrix const& other) const {

		auto mat = Matrix {};
		mat.m00 = m00 + other.m00;
		mat.m01 = m01 + other.m01;
		mat.m02 = m02 + other.m02;
		mat.m03 = m03 + other.m03;
		mat.m10 = m10 + other.m10;
		mat.m11 = m11 + other.m11;
		mat.m12 = m12 + other.m12;
		mat.m13 = m13 + other.m13;
		mat.m20 = m20 + other.m20;
		mat.m21 = m21 + other.m21;
		mat.m22 = m22 + other.m22;
		mat.m23 = m23 + other.m23;
		mat.m30 = m30 + other.m30;
		mat.m31 = m31 + other.m31;
		mat.m32 = m32 + other.m32;
		mat.m33 = m33 + other.m33;
		return mat;
	}

	Matrix operator-(Matrix const& other) const {

		auto mat = Matrix {};
		mat.m00 = m00 - other.m00;
		mat.m01 = m01 - other.m01;
		mat.m02 = m02 - other.m02;
		mat.m03 = m03 - other.m03;
		mat.m10 = m10 - other.m10;
		mat.m11 = m11 - other.m11;
		mat.m12 = m12 - other.m12;
		mat.m13 = m13 - other.m13;
		mat.m20 = m20 - other.m20;
		mat.m21 = m21 - other.m21;
		mat.m22 = m22 - other.m22;
		mat.m23 = m23 - other.m23;
		mat.m30 = m30 - other.m30;
		mat.m31 = m31 - other.m31;
		mat.m32 = m32 - other.m32;
		mat.m33 = m33 - other.m33;
		return mat;
	}

	Matrix& identity() {

		m00 = 1;
		m11 = 1;
		m22 = 1;
		m33 = 1;
		return *this;
	}

	/*static Matrix Translate(const Vector3<T>& rhs) {

		Matrix mat = Matrix::Identity();
		mat.m30 = rhs.x;
		mat.m31 = rhs.y;
		mat.m32 = rhs.z;
		return mat;
	}

	static Matrix Scale(const Vector3<T>& rhs) {

		Matrix mat = Matrix::Identity();
		mat.m00 = rhs.x;
		mat.m11 = rhs.y;
		mat.m22 = rhs.z;
		return mat;
	}*/

	Matrix& perspective(Type const fovy, Type const aspect, Type const near_dst, Type const far_dst) {

		m00 = 1 / (std::tan(fovy / 2) * aspect);
		m11 = 1 / std::tan(fovy / 2);
		m22 = (far_dst + near_dst) / (near_dst - far_dst);
		m23 = -1;
		m32 = (2 * near_dst * far_dst) / (near_dst - far_dst);
		return *this;
	}

	Matrix& orthographic(
		Type const left, Type const right,
		Type const bottom, Type const top,
		Type const near_dst, Type const far_dst
	) {

		m00 = 2 / (right - left);
		m11 = 2 / (top - bottom);
		m22 = -2 / (far_dst - near_dst);
		m30 = -(right + left) / (right - left);
		m31 = -(top + bottom) / (top - bottom);
		m32 = -(far_dst + near_dst) / (far_dst - near_dst);
		m33 = 1;
		return *this;
	}

	/*static Matrix LookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up) {

		Vector3<T> z(eye - target);
		z.Normalize();

		Vector3<T> x = Vector3<T>::CrossProduct(up, z);
		x.Normalize();

		Vector3<T> y = Vector3<T>::CrossProduct(z, x);

		Matrix mat = Matrix::Identity();
		mat.m00 = x.x;
		mat.m01 = y.x;
		mat.m02 = z.x;
		mat.m10 = x.y;
		mat.m11 = y.y;
		mat.m12 = z.y;
		mat.m20 = x.z;
		mat.m21 = y.z;
		mat.m22 = z.z;
		mat.m30 = Vector3<T>::DotProduct(-x, eye);
		mat.m31 = Vector3<T>::DotProduct(-y, eye);
		mat.m32 = Vector3<T>::DotProduct(-z, eye);
		return mat;
	}*/

	bool operator==(Matrix const& other) const {

		return std::tie(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33) == 
			std::tie(other.m00, other.m01, other.m02, other.m03, other.m10, other.m11, other.m12, other.m13, other.m20, other.m21, other.m22, other.m23, other.m30, other.m31, other.m32, other.m33);
	}

	bool operator!=(Matrix const& other) const {

		return std::tie(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33) != 
			std::tie(other.m00, other.m01, other.m02, other.m03, other.m10, other.m11, other.m12, other.m13, other.m20, other.m21, other.m22, other.m23, other.m30, other.m31, other.m32, other.m33);
	}
};

using Matrixf = Matrix<float>;
using Matrixd = Matrix<double>;

}