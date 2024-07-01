#pragma once

#include <algorithm>
#include <cmath>

#include "Vector.hpp"
#include "MathConstants.hpp"

namespace CommonUtilities
{
	template <class T>
	class Matrix4x4;

#ifdef max
#undef max
#endif

	template <class T>
	class Quaternion
	{
	public:
		T x;
		T y;
		T z;
		T w;

		Quaternion<T>();
		Quaternion<T>(const T& aW, const T& aX, const T& aY, const T& aZ);
		Quaternion<T>(const T& aPitch, const T& aYaw, const T& aRoll);
		Quaternion<T>(const Vector3<T>& aPitchYawRoll);
		Quaternion<T>(const Vector3<T>& aVector, const T aAngle);
		Quaternion<T>(const Matrix4x4<T>& aMatrix);

		void RotateWithEuler(const Vector3<T>& aEuler);

		inline void Normalize();
		inline Quaternion<T> GetNormalized() const;
		inline Quaternion<T> GetConjugate() const;

		Quaternion<T>& operator=(const Quaternion<T>& aQuat) = default;

		T Length() const;
		T Length2() const;
		inline Vector3<T> GetEulerAnglesRadians() const;
		inline Vector3<T> GetEulerAnglesRadiansd() const;
		inline Vector3<T> GetEulerAnglesDegrees() const;
		inline Matrix4x4<T> GetRotationMatrix4x4f() const;
		inline T Dot(const Quaternion<T>& aQuat) const;

		inline Vector3<T> GetRight() const;
		inline Vector3<T> GetUp() const;
		inline Vector3<T> GetForward() const;

		// Rotates a vector by the rotation stored in the Quaternion.
		inline static Vector3<T> RotateVectorByQuaternion(const Quaternion<T>& aQuaternion, const Vector3<T>& aVectorToRotate);
		inline static Quaternion<T> Lerp(const Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB, const T& aDelta);
		inline static Quaternion<T> Slerp(const Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB, const T& aDelta);
	private:
	};

	template<class T>
	inline Quaternion<T>::Quaternion()
	{
		w = static_cast<T>(1);
		x = static_cast<T>(0);
		y = static_cast<T>(0);
		z = static_cast<T>(0);
	}

	template<class T>
	inline Quaternion<T>::Quaternion(const T& aW, const T& aX, const T& aY, const T& aZ)
	{
		w = aW;
		x = aX;
		y = aY;
		z = aZ;
	}

	template<class T>
	inline Quaternion<T>::Quaternion(const T& aPitch, const T& aYaw, const T& aRoll)
	{
		T cy = cos(aPitch * T(0.5));
		T sy = sin(aPitch * T(0.5));
		T cr = cos(aRoll * T(0.5));
		T sr = sin(aRoll * T(0.5));
		T cp = cos(aYaw * T(0.5));
		T sp = sin(aYaw * T(0.5));

		w = cy * cr * cp + sy * sr * sp;
		x = cy * sr * cp - sy * cr * sp;
		y = cy * cr * sp + sy * sr * cp;
		z = sy * cr * cp - cy * sr * sp;
	}

	template<class T>
	inline Quaternion<T>::Quaternion(const Vector3<T>& aYawPitchRoll)
	{
		T cx = cos(aYawPitchRoll.x * T(0.5));
		T cy = cos(aYawPitchRoll.y * T(0.5));
		T cz = cos(aYawPitchRoll.z * T(0.5));
		T sx = sin(aYawPitchRoll.x * T(0.5));
		T sy = sin(aYawPitchRoll.y * T(0.5));
		T sz = sin(aYawPitchRoll.z * T(0.5));

		w = cx * cy * cz + sx * sy * sz;
		x = sx * cy * cz - cx * sy * sz;
		y = cx * sy * cz + sx * cy * sz;
		z = cx * cy * sz - sx * sy * cz;

	}

	template<class T>
	inline Quaternion<T>::Quaternion(const Vector3<T>& aVector, const T aAngle)
	{
		T halfAngle = aAngle / T(2);
		w = cos(halfAngle);
		T halfAngleSin = sin(halfAngle);
		x = aVector.x * halfAngleSin;
		y = aVector.y * halfAngleSin;
		z = aVector.z * halfAngleSin;

		//W = cos(aAngle / T(2));
		//X = cos(axis.x) * sin(aAngle / T(2));
		//Y = cos(axis.y) * sin(aAngle / T(2));
		//Z = cos(axis.z) * sin(aAngle / T(2));
	}

	template<class T>
	inline Quaternion<T>::Quaternion(const Matrix4x4<T>& aMatrix)
	{
		w = std::sqrt(std::max(T(0), T(1) + aMatrix(1, 1) + aMatrix(2, 2) + aMatrix(3, 3))) * T(0.5);
		x = std::sqrt(std::max(T(0), T(1) + aMatrix(1, 1) - aMatrix(2, 2) - aMatrix(3, 3))) * T(0.5);
		y = std::sqrt(std::max(T(0), T(1) - aMatrix(1, 1) + aMatrix(2, 2) - aMatrix(3, 3))) * T(0.5);
		z = std::sqrt(std::max(T(0), T(1) - aMatrix(1, 1) - aMatrix(2, 2) + aMatrix(3, 3))) * T(0.5);
		x = std::copysign(x, aMatrix(3, 2) - aMatrix(2, 3));
		y = std::copysign(y, aMatrix(1, 3) - aMatrix(3, 1));
		z = std::copysign(z, aMatrix(2, 1) - aMatrix(1, 2));
	}

	template <class T> inline Quaternion<T> operator*(const Quaternion<T>& aQuat, const T& aScalar)
	{
		return Quaternion<T>(aQuat.w * aScalar, aQuat.x * aScalar, aQuat.y * aScalar, aQuat.z * aScalar);
	}

	template <class T> inline Quaternion<T> operator*(const T& aScalar, const Quaternion<T>& aQuat)
	{
		return Quaternion<T>(aQuat.w * aScalar, aQuat.x * aScalar, aQuat.y * aScalar, aQuat.z * aScalar);
	}

	template <class T> inline Quaternion<T> operator*(const Quaternion<T>& aQuat0, const Quaternion<T>& aQuat1)
	{
		return Quaternion<T>(
			(aQuat1.w * aQuat0.w) - (aQuat1.x * aQuat0.x) - (aQuat1.y * aQuat0.y) - (aQuat1.z * aQuat0.z),
			(aQuat1.w * aQuat0.x) + (aQuat1.x * aQuat0.w) + (aQuat1.y * aQuat0.z) - (aQuat1.z * aQuat0.y),
			(aQuat1.w * aQuat0.y) + (aQuat1.y * aQuat0.w) + (aQuat1.z * aQuat0.x) - (aQuat1.x * aQuat0.z),
			(aQuat1.w * aQuat0.z) + (aQuat1.z * aQuat0.w) + (aQuat1.x * aQuat0.y) - (aQuat1.y * aQuat0.x)
		);
	}

	template <class T> inline void operator*=(Quaternion<T>& aQuat, const T& aScalar)
	{
		aQuat.w *= aScalar;
		aQuat.x *= aScalar;
		aQuat.y *= aScalar;
		aQuat.z *= aScalar;
	}

	template <class T> inline void operator*=(Quaternion<T>& aQuat0, const Quaternion<T>& aQuat1)
	{
		T w = aQuat0.w;
		T x = aQuat0.x;
		T y = aQuat0.y;
		T z = aQuat0.z;

		//aQuat0.w = w * aQuat1.w - x * aQuat1.x - y * aQuat1.y - z * aQuat1.z;
		//aQuat0.x = w * aQuat1.x + x * aQuat1.w + y * aQuat1.z - z * aQuat1.y;
		//aQuat0.y = w * aQuat1.y - x * aQuat1.z + y * aQuat1.w + z * aQuat1.x;
		//aQuat0.z = w * aQuat1.z + x * aQuat1.y - y * aQuat1.x + z * aQuat1.w;


		aQuat0.w = (aQuat1.w * w) - (aQuat1.x * x) - (aQuat1.y * y) - (aQuat1.z * z);
		aQuat0.x = (aQuat1.w * x) + (aQuat1.x * w) + (aQuat1.y * z) - (aQuat1.z * y);
		aQuat0.y = (aQuat1.w * y) + (aQuat1.y * w) + (aQuat1.z * x) - (aQuat1.x * z);
		aQuat0.z = (aQuat1.w * z) + (aQuat1.z * w) + (aQuat1.x * y) - (aQuat1.y * x);

	}

	template <class T> inline Quaternion<T> operator/(const Quaternion<T>& aQuat, const T& aScalar)
	{
		return Quaternion<T>(aQuat.w / aScalar, aQuat.x / aScalar, aQuat.y / aScalar, aQuat.z / aScalar);
	}

	template <class T> inline Quaternion<T> operator-(const Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB)
	{
		return Quaternion<T>(aQuatA.w - aQuatB.w, aQuatA.x - aQuatB.x, aQuatA.y - aQuatB.y, aQuatA.z - aQuatB.z);
	}

	template <class T> inline Quaternion<T> operator-(const Quaternion<T>& aQuat)
	{
		return Quaternion<T>(-aQuat.w, -aQuat.x, -aQuat.y, -aQuat.z);
	}

	template <class T> inline Quaternion<T> operator+(const Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB)
	{
		return Quaternion<T>(aQuatA.w + aQuatB.w, aQuatA.x + aQuatB.x, aQuatA.y + aQuatB.y, aQuatA.z + aQuatB.z);
	}

	template <class T> inline void operator+=(Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB)
	{
		aQuatA.w += aQuatB.w;
		aQuatA.x += aQuatB.x;
		aQuatA.y += aQuatB.y;
		aQuatA.z += aQuatB.z;
	}

	template<class T>
	inline void Quaternion<T>::RotateWithEuler(const Vector3<T>& anEuler)
	{
		*this *= Quaternion(anEuler);
	}

	template<class T>
	inline void Quaternion<T>::Normalize()
	{
		T length = T(1) / Length();
		w *= length;
		x *= length;
		y *= length;
		z *= length;
	}

	template<class T>
	inline Quaternion<T> Quaternion<T>::GetNormalized() const
	{
		T length = T(1) / Length();
		return Quaternion<T>(w * length, x * length, y * length, z * length);
	}

	template<class T>
	inline Quaternion<T> Quaternion<T>::GetConjugate() const
	{
		return Quaternion<T>(w, -x, -y, -z);
	}

	template<class T>
	inline T Quaternion<T>::Length2() const
	{
		return (x * x) + (y * y) + (z * z) + (w * w);
	}

	template<class T>
	inline T Quaternion<T>::Length() const
	{
		return sqrt(Length2());
	}

	template<class T>
	inline Matrix4x4<T> Quaternion<T>::GetRotationMatrix4x4f() const
	{
		Matrix4x4<T> result;

		T qxx(x * x);
		T qyy(y * y);
		T qzz(z * z);

		T qxz(x * z);
		T qxy(x * y);
		T qyz(y * z);

		T qwx(w * x);
		T qwy(w * y);
		T qwz(w * z);


		result(1, 1) = T(1) - T(2) * (qyy + qzz);
		result(2, 1) = T(2) * (qxy + qwz);
		result(3, 1) = T(2) * (qxz - qwy);

		result(1, 2) = T(2) * (qxy - qwz);
		result(2, 2) = T(1) - T(2) * (qxx + qzz);
		result(3, 2) = T(2) * (qyz + qwx);

		result(1, 3) = T(2) * (qxz + qwy);
		result(2, 3) = T(2) * (qyz - qwx);
		result(3, 3) = T(1) - T(2) * (qxx + qyy);
		return result;
	}

	template<class T>
	inline T Quaternion<T>::Dot(const Quaternion<T>& aQuat) const
	{
		return x * aQuat.x + y * aQuat.y + z * aQuat.z + w * aQuat.w;
	}

	template<class T>
	inline Vector3<T> Quaternion<T>::GetRight() const
	{
		Vector3<T> right = { 1.f, 0.f, 0.f };
		return Quaternion<T>::RotateVectorByQuaternion(*this, right);;
	}

	template<class T>
	inline Vector3<T> Quaternion<T>::GetUp() const
	{
		Vector3<T> up = { 0.f, 1.f, 0.f };
		return Quaternion<T>::RotateVectorByQuaternion(*this, up);
	}

	template<class T>
	inline Vector3<T> Quaternion<T>::GetForward() const
	{
		Vector3<T> forward = { 0.f, 0.f, 1.f };
		return Quaternion<T>::RotateVectorByQuaternion(*this, forward);
	}

	template<class T>
	inline Vector3<T> Quaternion<T>::GetEulerAnglesDegrees() const
	{
		Vector3<T> aRotationInRadians = GetEulerAnglesRadians();
		T aX = aRotationInRadians.x * static_cast<T>(RADIANS_TO_DEGREES);
		T aY = aRotationInRadians.y * static_cast<T>(RADIANS_TO_DEGREES);
		T aZ = aRotationInRadians.z * static_cast<T>(RADIANS_TO_DEGREES);
		Vector3<T> rotationInRadians(aX, aY, aZ);
		return rotationInRadians;
	}


	template<class T>
	inline Vector3<T> Quaternion<T>::GetEulerAnglesRadians() const
	{
		// roll (Z-axis rotation)
		T sinr = T(2.0) * (w * x + y * z);
		T cosr = T(1.0) - T(2.0) * (x * x + y * y);
		T pitch = atan2(sinr, cosr);// roll

		// pitch (X-axis rotation)
		T sinp = T(2.0) * (w * y - z * x);
		T yaw = T(0.0); // pitch
		if (abs(sinp) >= T(1))
		{
			yaw = copysign(static_cast<T>(PI) * T(0.5), sinp); // use 90 degrees if out of range //pitch
		}
		else
		{
			yaw = asin(sinp); // pitch
		}

		// yaw (Y-axis rotation)
		T siny = T(2.0) * (w * z + x * y);
		T cosy = T(1.0) - T(2.0) * (y * y + z * z);
		T roll = atan2(siny, cosy); // yaw

		return Vector3<T>(pitch, yaw, roll);
		//return Vector3<T>(roll, pitch, yaw);
	}

	template<class T>
	inline Vector3<T> Quaternion<T>::GetEulerAnglesRadiansd() const
	{
		float yaw, pitch, roll;
		float test = x * y + z * w;
		if (test > 0.499) { // singularity at north pole
			yaw = 2 * atan2(x, w);
			roll = PI_Half;
			pitch = 0;
			return Vector3<T>{ pitch, yaw, roll };
		}
		if (test < -0.499) { // singularity at south pole
			yaw = -2 * atan2(x, w);
			roll = -PI_Half;
			pitch = 0;
			return Vector3<T>{ pitch, yaw, roll };
		}
		float sqx = x * x;
		float sqy = y * y;
		float sqz = z * z;
		yaw = atan2(2 * y * w - 2 * x * z, 1 - 2 * sqy - 2 * sqz);
		roll = asin(2 * test);
		pitch = atan2(2 * x * w - 2 * y * z, 1 - 2 * sqx - 2 * sqz);
		return Vector3<T>{ pitch, yaw, roll };
	}



	template<class T>
	inline Vector3<T> Quaternion<T>::RotateVectorByQuaternion(const Quaternion<T>& aQuaternion, const Vector3<T>& aVectorToRotate)
	{
		Vector3<T> v(aQuaternion.x, aQuaternion.y, aQuaternion.z);
		Vector3<T> result =
			2.0f * v.Dot(aVectorToRotate) * v
			+ (aQuaternion.w * aQuaternion.w - v.Dot(v)) * aVectorToRotate
			+ 2.0f * aQuaternion.w * v.Cross(aVectorToRotate);
		return result;
	}

	template<class T>
	inline Quaternion<T> Quaternion<T>::Lerp(const Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB, const T& aDelta)
	{
		Quaternion<T> Result;
		float deltaInv = 1 - aDelta;
		Result.w = deltaInv * aQuatA.w + aDelta * aQuatB.w;
		Result.x = deltaInv * aQuatA.x + aDelta * aQuatB.x;
		Result.y = deltaInv * aQuatA.y + aDelta * aQuatB.y;
		Result.z = deltaInv * aQuatA.z + aDelta * aQuatB.z;
		Result.Normalize();
		return Result;
	}

	template<class T>
	inline Quaternion<T> Quaternion<T>::Slerp(const Quaternion<T>& aQuatA, const Quaternion<T>& aQuatB, const T& aDelta)
	{
		Quaternion<T> qz = aQuatB;

		T cosTheta = aQuatA.Dot(aQuatB);

		// If cosTheta < 0, the interpolation will take the long way around the sphere. 
		// To fix this, one quat must be negated.
		if (cosTheta < T(0))
		{
			qz = -qz;
			cosTheta = -cosTheta;
		}

		const T dotThreshold = static_cast<T>(0.9995);
		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if (cosTheta > T(1) - dotThreshold)
		{
			// Linear interpolation
			return Lerp(aQuatA, qz, aDelta);
		}
		else
		{
			// Essential Mathematics, page 467
			T angle = acos(cosTheta);
			return (sin((T(1) - aDelta) * angle) * aQuatA + sin(aDelta * angle) * qz) / sin(angle);
		}

	}

	typedef Quaternion<float> Quatf;
}