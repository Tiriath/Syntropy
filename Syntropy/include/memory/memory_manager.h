
/// \file memory_manager.h
/// \brief This header is part of the syntropy memory management system. It contains classes used to handle context-based dynamic allocations and
/// application-level allocator lifecycles.
///
/// \author Raffaele D. Facendola - 2017

#pragma once

#include <vector>
#include <memory>
#include <type_traits>

#include "memory.h"
#include "allocator.h"

/// \brief Instantiate a new object on the active syntropy::MemoryManager allocator.
/// \usage auto foo = SYNTROPY_MM_NEW Foo();
#define SYNTROPY_MM_NEW \
    SYNTROPY_NEW(syntropy::MemoryManager::GetInstance().GetAllocator())

/// \brief Delete an object that was allocated via an allocator registered to syntropy::MemoryManager.
/// Searches for any allocator owned by MemoryManager that can delete the object, regardless of the current active allocator and the allocator who made the allocation.
/// \usage SYNTROPY_MM_DELETE(pointer);
#define SYNTROPY_MM_DELETE(ptr) \
    SYNTROPY_DELETE(*syntropy::MemoryManager::GetInstance().GetAllocator(ptr), ptr)

/// \brief Allocate a new buffer on the active syntropy::MemoryManager allocator.
/// \usage void* buffer = SYNTROPY_MM_ALLOC(size);
#define SYNTROPY_MM_ALLOC(size) \
    SYNTROPY_ALLOC(syntropy::MemoryManager::GetInstance().GetAllocator(), size)

/// \brief Free a buffer that was allocated via an allocator registered to syntropy::MemoryManager.
/// Searches for any allocator owned by the MemoryManager who may deallocate the given block, regardless of the current active allocator and the allocator who made the allocation.
/// \usage SYNTROPY_MM_FREE(buffer);
#define SYNTROPY_MM_FREE(ptr) \
    SYNTROPY_FREE(*syntropy::MemoryManager::GetInstance().GetAllocator(ptr), ptr)

namespace syntropy
{

    /// \brief Class used to manage application allocators and allocation contexts.
    /// This singleton is meant to handle allocators that exist for application's entire duration.
    /// Allocators can be added to the manager at any given time, however, once created, they lasts until the application is closed.
    /// Each thread has its own stack of allocators: whenever a new dynamic allocation request is performed (see SYNTROPY_MM_NEW \ SYNTROPY_MM_ALLOC),
    /// the request is forwarded to the allocator on top of the stack. If the stack is empty, the request is handled by the default allocator.
    /// Use syntropy::MemoryContext to push new allocators.
    /// Use ::SetDefaultAllocator to set a new default allocator. If no default allocator is specified, the first allocator added to the manager becomes the default allocator.
    /// 
    /// \author Raffaele D. Facendola - March 2017
    class MemoryManager
    {
    public:

        /// \brief Get the singleton instance.
        /// \return Returns the singleton instance.
        static MemoryManager& GetInstance();

        /// \brief No copy constructor.
        MemoryManager(const MemoryManager&) = delete;

        /// \brief No assignment operator.
        MemoryManager& operator=(const MemoryManager&) = delete;

        /// \brief Default destructor.
        ~MemoryManager() = default;

        /// \brief Create a new allocator on the memory manager.
        /// The allocator name must be unique within the memory manager.
        /// \return Returns a reference to the new allocator.
        template <typename TAllocator, typename... TArguments>
        TAllocator& CreateAllocator(TArguments&&... arguments);

        /// \brief Set the default allocator.
        /// The default allocator is the allocator that is used when the allocator stack is empty.
        /// \param allocator_name Name of the new default allocator.
        /// \return Returns true if the default allocator was set successfully, returns false otherwise.
        /// \remarks The default allocator must have been created via CreateAllocator method.
        bool SetDefaultAllocator(const HashedString& allocator_name);

        /// \brief Get the default allocator.
        /// \return Returns the default allocator.
        Allocator& GetDefaultAllocator();

        /// \brief Get the current active allocator.
        /// \return Returns the allocator on top of the allocator stack.
        Allocator& GetAllocator();

        /// \brief Get the allocator owning a memory block.
        /// \return Returns the allocator owning the specified memory block. If no such allocator exists, returns nullptr.
        /// \remarks This method searches only inside the allocators owned by the MemoryManager.
        Allocator* GetAllocator(void* block);

        /// \brief Get an allocator by name.
        /// \param allocator_name Unique name of the allocator to get.
        /// \return Returns the allocator whose name is the specified one. If no such allocator exists, returns nullptr.
        Allocator* GetAllocator(const HashedString& allocator_name);

    private:

        friend class MemoryContext;

        /// \brief Push a named allocator on top of the current allocator stack.
        /// If no matching allocator was found, pushes the default allocator on top of the stack.
        /// \param allocator_name Name of the allocator to push on top of the stack.
        void PushContext(const HashedString& allocator_name);

        /// \brief Pop the last allocator from the allocator stack.
        void PopContext();

        /// \brief Private constructor.
        MemoryManager();

        std::vector<std::unique_ptr<Allocator>> allocators_;                ///< \brief List of allocators in this manager. The first element is the default allocator.

        static thread_local std::vector<Allocator*> allocator_stack_;       ///< \brief Current stack of allocators.

    };

    /// \brief Class used to push and pop a memory context via RAII paradigm.
    ///
    /// A memory context defines which allocator will handle dynamic allocation requests performed via SYNTROPY_MM_NEW and SYNTROPY_MM_ALLOC
    ///
    /// An allocation context is valid only within the current thread, different threads may have different contexts at any given time.
    /// Allocation contexts may be nested but not overlapped.
    ///
    /// If no matching context is found the default context is used instead. This ensures that, in case of a missing context, allocations are not spread
    /// on other allocators.
    ///
    /// \usage: 
    /// {
    ///     MemoryContext ctx("foo_allocator")
    ///
    ///     ... allocation performed here via SYNTROPY_MM_NEW are now forwarded to "foo_allocator" ...
    ///
    /// }   // The context is no longer active here
    ///
    /// \author Raffaele D. Facendola - March 2017
    class MemoryContext
    {
    public:

        /// \brief Push a new memory context.
        /// \param context_name Name of the context to push (allocator name).
        MemoryContext(const HashedString& context_name);

        /// \brief Destructor. Pop the previously pushed context.
        ~MemoryContext();

    };

}

// Implementation

namespace syntropy
{

    /************************************************************************/
    /* MEMORY MANAGER                                                       */
    /************************************************************************/

    template <typename TAllocator, typename... TArguments>
    TAllocator& MemoryManager::CreateAllocator(TArguments&&... arguments)
    {
        auto allocator = std::make_unique<TAllocator>(std::forward<TArguments>(arguments)...);

        SYNTROPY_ASSERT(!GetAllocator(allocator->GetName()));       // Make sure there's no other allocator with the same name.

        allocators_.push_back(std::move(allocator));                // Acquire allocator's ownership.

        return *static_cast<TAllocator*>(allocators_.back().get());
    }

}