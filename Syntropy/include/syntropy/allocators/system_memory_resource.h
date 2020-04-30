
/// \file system_memory_resource.h
/// \brief This header is part of the Syntropy allocators module. It contains memory resources using system heap memory.
///
/// \author Raffaele D. Facendola - 2017

#pragma once

#include "syntropy/syntropy.h"

#include "syntropy/memory/bytes.h"
#include "syntropy/memory/alignment.h"
#include "syntropy/memory/memory_address.h"
#include "syntropy/memory/memory_range.h"

namespace syntropy
{
    /************************************************************************/
    /* SYSTEM MEMORY RESOURCE                                               */
    /************************************************************************/

    /// \brief Tier 0 memory resource used to allocate memory on system heap via new\delete calls.
    /// \author Raffaele D. Facendola - February 2017
    class SystemMemoryResource
    {
    public:

        /// \brief Default constructor.
        SystemMemoryResource() noexcept = default;

        /// \brief Default copy constructor.
        SystemMemoryResource(const SystemMemoryResource&) noexcept = default;

        /// \brief Default move constructor.
        SystemMemoryResource(SystemMemoryResource&&) noexcept = default;

        /// \brief Default destructor.
        ~SystemMemoryResource() noexcept = default;

        /// \brief Default assignment operator.
        SystemMemoryResource& operator=(const SystemMemoryResource&) noexcept = default;

        /// \brief Allocate a new memory block.
        /// \param size Size of the memory block to allocate.
        /// \return Returns a range representing the requested memory block. If no allocation could be performed returns an empty range.
        MemoryRange Allocate(Bytes size) noexcept;

        /// \brief Allocate a new aligned memory block.
        /// \param size Size of the memory block to allocate.
        /// \param alignment Block alignment.
        /// \return Returns a range representing the requested aligned memory block. If no allocation could be performed returns an empty range.
        MemoryRange Allocate(Bytes size, Alignment alignment) noexcept;

        /// \brief Deallocate a memory block.
        /// \param block Block to deallocate.
        /// \remarks The behavior of this function is undefined unless the provided block was returned by a previous call to ::Allocate(size).
        void Deallocate(const MemoryRange& block) noexcept;

        /// \brief Deallocate an aligned memory block.
        /// \param block Block to deallocate. Must refer to any allocation performed via Allocate(size, alignment).
        /// \param alignment Block alignment.
        /// \remarks The behavior of this function is undefined unless the provided block was returned by a previous call to ::Allocate(size, alignment).
        void Deallocate(const MemoryRange& block, Alignment alignment) noexcept;

        /// \brief Check whether this memory resource owns the provided memory block.
        /// \param block Block to check the ownership of.
        /// \return Returns true if the provided memory range was allocated by this memory resource, returns false otherwise.
        bool Owns(const MemoryRange& block) const noexcept;

    };

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // SystemMemoryResource.

    inline MemoryRange SystemMemoryResource::Allocate(Bytes size) noexcept
    {
        if (auto block = MemoryAddress{ ::operator new(*size, std::nothrow) })
        {
            return { block, block + size };
        }

        return {};
    }

    inline MemoryRange SystemMemoryResource::Allocate(Bytes size, Alignment alignment) noexcept
    {
        if (auto block = MemoryAddress{ ::operator new(*size, alignment, std::nothrow) })
        {
            return { block, block + size };
        }

        return {};
    }

    inline void SystemMemoryResource::Deallocate(const MemoryRange& block) noexcept
    {
        ::operator delete(block.Begin(), std::nothrow);
    }

    inline void SystemMemoryResource::Deallocate(const MemoryRange& block, Alignment alignment) noexcept
    {
        ::operator delete(block.Begin(), alignment, std::nothrow);
    }

    inline bool SystemMemoryResource::Owns(const MemoryRange& block) const noexcept
    {
        // This is not correct, however system memory resource is expected to be used as last resort 
        // to other memory resources or used as the single allocator for the application.

        return true;
    }

}

