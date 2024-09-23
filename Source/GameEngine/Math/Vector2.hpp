#pragma once
#include <cmath>

namespace CommonUtilities
{
	template <class T>
	class Vector2
	{
	public:
		T x;
		T y;

		Vector2<T>();
		Vector2<T>(const T& aX, const T& aY);
		Vector2<T>(const Vector2<T>& aVector) = default;
		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;
		~Vector2<T>() = default;

		//Explicit Type operator, create a different vector with the same values.
		//Example creates a Tga::Vector2<T> from this CommonUtillities::Vector2<T>
		template <class OtherVectorClass>
		explicit operator OtherVectorClass() const;

		Vector2<T> operator-() const;
		T LengthSqr() const;
		T Length() const;
		Vector2<T> GetNormalized() const;
		void Normalize();
		T Dot(const Vector2<T>& aVector) const;
		static Vector2<T> Abs(const Vector2<T>& aVector);
		static T Distance(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
		static Vector2<T> Lerp(const Vector2<T>& aStart, const Vector2<T>& aEnd, const float aPercent);
	};

	template<class T>
	Vector2<T>::Vector2()
	{
		x = 0;
		y = 0;
	}

	template<class T>
	Vector2<T>::Vector2(const T& aX, const T& aY)
	{
		x = aX;
		y = aY;
	}

	template<class T>
	template<class OtherVectorClass>
	inline Vector2<T>::operator OtherVectorClass() const
	{
		return { x, y };
	}

	template<class T>
	Vector2<T> Vector2<T>::operator-() const
	{
		return Vector2<T>(-x, -y);
	}

	template<class T>
	T Vector2<T>::LengthSqr() const
	{
		return x * x + y * y;
	}

	template<class T>
	T Vector2<T>::Length() const
	{
		return static_cast<T>(sqrt(x * x + y * y));
	}

	template<class T>
	Vector2<T> Vector2<T>::GetNormalized() const
	{
		T length = Length();
		length = max(length, (T)0.0001);
		Vector2<T> newVector(x / length, y / length);
		return newVector;
	}

	template<class T>
	void Vector2<T>::Normalize()
	{
		*this = GetNormalized();
	}


	template<class T>
	T Vector2<T>::Dot(const Vector2<T>& aVector) const
	{
		return x * aVector.x + y * aVector.y;
	}

	template<class T>
	inline Vector2<T> Vector2<T>::Abs(const Vector2<T>& aVector)
	{
		return { Abs(aVector.x), Abs(aVector.y) };
	}

	template<class T>
	inline T Vector2<T>::Distance(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		const Vector2<T> direction = aVector1 - aVector0;
		return direction.Length();
	}

	template<class T>
	inline Vector2<T> Vector2<T>::Lerp(const Vector2<T>& aStart, const Vector2<T>& aEnd, const float aPercent)
	{
		return (aStart + aPercent * (aEnd - aStart));
	}

	//Returns the vector sum of aVector0 and aVector1
	template <class T>
	Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		T newX = aVector0.x + aVector1.x;
		T newY = aVector0.y + aVector1.y;
		return Vector2<T>(newX, newY);
	}

	//Returns the vector difference of aVector0 and aVector1
	template <class T>
	Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		T newX = aVector0.x - aVector1.x;
		T newY = aVector0.y - aVector1.y;
		return Vector2<T>(newX, newY);
	}

	// Returns the vector aVector1 component - multiplied by aVector1
	template <class T>
	Vector2<T> operator*(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x * aVector1.x, aVector0.y * aVector1.y);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template <class T>
	Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
	{
		return aVector * aScalar;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
	template <class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
	{
		return aVector * (1 / aScalar);
	}

	//Component-wise division
	template <class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const Vector2<T>& aOtherVector)
	{
		aVector.x /= aOtherVector.x;
		aVector.y /= aOtherVector.y;
		return aVector;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template <class T>
	void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0 = aVector0 + aVector1;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template <class T>
	void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0 = aVector0 - aVector1;
	}

	//Equivalent to setting aVector to (aVector * aScalar)
	template <class T>
	void operator*=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector = aVector * aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template <class T>
	void operator/=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector = aVector / aScalar;
	}

	template <class T>
	bool operator==(const Vector2<T>& aVector, const Vector2<T>& aVector1)
	{
		if (aVector.x == aVector1.x &&
			aVector.y == aVector1.y)
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	typedef Vector2<float> Vector2f;
}