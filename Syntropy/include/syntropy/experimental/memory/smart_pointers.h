
/// \file smart_pointers.h
/// \brief This header is part of Syntropy core module. It contains classes used to handle objects via smart-pointers.
///
/// \author Raffaele D. Facendola - 2020

#pragma once

#include <memory>

#include "syntropy/language/support.h"
#include "syntropy/experimental/core/memory.h"

#include "syntropy/allocators/allocator.h"

#include "syntropy/experimental/core/polymorphic_allocator.h"
#include "syntropy/experimental/core/polymorphic_deleter.h"

namespace Syntropy
{
    /************************************************************************/
    /* UNIQUE PTR                                                           */
    /************************************************************************/

    /// \brief Type alias for exclusive ownership smart pointer.
    template <typename TType>
    using UniquePtr = std::unique_ptr<TType, PolymorphicDeleter>;

    /************************************************************************/
    /* SHARED PTR                                                           */
    /************************************************************************/

    /// \brief Type alias for shared ownership smart pointer.
    template <typename TType>
    using SharedPtr = std::shared_ptr<TType>;

    /************************************************************************/
    /* WEAK PTR                                                             */
    /************************************************************************/

    /// \brief Type alias for a weak-reference smart pointer.
    template <typename TType>
    using WeakPtr = std::weak_ptr<TType>;

    /************************************************************************/
    /* NON-MEMBER FUNCTIONS                                                 */
    /************************************************************************/

    /// \brief Create a new object wrapped inside a UniquePtr.
    template <typename TType, typename... TArguments>
    UniquePtr<TType> MakeUnique(TArguments&&... arguments);

    /// \brief Create a new shared object wrapped inside a SharedPtr.
    template <typename TType, typename... TArguments>
    SharedPtr<TType> MakeShared(TArguments&&... arguments);

    /// \brief Create a new weak reference to an object from a SharedPtr.
    template <typename TType>
    WeakPtr<TType> MakeWeak(const SharedPtr<TType>& rhs);

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Non-member functions.

    template <typename TType, typename... TArguments>
    inline UniquePtr<TType> MakeUnique(TArguments&&... arguments)
    {
        auto& memory_resource = Memory::GetAllocator();

        auto object = Memory::FromTypeless<TType>(memory_resource.Allocate(Memory::SizeOf<TType>(), Memory::AlignmentOf<TType>()).GetData());

        ConstructAt(object, Forward<TArguments>(arguments)...);

        return UniquePtr<TType>(object, MakePolymorphicDeleter<TType>(memory_resource));
    }

    template <typename TType, typename... TArguments>
    inline SharedPtr<TType> MakeShared(TArguments&&... arguments)
    {
        return std::allocate_shared<TType>(PolymorphicAllocator<TType>{}, Forward<TArguments>(arguments)...);
    }

    template <typename TType>
    inline WeakPtr<TType> MakeWeak(const SharedPtr<TType>& rhs)
    {
        return WeakPtr<TType>(rhs);
    }

}

