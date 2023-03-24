#define NOMINMAX

#include "PkgInputStream.h"

#include <algorithm>

namespace rd
{
void PkgInputStream::rewind()
{
	buffer.rewind();
}

void PkgInputStream::require_available(int size)
{
	buffer.require_available(size);
}

size_t PkgInputStream::get_position() const
{
	return buffer.get_position();
}

Buffer::word_t* PkgInputStream::data()
{
	return buffer.data();
}

Buffer& PkgInputStream::get_buffer()
{
	return buffer;
}

int32_t PkgInputStream::try_read(Buffer::word_t* res, size_t size)
{
	if (memory == -1 || buffer.get_position() == memory)
	{
		memory = request_data();
		if (memory == -1)
		{
			return -1;
		}
	}
	const int32_t n = static_cast<int32_t>((std::min)(size, memory - buffer.get_position()));
	Buffer::word_t* start = buffer.current_pointer();
	std::copy(start, start + n, res);
	buffer.set_position(buffer.get_position() + n);
	return n;
}

bool PkgInputStream::read(Buffer::word_t* res, size_t size)
{
	//		spdlog::trace("PkgInputStream call: size={}, pos={}, memory={}", size, buffer.get_position(), memory);

	int32_t summary_size = 0;
	while (summary_size < size)
	{
		const int32_t bytes_read = try_read(res + summary_size, size - summary_size);
		if (bytes_read == -1)
		{
			return false;
		}
		summary_size += bytes_read;
	}
	return true;
}
}	 // namespace rd
