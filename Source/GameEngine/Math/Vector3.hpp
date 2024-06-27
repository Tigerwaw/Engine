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

		//Creates a null-vector
		Vector3<T>();

		//Creates a vector (aX, aY, aZ)
		Vector3<T>(const T& aX, const T& aY, const T& aZ);

		//Copy constructor (compiler generated)
		Vector3<T>(const Vector3<T>& aVector) = default;

		//Assignment operator (compiler generated)
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;

		//Destructor (compiler generated)
		~Vector3<T>() = default;

		//Explicit Type operator, create a different vector with the same values.
		//Example creates a Tga::Vector3<T> from this CommonUtillities::Vector3<T>
		template <class OtherVectorClass>
		explicit operator OtherVectorClass() const;

		//Returns a negated copy of the vector
		Vector3<T> operator-() const;

		//Returns the squared length of the vector
		T LengthSqr() const;

		//Returns the length of the vector
		T Length() const;

		//Returns a normalized copy of this vector
		Vector3<T> GetNormalized() const;

		//Normalizes the vector
		void Normalize();

		//Returns the dot product of this and aVector
		T Dot(const Vector3<T>& aVector) const;

		//Returns the cross product of this and aVector
		Vector3<T> Cross(const Vector3<T>& aVector) const;
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
		Vector3<T> newVector(x / Length(), y / Length(), z / Length());
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