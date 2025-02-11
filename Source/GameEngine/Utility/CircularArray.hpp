#pragma once
#include <array>

template <typename T, int MAXSIZE>
class CircularArray
{
public:
	CircularArray();
	T& Push_back(T& aItem);
	void Clear();
	int Size() const;

	const T& operator[](int aIndex) const;
	T& operator[](int aIndex);
private:
	std::array<T, MAXSIZE> myArray;
	int myFirstIndex = 0;
	int myLastIndex = 0;
	int mySize = 0;
};

template<typename T, int MAXSIZE>
inline CircularArray<T, MAXSIZE>::CircularArray()
{
	mySize = MAXSIZE;
}

template<typename T, int MAXSIZE>
inline T& CircularArray<T, MAXSIZE>::Push_back(T& aItem)
{
	int lastIndex = myLastIndex;
	int newLastIndex = (myLastIndex + 1) % mySize;

	if (newLastIndex == myFirstIndex)
	{
		myFirstIndex = (myFirstIndex + 1) % mySize;
	}

	myArray[myLastIndex] = aItem;
	myLastIndex = newLastIndex;

	return myArray[lastIndex];
}

template<typename T, int MAXSIZE>
inline void CircularArray<T, MAXSIZE>::Clear()
{
	myFirstIndex = 0;
	myLastIndex = 0;
}	

template<typename T, int MAXSIZE>
inline int CircularArray<T, MAXSIZE>::Size() const
{
	return mySize;
}

template<typename T, int MAXSIZE>
inline const T& CircularArray<T, MAXSIZE>::operator[](int aIndex) const
{
	assert(aIndex >= 0);
	assert(aIndex < mySize);
	assert(myLastIndex != myFirstIndex);

	return myArray[(myFirstIndex + aIndex) % (mySize + myLastIndex)];
}

template<typename T, int MAXSIZE>
inline T& CircularArray<T, MAXSIZE>::operator[](int aIndex)
{
	assert(aIndex >= 0);
	assert(aIndex < mySize);
	assert(myLastIndex != myFirstIndex);

	return myArray[(myFirstIndex + aIndex) % (mySize + myLastIndex)];
}
