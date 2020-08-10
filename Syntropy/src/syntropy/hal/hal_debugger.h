
/// \file hal_debugger.h
/// \brief This header is part of the Syntropy hardware abstraction layer module. It contains methods to access debugger functionalities.
///
/// \author Raffaele D. Facendola - 2019

#pragma once

#include "syntropy/core/types.h"
#include "syntropy/diagnostics/stack_trace.h"

namespace Syntropy
{
    /************************************************************************/
    /* HAL DEBUGGER                                                         */
    /************************************************************************/

    /// \brief Exposes debugging methods.
    /// \author Raffaele D. Facendola - May 2020.
    namespace HALDebugger
    {
        /// \brief Check whether the debugger is attached.
        Bool IsAttached();

        /// \brief Get the stack trace on the current thread.
        StackTrace GetStackTrace(StackTraceElement here);
    }

}