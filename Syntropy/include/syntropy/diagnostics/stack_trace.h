
/// \file stack_trace.h
/// \brief This header is part of the Syntropy diagnostic module. It contains function to access the stack trace.
///
/// \author Raffaele D. Facendola - 2020

#pragma once

#include "syntropy/core/range.h"
#include "syntropy/core/string.h"

#include "syntropy/containers/vector.h"

#include "syntropy/language/macro.h"

/// \brief Expands to an object representing the location of the current line of code.
#define SYNTROPY_HERE \
    syntropy::StackTraceElement{ SYNTROPY_FILE, SYNTROPY_FUNCTION, SYNTROPY_LINE }

namespace syntropy
{
    /************************************************************************/
    /* STACK TRACE                                                          */
    /************************************************************************/

    struct StackTraceElement;

    /// \brief Represents a stack trace.
    /// \author Raffaele D. Facendola - November 2016
    struct StackTrace
    {
    public:

        /// \brief Create a new stack trace from a single element.
        StackTrace(StackTraceElement stack_trace_element);

        /// \brief Copy ctor.
        StackTrace(const StackTrace& other) = default;

        /// \brief Default destructor.
        ~StackTrace() = default;

        /// \brief Move ctor.
        StackTrace(StackTrace&& other) noexcept = default;

        /// \brief Unified assignment operator.
        StackTrace& operator=(StackTrace other) noexcept;

        /// \brief Swap the content of two stack trace instances.
        void Swap(StackTrace& other) noexcept;

        /// \brief Add a new stack trace element.
        template <typename TStackTraceElement>
        StackTrace& operator+=(TStackTraceElement&& stack_trace_element);

        /// \brief Access the range of stack trace elements, from the most recent one.
        Range<Vector<StackTraceElement>::const_iterator> GetRange() const;

    private:

        /// \brief Elements in the stack trace, from the most recent one.
        Vector<StackTraceElement> stack_trace_;

        /// \brief Whether the trace has symbols for the called.
        bool has_symbols_{ false };
    };

    /************************************************************************/
    /* STACK TRACE ELEMENT                                                  */
    /************************************************************************/

    /// \brief Represents the location of a line of code within a source file.
    /// \author Raffaele D. Facendola - November 2016
    struct StackTraceElement
    {
        /// \brief Name of the file the element refers to.
        String file_;              

        /// \brief Name of the function the element refers to.
        String function_;

        /// \brief Line index inside the source file.
        std::int64_t line_{ 0 };
    };

    /************************************************************************/
    /* DEBUGGER                                                             */
    /************************************************************************/

    /// \brief Expose debugging functionalities.
    /// \author Raffaele D. Facendola - December 2016.
    namespace Debugger
    {
        /// \brief Get the stack trace of the current thread.
        StackTrace GetStackTrace(StackTraceElement here);
    }

    /************************************************************************/
    /* NON-MEMBER FUNCTIONS                                                 */
    /************************************************************************/

    /// \brief Get the iterator to the first element in the stack trace.
    auto begin(const StackTrace& stack_trace);

    /// \brief Get the iterator past the last element in a stack trace.
    auto end(const StackTrace& stack_trace);

    /// \brief Get the iterator to the first element in the stack trace.
    auto cbegin(const StackTrace& stack_trace);

    /// \brief Get the iterator past the last element in a stack trace.
    auto cend(const StackTrace& stack_trace);

        /// \brief Output a stack trace inside a stream.
    std::ostream& operator<< (std::ostream &out, const StackTrace& stack_trace);

    /// \brief Swaps two stack traces.
    void swap(StackTrace& lhs, StackTrace& rhs);

    /// \brief Equality comparison for StackTraceElement.
    bool operator==(const StackTraceElement& lhs, const StackTraceElement& rhs);

    /// \brief Equality comparison for StackTraceElement.
    bool operator!=(const StackTraceElement& lhs, const StackTraceElement& rhs);

    /// \brief Output a stack trace element inside a stream.
    std::ostream& operator<< (std::ostream &out, const StackTraceElement& element);

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // StackTrace.

    inline StackTrace::StackTrace(StackTraceElement stack_trace_element)
        : stack_trace_{ {std::move(stack_trace_element)} }
    {
        stack_trace_.reserve(64);       // Educated guess of the trace length.
    }

    inline StackTrace& StackTrace::operator=(StackTrace other) noexcept
    {
        other.Swap(*this);

        return *this;
    }

    template <typename TStackTraceElement>
    inline StackTrace& StackTrace::operator+=(TStackTraceElement&& stack_trace_element)
    {
        // If the application has symbols, the stack trace will also contain the caller from which the trace was generated at some point.
        // If that symbol is found, then each element before it in the stack trace was generated later as a part of the diagnostic module and gets discarded.
        // If no such symbol is ever found, it means it is hidden somewhere in the unknown entries and we can't do much about that.

        if (!has_symbols_ && stack_trace_element == stack_trace_.front())
        {
            stack_trace_.resize(1);

            has_symbols_ = true;
        }
        else
        {
            stack_trace_.emplace_back(std::forward<TStackTraceElement>(stack_trace_element));
        }

        return *this;
    }

    inline void StackTrace::Swap(StackTrace& other) noexcept
    {
        using std::swap;

        swap(stack_trace_, other.stack_trace_);
    }

    inline Range<Vector<StackTraceElement>::const_iterator> StackTrace::GetRange() const
    {
        return MakeConstRange(stack_trace_);
    }

    // Non-member functions.

    inline auto begin(const StackTrace& stack_trace)
    {
        return stack_trace.GetRange().Begin();
    }

    inline auto end(const StackTrace& stack_trace)
    {
        return stack_trace.GetRange().End();
    }

    inline auto cbegin(const StackTrace& stack_trace)
    {
        return begin(stack_trace);
    }

    inline auto cend(const StackTrace& stack_trace)
    {
        return end(stack_trace);
    }

    inline bool operator==(const StackTraceElement& lhs, const StackTraceElement& rhs)
    {
        return (lhs.file_ == rhs.file_) && (lhs.function_ == rhs.function_) && (lhs.line_ == rhs.line_);
    }

    inline bool operator!=(const StackTraceElement& lhs, const StackTraceElement& rhs)
    {
        return !(lhs == rhs);
    }

    inline void swap(StackTrace& lhs, StackTrace& rhs)
    {
        lhs.Swap(rhs);
    }

}