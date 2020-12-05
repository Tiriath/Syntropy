
/// \file byte.h
/// \brief This header is part of Syntropy memory module. It contains definitions for memory-related fundamental data types.
///
/// \author Raffaele D. Facendola - August  2020

#pragma once

#include <ostream>
#include <cstddef>

#include "syntropy/language/templates/traits.h"
#include "syntropy/language/foundation/foundation.h"

namespace Syntropy::Memory
{
    /************************************************************************/
    /* FUNDAMENTAL TYPES                                                    */
    /************************************************************************/

    /// \brief Represents a single byte.
    /// This type is neither a character type, nor an arithmetic type.
    enum class Byte : std::int8_t {};

    /************************************************************************/
    /* POINTER TYPES                                                        */
    /************************************************************************/

    /// \brief A non-owning raw pointer to a read-only memory location.
    using BytePtr = Pointer<Byte>;

    /// \brief A non-owning raw pointer to a read-write memory location.
    using RWBytePtr = RWPointer<Byte>;

    /************************************************************************/
    /* CONVERSION                                                           */
    /************************************************************************/

    /// \brief Convert a value to a byte value.
    template <typename TNumber>
    constexpr Byte ToByte(TNumber rhs) noexcept;

    /// \brief Convert rhs to a read-only byte pointer.
    template <typename TType>
    constexpr BytePtr ToBytePtr(Pointer<TType> rhs) noexcept;

    /// \brief Convert rhs to a read-write byte pointer.
    template <typename TType>
    constexpr RWBytePtr ToRWBytePtr(RWPointer<TType> rhs) noexcept;

    /************************************************************************/
    /* STREAM INSERTION                                                     */
    /************************************************************************/

    /// \brief Stream insertion for byte types.
    std::ostream& operator<<(std::ostream& out, Byte rhs) noexcept;

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Conversion.
    // ===========

    template <typename TNumber>
    constexpr Byte ToByte(TNumber rhs) noexcept
    {
        return static_cast<Byte>(rhs);
    }

    template <typename TType>
    constexpr BytePtr ToBytePtr(Pointer<TType> rhs) noexcept
    {
        return reinterpret_cast<BytePtr>(rhs);
    }

    template <typename TType>
    constexpr RWBytePtr ToRWBytePtr(RWPointer<TType> rhs) noexcept
    {
        return reinterpret_cast<RWBytePtr>(rhs);
    }

    // Stream insertion.
    // =================

    inline std::ostream& operator<<(std::ostream& out, Byte rhs) noexcept
    {
        out << ToInt(rhs);

        return out;
    }
}
