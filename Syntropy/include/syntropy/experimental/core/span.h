
/// \file span.h
/// \brief This header is part of Syntropy core module. It contains definition of spans.
///
/// \author Raffaele D. Facendola - 2017

#pragma once

#include "syntropy/language/type_traits.h"
#include "syntropy/core/types.h"

namespace syntropy
{
    /************************************************************************/
    /* SPAN <ELEMENT>                                                       */
    /************************************************************************/

    /// \brief Represents a contiguous range of elements.
    /// \author Raffaele D. Facendola - June 2020.
    template <typename TElement>
    class Span
    {
        template <typename UElement>
        friend ObserverPtr<UElement> begin(const Span<UElement>& span);

        template <typename UElement>
        friend ObserverPtr<UElement> end(const Span<UElement>& span);

        template <typename UElement>
        friend class Span;

    public:

        /// \brief Create an empty span.
        Span() = default;

        /// \brief Create a span given the first element and the span size.
        template <typename TFirst>
        Span(TFirst first, Int count);

        /// \brief Create a spawn given the first and one past the last element in the span.
        template <typename TFirst, typename TLast, typename = EnableIfT<!IsConvertibleV<TLast, Int>>>
        Span(TFirst first, TLast last);

        /// \brief Copy constructor.
        template <typename UElement>
        Span(const Span<UElement>& rhs);

        /// \brief Default destructor.
        ~Span() = default;

        /// \brief Copy assignment operator.
        template <typename UElement>
        Span& operator=(const Span<UElement>& rhs);

        /// \brief Check whether the span is non-empty.
        operator Bool() const;

        /// \brief Access the first element in the span.
        /// If the span is empty the behavior of this method is undefined.
        TElement& GetFront() const;

        /// \brief Access the first element in the span.
        /// If the span is empty the behavior of this method is undefined.
        TElement& GetBack() const;

        /// \brief Access an element by index.
        /// If the provided index is not within the span the behavior of this method is undefined.
        TElement& operator[](Int index) const;

        /// \brief Get the number of elements in the span.
        Int GetCount() const;

    private:

        /// \brief Pointer to the first element in the rage.
        ObserverPtr<TElement> begin_{ nullptr };

        /// \brief Number of elements in the span.
        Int count_{ 0 };

    };

    /************************************************************************/
    /* NON-MEMBER FUNCTIONS                                                 */
    /************************************************************************/

    template <typename TElement>
    inline ObserverPtr<TElement> begin(const Span<TElement>& span)
    {
        return span.begin_;
    }

    template <typename TElement>
    inline ObserverPtr<TElement> end(const Span<TElement>& span)
    {
        return span.begin_ + span.count_;
    }

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Span<TElement>.

    template <typename TElement>
    template <typename TFirst>
    inline Span<TElement>::Span(TFirst first, Int count)
        : begin_(first)
        , count_(count)
    {

    }

    template <typename TElement>
    template <typename TFirst, typename TLast, typename>
    inline Span<TElement>::Span(TFirst first, TLast last)
        : begin_(first)
        , count_(ToInt(last - first))
    {

    }

    template <typename TElement>
    template <typename UElement>
    inline Span<TElement>::Span(const Span<UElement>& rhs)
        : begin_(rhs.begin_)
        , count_(rhs.count_)
    {

    }

    template <typename TElement>
    template <typename UElement>
    inline Span<TElement>& Span<TElement>::operator=(const Span<UElement>& rhs)
    {
        begin_ = rhs.begin_;
        count_ = rhs.count_;
    }

    template <typename TElement>
    inline Span<TElement>::operator Bool() const
    {
        return count_ > 0;
    }

    template <typename TElement>
    inline TElement& Span<TElement>::GetFront() const
    {
        return *begin_;
    }

    template <typename TElement>
    inline TElement& Span<TElement>::GetBack() const
    {
        return *(begin_ + count_ - 1);
    }

    template <typename TElement>
    inline  TElement& Span<TElement>::operator[](Int index) const
    {
        return begin_[index];
    }

    template <typename TElement>
    inline Int Span<TElement>::GetCount() const
    {
        return count_;
    }

}
