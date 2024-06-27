#pragma once
#include <array>
#include <math.h>
#include <cassert>
#include "Vector3.hpp"

namespace CommonUtilities
{
	template <class T>
	class Matrix4x4;

	template <class T>
	class Matrix3x3
	{
	public:
		// Creates the identity matrix.
		Matrix3x3<T>();

		// Initializes the matrix with a list of elements.
		Matrix3x3<T>(const T a11, const T a12, const T a13, const T a21, const T a22, const T a23, const T a31, const T a32, const T a33);

		//Initializes the matrix with a number of vectors.
		Matrix3x3<T>(const Vector3<T> vectorA, const Vector3<T> vectorB, const Vector3<T> vectorC);

		// Copy Constructor.
		Matrix3x3<T>(const Matrix3x3<T>& aMatrix);

		// Copies the top left 3x3 part of the Matrix4x4.
		Matrix3x3<T>(const Matrix4x4<T>& aMatrix);

		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;

		// [] operator for accessing element for read/write or read, respectively.
		T& operator[](const int aIndex);
		const T& operator[](const int aIndex) const;

		void operator=(const Matrix3x3<T>& aMatrix);

		// Creates a transposed copy of the matrix.
		void Transpose();
		const Matrix3x3<T> GetTranspose() const;

		// Assumes aTransform is made up of nothing but rotations and translations.
		void FastInverse();
		const Matrix3x3<T> GetFastInverse() const;

		// Return rotation in radians.
		const T GetRotationInRadians() const;

		// Return row vector.
		const Vector3<T> GetRowVector(int aIndex) const;

		// Static functions for creating rotation matrices.
		static Matrix3x3<T> CreateRotationAroundX(const T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundY(const T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundZ(const T aAngleInRadians);

	private:
		std::array<std::array<T, 3>, 3> myMatrix;

	};

	template <class T>
	Matrix3x3<T>::Matrix3x3()
	{
		for (int rowIndex = 1; rowIndex <= 3; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 3; columnIndex++)
			{
				this->operator()(rowIndex, columnIndex) = 0;
			}
		}

		this->operator()(1, 1) = 1;
		this->operator()(2, 2) = 1;
		this->operator()(3, 3) = 1;
	}

	template <class T>
	Matrix3x3<T>::Matrix3x3(const T a11, const T a12, const T a13, const T a21, const T a22, const T a23, const T a31, const T a32, const T a33)
	{
		this->operator()(1, 1) = a11;
		this->operator()(1, 2) = a12;
		this->operator()(1, 3) = a13;
		this->operator()(2, 1) = a21;
		this->operator()(2, 2) = a22;
		this->operator()(2, 3) = a23;
		this->operator()(3, 1) = a31;
		this->operator()(3, 2) = a32;
		this->operator()(3, 3) = a33;
	}

	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Vector3<T> vectorA, const Vector3<T> vectorB, const Vector3<T> vectorC)
	{
		this->operator()(1, 1) = vectorA.x;
		this->operator()(1, 2) = vectorA.y;
		this->operator()(1, 3) = vectorA.z;
		this->operator()(2, 1) = vectorB.x;
		this->operator()(2, 2) = vectorB.y;
		this->operator()(2, 3) = vectorB.z;
		this->operator()(3, 1) = vectorC.x;
		this->operator()(3, 2) = vectorC.y;
		this->operator()(3, 3) = vectorC.z;
	}

	template <class T>
	Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix)
	{
		*this = aMatrix;
	}

	template <class T>
	Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	{
		for (int rowIndex = 1; rowIndex <= 3; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 3; columnIndex++)
			{
				this->operator()(rowIndex, columnIndex) = aMatrix(rowIndex, columnIndex);
			}
		}
	}

	template <class T>
	T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)
	{
		assert(aRow > 0 && aRow <= 3);
		assert(aColumn > 0 && aColumn <= 3);

		return myMatrix[aRow - 1][aColumn - 1];
	}

	template <class T>
	const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const
	{
		assert(aRow > 0 && aRow <= 3);
		assert(aColumn > 0 && aColumn <= 3);

		return myMatrix[aRow - 1][aColumn - 1];
	}

	template <class T>
	T& Matrix3x3<T>::operator[](const int aIndex)
	{
		assert(aIndex >= 0 && aIndex < 9);

		return myMatrix[aIndex / 3][aIndex % 3];
	}

	template <class T>
	const T& Matrix3x3<T>::operator[](const int aIndex) const
	{
		assert(aIndex >= 0 && aIndex < 9);

		return myMatrix[aIndex / 3][aIndex % 3];
	}

	template <class T>
	void Matrix3x3<T>::operator=(const Matrix3x3<T>& aMatrix)
	{
		for (int rowIndex = 1; rowIndex <= 3; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 3; columnIndex++)
			{
				this->operator()(rowIndex, columnIndex) = aMatrix(rowIndex, columnIndex);
			}
		}
	}

	template<class T>
	inline void Matrix3x3<T>::Transpose()
	{
		Matrix3x3<T> transposedMatrix;

		for (int rowIndex = 1; rowIndex <= 3; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 3; columnIndex++)
			{
				transposedMatrix(rowIndex, columnIndex) = this->operator()(columnIndex, rowIndex);
			}
		}

		*this = transposedMatrix;
	}

	template <class T>
	const Matrix3x3<T> Matrix3x3<T>::GetTranspose() const
	{
		Matrix3x3<T> transposedMatrix(*this);
		transposedMatrix.Transpose();
		return transposedMatrix;
	}

	template<class T>
	inline void Matrix3x3<T>::FastInverse()
	{
		Matrix3x3<T> inverseRotation = this->GetTranspose();
		inverseRotation(1, 3) = 0;
		inverseRotation(2, 3) = 0;

		Matrix3x3<T> inverseTranslationMatrix;
		inverseTranslationMatrix(3, 1) = -this->operator()(3, 1);
		inverseTranslationMatrix(3, 2) = -this->operator()(3, 2);

		*this = inverseTranslationMatrix * inverseRotation;
	}

	template <class T>
	const Matrix3x3<T> Matrix3x3<T>::GetFastInverse() const
	{
		Matrix3x3 inverseMatrix(*this);
		inverseMatrix.FastInverse();
		return inverseMatrix;
	}

	template<class T>
	const inline T Matrix3x3<T>::GetRotationInRadians() const
	{
		return std::atan2(this->operator()(2, 2), this->operator()(1, 2));
	}

	template<class T>
	const inline Vector3<T> Matrix3x3<T>::GetRowVector(int aIndex) const
	{
		assert(aIndex > 0 && aIndex <= 3);

		return Vector3<T>(this->operator()(aIndex, 1), this->operator()(aIndex, 2), this->operator()(aIndex, 3));
	}

	template <class T>
	Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(const T aAngleInRadians)
	{
		T cosRadians = cos(aAngleInRadians);
		T sinRadians = sin(aAngleInRadians);

		Matrix3x3<T> newMatrix(1, 0, 0, 
							   0, cosRadians, sinRadians,
							   0, -sinRadians, cosRadians);
		return newMatrix;
	}

	template <class T>
	Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(const T aAngleInRadians)
	{
		T cosRadians = cos(aAngleInRadians);
		T sinRadians = sin(aAngleInRadians);

		Matrix3x3<T> newMatrix(cosRadians, 0, -sinRadians,
							   0, 1, 0,
							   sinRadians, 0, cosRadians);
		return newMatrix;
	}

	template <class T>
	Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(const T aAngleInRadians)
	{
		T cosRadians = cos(aAngleInRadians);
		T sinRadians = sin(aAngleInRadians);

		Matrix3x3<T> newMatrix(cosRadians, sinRadians, 0,
							   -sinRadians, cosRadians, 0,
							   0, 0, 1);
		return newMatrix;
	}

	template <class T>
	const Matrix3x3<T> operator+(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		Matrix3x3<T> newMatrix;

		for (int rowIndex = 1; rowIndex <= 3; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 3; columnIndex++)
			{
				newMatrix(rowIndex, columnIndex) = aMatrix0(rowIndex, columnIndex) + aMatrix1(rowIndex, columnIndex);
			}
		}

		return newMatrix;
	}

	template <class T>
	const void operator+=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 + aMatrix1;
	}

	template <class T>
	const Matrix3x3<T> operator-(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		Matrix3x3<T> newMatrix;

		for (int rowIndex = 1; rowIndex <= 3; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 3; columnIndex++)
			{
				newMatrix(rowIndex, columnIndex) = aMatrix0(rowIndex, columnIndex) - aMatrix1(rowIndex, columnIndex);
			}
		}

		return newMatrix;
	}

	template <class T>
	const void operator-=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 - aMatrix1;
	}

	template <class T>
	const Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		Matrix3x3<T> newMatrix;

		for (int rowIndex = 1; rowIndex <= 3; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 3; columnIndex++)
			{
				T x = aMatrix0(rowIndex, 1) * aMatrix1(1, columnIndex);
				T y = aMatrix0(rowIndex, 2) * aMatrix1(2, columnIndex);
				T z = aMatrix0(rowIndex, 3) * aMatrix1(3, columnIndex);
				newMatrix(rowIndex, columnIndex) = x + y + z;
			}
		}

		return newMatrix;
	}

	template <class T>
	const void operator*=(Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		aMatrix0 = aMatrix0 * aMatrix1;
	}

	template <class T>
	const Vector3<T> operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix)
	{
		Vector3<T> newVector;

		newVector.x = aVector.x * aMatrix(1, 1) +
					  aVector.y * aMatrix(2, 1) +
					  aVector.z * aMatrix(3, 1);

		newVector.y = aVector.x * aMatrix(1, 2) +
					  aVector.y * aMatrix(2, 2) +
					  aVector.z * aMatrix(3, 2);

		newVector.z = aVector.x * aMatrix(1, 3) +
					  aVector.y * aMatrix(2, 3) +
					  aVector.z * aMatrix(3, 3);

		return newVector;
	}

	template <class T>
	const bool operator==(const Matrix3x3<T>& aMatrix0, const Matrix3x3<T>& aMatrix1)
	{
		for (int rowIndex = 1; rowIndex <= 3; rowIndex++)
		{
			for (int columnIndex = 1; columnIndex <= 3; columnIndex++)
			{
				if (aMatrix0(rowIndex, columnIndex) != aMatrix1(rowIndex, columnIndex))
				{
					return false;
				}
			}
		}

		return true;
	}


	typedef Matrix3x3<float> Matrix3x3f;
}