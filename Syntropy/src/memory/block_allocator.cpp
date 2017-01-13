#include "memory/block_allocator.h"

#include "platform/system.h"

#include "diagnostics/log.h"

namespace syntropy
{

    //////////////// BLOCK ALLOCATOR ////////////////

    BlockAllocator::BlockAllocator(size_t capacity, size_t block_size)
        : memory_(GetMemory())                                                              // Get a reference to the memory handler.
        , block_size_(Math::Ceil(block_size, memory_.GetAllocationGranularity()))           // Round up to the next system allocation granularity.
        , capacity_(Math::Ceil(capacity, block_size_))                                      // Round up to the next block size.
        , base_(reinterpret_cast<int8_t*>(memory_.Reserve(capacity)))                       // Reserve the entire virtual memory range without allocating
        , head_(base_)
    {
        SYNTROPY_ASSERT(base_);

        // TODO: Move to bookkeeping!!!
        // TODO: This allocation here stinks! It can waste a ton of space when using multiple block allocators.
        auto total_memory = platform::GetSystem().GetMemoryInfo().total_physical_memory_;               // Maximum amount of memory that can be allocated at any given time
        auto maximum_blocks = total_memory / block_size_;                                               // Maximum amount of blocks that can be allocated using this allocator, also upper bound for the free list
        auto bookkeeping_size = maximum_blocks * sizeof(size_t*);                                       // Size of the internal stack

        free_base_ = reinterpret_cast<uintptr_t*>(memory_.Allocate(bookkeeping_size));
        free_head_ = free_base_;
    }

    BlockAllocator::~BlockAllocator()
    {
        memory_.Free(base_);
    }

    void* BlockAllocator::Allocate()
    {
        return Allocate(block_size_);
    }

    void* BlockAllocator::Allocate(size_t size)
    {
        SYNTROPY_ASSERT(size <= block_size_);

        void* block;

        // TODO: Split to Reserve\Allocate (to support streaming)
        if (free_base_ == free_head_)                           // No free block?
        {
            block = head_;                                      // Allocate a new memory block on the allocator's head.
            head_ = Memory::Offset(head_, block_size_);         // Move the head forward.

            SYNTROPY_ASSERT(GetSize() <= capacity_);
        }
        else
        {
            --free_head_;                                       // Pop a free block address.
            block = reinterpret_cast<void*>(*free_head_);       // Address of the free block.
        }

        memory_.Commit(block, size);                            // Commit the requested memory size. Rounded up to the next memory page boundary.
        return block;
    }

    void BlockAllocator::Free(void* block)
    {
        SYNTROPY_ASSERT(ContainsAddress(block));

        block = Memory::AlignDown(block, block_size_);          // Get the base address of the block to free.

        *free_head_ = reinterpret_cast<uintptr_t>(block);       // Save the address of the free block.
        ++free_head_;                                           // Move the head forward.
        memory_.Decommit(block, block_size_);                   // Unmap the block from the system memory.
    }

    size_t BlockAllocator::GetBlockSize() const
    {
        return block_size_;
    }

    size_t BlockAllocator::GetSize() const
    {
        auto maximum_size = std::distance(base_, head_);                            // Maximum amount of blocks allocated
        auto free_size = std::distance(free_base_, free_head_) * block_size_;       // Amount of freed memory

        // TODO: This is actually the maximum size, the committed memory may be lower.
        return maximum_size - free_size;
    }

    size_t BlockAllocator::GetCapacity() const
    {
        return capacity_;
    }

    bool BlockAllocator::ContainsAddress(void* address) const
    {
        return std::distance(base_, reinterpret_cast<int8_t*>(address)) >= 0 &&
               std::distance(reinterpret_cast<int8_t*>(address), head_) > 0;
    }

    //////////////// MONOTONIC BLOCK ALLOCATOR ////////////////

    MonotonicBlockAllocator::MonotonicBlockAllocator(size_t capacity, size_t block_size)
        : memory_(GetMemory())                                                              // Get a reference to the memory handler.
        , block_size_(Math::Ceil(block_size, memory_.GetAllocationGranularity()))           // Round up to the next system allocation granularity.
        , capacity_(Math::Ceil(capacity, block_size_))                                      // Round up to the next block size.
        , base_(reinterpret_cast<int8_t*>(memory_.Reserve(capacity)))                       // Reserve the entire virtual memory range without allocating
        , head_(base_)
        , free_(nullptr)
    {
        SYNTROPY_ASSERT(base_);
    }

    MonotonicBlockAllocator::~MonotonicBlockAllocator()
    {
        memory_.Free(base_);
    }

    void* MonotonicBlockAllocator::Allocate()
    {
        auto block = free_;

        if (block)
        {
            free_ = block->next_;                               // Move to the next free block.
        } 
        else
        {
            block = reinterpret_cast<Block*>(head_);            // Allocate a new memory block on the allocator's head
            memory_.Commit(block, block_size_);                 // Commit the new memory block
            head_ = Memory::Offset(head_, block_size_);         // Move the head forward

            SYNTROPY_ASSERT(GetSize() <= capacity_);
        }

        return block;
    }

    void MonotonicBlockAllocator::Free(void* block)
    {
        SYNTROPY_ASSERT(ContainsAddress(block));

        block = Memory::AlignDown(block, block_size_);          // Get the base address of the block to free.

        auto free = reinterpret_cast<Block*>(block);

        free->next_ = free_;
        free_ = free;
    }

    size_t MonotonicBlockAllocator::GetBlockSize() const
    {
        return block_size_;
    }

    size_t MonotonicBlockAllocator::GetSize() const
    {
        return std::distance(base_, head_);
    }

    size_t MonotonicBlockAllocator::GetCapacity() const
    {
        return capacity_;
    }

    bool MonotonicBlockAllocator::ContainsAddress(void* address) const
    {
        return std::distance(base_, reinterpret_cast<int8_t*>(address)) >= 0 &&
               std::distance(reinterpret_cast<int8_t*>(address), head_) > 0;
    }

}