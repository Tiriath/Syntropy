
/// \file string.inl
///
/// \author Raffaele D. Facendola - February 2021

#pragma once

#include "syntropy/language/foundation/foundation.h"

#include "syntropy/core/ranges/sized_range.h"
#include "syntropy/core/support/compare.h"
#include "syntropy/core/support/swap.h"

#include "syntropy/memory/allocators/allocator.h"
#include "syntropy/memory/foundation/buffer.h"
#include "syntropy/memory/foundation/memory.h"
#include "syntropy/memory/foundation/byte_span.h"

// ===========================================================================

namespace Syntropy::Strings
{
    /************************************************************************/
    /* STRING                                                               */
    /************************************************************************/

    inline String
    ::String (Null) noexcept
        : String()
    {

    }

    template <Int TSize>
    String
    ::String(StringLiteral<TSize> characters) noexcept
        : code_points_(Memory::ToBytes(TSize))
    {
        Memory::Copy(code_points_,
                     Memory::MakeByteSpan(characters));
    }

    inline String
    ::operator StringView() const noexcept
    {
        return StringView{ code_points_ };
    }

    inline void String
    ::Swap(Movable<String> rhs) noexcept
    {
        Algorithm::Swap(code_points_, rhs.code_points_);
    }

    [[nodiscard]] Memory::ByteSpan String
    ::GetCodePoints() const noexcept
    {
        return ViewOf(code_points_);
    }

    [[nodiscard]] inline
    Mutable<Memory::BaseAllocator> String
    ::GetAllocator() const noexcept
    {
        return code_points_.GetAllocator();
    }

    /************************************************************************/
    /* NON-MEMBER FUNCTIONS                                                 */
    /************************************************************************/

    // Comparison.
    // ===========

    inline Bool
    operator==(Immutable<String> lhs, Immutable<String> rhs) noexcept
    {
        return ViewOf(lhs) == ViewOf(rhs);
    }

    template <Int TSize>
    inline Bool
    operator==(Immutable<String> lhs, StringLiteral<TSize> rhs) noexcept
    {
        return ViewOf(lhs) == rhs;
    }

    template <Int TSize>
    inline Bool
    operator==(StringLiteral<TSize> lhs, Immutable<String> rhs) noexcept
    {
        return lhs == ViewOf(rhs);
    }

    // Ranges.
    // =======

    inline StringView
    ViewOf(Immutable<String> string) noexcept
    {
        return string;
    }

}

// ===========================================================================
