#ifndef RD_CPP_RDSYMMETRICCALL_H
#define RD_CPP_RDSYMMETRICCALL_H

#include "task/RdCall.h"
#include "task/RdEndpoint.h"

namespace rd
{
template <typename TReq, typename TRes, typename ReqSer = Polymorphic<TReq>, typename ResSer = Polymorphic<TRes> >
class RdSymmetricCall : public RdCall<TReq, TRes, ReqSer, ResSer>, public RdEndpoint<TReq, TRes, ReqSer, ResSer>
{
public:
	void init(Lifetime lifetime) const override
	{
		RdCall<TReq, TRes, ReqSer, ResSer>::init(lifetime);
	}

	void on_wire_received(Buffer buffer) const override
	{
		RdEndpoint<TReq, TRes, ReqSer, ResSer>::on_wire_received(std::move(buffer));
	}

	friend bool operator==(const RdSymmetricCall& lhs, const RdSymmetricCall& rhs)
	{
		return &lhs == &rhs;
	}

	friend bool operator!=(const RdSymmetricCall& lhs, const RdSymmetricCall& rhs)
	{
		return !(rhs == lhs);
	}

	friend std::string to_string(RdSymmetricCall const& value)
	{
		return "RdSymmetricCall";
	}
};
}	 // namespace rd

#endif	  // RD_CPP_RDSYMMETRICCALL_H
