#ifndef RD_CPP_VIEWABLE_COLLECTIONS_H
#define RD_CPP_VIEWABLE_COLLECTIONS_H

#include <string>

namespace rd
{
enum class AddRemove
{
	ADD,
	REMOVE
};

inline std::string to_string(AddRemove kind)
{
	switch (kind)
	{
		case AddRemove::ADD:
			return "Add";
		case AddRemove::REMOVE:
			return "Remove";
		default:
			return "";
	}
}

enum class Op
{
	ADD,
	UPDATE,
	REMOVE,
	ACK
};

inline std::string to_string(Op op)
{
	switch (op)
	{
		case Op::ADD:
			return "Add";
		case Op::UPDATE:
			return "Update";
		case Op::REMOVE:
			return "Remove";
		case Op::ACK:
			return "Ack";
		default:
			return "";
	}
}
}	 // namespace rd

#endif	  // RD_CPP_VIEWABLE_COLLECTIONS_H
