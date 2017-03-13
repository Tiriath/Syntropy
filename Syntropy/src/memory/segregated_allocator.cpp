#include "memory/segregated_allocator.h"

#include <iterator>
#include <algorithm>
#include <new>

#include "memory/memory.h"
#include "diagnostics/debug.h"
#include "math/math.h"

namespace syntropy 
{

    /************************************************************************/
    /* LINEAR SEGREGATED FIT ALLOCATOR :: PAGE                              */
    /************************************************************************/

    LinearSegregatedFitAllocator::Page::Page(size_t block_size, size_t page_size)
        : next_(nullptr)
        , previous_(nullptr)
        , block_size_(block_size)
        , allocated_blocks_(0)
        , free_(GetFirstBlock())
    {
        // Fill the free-block linked list
        auto last_block = GetLastBlock(page_size);
        
        SYNTROPY_ASSERT(Memory::GetDistance(free_, last_block) >= 0);

        for(auto block = free_; block != last_block; block = block->next_)
        {
            block->next_ = Memory::AddOffset(block, block_size_);
        }
        
        last_block->next_ = nullptr;
    }

    void* LinearSegregatedFitAllocator::Page::AllocateBlock()
    {
        SYNTROPY_ASSERT(!IsFull());

        auto block = free_;
        free_ = block->next_;
        ++allocated_blocks_;

        return block;
    }

    void LinearSegregatedFitAllocator::Page::FreeBlock(void* block)
    {
        SYNTROPY_ASSERT(!IsEmpty());

        auto free_block = reinterpret_cast<Block*>(block);

        --allocated_blocks_;
        free_block->next_ = free_;
        free_ = free_block;
    }

    bool LinearSegregatedFitAllocator::Page::IsFull() const
    {
        return !free_;
    }

    bool LinearSegregatedFitAllocator::Page::IsEmpty() const
    {
        return allocated_blocks_ == 0;
    }

    LinearSegregatedFitAllocator::Block* LinearSegregatedFitAllocator::Page::GetFirstBlock()
    {
        // Find the first address past the page which is aligned to the block size.
        return reinterpret_cast<Block*>(Memory::Align(Memory::AddOffset(this, sizeof(Page)), block_size_));
    }

    LinearSegregatedFitAllocator::Block* LinearSegregatedFitAllocator::Page::GetLastBlock(size_t page_size)
    {
        page_size -= 2u * block_size_ + 1u;     // Ensures that there's enough space for a block and the required padding.

        return reinterpret_cast<Block*>(Memory::Align(Memory::AddOffset(this, page_size), block_size_));
    }

    /************************************************************************/
    /* LINEAR SEGREGATED FIT ALLOCATOR                                      */
    /************************************************************************/

    LinearSegregatedFitAllocator::LinearSegregatedFitAllocator(const HashedString& name, size_t capacity, size_t class_size, size_t order, size_t page_size)
        : Allocator(name)
        , allocator_(capacity, page_size)
        , free_lists_(order)
        , class_size_(class_size)
    {
        CheckPreconditions();
    }

    LinearSegregatedFitAllocator::LinearSegregatedFitAllocator(const HashedString& name, const MemoryRange& memory_range, size_t class_size, size_t order, size_t page_size)
        : Allocator(name)
        , allocator_(memory_range, page_size)
        , free_lists_(order)
        , class_size_(class_size)
    {
        CheckPreconditions();
    }

    void* LinearSegregatedFitAllocator::Allocate(size_t size)
    {
        SYNTROPY_ASSERT(size > 0);

        // Get a page from the list whose size is at least as big as the requested allocation size.

        auto list_index = GetListIndexBySize(size);
        auto page = free_lists_[list_index];

        if (!page)
        {
            page = AllocatePage(size);          // If no free page is found allocate a new one.
        }

        auto block = page->AllocateBlock();     // Allocate a new block inside the page.
        
        if (page->IsFull())
        {
            DiscardPage(list_index);            // If the page is full move to the next one.
        }

        return block;
    }

    void* LinearSegregatedFitAllocator::Allocate(size_t size, size_t alignment)
    {
        SYNTROPY_ASSERT(Math::IsPow2(alignment));

        // Since blocks are aligned to their own size, we are looking for a block large enough that is also a multiple of the requested alignment.
        auto block = Allocate(Math::Ceil(size, alignment));

        SYNTROPY_ASSERT(Memory::IsAlignedTo(block, alignment));

        return block;
    }

    void LinearSegregatedFitAllocator::Free(void* address)
    {
        auto page = reinterpret_cast<Page*>(Memory::AlignDown(address, GetPageSize()));     // Page the block belongs to

        auto is_full = page->IsFull();

        page->FreeBlock(address);               // Add the block to free block list

        if (page->IsEmpty())
        {
            FreePage(page);                     // If the page became empty return it to the allocator so it can be recycled by other lists.
        }
        else if (is_full)
        {
            RestorePage(page);                  // If the page was full insert it into the proper free list so that new blocks can be allocated inside it.
        }
    }

    bool LinearSegregatedFitAllocator::Belongs(void* block) const
    {
        return allocator_.GetRange().Contains(block);
    }

    size_t LinearSegregatedFitAllocator::GetMaxAllocationSize() const
    {
        return GetOrder() * class_size_;        // Size of the largest class the allocator can handle.
    }

    size_t LinearSegregatedFitAllocator::GetOrder() const
    {
        return free_lists_.size();          // One free list per order.
    }

    size_t LinearSegregatedFitAllocator::GetPageSize() const
    {
        return allocator_.GetBlockSize();
    }

    const MemoryRange& LinearSegregatedFitAllocator::GetRange() const
    {
        return allocator_.GetRange();
    }

    size_t LinearSegregatedFitAllocator::GetListIndexBySize(size_t size) const
    {
        return (size - 1u) / class_size_;
    }

    LinearSegregatedFitAllocator::Page* LinearSegregatedFitAllocator::AllocatePage(size_t block_size)
    {
        auto storage = reinterpret_cast<Page*>(allocator_.Allocate());          // Storage for a new page.

        block_size = Math::Ceil(block_size, class_size_);                       // The allocation granularity is determined by the class size.

        auto page = new (storage) Page(block_size, GetPageSize());              // Create a new page.

        auto list_index = GetListIndexBySize(block_size);

        free_lists_[list_index] = page;                                         // Add the page to the proper list

        return page;
    }

    void LinearSegregatedFitAllocator::DiscardPage(size_t list_index)
    {
        // Pop the page from the head of the free list. Note that any reference to that page is lost until one block inside is freed.

        auto page = free_lists_[list_index];

        page = page->next_;

        if (page)
        {
            page->previous_ = nullptr;
        }

        free_lists_[list_index] = page;
    }

    void LinearSegregatedFitAllocator::FreePage(Page* page)
    {
        // Fix the double-linked list pointers

        if (page->previous_)
        {
            page->previous_->next_ = page->next_;       // The page is in the middle of the segregated list.
        }
        else
        {
            auto list_index = GetListIndexBySize(page->block_size_);

            free_lists_[list_index] = page->next_;      // The page was the head of a free list.
        }

        if (page->next_)
        {
            page->next_->previous_ = page->previous_;   // The page is not the last in the segregated list.
        }

        allocator_.Free(page);                          // Return the page to the shared allocator.
    }

    void LinearSegregatedFitAllocator::RestorePage(Page* page)
    {
        // Push the page on top of the proper free list.

        auto list_index = GetListIndexBySize(page->block_size_);

        page->next_ = free_lists_[list_index];

        if (page->next_)
        {
            page->next_->previous_ = page;
        }

        free_lists_[list_index] = page;
    }

    void LinearSegregatedFitAllocator::CheckPreconditions() const
    {
        // Each allocated block must be large enough to fit an address since free blocks form a linked list.
        SYNTROPY_ASSERT(class_size_ >= kMinimumAllocationSize);

        // The class size must be a power of 2.
        SYNTROPY_ASSERT(Math::IsPow2(class_size_));

        // Each page must be large enough to contain the header and at least one block as big as the maximum allocation, aligned to its own size.
        SYNTROPY_ASSERT(GetPageSize() <= sizeof(Page) + GetMaxAllocationSize() + GetMaxAllocationSize() - 1);
    }

    /************************************************************************/
    /* EXPONENTIAL SEGREGATED FIT ALLOCATOR                                 */
    /************************************************************************/

    ExponentialSegregatedFitAllocator::ExponentialSegregatedFitAllocator(const HashedString& name, size_t capacity, size_t base_allocation_size, size_t order)
        : Allocator(name)
        , base_allocation_size_(Memory::CeilToPageSize(base_allocation_size))           // Round to the next memory page boundary.
        , memory_pool_(capacity, base_allocation_size)                                  // Allocate a new virtual address range.
        , memory_range_(memory_pool_)                                                   // Get the full range out of the memory pool.
        , order_(order)
    {
        SYNTROPY_ASSERT(order >= 1);

        allocators_.reserve(order);

        capacity /= order;                                                              // Distribute the capacity evenly among the different classes.

        auto class_size = base_allocation_size_;

        while (order-- > 0)
        {
            allocators_.emplace_back(capacity, class_size);
            class_size <<= 1;                                                           // Double the allocation size for the next class.
        }
    }

    void* ExponentialSegregatedFitAllocator::Allocate(size_t size)
    {
        return GetAllocatorBySize(size).Allocate(size);
    }

    void* ExponentialSegregatedFitAllocator::Allocate(size_t size, size_t alignment)
    {
        // Allocate enough space for both the block and the maximum padding due to the requested alignment
        return Memory::Align(Allocate(size + alignment - 1), alignment);
    }

    void ExponentialSegregatedFitAllocator::Free(void* address)
    {
        // TODO: Use a single contiguous address range and find the proper allocator in O(1)

        auto it = std::find_if(allocators_.begin(),
            allocators_.end(),
            [address](const BlockAllocator& allocator)
        {
            return allocator.GetRange().Contains(address);
        });

        SYNTROPY_ASSERT(it != allocators_.end());

        it->Free(address);
    }

    bool ExponentialSegregatedFitAllocator::Belongs(void* block) const
    {
        return memory_range_.Contains(block);
    }

    size_t ExponentialSegregatedFitAllocator::GetMaxAllocationSize() const
    {
        return base_allocation_size_ * (1ull << (allocators_.size() - 1u));        // base * 2^(order-1)
    }

    void* ExponentialSegregatedFitAllocator::Reserve(size_t size)
    {
        return GetAllocatorBySize(size).Reserve();
    }

    void* ExponentialSegregatedFitAllocator::Reserve(size_t size, size_t alignment)
    {
        // Reserve enough space for both the block and the maximum padding due to the requested alignment
        return Memory::Align(Reserve(size + alignment - 1), alignment);
    }

    size_t ExponentialSegregatedFitAllocator::GetAllocationSize() const
    {
        return 0;       // Not yet implemented.
    }

    size_t ExponentialSegregatedFitAllocator::GetCommitSize() const
    {
        return 0;       // Not yet implemented.
    }

    const MemoryRange& ExponentialSegregatedFitAllocator::GetRange() const
    {
        return memory_range_;
    }

    BlockAllocator& ExponentialSegregatedFitAllocator::GetAllocatorBySize(size_t block_size)
    {
        auto index = Math::CeilLog2((block_size + base_allocation_size_ - 1) / base_allocation_size_);

        SYNTROPY_ASSERT(index < allocators_.size());

        return allocators_[index];
    }

    /************************************************************************/
    /* TWO LEVEL SEGREGATED FIT ALLOCATOR :: BLOCK HEADER                   */
    /************************************************************************/

    size_t TwoLevelSegregatedFitAllocator::BlockHeader::GetSize() const
    {
        return size_ & ~kSizeMask;
    }

    void TwoLevelSegregatedFitAllocator::BlockHeader::SetSize(size_t size)
    {
        SYNTROPY_ASSERT((size & kSizeMask) == 0)        // The new size must not interfere with the status bits at the end!.

        size_ = size | (size_ & kSizeMask);             // Preserve the status of the two flags at the end.
    }

    bool TwoLevelSegregatedFitAllocator::BlockHeader::IsBusy() const
    {
        return (size_ & kBusyBlockFlag) != 0;
    }

    void TwoLevelSegregatedFitAllocator::BlockHeader::SetBusy(bool is_busy)
    {
        if (is_busy)
        {
            size_ |= kBusyBlockFlag;
        }
        else
        {
            size_ &= ~kBusyBlockFlag;
        }
    }

    bool TwoLevelSegregatedFitAllocator::BlockHeader::IsLast() const
    {
        return (size_ & kLastBlockFlag) != 0;
    }

    void TwoLevelSegregatedFitAllocator::BlockHeader::SetLast(bool is_last)
    {
        if (is_last)
        {
            size_ |= kLastBlockFlag;
        }
        else
        {
            size_ &= ~kLastBlockFlag;
        }
    }

    void* TwoLevelSegregatedFitAllocator::BlockHeader::begin()
    {
        return Memory::AddOffset(this, sizeof(BlockHeader));
    }

    void* TwoLevelSegregatedFitAllocator::BlockHeader::end()
    {
        return Memory::AddOffset(this, GetSize());
    }

    /************************************************************************/
    /* TWO LEVEL SEGREGATED FIT ALLOCATOR :: FREE BLOCK HEADER              */
    /************************************************************************/

    void* TwoLevelSegregatedFitAllocator::FreeBlockHeader::begin()
    {
        return Memory::AddOffset(this, sizeof(FreeBlockHeader));
    }

    void* TwoLevelSegregatedFitAllocator::FreeBlockHeader::end()
    {
        return Memory::AddOffset(this, GetSize());
    }

    /************************************************************************/
    /* TWO LEVEL SEGREGATED FIT ALLOCATOR                                   */
    /************************************************************************/
    
    TwoLevelSegregatedFitAllocator::TwoLevelSegregatedFitAllocator(const HashedString& name, size_t capacity, size_t second_level_index)
        : Allocator(name)
        , pool_(capacity, 1)
        , last_block_(nullptr)
        , second_level_index_(second_level_index)
    {
        free_lists_.resize((Math::FloorLog2(pool_.GetRange().GetSize()) + 1u) * (1ull << second_level_index_));
    }

    TwoLevelSegregatedFitAllocator::TwoLevelSegregatedFitAllocator(const HashedString& name, const MemoryRange& memory_range, size_t second_level_index)
        : Allocator(name)
        , pool_(memory_range, 1)
        , last_block_(nullptr)
        , second_level_index_(second_level_index)
    {
        free_lists_.resize((Math::FloorLog2(pool_.GetRange().GetSize()) + 1u) * (1ull << second_level_index_));
    }

    void* TwoLevelSegregatedFitAllocator::Allocate(size_t size)
    {
        return GetFreeBlockBySize(size)->begin();
    }

    void* TwoLevelSegregatedFitAllocator::Allocate(size_t size, size_t alignment)
    {
        return GetFreeBlockBySize(size + alignment)->begin();
    }

    void TwoLevelSegregatedFitAllocator::Free(void* block)
    {
        auto free_block = reinterpret_cast<BlockHeader*>(Memory::SubOffset(block, sizeof(BlockHeader)));

        PushBlock(free_block);
    }

    bool TwoLevelSegregatedFitAllocator::Belongs(void* block) const
    {
        return pool_.GetRange().Contains(block);
    }

    size_t TwoLevelSegregatedFitAllocator::GetMaxAllocationSize() const
    {
        return pool_.GetRange().GetSize();
    }

    TwoLevelSegregatedFitAllocator::BlockHeader* TwoLevelSegregatedFitAllocator::GetFreeBlockBySize(size_t size)
    {
        size += sizeof(BlockHeader);                            // Reserve more space for the header.
        size = std::max(size, kMinimumBlockSize);               // The size must be at least as big as the minimum size allowed.
        size = Math::Ceil(size, BlockHeader::kSizeMask + 1);    // The size must not interfere with the status bits of the block.

        auto index = GetFreeListIndexBySize(size);

        // Search a free block big enough to handle the allocation.
        // TODO: Remove this loop, use bitmaps instead.
        while (index < free_lists_.size() && !free_lists_[index])
        {
            ++index;
        }

        BlockHeader* block;

        if (index < free_lists_.size())
        {
            // Pop a free block from the given segregated free list.
            block = PopBlock(index);

            SplitBlock(block, size);                    // Split the block (if needed).
        }
        else
        {
            // Get a new block from the pool
            block = reinterpret_cast<BlockHeader*>(pool_.Allocate(size));

            block->SetSize(size);
            block->SetBusy(true);
            block->SetLast(true);
            block->previous_ = last_block_;

            if (last_block_)
            {
                last_block_->SetLast(false);            // The last block is no longer the last
            }

            last_block_ = block;
        }

        MemoryDebug::MarkUninitialized(block->begin(), block->end());

        return block;
    }

    TwoLevelSegregatedFitAllocator::BlockHeader* TwoLevelSegregatedFitAllocator::PopBlock(size_t index)
    {
        auto block = free_lists_[index];

        if (block)
        {
            SYNTROPY_ASSERT(!block->IsBusy());

            // Promote the next free block as head of the list
            auto next_free = block->next_free_;

            free_lists_[index] = next_free;

            if (next_free)
            {
                next_free->previous_free_ = nullptr;
            }

            // Mark the popped block as "busy"
            block->SetBusy(true);
        }

        return reinterpret_cast<BlockHeader*>(block);
    }

    void TwoLevelSegregatedFitAllocator::PushBlock(BlockHeader* block)
    {
        auto merged_block = reinterpret_cast<FreeBlockHeader*>(block);

        auto previous_block = reinterpret_cast<FreeBlockHeader*>(block->previous_);
        auto next_block = reinterpret_cast<FreeBlockHeader*>(block->end());

        // Merge the block with the previous physical block

        if (previous_block && !previous_block->IsBusy())
        {
            RemoveBlock(previous_block);                                                // Remove the previous block from its segregated list.

            previous_block->SetSize(previous_block->GetSize() + block->GetSize());      // Grow the block size.
            previous_block->SetLast(block->IsLast());                                   // Merging with the 'last' block yields another 'last' block.

            merged_block = previous_block;                                              // The previous block and the original one are now merged
        }

        // Merge the block with the next physical block

        if (!block->IsLast() && !next_block->IsBusy())
        {
            RemoveBlock(next_block);                                                    // Remove the next block from its segregated list.

            block->SetSize(block->GetSize() + next_block->GetSize());                   // Grow the block size.
            block->SetLast(next_block->IsLast());                                       // Merging with the 'last' block yields another 'last' block.
        }

        if (merged_block->IsLast())
        {
            last_block_ = merged_block;                                                 // Update the pointer to last block.
        }
        else
        {
            next_block = reinterpret_cast<FreeBlockHeader*>(merged_block->end());
            next_block->previous_ = merged_block;                                       // The pointer to the previous physical block of the next block is no longer valid since that block may have been merged.
        }

        // Insert the merged block in the proper free list

        merged_block->SetBusy(false);                                                   // The block is no longer busy

        MemoryDebug::MarkFree(merged_block->begin(), merged_block->end());

        InsertBlock(merged_block);
    }

    void TwoLevelSegregatedFitAllocator::SplitBlock(BlockHeader* block, size_t size)
    {
        if (block->GetSize() > size + kMinimumBlockSize)                // Do not split if the remaining block size would fall below the minimum size allowed.
        {
            auto remaining_block = Memory::AddOffset(block, size);

            // Setup the new block
            remaining_block->previous_ = block;                         // Previous physical block
            remaining_block->SetBusy(false);                            // This block is free
            remaining_block->SetLast(block->IsLast());                  // This block is the last only if the original one was the last
            remaining_block->SetSize(block->GetSize() - size);          // Remaining size

            // Update the block info
            block->SetSize(size);                                       // Shrink the size
            block->SetLast(false);                                      // It can't be the last block anymore

            SYNTROPY_ASSERT(block->IsBusy());                           // Make sure the original block is busy, otherwise PushBlock will merge again block and remaining_block!

            PushBlock(remaining_block);
        }
    }

    void TwoLevelSegregatedFitAllocator::RemoveBlock(FreeBlockHeader* block)
    {
        // Fix-up the double linked list

        if (block->previous_free_)
        {
            block->previous_free_->next_free_ = block->next_free_;
        }
        else
        {
            // The block has no previous block: fix the head of the free list
            auto index = GetFreeListIndexBySize(block->GetSize());

            free_lists_[index] = block->next_free_;
        }

        if (block->next_free_)
        {
            block->next_free_->previous_free_ = block->previous_free_;
        }
    }

    void TwoLevelSegregatedFitAllocator::InsertBlock(FreeBlockHeader* block)
    {
        // Push on the head of the proper segregated list
        auto index = GetFreeListIndexBySize(block->GetSize());

        block->previous_free_ = nullptr;
        block->next_free_ = free_lists_[index];
        free_lists_[index] = block;
    }

    size_t TwoLevelSegregatedFitAllocator::GetFreeListIndexBySize(size_t size) const
    {
        auto first_level = Math::FloorLog2(size);

        auto second_level = (size ^ (1ull << first_level)) >> (first_level - second_level_index_);

        SYNTROPY_ASSERT(second_level < (1ull << second_level_index_));

        return first_level * (1ull << second_level_index_) + second_level;          // Flatten the index.
    }

}
