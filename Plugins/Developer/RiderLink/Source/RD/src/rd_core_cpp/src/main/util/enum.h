#ifndef RD_CPP_ENUM_H
#define RD_CPP_ENUM_H

#include <type_traits>

#define RD_DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE)                                                                     \
	extern "C++"                                                                                                    \
	{                                                                                                               \
		inline ENUMTYPE operator|(ENUMTYPE a, ENUMTYPE b)                                                           \
		{                                                                                                           \
			return ENUMTYPE(((std::underlying_type_t<ENUMTYPE>) a) | ((std::underlying_type_t<ENUMTYPE>) b));       \
		}                                                                                                           \
		inline ENUMTYPE& operator|=(ENUMTYPE& a, ENUMTYPE b)                                                        \
		{                                                                                                           \
			return (ENUMTYPE&) (((std::underlying_type_t<ENUMTYPE>&) a) |= ((std::underlying_type_t<ENUMTYPE>) b)); \
		}                                                                                                           \
		inline ENUMTYPE operator&(ENUMTYPE a, ENUMTYPE b)                                                           \
		{                                                                                                           \
			return ENUMTYPE(((std::underlying_type_t<ENUMTYPE>) a) & ((std::underlying_type_t<ENUMTYPE>) b));       \
		}                                                                                                           \
		inline ENUMTYPE& operator&=(ENUMTYPE& a, ENUMTYPE b)                                                        \
		{                                                                                                           \
			return (ENUMTYPE&) (((std::underlying_type_t<ENUMTYPE>&) a) &= ((std::underlying_type_t<ENUMTYPE>) b)); \
		}                                                                                                           \
		inline ENUMTYPE operator~(ENUMTYPE a)                                                                       \
		{                                                                                                           \
			return ENUMTYPE(~((std::underlying_type_t<ENUMTYPE>) a));                                               \
		}                                                                                                           \
		inline ENUMTYPE operator^(ENUMTYPE a, ENUMTYPE b)                                                           \
		{                                                                                                           \
			return ENUMTYPE(((std::underlying_type_t<ENUMTYPE>) a) ^ ((std::underlying_type_t<ENUMTYPE>) b));       \
		}                                                                                                           \
		inline ENUMTYPE& operator^=(ENUMTYPE& a, ENUMTYPE b)                                                        \
		{                                                                                                           \
			return (ENUMTYPE&) (((std::underlying_type_t<ENUMTYPE>&) a) ^= ((std::underlying_type_t<ENUMTYPE>) b)); \
		}                                                                                                           \
	}

#endif	  // RD_CPP_ENUM_H
