
/// \file sized_range.h
///
/// \brief This header is part of the Syntropy core module. It contains
///        definitions for ranges whose elements can be visited sequentially
///        and whose size can be computed in constant time.
///
/// Ranges specifications based on the awesome
/// https://www.slideshare.net/rawwell/iteratorsmustgo
///
/// \author Raffaele D. Facendola - Nov 2020
/// \author Raffaele D. Facendola - Jan 2021

#pragma once

#include "syntropy/language/foundation/foundation.h"
#include "syntropy/language/templates/concepts.h"

#include "syntropy/diagnostics/assert.h"
#include "syntropy/core/foundation/tuple.h"

#include "syntropy/core/concepts/forward_range.h"

#include "syntropy/core/concepts/details/sized_range_details.h"

// ===========================================================================

namespace Syntropy::Ranges::Templates
{
    /************************************************************************/
    /* TYPE TRAITS                                                          */
    /************************************************************************/

    /// \brief Range's count type.
    template <typename TRange>
    using RangeCountType = Syntropy::Templates::RemoveConstReference<
        decltype(Details::RouteCount(Syntropy::Templates::Declval<TRange>()))>;
}

// ===========================================================================

namespace Syntropy::Ranges::Concepts
{
    /************************************************************************/
    /* SIZED RANGE                                                          */
    /************************************************************************/

    /// \brief Minimal interface for ranges whose elements can be visited
    ///        sequentially and whose size can be computed in constant time.
    ///
    /// \author Raffaele D. Facendola - November 2020.
    template <typename TRange>
    concept BaseSizedRange = requires(Immutable<TRange> range)
        {
            /// \brief Get range's elements count.
            { Details::RouteCount(range) };
        };

    /// \brief Range whose elements can be visited sequentially and whose size
    ///        can be computed in constant time.
    ///
    /// \author
    /// Raffaele D. Facendola - November 2020.
    template <typename TRange>
    concept SizedRange = BaseSizedRange<TRange> && ForwardRange<TRange>;
}

// ===========================================================================

namespace Syntropy::Ranges
{
    /************************************************************************/
    /* NON-MEMBER FUNCTIONS                                                 */
    /************************************************************************/

    /// \brief Get range's elements count.
    template <Concepts::SizedRange TRange>
    [[nodiscard]] constexpr auto
    Count(Immutable<TRange> range) noexcept;

    /// \brief Check whether lhs and rhs are equal.
    ///
    /// \remarks Equality implies equivalence, therefore if this method returns
    ///          true AreEquivalent also returns true.
    template <Concepts::SizedRange TRange, Concepts::SizedRange URange>
    [[nodiscard]] constexpr Bool
    AreEqual(Immutable<TRange> lhs, Immutable<URange> rhs) noexcept;

    /// \brief Check whether lhs and rhs are equivalent.
    ///
    /// \remarks Equality implies equivalence but not the other way around!
    ///          If AreEqual returns false this method can either return true
    ///          or false.
    template <Concepts::SizedRange TRange, Concepts::SizedRange URange>
    [[nodiscard]] constexpr Bool
    AreEquivalent(Immutable<TRange> lhs, Immutable<URange> rhs) noexcept;

    /// \brief
    /// Compare two ranges lexicographically.
    template <Concepts::SizedRange TRange,
              Concepts::SizedRange URange>
    [[nodiscard]] constexpr Ordering
    Compare(Immutable<TRange> lhs, Immutable<URange> rhs) noexcept;
}

// ===========================================================================

namespace Syntropy::Ranges::Extensions
{
    /************************************************************************/
    /* SIZED RANGE EXTENSIONS                                               */
    /************************************************************************/

    /// \brief Get range's elements count.
    template <typename TType>
    struct Count;

    /// \brief Check whether a range is empty.
    ///
    /// \remarks This extension adapts SizedRange type such that all its
    ///          instances are also ForwardRanges.
    template <Concepts::BaseSizedRange TRange>
    struct IsEmpty<TRange>
    {
        Bool operator()(Immutable<TRange> range) const noexcept;
    };
}

// ===========================================================================

namespace Syntropy::Ranges
{
    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Non-member functions.
    // =====================

    template <Concepts::SizedRange TRange>
    [[nodiscard]] constexpr auto
    Count(Immutable<TRange> range) noexcept
    {
        return Details::RouteCount(range);
    }

    template <Concepts::SizedRange TRange, Concepts::SizedRange URange>
    [[nodiscard]] constexpr Bool
    AreEqual(Immutable<TRange> lhs, Immutable<URange> rhs) noexcept
    {
        return (PtrOf(lhs) == PtrOf(rhs));
    }

    template <Concepts::SizedRange TRange, Concepts::SizedRange URange>
    [[nodiscard]] constexpr Bool
    AreEquivalent(Immutable<TRange> lhs, Immutable<URange> rhs) noexcept
    {
        if (AreEqual(lhs, rhs))
        {
            return true;
        }

        if (Details::RouteCount(lhs) == Details::RouteCount(rhs))
        {
            auto lhs_copy = lhs;
            auto rhs_copy = rhs;

            for (;   (!Details::RouteIsEmpty(lhs_copy))
                  && (!Details::RouteIsEmpty(rhs_copy))
                  && (Details::RouteFront(lhs_copy)
                    == Details::RouteFront(rhs_copy)); )
            {
                lhs_copy = Details::RoutePopFront(lhs_copy);
                rhs_copy = Details::RoutePopFront(rhs_copy);
            }

            return Details::RouteIsEmpty(lhs_copy);
        }

        return false;
    }

    template <Concepts::SizedRange TRange, Concepts::SizedRange URange>
    [[nodiscard]] constexpr Ordering
    Compare(Immutable<TRange> lhs, Immutable<URange> rhs) noexcept
    {
        auto lhs_copy = lhs;
        auto rhs_copy = rhs;

        for (;    !Details::RouteIsEmpty(lhs_copy)
               && !Details::RouteIsEmpty(rhs_copy); )
        {
            auto compare
                 = (Details::RouteFront(lhs_copy)
                <=> Details::RouteFront(rhs_copy));

            if (compare == Ordering::kLess)
            {
                return Ordering::kLess;
            }

            if (compare == Ordering::kGreater)
            {
                return Ordering::kGreater;
            }

            lhs_copy = Details::RoutePopFront(lhs_copy);
            rhs_copy = Details::RoutePopFront(rhs_copy);
        }

        if (Details::RouteIsEmpty(lhs_copy) && Details::RouteIsEmpty(rhs_copy))
        {
            return Ordering::kEquivalent;
        }

        return Details::RouteIsEmpty(lhs_copy) ?
            Ordering::kLess :
            Ordering::kGreater;
    }
}

// ===========================================================================

namespace Syntropy::Ranges::Extensions
{
    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Sized range extensions.
    // =======================

    template <Concepts::BaseSizedRange TRange>
    Bool IsEmpty<TRange>::operator()(Immutable<TRange> range) const noexcept
    {
        return
            Details::RouteCount(range) == Templates::RangeCountType<TRange>{};
    }
}

// ===========================================================================