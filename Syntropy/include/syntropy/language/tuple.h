
/// \file tuple.h
/// \brief This header is part of Syntropy language module. It contains extensions to standard header <tuple>
///
/// \author Raffaele D. Facendola - 2020.

#pragma once

#include <cstdint>
#include <utility>
#include <type_traits>
#include <tuple>

#include "syntropy/core/types.h"

namespace syntropy
{
    /************************************************************************/
    /* TUPLE <T...>                                                         */
    /************************************************************************/

    /// \brief Type alias for a tuple.
    template <typename... TElements>
    using Tuple = std::tuple<TElements...>;

    /************************************************************************/
    /* LOCKSTEP                                                             */
    /************************************************************************/

    /// \brief Constant equal to the minimum rank among all provided tuple-like types.
    template <typename TTuple, typename... TTuples>
    constexpr Int LockstepRankV = std::min(LockstepRankV<TTuple>, LockstepRankV<TTuples...>);

    /// \brief Partial specialization for tuples which exposes its rank.
    template<typename... TElements>
    constexpr Int LockstepRankV<std::tuple<TElements...>> = sizeof...(TElements);

    /// \brief Apply a function to each tuple obtained by projecting each element in each tuple in lockstep.
    /// If tuples have different ranks, the minimum rank among them is selected.
    template <typename TFunction, typename... TTuples>
    constexpr void LockstepApply(TFunction&& function, TTuples&&... tuples);

    /// \brief Apply a function to a tuple obtained by projecting the I-th element of all provided tuples-like objects.
    /// \remarks Uses the definition of a generic function "get<I>" to access the I-th element of each tuple object.
    template <Int I, typename TFunction, typename... TTuples>
    constexpr void LockstepApplyAt(TFunction function, TTuples&&... tuples);

    /************************************************************************/
    /* NON-MEMBER FUNCTIONS                                                 */
    /************************************************************************/

    /// \brief Access a tuple element.
    template <Int index, typename TTuple>
    constexpr decltype(auto) Get(TTuple&& tuple) noexcept;

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    namespace details
    {
        /// \brief Apply a function to all tuples generated by projecting the I-th element of all provided tuples for each index in Is.
        template <typename TFunction, typename... TTuples, Int... Is>
        inline constexpr void LockstepApply(TFunction&& function, TTuples&&... tuples, std::index_sequence<Is...>)
        {
            (LockstepApplyAt<Is>(std::forward<TFunction>(function), std::forward<TTuples>(tuples)...), ...);
        }
    }

    // Lockstep.

    template <typename TFunction, typename... TTuples>
    constexpr void LockstepApply(TFunction&& function, TTuples&&... tuples)
    {
        details::LockstepApply<TFunction, TTuples...>(std::forward<TFunction>(function), std::forward<TTuples>(tuples)..., std::make_index_sequence<LockstepRankV<std::decay_t<TTuples>...>>{});
    }

    template <Int I, typename TFunction, typename... TTuples>
    constexpr void LockstepApplyAt(TFunction function, TTuples&&... tuples)
    {
        using std::get;

        function(get<I>(tuples)...);
    }

    // Non-member functions.

    template <Int index, typename TTuple>
    constexpr decltype(auto) Get(TTuple&& tuple) noexcept
    {
        return std::get<static_cast<std::size_t>(index)>(std::forward<TTuple>(tuple));
    }

}
