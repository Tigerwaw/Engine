#pragma once
#include <string>
#include <algorithm>

namespace Utilities
{
	inline void ToLower(std::string& aString)
	{
		std::transform(aString.begin(), aString.end(), aString.begin(),
			[](unsigned char aCharacter)
			{
				return static_cast<unsigned char>(std::tolower(aCharacter));
			});
	}

	inline void ToUpper(std::string& aString)
	{
		std::transform(aString.begin(), aString.end(), aString.begin(),
			[](unsigned char aCharacter)
			{
				return static_cast<unsigned char>(std::toupper(aCharacter));
			});
	}

	inline std::string ToLowerCopy(const std::string& aString)
	{
		std::string lowerCopy = aString;
		ToLower(lowerCopy);
		return lowerCopy;
	}

	inline std::string ToUpperCopy(const std::string& aString)
	{
		std::string upperCopy = aString;
		ToUpper(upperCopy);
		return upperCopy;
	}
}