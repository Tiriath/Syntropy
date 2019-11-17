#include "syntropy/serialization/msgpack/msgpack_stream.h"

namespace syntropy
{
    /************************************************************************/
    /* MSGPACK STREAM                                                       */
    /************************************************************************/

    MsgpackStream& MsgpackStream::operator<<(std::int8_t rhs)
    {
        if (Msgpack::IsPositiveFixInt(rhs))
        {
            Put(Msgpack::EncodePositiveFixInt(rhs));
        }
        else if (Msgpack::IsNegativeFixInt(rhs))
        {
            Put(Msgpack::EncodeNegativeFixInt(rhs));
        }
        else
        {
            Put(MsgpackFormat::kInt8);
            Put(Msgpack::Encode(rhs));
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator<<(std::int16_t rhs)
    {
        if (Msgpack::IsInt8(rhs))
        {
            operator<<(std::int8_t(rhs));
        }
        else
        {
            Put(MsgpackFormat::kInt16);
            Put(Msgpack::Encode(rhs));
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator<<(std::int32_t rhs)
    {
        if (Msgpack::IsInt16(rhs))
        {
            operator<<(std::int16_t(rhs));
        }
        else
        {
            Put(MsgpackFormat::kInt32);
            Put(Msgpack::Encode(rhs));
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator<<(std::int64_t rhs)
    {
        if (Msgpack::IsInt32(rhs))
        {
            operator<<(std::int32_t(rhs));
        }
        else
        {
            Put(MsgpackFormat::kInt64);
            Put(Msgpack::Encode(rhs));
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator<<(std::uint8_t rhs)
    {
        if (Msgpack::IsPositiveFixInt(rhs))
        {
            Put(Msgpack::EncodePositiveFixInt(rhs));
        }
        else
        {
            Put(MsgpackFormat::kUInt8);
            Put(Msgpack::Encode(rhs));
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator<<(std::uint16_t rhs)
    {
        if (Msgpack::IsUInt8(rhs))
        {
            operator<<(std::uint8_t(rhs));
        }
        else
        {
            Put(MsgpackFormat::kUInt16);
            Put(Msgpack::Encode(rhs));
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator<<(std::uint32_t rhs)
    {
        if (Msgpack::IsUInt16(rhs))
        {
            operator<<(std::uint16_t(rhs));
        }
        else
        {
            Put(MsgpackFormat::kUInt32);
            Put(Msgpack::Encode(rhs));
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator<<(std::uint64_t rhs)
    {
        if (Msgpack::IsUInt32(rhs))
        {
            operator<<(std::uint32_t(rhs));
        }
        else
        {
            Put(MsgpackFormat::kUInt64);
            Put(Msgpack::Encode(rhs));
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator<<(const std::string& rhs)
    {
        if (Msgpack::IsFixStr(rhs))
        {
            Put(Msgpack::EncodeFixStrLength(std::uint8_t(rhs.size())));
        }
        else if (Msgpack::IsStr8(rhs))
        {
            Put(MsgpackFormat::kStr8);
            Put(Msgpack::Encode(std::uint8_t(rhs.size())));
        }
        else if (Msgpack::IsStr16(rhs))
        {
            Put(MsgpackFormat::kStr16);
            Put(Msgpack::Encode(std::uint16_t(rhs.size())));
        }
        else if (Msgpack::IsStr32(rhs))
        {
            Put(MsgpackFormat::kStr32);
            Put(Msgpack::Encode(std::uint32_t(rhs.size())));
        }

        Put(rhs.data(), rhs.size());

        return *this;
    }

    MsgpackStream& MsgpackStream::operator<<(const std::vector<std::uint8_t>& rhs)
    {
        if (Msgpack::IsBin8(rhs))
        {
            Put(MsgpackFormat::kBin8);
            Put(Msgpack::Encode(std::uint8_t(rhs.size())));
        }
        else if (Msgpack::IsBin16(rhs))
        {
            Put(MsgpackFormat::kBin16);
            Put(Msgpack::Encode(std::uint16_t(rhs.size())));
        }
        else if (Msgpack::IsBin32(rhs))
        {
            Put(MsgpackFormat::kBin32);
            Put(Msgpack::Encode(std::uint32_t(rhs.size())));
        }

        Put(rhs.data(), rhs.size());

        return *this;
    }

    MsgpackStream& MsgpackStream::operator>>(bool& rhs)
    {
        auto sentry = Sentry(*this);

        if (Test(MsgpackFormat::kTrue))
        {
            rhs = true;
            sentry.Dismiss();
        }
        else if (Test(MsgpackFormat::kFalse))
        {
            rhs = false;
            sentry.Dismiss();
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator>>(std::int8_t& rhs)
    {
        auto sentry = Sentry(*this);

        if (Msgpack::IsPositiveFixIntFormat(Peek()))
        {
            rhs = Msgpack::DecodePositiveFixInt(Get());
            sentry.Dismiss();
        }
        else if (Msgpack::IsNegativeFixIntFormat(Peek()))
        {
            rhs = Msgpack::DecodeNegativeFixInt(Get());
            sentry.Dismiss();
        }
        else if (Test(MsgpackFormat::kInt8))
        {
            rhs = Msgpack::DecodeInt8(Get());
            sentry.Dismiss();
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator>>(std::int16_t& rhs)
    {
        auto sentry = Sentry(*this);

        if (Test(MsgpackFormat::kInt16))
        {
            auto rhs_high = std::int16_t{};
            Get(rhs_high);
            rhs = Msgpack::DecodeInt16(rhs_high);
            sentry.Dismiss();
        }
        else
        {
            auto rhs_low = std::int8_t{};
            operator>>(rhs_low);
            rhs = rhs_low;
            sentry.Dismiss();
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator>>(std::int32_t& rhs)
    {
        auto sentry = Sentry(*this);

        if (Test(MsgpackFormat::kInt32))
        {
            auto rhs_high = std::int32_t{};
            Get(rhs_high);
            rhs = Msgpack::DecodeInt32(rhs_high);
            sentry.Dismiss();
        }
        else
        {
            auto rhs_low = std::int16_t{};
            operator>>(rhs_low);
            rhs = rhs_low;
            sentry.Dismiss();
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator>>(std::int64_t& rhs)
    {
        auto sentry = Sentry(*this);

        if (Test(MsgpackFormat::kInt64))
        {
            auto rhs_high = std::int64_t{};
            Get(rhs_high);
            rhs = Msgpack::DecodeInt64(rhs_high);
            sentry.Dismiss();
        }
        else
        {
            auto rhs_low = std::int32_t{};
            operator>>(rhs_low);
            rhs = rhs_low;
            sentry.Dismiss();
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator>>(std::uint8_t& rhs)
    {
        auto sentry = Sentry(*this);

        if (Msgpack::IsPositiveFixIntFormat(Peek()))
        {
            rhs = Msgpack::DecodePositiveFixUInt(Get());
            sentry.Dismiss();
        }
        else if (Test(MsgpackFormat::kUInt8))
        {
            rhs = Msgpack::DecodeUInt8(Get());
            sentry.Dismiss();
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator>>(std::uint16_t& rhs)
    {
        auto sentry = Sentry(*this);

        if (Test(MsgpackFormat::kUInt16))
        {
            auto rhs_high = std::int16_t{};
            Get(rhs_high);
            rhs = Msgpack::DecodeUInt16(rhs_high);
            sentry.Dismiss();
        }
        else
        {
            auto rhs_low = std::uint8_t{};
            operator>>(rhs_low);
            rhs = rhs_low;
            sentry.Dismiss();
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator>>(std::uint32_t& rhs)
    {
        auto sentry = Sentry(*this);

        if (Test(MsgpackFormat::kUInt32))
        {
            auto rhs_high = std::int32_t{};
            Get(rhs_high);
            rhs = Msgpack::DecodeUInt32(rhs_high);
            sentry.Dismiss();
        }
        else
        {
            auto rhs_low = std::uint16_t{};
            operator>>(rhs_low);
            rhs = rhs_low;
            sentry.Dismiss();
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator>>(std::uint64_t& rhs)
    {
        auto sentry = Sentry(*this);

        if (Test(MsgpackFormat::kUInt64))
        {
            auto rhs_high = std::int64_t{};
            Get(rhs_high);
            rhs = Msgpack::DecodeUInt64(rhs_high);
            sentry.Dismiss();
        }
        else
        {
            auto rhs_low = std::uint32_t{};
            operator>>(rhs_low);
            rhs = rhs_low;
            sentry.Dismiss();
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator>>(float& rhs)
    {
        auto sentry = Sentry(*this);

        if (Test(MsgpackFormat::kFloat32))
        {
            auto rhs_encoded = std::int32_t{};
            Get(rhs_encoded);
            rhs = Msgpack::DecodeFloat(rhs_encoded);
            sentry.Dismiss();
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator>>(double& rhs)
    {
        auto sentry = Sentry(*this);

        if (Test(MsgpackFormat::kFloat64))
        {
            auto rhs_encoded = std::int64_t{};
            Get(rhs_encoded);
            rhs = Msgpack::DecodeDouble(rhs_encoded);
            sentry.Dismiss();
        }

        return *this;
    }

    MsgpackStream& MsgpackStream::operator>>(std::string& rhs)
    {
        auto sentry = Sentry(*this);

        auto length = std::optional<std::size_t>{};

        if (Msgpack::IsFixStrFormat(Peek()))
        {
            length = Msgpack::DecodeFixStrLength(Get());
        }
        else if (Test(MsgpackFormat::kStr8))
        {
            auto length_encoded = std::int8_t{};
            Get(length_encoded);
            length = Msgpack::DecodeUInt8(length_encoded);
        }
        else if (Test(MsgpackFormat::kStr16))
        {
            auto length_encoded = std::int16_t{};
            Get(length_encoded);
            length = Msgpack::DecodeUInt16(length_encoded);
        }
        else if (Test(MsgpackFormat::kStr32))
        {
            auto length_encoded = std::int32_t{};
            Get(length_encoded);
            length = Msgpack::DecodeUInt32(length_encoded);
        }

        if (length)
        {
            rhs.resize(*length);

            Get(rhs.data(), *length);

            sentry.Dismiss();
        }

        return *this;
    }

}