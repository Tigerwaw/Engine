#pragma once
#include <string>
#include <algorithm>

namespace Utilities
{
	void ToLower(std::string& aString)
	{
		std::transform(aString.begin(), aString.end(), aString.begin(),
			[](unsigned char aCharacter)
			{
				return static_cast<unsigned char>(std::tolower(aCharacter));
			});
	}

	void ToUpper(std::string& aString)
	{
		std::transform(aString.begin(), aString.end(), aString.begin(),
			[](unsigned char aCharacter)
			{
				return static_cast<unsigned char>(std::toupper(aCharacter));
			});
	}
}