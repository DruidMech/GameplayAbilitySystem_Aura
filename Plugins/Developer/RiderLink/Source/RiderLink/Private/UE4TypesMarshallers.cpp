#include "UE4TypesMarshallers.h"

#include "Containers/StringConv.h"
#include "serialization/ArraySerializer.h"
#include "Templates/UniquePtr.h"

//region FString

namespace rd {

    FString Polymorphic<FString, void>::read(SerializationCtx& ctx, Buffer& buffer) {
        UCS2CHAR * str = buffer.read_char16_string();
        FString tmp{str};
        delete str;
        return tmp;
    }

    void Polymorphic<FString, void>::write(SerializationCtx& ctx, Buffer& buffer, FString const& value) {
        buffer.write_char16_string(reinterpret_cast<const uint16_t*>(GetData(value)), value.Len());
    }


    size_t hash<FString>::operator()(const FString& value) const noexcept {
        return GetTypeHash(value);
    }


}

template class rd::Polymorphic<FString>;
template class rd::Polymorphic<rd::Wrapper<FString>>;
template struct rd::hash<FString>;
// template class rd::Polymorphic<TArray<FString>, void>;

//endregion
