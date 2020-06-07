
/// \file memory.h
/// \brief This header is part of the Syntropy memory module. It contains generic functionalities used to manipulate memory.
///
/// \author Raffaele D. Facendola - 2017

#pragma once

#include <cstring>
#include <algorithm>

#include "syntropy/language/type_traits.h"
#include "syntropy/core/types.h"
#include "syntropy/memory/bytes.h"
#include "syntropy/memory/memory_range.h"

#include "syntropy/diagnostics/assert.h"

namespace syntropy
{
    /************************************************************************/
    /* MEMORY                                                               */
    /************************************************************************/

    /// \brief Exposes utility methods to manipulate memory.
    namespace Memory
    {
        /// \brief Copy a source memory region to a destination memory region. Neither range is exceed during the process.
        /// \return Returns the bytes copied as a result of this call.
        Bytes Copy(const MemoryRange& destination, const ConstMemoryRange& source);

        /// \brief Copy a circular source memory region to a linear destination memory region, unfolding the content. Neither range is exceeded.
        /// \return Returns the bytes copied as a result of this call.
        Bytes CopyUnfold(const MemoryRange& destination, const ConstMemoryRange& source, Bytes source_offset);

        /// \brief Copy a source memory region to a circular destination memory region, folding the content. Neither range is exceeded.
        /// \return Returns the bytes copied as a result of this call.
        Bytes CopyFold(const MemoryRange& destination, const ConstMemoryRange& source, Bytes destination_offset);

        /// \brief Set a value to each byte in a destination range.
        void Set(const MemoryRange& destination, Byte value);

        /// \brief Zero-out a memory region.
        void Zero(const MemoryRange& destination);

        /// \brief Reinterpret an object representation from a type to another type.
        template <typename TTo, typename TFrom>
        TTo BitCast(const TFrom& rhs);

        /// \brief Reinterpret an address as an integer value.
        template <typename TType>
        constexpr Int ToIntAddress(const TType* rhs);
    };

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Memory.

    inline void Memory::Set(const MemoryRange& destination, Byte value)
    {
        std::memset(*destination.Begin(), ToFix8(value), destination.GetSize().GetCount());
    }

    inline void Memory::Zero(const MemoryRange& destination)
    {
        Set(destination, Byte{ 0 });
    }

    template <typename TTo, typename TFrom>
    inline TTo Memory::BitCast(const TFrom& rhs)
    {
        static_assert(sizeof(TTo) == sizeof(TFrom), "TTo and TFrom must have the same size.");
        static_assert(IsTriviallyCopyableV<TFrom>, "TFrom must be trivially copyable.");
        static_assert(IsTrivialV<TTo>, "TTo must be trivial.");
        static_assert(IsCopyConstructibleV<TTo> || IsMoveConstructibleV<TTo>, "TTo must either be copy constructible or move constructible.");
        static_assert(IsTriviallyDefaultConstructibleV<TTo>, "TTo must be trivially default constructible.");

        auto lhs = TTo{};

        std::memcpy(&lhs, &rhs, sizeof(TTo));

        return lhs;
    }

    template <typename TType>
    constexpr Int Memory::ToIntAddress(const TType* rhs)
    {
        static_assert(sizeof(Int) >= sizeof(std::intptr_t));

        return reinterpret_cast<Int>(rhs);
    }

}

