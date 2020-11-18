
/// \file zip_range.h
/// \brief This header is part of the Syntropy core module. It contains definitions for adapters used to zip multiple ranges together.
///
/// Ranges specifications based on the awesome https://www.slideshare.net/rawwell/iteratorsmustgo
/// 
/// \author Raffaele D. Facendola - November 2020

#pragma once

#include "syntropy/language/foundation/foundation.h"

#include "syntropy/experimental/core/foundation/range.h"

#include "syntropy/core/foundation/tuple.h"

// ===========================================================================

namespace Syntropy
{
    /************************************************************************/
    /* ZIP RANGE                                                            */
    /************************************************************************/

    /// \brief Adapter class used to reverse bidirectional ranges.
    /// \author Raffaele D. Facendola - November 2020.
    template <Concepts::RangeT... TRanges>
    class ZipRange
    {
    public:

        // Create a new range by zipping together one or more ranges.
        constexpr ZipRange(const TRanges&... ranges) noexcept;

    private:

        /// \brief Underlying ranges.
        TupleT<TRanges...> ranges_;

    };

    /************************************************************************/
    /* NON-MEMBER FUNCTIONS                                                 */
    /************************************************************************/

    // Range.
    // ======

    /// \brief Create a new range by joining together multiple ranges.
    /// The range is considered to be non-empty until at least one of the ranges becomes empty.
    template <Concepts::RangeT... TRanges>
    ZipRange<TRanges...> Zip(const TRanges&... ranges) noexcept;

    // Forward range.
    // ==============

    /// \brief Access the first element in a zip-range.
    /// \remarks Accessing the first element of an empty zip-range results in undefined behavior.
    template <Concepts::ForwardRangeT... TRanges>
    constexpr TupleT<TRanges...>& Front(const ZipRange<TRanges...>& zip_range) noexcept;

    /// \brief Discard the first count elements in a zip-range and return the resulting sub-zip-range.
    /// \remarks If this method would cause the sub-zip-range to exceed the original zip-range, the behavior of this method is undefined.
    template <Concepts::ForwardRangeT... TRanges>
    constexpr ZipRange<TRanges...> PopFront(const ZipRange<TRanges...>& zip_range) noexcept;

    /// \brief Check whether a zip-range is empty.
    /// \return Returns true if the zip-range is empty, returns false otherwise.
    template <Concepts::ForwardRangeT... TRanges>
    constexpr Bool IsEmpty(const ZipRange<TRanges...>& zip_range) noexcept;

    // Bidirectional range.
    // ====================

    /// \brief Access the last element in a zip-range.
    /// \remarks Accessing the last element of an empty zip-range results in undefined behavior.
    template <Concepts::BidirectionalRangeT... TRanges>
    constexpr TupleT<TRanges...>& Back(const ZipRange<TRanges...>& zip_range) noexcept;

    /// \brief Discard the last count elements in a zip-range and return the resulting sub-zip-range.
    /// \remarks If this method would cause the sub-zip-range to exceed the original zip-range, the behavior of this method is undefined.
    template <Concepts::BidirectionalRangeT... TRanges>
    constexpr ZipRange<TRanges...> PopBack(const ZipRange<TRanges...>& zip_range) noexcept;

    // Sized range.
    // ============

    /// \brief Get the number of elements in a zip-range.
    template <Concepts::SizedRangeT... TRanges>
    constexpr Int Count(const ZipRange<TRanges...>& zip_range) noexcept;

    // Random access range.
    // ====================

    /// \brief Obtain a sub-zip-range given an offset and a number of elements.
    /// \remarks Exceeding zip-range boundaries results in undefined behavior.
    template <Concepts::RandomAccessRangeT... TRanges>
    constexpr ZipRange<TRanges...> Select(const ZipRange<TRanges...>& zip_range, Int offset, Int count) noexcept;

    /// \brief Obtain a zip-range element at given index.
    /// \remarks Exceeding zip-range boundaries results in undefined behavior.
    template <Concepts::RandomAccessRangeT... TRanges>
    constexpr TupleT<TRanges...>& Select(const ZipRange<TRanges...>& zip_range, Int index) noexcept;

    // Contiguous range.
    // =================

    /// \brief Access underlying zip-range data.
    /// \remarks Accessing data of an empty zip-range is allowed but the returned value is unspecified.
    template <Concepts::ContiguousRangeT... TRanges>
    constexpr TupleT<TRanges*...> Data(const ZipRange<TRanges...>& zip_range) noexcept;

}

// ===========================================================================

namespace Syntropy::Templates
{
    /************************************************************************/
    /* RANGE TRAITS                                                         */
    /************************************************************************/

    /// \brief Specialization for zip-ranges.
    template <Concepts::RangeT... TRanges>
    struct RangeTraits<ZipRange<TRanges...>>
    {
        /// \brief Type of a reference to a range element.
        using ElementReferenceType = TupleT<RangeElementReferenceType<TRanges>...>;
    };
}

// ===========================================================================

namespace Syntropy
{
    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // ZipRange.
    // =========

    template <Concepts::RangeT... TRanges>
    inline constexpr ZipRange<TRanges...>::ZipRange(const TRanges&... ranges) noexcept
        : ranges_(ranges...)
    {

    }

    // Non-member functions.
    // =====================

    template <Concepts::RangeT... TRanges>
    ZipRange<TRanges...> Zip(const TRanges&... ranges) noexcept
    {
        return ZipRange{ ranges... };
    }

}

// ===========================================================================