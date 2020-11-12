#/// \file range_unit_test.h
///
/// \author Raffaele D. Facendola - November 2020.

#pragma once

#include "syntropy/language/foundation/foundation.h"

#include "syntropy/core/containers/fix_array.h"
#include "syntropy/core/foundation/span.h"

#include "syntropy/experimental/core/foundation/range.h"
#include "syntropy/experimental/core/algorithm/search.h"

#include "syntropy/unit_test/unit_test.h"

// ===========================================================================

namespace Syntropy::Algorithm::UnitTest
{
    /************************************************************************/
    /* SEARCH TEST FIXTURE                                                  */
    /************************************************************************/

    /// \brief Search test fixture.
    struct SearchTestFixture
    {
        /// \brief Integer sequence.
        FixArray<Int, 10> ints_ = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        /// \brief Integer sequence span.
        SpanT<Int> ints_span_;

        /// \brief Empty integer sequence span.
        SpanT<Int> empty_span_;

        /// \brief Executed before each test case.
        void Before();
    };

    /************************************************************************/
    /* UNIT TEST                                                            */
    /************************************************************************/

    inline const auto& search_unit_test = MakeAutoUnitTest<SearchTestFixture>("search.algorithm.core.syntropy")

    .TestCase("???", [](auto& fixture)
    {

    });

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // SearchTestFixture.

    inline void SearchTestFixture::Before()
    {
        ints_span_ = { &ints_[0], 10 };
        empty_span_ = {};
    }

}

// ===========================================================================