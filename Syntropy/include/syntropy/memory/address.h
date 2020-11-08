
/// \file address.h
/// \brief This header is part of Syntropy memory module. It contains definitions for numeric address values.
///
/// \author Raffaele D. Facendola - August 2020

#pragma once

#include <cstdint>

#include "syntropy/language/templates/traits.h"
#include "syntropy/language/foundation/foundation.h"

namespace Syntropy::Memory
{
    /************************************************************************/
    /* ADDRESS                                                              */
    /************************************************************************/

    /// \brief Numeric representation of an address to a read-only memory location.
    enum class Address : std::intptr_t {};

    /// \brief Numeric representation of an address to a read-write memory location.
    enum class RWAddress : std::intptr_t {};

    /************************************************************************/
    /* ARITHMETIC                                                           */
    /************************************************************************/

    /// \brief Move the address forwards.
    constexpr Address operator+(Address lhs, Int rhs) noexcept;

    /// \brief Move the address backwards
    constexpr Address operator-(Address lhs, Int rhs) noexcept;

    /// \brief Move the address forwards.
    constexpr RWAddress operator+(RWAddress lhs, Int rhs) noexcept;

    /// \brief Move the address backwards
    constexpr RWAddress operator-(RWAddress lhs, Int rhs) noexcept;

    /// \brief Bit-wise mask of a numeric address to a read-only location.
    constexpr Address operator&(Address lhs, Int rhs) noexcept;

    /// \brief Bit-wise mask of a numeric address to a read-write location.
    constexpr RWAddress operator&(RWAddress lhs, Int rhs) noexcept;

    /************************************************************************/
    /* CONVERSION                                                           */
    /************************************************************************/

    /// \brief Convert a read-only address to its numeric value.
    constexpr Int ToInt(Address rhs) noexcept;

    /// \brief Convert a read-write address to its numeric value.
    constexpr Int ToInt(RWAddress rhs) noexcept;

    /// \brief Convert a typeless pointer to a read-only memory region numeric address value.
    Address ToAddress(TypelessPtr rhs) noexcept;

    /// \brief Convert a typeless pointer to a read-write memory region numeric address value.
    RWAddress ToRWAddress(TypelessPtr rhs) noexcept;

    /// \brief Convert an address to a read-only location to a read-only strongly-typed pointer.
    /// If the pointed object is not an instance of TType, the behavior of this method is undefined.
    template <typename TType = Syntropy::Templates::AddConst<Byte>>
    Pointer<TType> FromAddress(Address rhs) noexcept;

    /// \brief Convert an address to a read-only location to a read-only strongly-typed pointer.
    /// If the pointed object is not an instance of TType, the behavior of this method is undefined.
    template <typename TType = Syntropy::Templates::AddConst<Byte>>
    Pointer<TType> FromAddress(RWAddress rhs) noexcept;

    /// \brief Convert an address to a read-write location to a read-write strongly-typed pointer.
    /// If the pointed object is not an instance of const TType, the behavior of this method is undefined.
    template <typename TType = Byte>
    RWPointer<TType> FromRWAddress(RWAddress rhs) noexcept;

    /// \brief Convert a numeric address to a numeric address to a read-only location.
    [[nodiscard]] constexpr Address ReadOnly(Address rhs) noexcept;

    /// \brief Convert a numeric address to a numeric address to a read-only location.
    [[nodiscard]] constexpr Address ReadOnly(RWAddress rhs) noexcept;

    /// \brief Convert a numeric address to a numeric address to a read-write location.
    /// If rhs doesn't refer to an original read-write memory location, the behavior of this method is undefined.
    [[nodiscard]] constexpr RWAddress ReadWrite(Address rhs) noexcept;

    /// \brief Convert a numeric address to a numeric address to a read-write location.
    [[nodiscard]] constexpr RWAddress ReadWrite(RWAddress rhs) noexcept;

    /************************************************************************/
    /* STREAM INSERTION                                                     */
    /************************************************************************/

    /// \brief Stream insertion for an address to a read-only memory location.
    std::ostream& operator<<(std::ostream& out, Address rhs) noexcept;

    /// \brief Stream insertion for an address to a read-write memory location.
    std::ostream& operator<<(std::ostream& out, RWAddress rhs) noexcept;

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Arithmetic.
    // ===========

    constexpr Address operator+(Address lhs, Int rhs) noexcept
    {
        return Address{ ToInt(lhs) + rhs };
    }

    constexpr Address operator-(Address lhs, Int rhs) noexcept
    {
        return Address{ ToInt(lhs) - rhs };
    }

    constexpr RWAddress operator+(RWAddress lhs, Int rhs) noexcept
    {
        return ReadWrite(ReadOnly(lhs) + rhs);
    }

    constexpr RWAddress operator-(RWAddress lhs, Int rhs) noexcept
    {
        return ReadWrite(ReadOnly(lhs) - rhs);
    }

    constexpr Address operator&(Address lhs, Int rhs) noexcept
    {
        return Address{ ToInt(lhs) & rhs };
    }

    constexpr RWAddress operator&(RWAddress lhs, Int rhs) noexcept
    {
        return RWAddress{ ToInt(lhs) & rhs };
    }

    // Conversion.
    // ===========

    constexpr Int ToInt(Address rhs) noexcept
    {
        return static_cast<Int>(rhs);
    }

    constexpr Int ToInt(RWAddress rhs) noexcept
    {
        return static_cast<Int>(rhs);
    }

    constexpr Address ReadOnly(Address rhs) noexcept
    {
        return rhs;
    }

    constexpr Address ReadOnly(RWAddress rhs) noexcept
    {
        return Address{ ToInt(rhs) };
    }

    constexpr RWAddress ReadWrite(Address rhs) noexcept
    {
        return RWAddress{ ToInt(rhs) };
    }

    constexpr RWAddress ReadWrite(RWAddress rhs) noexcept
    {
        return rhs;
    }

    inline Address ToAddress(TypelessPtr rhs) noexcept
    {
        auto address = reinterpret_cast<std::intptr_t>(rhs);

        return static_cast<Address>(address);
    }

    inline RWAddress ToRWAddress(TypelessPtr rhs) noexcept
    {
        auto address = reinterpret_cast<std::intptr_t>(rhs);

        return static_cast<RWAddress>(address);
    }

    template <typename TType>
    inline Pointer<TType> FromAddress(Address rhs) noexcept
    {
        auto address = static_cast<std::intptr_t>(rhs);

        return reinterpret_cast<Pointer<TType>>(address);
    }

    template <typename TType>
    inline Pointer<TType> FromAddress(RWAddress rhs) noexcept
    {
        auto address = static_cast<std::intptr_t>(rhs);

        return reinterpret_cast<Pointer<TType>>(address);
    }

    template <typename TType>
    inline RWPointer<TType> FromRWAddress(RWAddress rhs) noexcept
    {
        auto address = static_cast<std::intptr_t>(rhs);

        return reinterpret_cast<RWPointer<TType>>(address);
    }

    // Stream insertion.
    // =================

    inline std::ostream& operator<<(std::ostream& out, Address rhs) noexcept
    {
        out << ToInt(rhs);

        return out;
    }

    inline std::ostream& operator<<(std::ostream& out, RWAddress rhs) noexcept
    {
        out << ToInt(rhs);

        return out;
    }

}


