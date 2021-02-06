
/// \file forward_range.h
///
/// \brief This header is part of the Syntropy core module.
///        It contains definitions for ranges whose elements can be visited
///        sequentially.
///
/// Ranges specifications based on the awesome
///     https://www.slideshare.net/rawwell/iteratorsmustgo
///
/// \author Raffaele D. Facendola - Nov 2020
/// \author Raffaele D. Facendola - Jan 2021

#pragma once

#include "syntropy/language/foundation/foundation.h"
#include "syntropy/language/templates/concepts.h"

#include "syntropy/diagnostics/assert.h"
#include "syntropy/core/foundation/tuple.h"

#include "syntropy/core/concepts/details/forward_range_details.h"

// ===========================================================================

namespace Syntropy::Ranges::Templates
{
    /************************************************************************/
    /* TYPE TRAITS                                                          */
    /************************************************************************/

    /// \brief Range's element reference type.
    template <typename TRange>
    using RangeElementReferenceType= decltype(
        Details::RouteFront(Syntropy::Templates::Declval<TRange>()));

    /// \brief Range's element value type.
    template <typename TRange>
    using RangeElementValueType = Syntropy::Templates::RemoveReference<
        RangeElementReferenceType<TRange>>;

    /// \brief Range's element pointer type.
    template <typename TRange>
    using RangeElementPointerType = Syntropy::Templates::AddPointer<
        RangeElementReferenceType<TRange>>;
}

// ===========================================================================

namespace Syntropy::Ranges::Concepts
{
    /************************************************************************/
    /* FORWARD RANGE                                                        */
    /************************************************************************/

    /// \brief Minimal interface for ranges whose elements can be
    ///        visited sequentially.
    /// \author Raffaele D. Facendola - November 2020.
    template <typename TRange>
    concept BaseForwardRange = requires(Immutable<TRange> range)
    {
        /// \brief Access range's first element.
        { Details::RouteFront(range) };

        /// \brief Discard range's first element and return the resulting
        ///        range.
        { Details::RoutePopFront(range) };

        /// \brief Check whether the range is empty.
        { Details::RouteIsEmpty(range) };
    };

    /// \brief Range whose elements can be visited sequentially.
    /// \author Raffaele D. Facendola - November 2020.
    template <typename TRange>
    concept ForwardRange = BaseForwardRange<TRange>;
}

// ===========================================================================

namespace Syntropy::Ranges
{
    /************************************************************************/
    /* NON-MEMBER FUNCTIONS                                                 */
    /************************************************************************/

    // Forward range.
    // ==============

    /// \brief Access range's first element.
    /// \remarks Accessing the first element of an empty range results
    ///          in undefined behavior.
    template <Concepts::ForwardRange TRange>
    [[nodiscard]] constexpr decltype(auto)
    Front(Immutable<TRange> range) noexcept;

    /// \brief Discard range's first element and return the resulting range.
    /// \remarks If the provided range is empty, the behavior of this method
    ///          is undefined.
    template <Concepts::ForwardRange TRange>
    [[nodiscard]] constexpr TRange
    PopFront(Immutable<TRange> range) noexcept;

    /// \brief Check whether the range is empty.
    template <Concepts::ForwardRange TRange>
    [[nodiscard]] constexpr Bool
    IsEmpty(Immutable<TRange> range) noexcept;

    /// \brief Apply a function to each element in the range.
    template <Concepts::ForwardRange TRange, typename TFunction>
    constexpr void
    ForEach(Immutable<TRange> range, TFunction function) noexcept;

    /// \brief Advance both ranges in lockstep, copy elements from lhs to
    ///        rhs until either lhs or rhs is exhausted.
    /// \return Returns the ranges to the elements that were not copied: at
    ///         least one of the two is guaranteed to be empty.
    template <Concepts::ForwardRange TRange, Concepts::ForwardRange URange>
    constexpr Tuples::Tuple<TRange, URange>
    Copy(Immutable<TRange> lhs, Immutable<URange> rhs) noexcept;

    /// \brief Advance both ranges in lockstep, moving elements from lhs to
    ///        rhs until either lhs or rhs is exhausted.
    /// \return Returns the ranges to the elements that were not copied: at
    ///         least one of the two is guaranteed to be empty.
    template <Concepts::ForwardRange TRange, Concepts::ForwardRange URange>
    constexpr Tuples::Tuple<TRange, URange>
    Move(Immutable<TRange> lhs, Immutable<URange> rhs) noexcept;

    /// \brief Advance both ranges in lockstep, swapping elements from both
    ///        until either lhs or rhs is exhausted.
    /// \return Returns the ranges to the elements that were not swapped: at
    ///         least one of the two is guaranteed to be empty.
    template <Concepts::ForwardRange TRange, Concepts::ForwardRange URange>
    constexpr Tuples::Tuple<TRange, URange>
    Swap(Immutable<TRange> lhs, Immutable<URange> rhs) noexcept;

}

// ===========================================================================

namespace Syntropy
{
    /// \brief Get an iterator to the first element in a range.
    template <Ranges::Concepts::ForwardRange TRange>
    constexpr auto begin(Immutable<TRange> range) noexcept;

    /// \brief Get an iterator past the last element in a range.
    template <Ranges::Concepts::ForwardRange TRange>
    constexpr auto end(Immutable<TRange> range) noexcept;
}

// ===========================================================================

namespace Syntropy::Ranges::Extensions
{
    /************************************************************************/
    /* FORWARD RANGE EXTENSIONS                                             */
    /************************************************************************/

    // Functors to extend ranges support to custom types.

    /// \brief Access range's first element.
    template <typename TType>
    struct Front;

    /// \brief Discard range's first element and return the resulting range.
    template <typename TType>
    struct PopFront;

    /// \brief Check whether a range is empty.
    template <typename TType>
    struct IsEmpty;
}

// ===========================================================================

namespace Syntropy::Ranges
{
    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Non-member functions.
    // =====================

    // Forward range.

    template <Concepts::ForwardRange TRange>
    [[nodiscard]] constexpr decltype(auto)
    Front(Immutable<TRange> range) noexcept
    {
        return Details::RouteFront(range);
    }

    template <Concepts::ForwardRange TRange>
    [[nodiscard]] constexpr TRange
    PopFront(Immutable<TRange> range) noexcept
    {
        return Details::RoutePopFront(range);
    }

    template <Concepts::ForwardRange TRange>
    [[nodiscard]] constexpr Bool
    IsEmpty(Immutable<TRange> range) noexcept
    {
        return Details::RouteIsEmpty(range);
    }

    template <Concepts::ForwardRange TRange, typename TFunction>
    constexpr void
    ForEach(Immutable<TRange> range, TFunction function) noexcept
    {
        for (auto rest = range;
             !Details::RouteIsEmpty(rest);
             rest = Details::RoutePopFront(rest))
        {
            function(Details::RouteFront(rest));
        }
    }

    template <Concepts::ForwardRange TRange, Concepts::ForwardRange URange>
    constexpr Tuples::Tuple<TRange, URange>
    Copy(Immutable<TRange> lhs, Immutable<URange> rhs) noexcept
    {
        auto source = lhs;
        auto destination = rhs;

        for (; !Details::RouteIsEmpty(source)
               && !Details::RouteIsEmpty(destination);)
        {
            Details::RouteFront(destination) = Details::RouteFront(source);

            source = Details::RoutePopFront(source);
            destination = Details::RoutePopFront(destination);
        }

        return { source , destination };
    }

    template <Concepts::ForwardRange TRange, Concepts::ForwardRange URange>
    constexpr Tuples::Tuple<TRange, URange>
    Move(Immutable<TRange> lhs, Immutable<URange> rhs) noexcept
    {
        auto source = lhs;
        auto destination = rhs;

        for (; !Details::RouteIsEmpty(source)
               && !Details::RouteIsEmpty(destination);)
        {
            Details::RouteFront(destination)
                = Syntropy::Move(Details::RouteFront(source));

            source = Details::RoutePopFront(source);
            destination = Details::RoutePopFront(destination);
        }

        return { source , destination };
    }

    template <Concepts::ForwardRange TRange, Concepts::ForwardRange URange>
    constexpr Tuples::Tuple<TRange, URange>
    Swap(Immutable<TRange> lhs, Immutable<URange> rhs) noexcept
    {
        auto source = lhs;
        auto destination = rhs;

        for (; !Details::RouteIsEmpty(source)
               && !Details::RouteIsEmpty(destination);)
        {
            Swap(Details::RouteFront(source),
                 Details::RouteFront(destination));

            source = Details::RoutePopFront(source);
            destination = Details::RoutePopFront(destination);
        }

        return { source , destination };
    }

}

// ===========================================================================

namespace Syntropy
{
    /// \brief Get an iterator to the first element in a range.
    template <Ranges::Concepts::ForwardRange TRange>
    constexpr auto begin(Immutable<TRange> range) noexcept
    {
        return Ranges::Details::RangeIterator<TRange>{ range };
    }

    /// \brief Get an iterator past the last element in a range.
    template <Ranges::Concepts::ForwardRange TRange>
    constexpr auto end(Immutable<TRange> range) noexcept
    {
        return Ranges::Details::RangeIterator<TRange>{};
    }
}

// ===========================================================================