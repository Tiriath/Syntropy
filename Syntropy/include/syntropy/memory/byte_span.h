
/// \file byte_span.h
/// \brief This header is part of Syntropy memory module. It contains definitions for byte spans and related utility functions.
///
/// \author Raffaele D. Facendola - July 2020

#pragma once

#include <typeinfo>

#include "syntropy/diagnostics/assert.h"

#include "syntropy/core/types.h"
#include "syntropy/core/span.h"

#include "syntropy/memory/bytes.h"

namespace syntropy
{
    /************************************************************************/
    /* BYTE SPAN                                                            */
    /************************************************************************/

    /// \brief Represents a span of read-only bytes.
    using ByteSpan = Span<Byte>;

    /// \brief Represents a span of read-write bytes.
    using RWByteSpan = RWSpan<Byte>;

    /************************************************************************/
    /* MEMORY                                                               */
    /************************************************************************/

    /// \brief Exposes alignment definitions.
    namespace Memory
    {
        /// \brief Get the memory footprint of a span.
        template <typename TElement>
        constexpr Bytes Size(const SpanT<TElement>& span) noexcept;

        /// \brief Get the read-only object representation of an object.
        /// An object representation is the sequence of bytes starting from the object address.
        /// If the object type is not exactly TObject, the behavior of this method is undefined.
        template <typename TObject>
        ByteSpan BytesOf(const TObject& object);

        /// \brief Get the read-write object representation of an object.
        /// An object representation is the sequence of bytes starting from the object address.
        /// If the object type is not exactly TObject, the behavior of this method is undefined.
        template <typename TObject>
        RWByteSpan RWBytesOf(TObject& object);
    }

    /************************************************************************/
    /* TYPE CAST                                                            */
    /************************************************************************/

    /// \brief Convert a read-only byte span to a read-only typed span.
    /// If the byte span doesn't refer to instances of TElements or it has a non-integer number of elements, the behavior of this method is undefined.
    template <typename TElement>
    Span<TElement> ToSpan(const ByteSpan& byte_span) noexcept;

    /// \brief Convert a read-write byte span to a read-write typed span.
    /// If the byte span doesn't refer to instances of TElements or it has a non-integer number of elements, the behavior of this method is undefined.
    template <typename TElement>
    RWSpan<TElement> ToRWSpan(const RWByteSpan& byte_span) noexcept;

    /// \brief Convert an read-only span to a read-only byte span.
    template <typename TElement>
    ByteSpan ToByteSpan(const SpanT<TElement>& span) noexcept;

    /// \brief Convert a read-write span to a read-write byte span.
    template <typename TElement>
    RWByteSpan ToRWByteSpan(const SpanT<TElement>& span) noexcept;

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Memory.
    // =======

    template <typename TElement>
    constexpr Bytes Memory::Size(const SpanT<TElement>& span) noexcept
    {
        return Count(span) * SizeOf<TElement>();
    }

    template <typename TObject>
    inline ByteSpan Memory::BytesOf(const TObject& object)
    {
        auto& type_from = typeid(object);
        auto& type_to = typeid(TObject);

        SYNTROPY_UNDEFINED_BEHAVIOR(type_from == type_to, "Dynamic type mismatch.");

        return ByteSpan{ ToBytePtr(&object), ToInt(SizeOf<TObject>()) };
    }

    template <typename TObject>
    inline RWByteSpan Memory::RWBytesOf(TObject& object)
    {
        auto& type_from = typeid(object);
        auto& type_to = typeid(TObject);

        SYNTROPY_UNDEFINED_BEHAVIOR(type_from == type_to, "Dynamic type mismatch.");

        return RWByteSpan{ ToRWBytePtr(&object), ToInt(SizeOf<TObject>()) };
    }

    // Type cast.
    // ==========

    template <typename TElement>
    inline Span<TElement> ToSpan(const ByteSpan& byte_span) noexcept
    {
        using TPointer = Pointer<AddConstT<TElement>>;

        auto begin = reinterpret_cast<TPointer>(Begin(byte_span));
        auto end = reinterpret_cast<TPointer>(End(byte_span));

        return { begin, end };
    }

    template <typename TElement>
    inline RWSpan<TElement> ToRWSpan(const RWByteSpan& byte_span) noexcept
    {
        using TRWPointer = Pointer<RemoveConstT<TElement>>;

        auto begin = reinterpret_cast<TRWPointer>(Begin(byte_span));
        auto end = reinterpret_cast<TRWPointer>(End(byte_span));

        return { begin, end };
    }

    template <typename TElement>
    inline ByteSpan ToByteSpan(const SpanT<TElement>& span) noexcept
    {
        auto begin = ToBytePtr(Begin(span));
        auto end = ToBytePtr(End(span));

        return { begin, end };
    }

    template <typename TElement>
    inline RWByteSpan ToRWByteSpan(const SpanT<TElement>& span) noexcept
    {
        auto begin = ToRWBytePtr(Begin(span));
        auto end = ToRWBytePtr(End(span));

        return { begin, end };
    }

}