
/// \file block_allocator.h
/// \brief This header is part of the syntropy memory management system. It contains allocators for large chunks of memory that can be used as a base for more specialized ones.
///
/// \author Raffaele D. Facendola - 2017

#pragma once

#include "memory.h"
#include "diagnostics/debug.h"
#include "math/math.h"

namespace syntropy
{

    /// \brief Block allocator used to allocate fixed-size memory blocks on a contiguous address range.
    /// Both allocations and deallocations are performed on demand: when a memory block is needed it gets mapped to the system memory; when no longer needed the memory is returned to the system.
    /// This allocator ensures that only the memory being currently used is allocated but may suffer from kernel calls overhead. If a smaller, more performance-aware, allocator is needed check MonotonicBlockAllocator.
    /// \author Raffaele D. Facendola - 2017
    class BlockAllocator
    {
    public:

        /// \brief Initialize the block allocator.
        BlockAllocator(size_t capacity, size_t block_size);

        /// \brief Default destructor
        ~BlockAllocator();

        /// \brief Allocate a memory block.
        /// \return Returns a pointer to the allocated memory block.
        void* Allocate();

        /// \brief Free a memory block.
        /// \param block Address of the block to free.
        void Free(void* block);

        /// \brief Get the size of each block.
        /// \return Returns the size of each block in bytes.
        size_t GetBlockSize() const;

        /// \brief Get the effective size of the allocator.
        /// \return Returns the effective size of the allocator, in bytes.
        size_t GetSize() const;

        /// \brief Get total capacity of this allocator.
        /// \return Returns the total capacity of this allocator, in bytes.
        size_t GetCapacity() const;

    private:

        Memory& memory_;                ///< \brief Handles virtual memory.

        size_t block_size_;             ///< \brief Size of each block in bytes.

        size_t capacity_;               ///< \brief Capacity of the allocator.

        void* base_;                    ///< \brief Base pointer to the memory chunk reserved for this allocator.

        void* head_;                    ///< \brief Pointer to the first unmapped block.

        size_t* free_base_;             ///< \brief Base of the free list.

        size_t* free_head_;             ///< \brief Points past the last free index.

    };

    /// \brief Block allocator used to allocate fixed-size memory blocks on a contiguous address range.
    /// This allocator uses a no-deallocation policy to avoid kernel calls: free blocks are kept allocated and recycled when possible.
    /// Allocations are performed on demand.
    /// \author Raffaele D. Facendola - 2017
    class MonotonicBlockAllocator
    {
    public:

        /// \brief Initialize the block allocator.
        MonotonicBlockAllocator(size_t capacity, size_t block_size);

        /// \brief Default destructor
        ~MonotonicBlockAllocator();

        /// \brief Allocate a memory block.
        /// \return Returns a pointer to the allocated memory block.
        void* Allocate();
        
        /// \brief Free a memory block.
        /// \param block Address of the block to free.
        void Free(void* block);

        /// \brief Get the size of each block.
        /// \return Returns the size of each block in bytes.
        size_t GetBlockSize() const;

        /// \brief Get the effective size of the allocator.
        /// \return Returns the effective size of the allocator, in bytes.
        size_t GetSize() const;

        /// \brief Get total capacity of this allocator.
        /// \return Returns the total capacity of this allocator, in bytes.
        size_t GetCapacity() const;

    private:

        /// \brief Utility structure for free blocks.
        struct Block
        {
            Block* next_;               ///< \brief Pointer to the next free block.
        };

        Memory& memory_;                ///< \brief Handles virtual memory.

        size_t block_size_;             ///< \brief Size of each block in bytes.

        size_t capacity_;               ///< \brief Capacity of the allocator.

        void* base_;                    ///< \brief Base pointer to the memory chunk reserved for this allocator.

        void* head_;                    ///< \brief Pointer to the first unmapped block.

        Block* free_;                   ///< \brief First free block.

    };

}