
/// \file templates.h
/// \brief This header is part of Syntropy language module. It contains basic definitions for template machinery.
///
/// \author Raffaele D. Facendola - 2020.

#pragma once

#include "syntropy/language/foundation/types.h"

#include "syntropy/language/templates/details/templates_details.h"

// ===========================================================================

namespace Syntropy::Templates
{
    /************************************************************************/
    /* CONSTANT                                                             */
    /************************************************************************/

    /// \brief Wraps a static constant of type TType.
    template <typename TType, TType VValue>
    struct Constant
    {
        static constexpr TType kValue = VValue;
    };

    /************************************************************************/
    /* ALIAS                                                                */
    /************************************************************************/

    /// \brief Wraps an alias type Type.
    template <typename TType>
    struct Alias
    {
        using Type = typename TType;
    };

    /************************************************************************/
    /* TRUE                                                                 */
    /************************************************************************/

    /// \brief Boolean constant equal to true.
    using True = Constant<Bool, true>;

    /************************************************************************/
    /* FALSE                                                                */
    /************************************************************************/

    /// \brief Boolean constant equal to false.
    using False = Constant<Bool, false>;

    /************************************************************************/
    /* ALWAYS TRUE                                                          */
    /************************************************************************/

    /// \brief Boolean constant which consume any template argument and evaluates to true.
    template <typename...>
    inline constexpr Bool AlwaysTrue = false;

    /************************************************************************/
    /* ALWAYS FALSE                                                         */
    /************************************************************************/

    /// \brief Boolean constant which consume any template argument and evaluates to false.
    template <typename...>
    inline constexpr Bool AlwaysFalse = false;

    /************************************************************************/
    /* ILL-FORMED                                                           */
    /************************************************************************/

    /// \brief An alias type that results in an ill-formed program.
    template <typename... TTypes>
    struct IllFormed
    {
        static_assert(AlwaysFalse<TTypes...>, "The program is ill-formed.");
    };

    /************************************************************************/
    /* VOID                                                                 */
    /************************************************************************/

    /// \brief Metafunction that maps a sequence of types to void.
    template <typename...>
    using Void = void;

    /************************************************************************/
    /* IDENTITY                                                             */
    /************************************************************************/

    /// \brief Type equal to TType.
    /// \remarks The identity transform is meant to establish non-deduced contexts in template argument deduction.
    template <typename TType>
    using Identity = typename Details::Identity<TType>::Type;

}

// ===========================================================================