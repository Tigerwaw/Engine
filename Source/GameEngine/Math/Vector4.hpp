#pragma once
#include <cmath>

namespace CommonUtilities
{
	template <class T>
	class Vector4
	{
	public:
		T x;
		T y;
		T z;
		T w;

		Vector4<T>();
		Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW);
		Vector4<T>(const Vector4<T>& aVector) = default;
		Vector4<T>& operator=(const Vector4<T>& aVector4) = default;
		~Vector4<T>() = default;

		//Explicit Type operator, create a different vector with the same values.
		//Example creates a Tga::Vector4<T> from this CommonUtillities::Vector4<T>
		template <class OtherVectorClass>
		explicit operator OtherVectorClass() const;

		Vector4<T> operator-() const;
		T LengthSqr() const;
		T Length() const;
		Vector4<T> GetNormalized() const;
		void Normalize();
		T Dot(const Vector4<T>& aVector) const;
		static Vector4<T> Abs(const Vector4<T>& aVector);
		static T Distance(const Vector4<T>& aVector0, const Vector4<T>& aVector1);
		static Vector4<T> Lerp(const Vector4<T>& aStart, const Vector4<T>& aEnd, const float aPercent);
	};

	template<class T>
	Vector4<T>::Vector4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	template<class T>
	Vector4<T>::Vector4(const T& aX, const T& aY, const T& aZ, const T& aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
	}

	template<class T>
	template<class OtherVectorClass>
	inline Vector4<T>::operator OtherVectorClass() const
	{
		return { x, y, z, w };
	}

	template<class T>
	Vector4<T> Vector4<T>::operator-() const
	{
		return Vector4<T>(-x, -y, -z, -w);
	}

	template<class T>
	T Vector4<T>::LengthSqr() const
	{
		return x * x + y * y + z * z + w * w;
	}

	template<class T>
	T Vector4<T>::Length() const
	{
		return static_cast<T>(sqrt(x * x + y * y + z * z + w * w));
	}

	template<class T>
	Vector4<T> Vector4<T>::GetNormalized() const
	{
		Vector4<T> newVector(x / Length(), y / Length(), z / Length(), w / Length());
		return newVector;
	}

	template<class T>
	void Vector4<T>::Normalize()
	{
		*this = GetNormalized();
	}


	template<class T>
	T Vector4<T>::Dot(const Vector4<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y + z * aVector.z + w * aVector.w;
	}

	template<class T>
	inline Vector4<T> Vector4<T>::Abs(const Vector4<T>& aVector)
	{
		return { Abs(aVector.x), Abs(aVector.y), Abs(aVector.z), Abs(aVector.w) };
	}

	template<class T>
	inline T Vector4<T>::Distance(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		const Vector4<T> direction = aVector1 - aVector0;
		return direction.Length();
	}

	template<class T>
	inline Vector4<T> Vector4<T>::Lerp(const Vector4<T>& aStart, const Vector4<T>& aEnd, const float aPercent)
	{
		return (aStart + aPercent * (aEnd - aStart));
	}

	//Returns the vector sum of aVector0 and aVector1
	template <class T>
	Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		T newX = aVector0.x + aVector1.x;
		T newY = aVector0.y + aVector1.y;
		T newZ = aVector0.z + aVector1.z;
		T newW = aVector0.w + aVector1.w;
		return Vector4<T>(newX, newY, newZ, newW);
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T>
	Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		T newX = aVector0.x - aVector1.x;
		T newY = aVector0.y - aVector1.y;
		T newZ = aVector0.z - aVector1.z;
		T newW = aVector0.w - aVector1.w;
		return Vector4<T>(newX, newY, newZ, newW);
	}

	// Returns the vector aVector1 component - multiplied by aVector1
	template <class T>
	Vector4<T> operator*(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>(aVector0.x * aVector1.x, aVector0.y * aVector1.y, aVector0.z * aVector1.z, aVector0.w * aVector1.w);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		return aVector * aScalar;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <class T>
	Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		return aVector * (1 / aScalar);
	}

	//Component-wise division
	template <class T>
	Vector4<T> operator/(const Vector4<T>& aVector, const Vector4<T>& aOtherVector)
	{
		aVector.x /= aOtherVector.x;
		aVector.y /= aOtherVector.y;
		aVector.z /= aOtherVector.z;
		aVector.w /= aOtherVector.w;
		return aVector;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T>
	void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0 = aVector0 + aVector1;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0 = aVector0 - aVector1;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector = aVector * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vector4<T>& aVector, const T& aScalar)
	{
		aVector = aVector / aScalar;
	}

	template <class T>
	bool operator==(const Vector4<T>& aVector, const Vector4<T>& aVector1)
	{
		if (aVector.x == aVector1.x &&
			aVector.y == aVector1.y &&
			aVector.z == aVector1.z &&
			aVector.w == aVector1.w)
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	typedef Vector4<float> Vector4f;
}