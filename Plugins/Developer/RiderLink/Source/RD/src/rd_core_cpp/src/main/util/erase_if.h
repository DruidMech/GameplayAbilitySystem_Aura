#ifndef RD_CPP_ERASE_IF_H
#define RD_CPP_ERASE_IF_H

namespace rd
{
namespace util
{
template <typename ContainerT, class _Pr>
void erase_if(ContainerT& items, _Pr _Pred)
{
	for (auto it = items.begin(); it != items.end();)
	{
		if (_Pred(it->second))
			it = items.erase(it);
		else
			++it;
	}
}
}	 // namespace util
}	 // namespace rd

#endif	  // RD_CPP_ERASE_IF_H
