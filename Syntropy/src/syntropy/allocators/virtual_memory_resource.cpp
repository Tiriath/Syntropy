#include "syntropy/allocators/virtual_memory_resource.h"

namespace syntropy
{
    /************************************************************************/
    /* VIRTUAL MEMORY RESOURCE :: FREE LIST                                 */
    /************************************************************************/

    struct VirtualMemoryResource::FreeList
    {
        /// \brief Next free list.
        Pointer<FreeList> next_{ nullptr };

        /// \brief Available memory in the free list.
        RWSpan<RWBytePtr> span_;

        /// \brief Unallocated space in the free list.
        RWSpan<RWBytePtr> unallocated_;
    };

    /************************************************************************/
    /* VIRTUAL MEMORY RESOURCE                                              */
    /************************************************************************/

    RWByteSpan VirtualMemoryResource::Allocate(Bytes size, Alignment alignment) noexcept
    {
        if ((size <= page_size_) && (alignment <= page_alignment_))
        {
            if (auto block = Allocate())
            {
                VirtualMemory::Commit(block);           // Kernel call: commit the entire block.

                return Front(block, ToInt(size));
            }
        }

        return {};
    }

    void VirtualMemoryResource::Deallocate(const RWByteSpan& block, Alignment alignment)
    {
        SYNTROPY_ASSERT(Owns(block));
        SYNTROPY_ASSERT(alignment <= page_alignment_);

        if (!free_ || IsEmpty(free_->unallocated_))
        {
            // The current free list is full: the block is recycled as a new free list linked to the current one.

            auto next_free = free_;

            free_ = reinterpret_cast<Pointer<FreeList>>(block.GetData());

            free_->next_ = next_free;
            free_->span_ = ToRWSpan<RWBytePtr>(PopFront(block, ToInt(SizeOf<FreeList>())));
            free_->unallocated_ = free_->span_;
        }
        else
        {
            // Append to the existing free list.

            Front(free_->unallocated_) = block.GetData();

            free_->unallocated_ = PopFront(free_->unallocated_);

            // Kernel call: decommit the entire block.

            auto virtual_block = RWByteSpan{ block.GetData(), ToInt(page_size_) };

            VirtualMemory::Decommit(virtual_block);
        }
    }

    void VirtualMemoryResource::Swap(VirtualMemoryResource& rhs) noexcept
    {
        using std::swap;

        swap(virtual_memory_, rhs.virtual_memory_);
        swap(unallocated_, rhs.unallocated_);
        swap(page_size_, rhs.page_size_);
        swap(page_alignment_, rhs.page_alignment_);
        swap(free_, rhs.free_);
    }

    RWByteSpan VirtualMemoryResource::Allocate()
    {
        // Attempt to recycle a free block from the current free list. The last block causes the list itself to be recycled.

        if (free_)
        {
            if (free_->unallocated_.GetCount() != free_->span_.GetCount())
            {
                auto free_blocks = DifferenceFront(free_->span_, free_->unallocated_);

                auto block = RWByteSpan{ Back(free_blocks), ToInt(page_size_) };

                free_->unallocated_ = Intersection(free_->span_, PopBack(free_blocks));

                return block;
            }
            else
            {
                auto block = RWByteSpan{ reinterpret_cast<RWBytePtr>(&free_), ToInt(page_size_) };

                free_ = free_->next_;

                return block;
            }
        }

        // Allocate directly from the underlying virtual memory range.

        if (Size(unallocated_) <= page_size_)
        {
            auto block = Front(unallocated_, ToInt(page_size_));

            unallocated_ = PopFront(unallocated_, ToInt(page_size_));

            return block;
        }

        return {};           // Out of memory!
    }
}