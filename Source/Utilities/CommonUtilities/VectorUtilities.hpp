#include <vector>

namespace Utilities
{
	template<typename T>
	inline bool VectorContains(const std::vector<T>& aVector, T aValue)
	{
		return std::find(aVector.begin(), aVector.end(), aValue) != aVector.end();
	}
}