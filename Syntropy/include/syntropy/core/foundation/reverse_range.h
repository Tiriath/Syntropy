
/// \file reverse_range.h
/// \brief This header is part of the Syntropy core module. It contains definitions for adapters used to iterate a range in reverse order.
///
/// Ranges specifications based on the awesome https://www.slideshare.net/rawwell/iteratorsmustgo
/// 
/// \author Raffaele D. Facendola - Nov 2020

#pragma once

#include "syntropy/language/foundation/foundation.h"

#include "syntropy/core/foundation/range.h"

// ===========================================================================

namespace Syntropy
{
    /************************************************************************/
    /* REVERSE RANGE                                                        */
    /************************************************************************/

    /// \brief Adapter class used to reverse bidirectional ranges.
    /// \author Raffaele D. Facendola - November 2020.
    template <Concepts::BidirectionalRange TRange>
    class ReverseRange
    {
        // Forward range.

        template <Concepts::BidirectionalRange TRange, Concepts::BidirectionalRange URange>
        friend constexpr Bool operator==(Immutable<ReverseRange<TRange>> lhs, Immutable<ReverseRange<URange>> rhs) noexcept;

        template <Concepts::BidirectionalRange TRange>
        friend constexpr Templates::RangeElementReferenceType<TRange> Front(Immutable<ReverseRange<TRange>> range) noexcept;

        template <Concepts::BidirectionalRange TRange>
        friend constexpr ReverseRange<TRange> PopFront(Immutable<ReverseRange<TRange>> range) noexcept;

        template <Concepts::BidirectionalRange TRange>
        friend constexpr Bool IsEmpty(Immutable<ReverseRange<TRange>> range) noexcept;

        // Sized range.

        template <Concepts::SizedRange TRange>
        friend constexpr Templates::RangeElementCountType<ReverseRange<TRange>> Count(Immutable<ReverseRange<TRange>> rhs) noexcept;

        // Bidirectional range.

        template <Concepts::BidirectionalRange TRange>
        friend constexpr Templates::RangeElementReferenceType<TRange> Back(Immutable<ReverseRange<TRange>> range) noexcept;

        template <Concepts::BidirectionalRange TRange>
        friend constexpr ReverseRange<TRange> PopBack(Immutable<ReverseRange<TRange>> range) noexcept;

        // Random access range.

        template <Concepts::RandomAccessRange TRange>
        friend constexpr ReverseRange<TRange> Select(Immutable<ReverseRange<TRange>> rhs, Templates::RangeElementCountType<ReverseRange<TRange>> offset, Templates::RangeElementCountType<ReverseRange<TRange>> count) noexcept;

        template <Concepts::RandomAccessRange TRange>
        friend constexpr Templates::RangeElementReferenceType<TRange> Select(Immutable<ReverseRange<TRange>> rhs, Templates::RangeElementCountType<ReverseRange<TRange>> index) noexcept;

        template <Concepts::BidirectionalRange TRange>
        friend constexpr TRange Reverse(Immutable<ReverseRange<TRange>> range) noexcept;

    public:

        /// \brief Create a new reverse range.
        constexpr ReverseRange(Immutable<TRange> range) noexcept;

    private:

        /// \brief Underlying range.
        TRange range_;
    };

    /************************************************************************/
    /* NON-MEMBER FUNCTIONS                                                 */
    /************************************************************************/

    // Comparison.
    // ===========

    /// \brief Check whether lhs and rhs are equivalent.
    template <Concepts::BidirectionalRange TRange, Concepts::Range URange>
    constexpr Bool operator==(Immutable<ReverseRange<TRange>> lhs, Immutable<URange> rhs) noexcept;

    /// \brief Check whether lhs and rhs are equivalent.
    template <Concepts::Range TRange, Concepts::BidirectionalRange URange>
    constexpr Bool operator==(Immutable<TRange> lhs, Immutable<ReverseRange<URange>> rhs) noexcept;

    /// \brief Check whether lhs and rhs are equivalent.
    template <Concepts::BidirectionalRange TRange, Concepts::BidirectionalRange URange>
    constexpr Bool operator==(Immutable<ReverseRange<TRange>> lhs, Immutable<ReverseRange<URange>> rhs) noexcept;

    // Forward range.
    // ==============

    /// \brief Access the first element in a range.
    /// \remarks Accessing the first element of an empty range results in undefined behavior.
    template <Concepts::BidirectionalRange TRange>
    constexpr Templates::RangeElementReferenceType<TRange> Front(Immutable<ReverseRange<TRange>> range) noexcept;

    /// \brief Discard the first count elements in a range and return the resulting subrange.
    /// \remarks If the provided range is empty, the behavior of this method is undefined.
    template <Concepts::BidirectionalRange TRange>
    constexpr ReverseRange<TRange> PopFront(Immutable<ReverseRange<TRange>> range) noexcept;

    /// \brief Check whether a reverse range is empty.
    /// \return Returns true if the range is empty, returns false otherwise.
    template <Concepts::BidirectionalRange TRange>
    constexpr Bool IsEmpty(Immutable<ReverseRange<TRange>> range) noexcept;

    // Sized range.
    // ============

    /// \brief Get the number of elements in a range.
    template <Concepts::SizedRange TRange>
    constexpr Templates::RangeElementCountType<ReverseRange<TRange>> Count(Immutable<ReverseRange<TRange>> rhs) noexcept;

    // Bidirectional range.
    // ====================

    /// \brief Access the last element in a range.
    /// \remarks Accessing the last element of an empty range results in undefined behavior.
    template <Concepts::BidirectionalRange TRange>
    constexpr Templates::RangeElementReferenceType<TRange> Back(Immutable<ReverseRange<TRange>> range) noexcept;

    /// \brief Discard the last count elements in a range and return the resulting subrange.
    /// \remarks If this method would cause the subrange to exceed the original range, the behavior of this method is undefined.
    template <Concepts::BidirectionalRange TRange>
    constexpr ReverseRange<TRange> PopBack(Immutable<ReverseRange<TRange>> range) noexcept;

    // Random access range.
    // ====================

    /// \brief Obtain a sub-range given an offset and a number of elements.
    /// \remarks Exceeding range boundaries results in undefined behavior.
    template <Concepts::RandomAccessRange TRange>
    constexpr ReverseRange<TRange> Select(Immutable<ReverseRange<TRange>> rhs, Templates::RangeElementCountType<ReverseRange<TRange>> offset, Templates::RangeElementCountType<ReverseRange<TRange>> count) noexcept;

    /// \brief Obtain a range element at given index.
    /// \remarks Exceeding range boundaries results in undefined behavior.
    template <Concepts::RandomAccessRange TRange>
    constexpr Templates::RangeElementReferenceType<TRange> Select(Immutable<ReverseRange<TRange>> rhs, Templates::RangeElementCountType<ReverseRange<TRange>> index) noexcept;

    // Contiguous range.
    // =================

    // #REMINDER A reverse pointer is feasible (incrementing it will actually decrement it and vice-versa), however that pointer won't be safe
    //           to be used with memcpy-like functions, which is the main purpose of a contiguous range.
    //
    // /// \brief Access underlying range data.
    // /// \remarks Accessing data of an empty range is allowed but the returned value is unspecified.
    // template <Concepts::RandomAccessRange TRange>
    // constexpr ReversePointer<TType> Data(Immutable<ReverseRange<TRange>> rhs) noexcept;

    // Utilities.
    // ==========

    /// \brief Reverse a bidirectional range.
    template <Concepts::BidirectionalRange TRange>
    constexpr ReverseRange<TRange> Reverse(Immutable<TRange> range) noexcept;

    /// \brief Reverse a reversed range.
    template <Concepts::BidirectionalRange TRange>
    constexpr TRange Reverse(Immutable<ReverseRange<TRange>> range) noexcept;
}

// ===========================================================================

namespace Syntropy::Templates
{
    /************************************************************************/
    /* RANGE TRAITS                                                         */
    /************************************************************************/

    /// \brief Specialization for reverse ranges.
    template <Concepts::BidirectionalRange TRange>
    struct EnableRangeTraits<ReverseRange<TRange>> : Alias<void> {};

    /// \brief Specialization for reverse ranges.
    template <Concepts::BidirectionalRange TRange>
    struct RangeTraitsElementReferenceType<ReverseRange<TRange>> : Alias<RangeElementReferenceType<TRange>> {};

    /// \brief Specialization for reverse ranges.
    template <Concepts::BidirectionalRange TRange>
    struct RangeTraitsElementPointerType<ReverseRange<TRange>> : Alias<RangeElementPointerType<TRange>> {};

    /// \brief Specialization for reverse ranges.
    template <Concepts::BidirectionalRange TRange>
    struct RangeTraitsElementCountType<ReverseRange<TRange>> : Alias<RangeElementCountType<TRange>> {};
}

// ===========================================================================

namespace Syntropy
{
    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // ReverseRange.
    // ============

    template <Concepts::BidirectionalRange TRange>
    constexpr ReverseRange<TRange>::ReverseRange(Immutable<TRange> range) noexcept
        : range_(range)
    {

    }

    // Non-member functions.
    // =====================

    // Comparison.

    template <Concepts::BidirectionalRange TRange, Concepts::Range URange>
    constexpr Bool operator==(Immutable<ReverseRange<TRange>> lhs, Immutable<URange> rhs) noexcept
    {
        return AreEquivalent(lhs, rhs);
    }

    template <Concepts::Range TRange, Concepts::BidirectionalRange URange>
    constexpr Bool operator==(Immutable<TRange> lhs, Immutable<ReverseRange<URange>> rhs) noexcept
    {
        return AreEquivalent(lhs, rhs);
    }

    template <Concepts::BidirectionalRange TRange, Concepts::BidirectionalRange URange>
    constexpr Bool operator==(Immutable<ReverseRange<TRange>> lhs, Immutable<ReverseRange<URange>> rhs) noexcept
    {
        // Trick! Comparing non-reversed ranges allows for more efficient comparison (if provided).

        return lhs.range_ == rhs.range_;
    }

    // Forward range.

    template <Concepts::BidirectionalRange TRange>
    constexpr Templates::RangeElementReferenceType<TRange> Front(Immutable<ReverseRange<TRange>> range) noexcept
    {
        return Back(range.range_);
    }

    template <Concepts::BidirectionalRange TRange>
    constexpr ReverseRange<TRange> PopFront(Immutable<ReverseRange<TRange>> range) noexcept
    {
        return PopBack(range.range_);
    }

    template <Concepts::BidirectionalRange TRange>
    constexpr Bool IsEmpty(Immutable<ReverseRange<TRange>> range) noexcept
    {
        return IsEmpty(range.range_);
    }

    // Sized range.

    /// \brief Get the number of elements in a span.
    template <Concepts::SizedRange TRange>
    constexpr Templates::RangeElementCountType<ReverseRange<TRange>> Count(Immutable<ReverseRange<TRange>> rhs) noexcept
    {
        return Count(rhs.range_);
    }

    // Bidirectional range.

    template <Concepts::BidirectionalRange TRange>
    constexpr Templates::RangeElementReferenceType<TRange> Back(Immutable<ReverseRange<TRange>> range) noexcept
    {
        return Front(range.range_);
    }

    template <Concepts::BidirectionalRange TRange>
    constexpr ReverseRange<TRange> PopBack(Immutable<ReverseRange<TRange>> range) noexcept
    {
        return PopFront(range.range_);
    }

    // Random access range.

    template <Concepts::RandomAccessRange TRange>
    constexpr ReverseRange<TRange> Select(Immutable<ReverseRange<TRange>> rhs, Templates::RangeElementCountType<ReverseRange<TRange>> offset, Templates::RangeElementCountType<ReverseRange<TRange>> count) noexcept
    {
        return Reverse(Select(rhs.range_, Count(rhs) - offset - 1), count);
    }

    template <Concepts::RandomAccessRange TRange>
    constexpr Templates::RangeElementReferenceType<TRange> Select(Immutable<ReverseRange<TRange>> rhs, Templates::RangeElementCountType<ReverseRange<TRange>> index) noexcept
    {
        return Select(rhs.range_, Count(rhs) - index - 1);
    }

    // Utilities.

    template <Concepts::BidirectionalRange TRange>
    constexpr ReverseRange<TRange> Reverse(Immutable<TRange> range) noexcept
    {
        return range;
    }

    template <Concepts::BidirectionalRange TRange>
    constexpr TRange Reverse(Immutable<ReverseRange<TRange>> range) noexcept
    {
        return range.range_;
    }
}

// ===========================================================================