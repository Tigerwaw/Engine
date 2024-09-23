#pragma once
#include <cmath>
#include <cassert>

namespace CommonUtilities
{
	template <class T>
	class Vector3
	{
	public:
		T x;
		T y;
		T z;

		Vector3<T>();
		Vector3<T>(const T& aX, const T& aY, const T& aZ);
		Vector3<T>(const Vector3<T>& aVector) = default;
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;
		~Vector3<T>() = default;

		//Explicit Type operator, create a different vector with the same values.
		//Example creates a Tga::Vector3<T> from this CommonUtillities::Vector3<T>
		template <class OtherVectorClass>
		explicit operator OtherVectorClass() const;

		Vector3<T> operator-() const;
		T LengthSqr() const;
		T Length() const;
		Vector3<T> GetNormalized() const;

		void Normalize();
		T Dot(const Vector3<T>& aVector) const;
		Vector3<T> Cross(const Vector3<T>& aVector) const;
		static Vector3<T> Abs(const Vector3<T>& aVector);
		static T Distance(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
		static Vector3<T> Lerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const float aPercent);
	};

	template<class T>
	Vector3<T>::Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	template<class T>
	Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ)
	{
		x = aX;
		y = aY;
		z = aZ;
	}

	template<class T>
	template<class OtherVectorClass>
	inline Vector3<T>::operator OtherVectorClass() const 
	{
		return { x, y, z };
	}

	template<class T>
	Vector3<T> Vector3<T>::operator-() const
	{
		return Vector3<T>(-x, -y, -z);
	}

	template<class T>
	T Vector3<T>::LengthSqr() const
	{
		return x * x + y * y + z * z;
	}

	template<class T>
	T Vector3<T>::Length() const
	{
		return static_cast<T>(sqrt(LengthSqr()));
	}

	template<class T>
	Vector3<T> Vector3<T>::GetNormalized() const
	{
		T length = Length();
		length = max(length, (T)0.0001);
		Vector3<T> newVector(x / length, y / length, z / length);
		return newVector;
	}

	template<class T>
	void Vector3<T>::Normalize()
	{
		*this = GetNormalized();
	}


	template<class T>
	T Vector3<T>::Dot(const Vector3<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y + z * aVector.z;
	}

	template<class T>
	Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
	{
		T newX = y * aVector.z - z * aVector.y;
		T newY = z * aVector.x - x * aVector.z;
		T newZ = x * aVector.y - y * aVector.x;
		return Vector3<T>(newX, newY, newZ);
	}

	template<class T>
	inline Vector3<T> Vector3<T>::Abs(const Vector3<T>& aVector)
	{
		return { static_cast<T>(abs(aVector.x)), static_cast<T>(abs(aVector.y)), static_cast<T>(abs(aVector.z)) };
	}

	template<class T>
	inline T Vector3<T>::Distance(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		const Vector3<T> direction = aVector1 - aVector0;
		return direction.Length();
	}

	template<class T>
	inline Vector3<T> Vector3<T>::Lerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const float aPercent)
	{
		return (aStart + aPercent * (aEnd - aStart));
	}

	//Returns the vector sum of aVector0 and aVector1
	template <class T>
	Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		T newX = aVector0.x + aVector1.x;
		T newY = aVector0.y + aVector1.y;
		T newZ = aVector0.z + aVector1.z;
		return Vector3<T>(newX, newY, newZ);
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T>
	Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		T newX = aVector0.x - aVector1.x;
		T newY = aVector0.y - aVector1.y;
		T newZ = aVector0.z - aVector1.z;
		return Vector3<T>(newX, newY, newZ);
	}

	// Returns the vector aVector1 component - multiplied by aVector1
	template <class T>
	Vector3<T> operator*(const Vector3<T>&aVector0, const Vector3<T>&aVector1)
	{
		return Vector3<T>(aVector0.x * aVector1.x, aVector0.y * aVector1.y, aVector0.z * aVector1.z);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar) 
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		return aVector * aScalar;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <class T>
	Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar) 
	{
		return aVector * (1 / aScalar);
	}

	//Component-wise division
	template <class T>
	Vector3<T> operator/(const Vector3<T>& aVector, const Vector3<T>& aOtherVector)
	{
		aVector.x /= aOtherVector.x;
		aVector.y /= aOtherVector.y;
		aVector.z /= aOtherVector.z;
		return aVector;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T>
	void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1) 
	{
		aVector0 = aVector0 + aVector1;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0 = aVector0 - aVector1;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector = aVector * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vector3<T>& aVector, const T& aScalar) 
	{
		aVector = aVector / aScalar;
	}

	template <class T>
	bool operator==(const Vector3<T>& aVector, const Vector3<T>& aVector1)
	{
		if (aVector.x != aVector1.x) { return false; }
		if (aVector.y != aVector1.y) { return false; }
		if (aVector.z != aVector1.z) { return false; }

		return true;
	}


	typedef Vector3<float> Vector3f;
}