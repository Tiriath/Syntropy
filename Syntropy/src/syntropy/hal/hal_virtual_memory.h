
/// \file hal_virtual_memory.h
/// \brief This header is part of the Syntropy hardware abstraction layer module. It exposes APIs needed to handle virtual memory.
///
/// \author Raffaele D. Facendola - 2017

#pragma once

#include "syntropy/memory/bytes.h"
#include "syntropy/memory/alignment.h"
#include "syntropy/memory/memory_range.h"

namespace syntropy
{
    /************************************************************************/
    /* HAL VIRTUAL MEMORY                                                   */
    /************************************************************************/

    /// \brief Abstracts low level primitives used to handle virtual memory.
    /// \author Raffaele D. Facendola - 2017
    namespace HALVirtualMemory
    {
        /// \brief Get the virtual memory page size.
        /// \return Returns the virtual memory page size, in bytes.
        Bytes GetPageSize();

        /// \brief Get the virtual memory page alignment.
        /// \return Returns the virtual memory page alignment, in bytes.
        Alignment GetPageAlignment();

        /// \brief Reserve a range of virtual memory addresses.
        /// Reserved memory region must be committed via Commit() before accessing it.
        /// \param size Size of the range to reserve, in bytes.
        /// \return Returns the reserved memory range. If the method fails returns an empty range.
        MemoryRange Reserve(Bytes size);

        /// \brief Allocate a range of virtual memory addresses.
        /// This method has the same effect as a Reserve() followed by a Commit().
        /// \param size Size of the range to reserve, in bytes.
        /// \return Returns the reserved virtual memory range. If the method fails returns an empty range.
        MemoryRange Allocate(Bytes size);

        /// \brief Release a range of virtual memory addresses.
        /// \param address First address of the range to release. Must match any return value of a previous Reserve() / Allocate(), otherwise the behaviour is unspecified.
        /// \return Returns true if the range could be released, returns false otherwise.
        bool Release(const MemoryRange& memory_range);

        /// \brief Commit a reserved virtual memory block.
        /// This method allocates all the pages containing at least one byte in the provided range and makes them accessible by the application.
        /// \param memory_range Memory range to commit.
        /// \return Returns true if the memory could be committed, returns false otherwise.
        /// \remarks The provided memory range must refer to a memory region that was previously reserved via Reserve().
        bool Commit(const MemoryRange& memory_range);

        /// \brief Decommit a virtual memory block.
        /// This method decommits all the pages containing at least one byte in the provided range.
        /// \param memory_range Memory range to decommit.
        bool Decommit(const MemoryRange& memory_range);

    };

}