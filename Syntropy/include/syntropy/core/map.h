
/// \file map.h
/// \brief This header is part of the Syntropy containers module. It contains wrappers for maps and multimaps.
///
/// \author Raffaele D. Facendola - 2020

#pragma once

#include <unordered_map>

#include "syntropy/language/functional.h"
#include "syntropy/language/utility.h"
#include "syntropy/math/hash.h"

#include "syntropy/allocators/polymorphic_allocator.h"

namespace syntropy
{
    /************************************************************************/
    /* MAP <TKEY, TVALUE, THASH, TPRED>                                     */
    /************************************************************************/

    /// \brief Alias type for std::unordered_map with polymorphic allocator type.
    template <typename TKey, typename TValue, typename THash = HashFunctor64<TKey>, typename TPred = std::equal_to<TKey>>
    using Map = std::unordered_map<TKey, TValue, THash, TPred, PolymorphicAllocator<std::pair<const TKey, TValue>>>;

}