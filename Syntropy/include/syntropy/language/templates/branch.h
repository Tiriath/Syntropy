
/// \file branch.h
/// \brief This header is part of Syntropy language module. It contains control-flow-related template machinery.
///
/// \author Raffaele D. Facendola - 2020.

#pragma once

#include <type_traits>

#include "syntropy/language/foundation/types.h"

namespace Syntropy::Templates
{
    /************************************************************************/
    /* CONDITIONAL                                                          */
    /************************************************************************/

    /// \brief Type equal to TTrue if VCondition is true, equal to TFalse otherwise.
    template <Bool VCondition, typename TTrue, typename TFalse>
    using Conditional = std::conditional_t<VCondition, TTrue, TFalse>;

}