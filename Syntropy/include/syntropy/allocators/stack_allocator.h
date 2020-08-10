
/// \file stack_allocator.h
/// \brief This header is part of the Syntropy allocators module. It contains definitions for stack allocators (aka linear allocators).
///
/// \author Raffaele D. Facendola - 2017

#pragma once

#include "syntropy/math/math.h"
#include "syntropy/core/types.h"
#include "syntropy/memory/bytes.h"
#include "syntropy/memory/alignment.h"
#include "syntropy/memory/byte_span.h"
#include "syntropy/diagnostics/assert.h"

namespace syntropy
{
    /************************************************************************/
    /* STACK ALLOCATOR <ALLOCATOR>                                          */
    /************************************************************************/

    /// \brief Tier 1 allocator that use an underlying allocator to allocate over a contiguous range of memory addresses.
    /// Memory is allocated sequentially and divided into chunks. Pointer-level deallocation is not supported.
    /// When the current allocation chunk is exhausted a new chunk is requested from the underlying allocator automatically.
    /// \author Raffaele D. Facendola - January 2017, August 2018
    template <typename TAllocator>
    class StackAllocator
    {
    public:

        /// \brief A checkpoint used to restore the allocator status.
        class TCheckpoint;

        /// \brief Create a new allocator.
        /// \param granularity Allocation granularity for each allocated chunk.
        /// \param arguments Arguments used to construct the underlying allocator.
        template <typename... TArguments>
        StackAllocator(Bytes granularity, TArguments&&... arguments) noexcept;

        /// \brief No copy constructor.
        StackAllocator(const StackAllocator&) = delete;

        /// \brief Move constructor.
        StackAllocator(StackAllocator&& rhs) noexcept;

        /// \brief Destructor.
        ~StackAllocator();

        /// \brief Unified assignment operator.
        StackAllocator& operator=(StackAllocator rhs) noexcept;

        /// \brief Allocate a new memory block.
        /// If a memory block could not be allocated, returns an empty block.
        RWByteSpan Allocate(Bytes size, Alignment alignment) noexcept;

        /// \brief Deallocate a memory block.
        /// \remarks The behavior of this function is undefined unless the provided block was returned by a previous call to ::Allocate(size, alignment).
        void Deallocate(const RWByteSpan& block, Alignment alignment) noexcept;

        /// \brief Check whether a block belongs to the underlying allocator.
        /// This method only participates in overload resolution if the underlying allocator implements the ::Own(block) method.
        template<typename = EnableIfValidExpressionT<AllocatorImplementsOwn, TAllocator>>
        Bool Owns(const ByteSpan& block) const noexcept;

        /// \brief Deallocate every allocation performed on this allocator so far, invalidating all outstanding checkpoints.
        void DeallocateAll() noexcept;

        /// \brief Swap this allocator with the provided instance.
        void Swap(StackAllocator& rhs) noexcept;

        /// \brief Get the current state of the allocator.
        TCheckpoint Checkpoint() const noexcept;

        /// \brief Restore the allocator to a previous state.
        /// If the provided checkpoint wasn't obtained by means of ::Checkpoint(), the behavior of this method is undefined.
        /// This method invalidates all checkpoints obtained after the provided checkpoint. Rewinding to an invalid checkpoint results in undefined behavior.
        void Rewind(const TCheckpoint& checkpoint) noexcept;

    private:

        /// \brief A chunk in the allocation chain.
        struct Chunk;

        /// \brief Allocate a new memory block on the provided chunk.
        /// If the block could not be allocated, returns an empty block.
        RWByteSpan Allocate(Pointer<Chunk> chunk, Bytes size, Alignment alignment) const noexcept;

        /// \brief Allocate a new chunk whose payload size is at least as big as the provided size.
        Pointer<Chunk> AllocateChunk(Bytes size) const noexcept;

        /// \brief Underlying allocator.
        TAllocator allocator_;

        /// \brief Size of each chunk.
        Bytes granularity_;

        /// \brief Current active chunk.
        Pointer<Chunk> chunk_{ nullptr };

    };

    /************************************************************************/
    /* STACK ALLOCATOR <ALLOCATOR> :: CHECKPOINT                            */
    /************************************************************************/

    /// \brief Represents a checkpoint used to rewind a virtual stack allocator back to a previous state.
    template <typename TAllocator>
    class StackAllocator<TAllocator>::TCheckpoint
    {
        friend class StackAllocator<TAllocator>;

        /// \brief Chunk when the checkpoint was initially created.
        Pointer<Chunk> chunk_{ nullptr };

        /// \brief Free memory range when the checkpoint was initially created.
        RWByteSpan free_span_;
    };

    /************************************************************************/
    /* STACK ALLOCATOR <ALLOCATOR> :: CHUNK                                 */
    /************************************************************************/

    /// \brief A chunk in the allocation chain.
    template <typename TAllocator>
    struct StackAllocator<TAllocator>::Chunk
    {
        /// \brief Pointer to the previous chunk.
        Pointer<Chunk> previous_{ nullptr };

        /// \brief Memory range in this chunk.
        RWByteSpan chunk_span_;

        /// \brief Memory range containing allocated data.
        RWByteSpan payload_span_;

        /// \brief Free memory range, relative to the payload span.
        RWByteSpan free_span_;
    };

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // StackAllocator<TAllocator>
    // ==========================

    template <typename TAllocator>
    template <typename... TArguments>
    inline StackAllocator<TAllocator>::StackAllocator(Bytes granularity, TArguments&&... arguments) noexcept
        : allocator_(std::forward<TArguments>(arguments)...)
        , granularity_(granularity)
    {

    }

    template <typename TAllocator>
    inline StackAllocator<TAllocator>::StackAllocator(StackAllocator&& rhs) noexcept
        : allocator_(std::move(rhs.allocator_))
        , granularity_(rhs.granularity_)
        , chunk_(rhs.chunk_)
    {
        rhs.chunk_ = nullptr;
    }

    template <typename TAllocator>
    inline StackAllocator<TAllocator>::~StackAllocator()
    {
        DeallocateAll();
    }

    template <typename TAllocator>
    inline StackAllocator<TAllocator>& StackAllocator<TAllocator>::operator=(StackAllocator rhs) noexcept
    {
        rhs.Swap(*this);

        return *this;
    }

    template <typename TAllocator>
    RWByteSpan StackAllocator<TAllocator>::Allocate(Bytes size, Alignment alignment) noexcept
    {
        // Allocate on the current chunk. Fast-path.

        if (auto block = Allocate(chunk_, size, alignment))
        {
            return block;
        }
        
        // Allocate on a new chunk.

        if(auto chunk = AllocateChunk(size + ToBytes(alignment) - ToBytes(1)))
        {
            chunk->previous_ = chunk_;
            chunk_ = chunk;

            return Allocate(chunk_, size, alignment);
        }

        // Out-of-memory.

        return {};
    }

    template <typename TAllocator>
    inline void StackAllocator<TAllocator>::Deallocate(const RWByteSpan& block, Alignment /*alignment*/) noexcept
    {
        SYNTROPY_UNDEFINED_BEHAVIOR(Owns(block), "The provided block doesn't belong to this allocator instance.");
    }

    template <typename TAllocator>
    template <typename>
    inline Bool StackAllocator<TAllocator>::Owns(const ByteSpan& block) const noexcept
    {
        for (auto chunk = chunk_; chunk; chunk = chunk->previous_)
        {
            if (chunk->payload_span_.Contains(block))
            {
                return true;
            }
        }

        return false;
    }

    template <typename TAllocator>
    inline void StackAllocator<TAllocator>::DeallocateAll() noexcept
    {
        for (; chunk_ != nullptr;)
        {
            auto previous = chunk_->previous_;

            allocator_.Deallocate(chunk_->chunk_span_);

            chunk_ = previous;
        }
    }

    template <typename TAllocator>
    inline void StackAllocator<TAllocator>::Swap(StackAllocator& rhs) noexcept
    {
        using std::swap;

        swap(allocator_, rhs.allocator_);
        swap(granularity_, rhs.granularity_);
        swap(chunk_, rhs.chunk_);
        swap(head_, rhs.head_);
    }

    template <typename TAllocator>
    inline StackAllocator<TAllocator>::TCheckpoint StackAllocator<TAllocator>::Checkpoint() const
    {
        auto checkpoint = TCheckpoint{};

        checkpoint.chunk_ = chunk_;
        checkpoint.free_span_ = chunk_ ? (chunk_->free_span_) : nullptr;

        return checkpoint;
    }

    template <typename TAllocator>
    void StackAllocator<TAllocator>::Rewind(const TCheckpoint& checkpoint)
    {
        // Deallocate chunks until the checkpoint chunk becomes the active one.

        for (; chunk_ != checkpoint_.chunk_;)
        {
            auto previous = chunk_->previous_;
             
            allocator_.Deallocate(chunk_->chunk_span_);

            chunk_ = previous;
        }

        // Rewind the chunk status.

        if (chunk_)
        {
            chunk_->free_span_ = checkpoint.free_span_;
        }
    }

    template <typename TAllocator>
    RWByteSpan StackAllocator<TAllocator>::Allocate(Pointer<Chunk> chunk, Bytes size, Alignment alignment) const noexcept
    {
        if (chunk)
        {
            if (auto chunk_span = Memory::Align(chunk->free_span_, alignment); Memory::Size(chunk_span) >= size)
            {
                auto [block, free_span] = SliceFront(chunk_span, ToInt(size));

                chunk->free_span_ = free_span;

                return block;
            }
        }

        return {};
    }

    template <typename TAllocator>
    Pointer<typename StackAllocator<TAllocator>::Chunk> StackAllocator<TAllocator>::AllocateChunk(Bytes size) const noexcept
    {
        // Reserve additional space for chunk header.

        auto chunk_size = Math::Ceil(size + SizeOf<Chunk>(), granularity_);

        if (auto chunk_span = allocator_.Allocate(chunk_size))
        {
            auto chunk = FromTypeless<Chunk>(Begin(block));

            chunk->previous_ = nullptr;
            chunk->chunk_span_ = chunk_span;
            chunk->payload_span_ = PopFront(chunk_span, ToInt(SizeOf<Chunk>()));
            chunk->free_span_ = chunk->payload_span_;

            return chunk;
        }

        return nullptr;
    }

}
