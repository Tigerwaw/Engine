#include <vector>

namespace Utilities
{
	template<typename T>
	inline bool VectorContains(const std::vector<T>& aVector, const T& aValue)
	{
		return std::find(aVector.begin(), aVector.end(), aValue) != aVector.end();
	}
}