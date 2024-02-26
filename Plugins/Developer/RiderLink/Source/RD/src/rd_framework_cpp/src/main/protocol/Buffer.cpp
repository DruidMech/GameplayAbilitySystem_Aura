#include <utility>

#include "protocol/Buffer.h"

#include <string>
#include <algorithm>

namespace rd
{
Buffer::Buffer() : Buffer(16)
{
}

Buffer::Buffer(size_t initialSize) : data_(initialSize)
{
}

Buffer::Buffer(ByteArray array, size_t offset) : data_(std::move(array)), offset(offset)
{
}

size_t Buffer::get_position() const
{
	return offset;
}

void Buffer::set_position(size_t value)
{
	offset = value;
}

void Buffer::check_available(size_t moreSize) const
{
	if (offset + moreSize > size())
	{
		throw std::out_of_range(
			"Expected " + std::to_string(moreSize) + " bytes in buffer, only" + std::to_string(size() - offset) + "available");
	}
}

void Buffer::read(word_t* dst, size_t size)
{
	if (size == 0)
		return;
	check_available(size);
	std::copy(&data_[offset], &data_[offset] + size, dst);
	offset += size;
}

void Buffer::write(const word_t* src, size_t size)
{
	if (size == 0)
		return;
	require_available(size);
	std::copy(src, src + size, &data_[offset]);
	offset += size;
}

void Buffer::require_available(size_t moreSize)
{
	if (offset + moreSize >= size())
	{
		const size_t new_size = (std::max)(size() * 2, offset + moreSize);
		data_.resize(new_size);
	}
}

void Buffer::rewind()
{
	set_position(0);
}

Buffer::ByteArray Buffer::getArray() const&
{
	return data_;
}

Buffer::ByteArray Buffer::getArray() &&
{
	rewind();
	return std::move(data_);
}

Buffer::ByteArray Buffer::getRealArray() const&
{
	auto res = getArray();
	res.resize(offset);
	return res;
}

Buffer::ByteArray Buffer::getRealArray() &&
{
	auto res = std::move(data_);
	res.resize(offset);
	rewind();
	return res;
}

Buffer::word_t const* Buffer::data() const
{
	return data_.data();
}

Buffer::word_t* Buffer::data()
{
	return data_.data();
}

Buffer::word_t const* Buffer::current_pointer() const
{
	return data() + offset;
}

Buffer::word_t* Buffer::current_pointer()
{
	return data() + offset;
}

size_t Buffer::size() const
{
	return data_.size();
}

/*std::string Buffer::readString() const {
auto v = readArray<uint8_t>();
return std::string(v.begin(), v.end());
}

void Buffer::writeString(std::string const &value) const {
std::vector<uint8_t> v(value.begin(), value.end());
writeArray<uint8_t>(v);
}*/

template <int>
std::wstring read_wstring_spec(Buffer& buffer)
{
	auto v = buffer.read_array<std::vector, uint16_t>();
	return std::wstring(v.begin(), v.end());
}

template <>
std::wstring read_wstring_spec<2>(Buffer& buffer)
{
	const int32_t len = buffer.read_integral<int32_t>();
	RD_ASSERT_MSG(len >= 0, "read null string(length =" + std::to_string(len) + ")");
	std::wstring result;
	result.resize(len);
	buffer.read(reinterpret_cast<Buffer::word_t*>(&result[0]), sizeof(wchar_t) * len);
	return result;
}

std::wstring Buffer::read_wstring()
{
	return read_wstring_spec<sizeof(wchar_t)>(*this);
}

template <int>
void write_wstring_spec(Buffer& buffer, wstring_view value)
{
	const std::vector<uint16_t> v(value.begin(), value.end());
	buffer.write_array<std::vector, uint16_t>(v);
}

template <>
void write_wstring_spec<2>(Buffer& buffer, wstring_view value)
{
	buffer.write_integral<int32_t>(static_cast<int32_t>(value.size()));
	buffer.write(reinterpret_cast<Buffer::word_t const*>(value.data()), sizeof(wchar_t) * value.size());
}

void Buffer::write_wstring(std::wstring const& value)
{
	write_wstring(wstring_view(value));
}

void Buffer::write_char16_string(const uint16_t* data, size_t len)
{
	write_integral<int32_t>(static_cast<int32_t>(len));
	write(reinterpret_cast<word_t const*>(data), sizeof(uint16_t) * len);
}

uint16_t* Buffer::read_char16_string()
{	
	const int32_t len = read_integral<int32_t>();
	RD_ASSERT_MSG(len >= 0, "read null string(length =" + std::to_string(len) + ")");
	uint16_t * result = new uint16_t[len+1];
	read(reinterpret_cast<Buffer::word_t*>(&result[0]), sizeof(uint16_t) * len);
	result[len] = 0;
	return result;
}

void Buffer::write_wstring(wstring_view value)
{
	write_wstring_spec<sizeof(wchar_t)>(*this, value);
}

void Buffer::write_wstring(Wrapper<std::wstring> const& value)
{
	write_wstring(*value);
}

int64_t TICKS_AT_EPOCH = 621355968000000000L;
int64_t TICKS_PER_MILLISECOND = 10000000;

DateTime Buffer::read_date_time()
{
	int64_t time_in_ticks = read_integral<int64_t>();
	time_t t = static_cast<time_t>(time_in_ticks / TICKS_PER_MILLISECOND - TICKS_AT_EPOCH / TICKS_PER_MILLISECOND);
	return DateTime{t};
}

void Buffer::write_date_time(DateTime const& date_time)
{
	uint64_t t = date_time.seconds * TICKS_PER_MILLISECOND + TICKS_AT_EPOCH;
	write_integral<int64_t>(t);
}

bool Buffer::read_bool()
{
	const auto res = read_integral<uint8_t>();
	RD_ASSERT_MSG(res == 0 || res == 1, "get byte:" + std::to_string(res) + " instead of 0 or 1");
	return res == 1;
}

void Buffer::write_bool(bool value)
{
	write_integral<word_t>(value ? 1 : 0);
}

wchar_t Buffer::read_char()
{
	return static_cast<wchar_t>(read_integral<uint16_t>());
}

void Buffer::write_char(wchar_t value)
{
	write_integral<uint16_t>(value);
}

void Buffer::read_byte_array(ByteArray& array)
{
	const int32_t length = read_integral<int32_t>();
	array.resize(length);
	read_byte_array_raw(array);
}

void Buffer::read_byte_array_raw(ByteArray& array)
{
	read(array.data(), array.size());
}

void Buffer::write_byte_array_raw(const ByteArray& array)
{
	write(array.data(), array.size());
}

Buffer::ByteArray& Buffer::get_data()
{
	return data_;
}
}	 // namespace rd
