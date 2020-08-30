
/// \file queries.h
/// \brief This header is part of Syntropy language module. It contains implementation details of queries-related type traits.
///
/// \author Raffaele D. Facendola - 2020.

#pragma once

#include "syntropy/language/foundation/types.h"
#include "syntropy/language/support/type_list.h"

namespace Syntropy::Traits::Details
{
    /************************************************************************/
    /* ELEMENT TYPE                                                         */
    /************************************************************************/

    /// \brief Provides a type alias Type which is equal to the kIndex-th type in a type list.
    /// If the provided index exceeds the type-list range, the program is ill-formed.
    template <Int kIndex, typename TType, typename... TTypes>
    struct ElementType : ElementType<kIndex - 1, TTypes...>
    {
        static_assert(kIndex >= 0, "Invalid index");
    };

    /// \brief Provides a type alias Type which is equal to the kIndex-th type in a type list.
    /// Partial specialization which returns the first type in a type list.
    template <typename TType, typename... TTypes>
    struct ElementType<0, TType, TTypes...>
    {
        using Type = TType;
    };

    /// \brief Provides a type alias Type which is equal to the kIndex-th type in a type list.
    /// Partial specialization for type lists.
    template <Int kIndex, typename... TTypes>
    struct ElementType<kIndex, TypeList<TTypes...>> : ElementType<kIndex, TTypes...>
    {

    };

    /************************************************************************/
    /* ELEMENT INDEX                                                        */
    /************************************************************************/

    /// \brief Provide a member constant Value which is equal to the index of the element with type TMatch among a list of types [TType, TTypes...].
    template <typename TMatch, typename TType, typename... TTypes>
    struct ElementIndex
    {
        static constexpr Int Value = 1 + ElementIndex<TMatch, TTypes...>::Value;
    };

    /// \brief Provide a member constant Value which is equal to the index of the element with type TMatch among a list of types [TMatch, TTypes...].
    /// Partial specialization for a list of types starting with TMatch as type.
    template <typename TMatch, typename... TTypes>
    struct ElementIndex<TMatch, TMatch, TTypes...>
    {
        static constexpr Int Value = 0;
    };

    /// \brief Provide a member constant Value which is equal to the index of the element with type TMatch among a list of types [TTypes...].
    /// Partial specialization for type lists.
    template <typename TMatch, typename... TTypes>
    struct ElementIndex<TMatch, TypeList<TTypes...>> : ElementIndex<TMatch, TTypes...>
    {

    };

    /************************************************************************/
    /* FUNCTION ARGUMENTS                                                   */
    /************************************************************************/

    /// \brief Provides the a type alias Type, which is the argument types a callable object can be called with.
    /// Partial specialization for lambdas and callable objects.
    template <typename TCallable>
    struct FunctionArguments : FunctionArguments<decltype(&Decay<TCallable>::operator())>
    {
        
    };

    /// \brief Provides the a type alias Type, which is the argument types a callable object can be called with.
    /// Partial specialization for non-const member functions.
    template <typename TCallable, typename TReturn, typename... TArguments>
    struct FunctionArguments<TReturn(TCallable::*)(TArguments...)>
    {
        using Type = TypeList<TArguments...>;
    };

    /// \brief Provides the a type alias Type, which is the argument types a callable object can be called with.
    /// Partial specialization for const member functions.
    template <typename TCallable, typename TReturn, typename... TArguments>
    struct FunctionArguments<TReturn(TCallable::*)(TArguments...) const>
    {
        using Type = TypeList<TArguments...>;
    };

}