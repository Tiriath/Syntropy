
/// \file assert.h
///
/// \brief This header is part of the Syntropy diagnostic module.
///        It contains utility macros used to check for code invariants
///        and assertions.
///
/// \author Raffaele D. Facendola - 2016

#pragma once

#include "syntropy/language/preprocessor/macro.h"

#include "syntropy/diagnostics/debugger.h"

// ===========================================================================

namespace Syntropy
{
    /************************************************************************/
    /* RUN-TIME DIAGNOSTICS                                                 */
    /************************************************************************/

    /// \brief If the condition is not verified causes the debugger to break
    ///        if attached or the application to terminate.
#define SYNTROPY_ASSERT(condition) \
        SYNTROPY_MACRO_DECLARATION(condition)

    /// \brief Macro used to check on conditions that will cause undefined
    ///        behaviors.
    ///
    /// If the provided condition is not verified causes the debugger to break
    /// if attached or the application to terminate.
#define SYNTROPY_UNDEFINED_BEHAVIOR(condition, message) \
        SYNTROPY_MACRO_DECLARATION(condition)

    /************************************************************************/
    /* COMPILE-TIME DIAGNOSTICS                                             */
    /************************************************************************/

    /// \brief Boolean constant which consume any template argument and
    ///        evaluates to false. Useful for static_assert that should always
    ///        trigger a compilation error.
    template <typename... TTypes>
    inline constexpr Bool
    AlwaysFalse = false;

    /// \brief Strucure which consume any template argument and result in the
    //         program to be ill-formed.
    template <typename TType, typename... TTypes>
    struct IllFormed
    {
        static_assert(AlwaysFalse<TTypes...>, "The program is ill-formed");

        /// \brief Dummy value.
        TType kValue = TType{};
    };

}

// ===========================================================================

namespace Syntropy
{
    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Macros.
    // =======

    #undef SYNTROPY_ASSERT
    #define SYNTROPY_ASSERT(condition) \
        if(!(condition)) { SYNTROPY_TRAP; }

    #undef SYNTROPY_UNDEFINED_BEHAVIOR
    #define SYNTROPY_UNDEFINED_BEHAVIOR(condition, message) \
        if(!(condition)) { SYNTROPY_TRAP; }

}

// ===========================================================================
