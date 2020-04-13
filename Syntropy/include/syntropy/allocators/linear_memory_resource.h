
/// \file linear_memory_resource.h
/// \brief This header is part of the syntropy memory management system. It contains linear memory resources.
///
/// \author Raffaele D. Facendola - 2017

#pragma once

#include <algorithm>

#include "syntropy/memory/bytes.h"
#include "syntropy/memory/alignment.h"
#include "syntropy/memory/memory_address.h"
#include "syntropy/memory/memory_range.h"

#include "syntropy/diagnostics/assert.h"

namespace syntropy
{
    /************************************************************************/
    /* LINEAR MEMORY RESOURCE <TMEMORY RESOURCE>                            */
    /************************************************************************/

    /// \brief Tier 1 memory resource that use an underlying memory resource to allocate over a contiguous range of memory addresses.
    /// Memory is allocated sequentially and divided into chunks. Pointer-level deallocation is not supported.
    /// When the current allocation chunk is exhausted a new chunk is requested from the underlying memory resource automatically.
    /// \author Raffaele D. Facendola - January 2017, August 2018
    template <typename TMemoryResource>
    class LinearMemoryResource
    {
    public:

        /// \brief Create a new memory resource.
        /// \param chunk_size Allocation granularity for each allocated chunk.
        template <typename... TArguments>
        LinearMemoryResource(Bytes chunk_size, TArguments&&... arguments) noexcept;

        /// \brief No copy constructor.
        LinearMemoryResource(const LinearMemoryResource&) = delete;

        /// \brief Move constructor.
        LinearMemoryResource(LinearMemoryResource&& rhs) noexcept;

        /// \brief Destructor.
        ~LinearMemoryResource();

        /// \brief Unified assignment operator.
        LinearMemoryResource& operator=(LinearMemoryResource rhs) noexcept;

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
        /// Pointer-level deallocations are not supported, therefore this method does nothing.
        void Deallocate(const MemoryRange& block) noexcept;

        /// \brief Deallocate an aligned memory block.
        /// Pointer-level deallocations are not supported, therefore this method does nothing.
        void Deallocate(const MemoryRange& block, Alignment alignment) noexcept;

        /// \brief Deallocate every allocation performed so far.
        void DeallocateAll() noexcept;

        /// \brief Check whether this memory resource owns the provided memory block.
        /// \param block Block to check the ownership of.
        /// \return Returns true if the provided memory range was allocated by this memory resource, returns false otherwise.
        bool Owns(const MemoryRange& block) const noexcept;

        /// \brief Get the maximum allocation size that can be handled by this memory resource.
        /// The returned value shall not be used to determine whether a call to "Allocate" will fail.
        /// \return Returns the maximum allocation size that can be handled by this memory resource.
        Bytes GetMaxAllocationSize() const noexcept;

        /// \brief Swap this memory resource with the provided instance.
        void Swap(LinearMemoryResource& rhs) noexcept;

    private:

        /// \brief A chunk in the allocation chain.
        struct Chunk
        {
            /// \brief Pointer to the previous chunk.
            Chunk* previous_;

            /// \brief Pointer past the last allocable address in the chunk.
            MemoryAddress end_;
        };

        /// \brief Underlying memory resource.
        TMemoryResource memory_resource_;

        /// \brief Size of each chunk in the allocation chain.
        Bytes chunk_size_;

        /// \brief Pointer past the last allocated address in the active chunk.
        MemoryAddress head_;

        /// \brief Current active chunk.
        Chunk* chunk_{ nullptr };

    };

    /// \brief Swaps two SequentialMemoryResource.
    template <typename TMemoryResource>
    void swap(syntropy::LinearMemoryResource<TMemoryResource>& lhs, syntropy::LinearMemoryResource<TMemoryResource>& rhs) noexcept;
 
    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // LinearMemoryResource<TMemoryResource>.

    template <typename TMemoryResource>
    template <typename... TArguments>
    inline LinearMemoryResource<TMemoryResource>::LinearMemoryResource(Bytes chunk_size, TArguments&&... arguments) noexcept
        : memory_resource_(std::forward<TArguments>(arguments)...)
        , chunk_size_(chunk_size)
    {

    }

    template <typename TMemoryResource>
    inline LinearMemoryResource<TMemoryResource>::LinearMemoryResource(LinearMemoryResource&& rhs) noexcept
        : memory_resource_(std::move(rhs.memory_resource_))
        , chunk_size_(rhs.chunk_size_)
        , chunk_(rhs.chunk_)
        , head_(rhs.head_)
    {
        rhs.chunk_ = nullptr;
    }

    template <typename TMemoryResource>
    inline LinearMemoryResource<TMemoryResource>::~LinearMemoryResource()
    {
        DeallocateAll();
    }

    template <typename TMemoryResource>
    inline LinearMemoryResource<TMemoryResource>& LinearMemoryResource<TMemoryResource>::operator=(LinearMemoryResource rhs) noexcept
    {
        rhs.Swap(*this);
        return *this;
    }

    template <typename TMemoryResource>
    inline MemoryRange LinearMemoryResource<TMemoryResource>::Allocate(Bytes size) noexcept
    {
        return Allocate(size, Alignment());
    }

    template <typename TMemoryResource>
    MemoryRange LinearMemoryResource<TMemoryResource>::Allocate(Bytes size, Alignment alignment) noexcept
    {
        // Attempt to allocate on the current chunk. Fast-path.

        {
            auto head = head_.GetAligned(alignment);
            auto block = MemoryRange{ head, size };

            if (chunk_ && (block.End() <= chunk_->end_))
            {
                head_ = block.End();

                return block;
            }
        }

        // Allocate a new chunk accounting for Chunk header and alignment requirements. Performance cost depends on the underlying memory resource.

        auto chunk_size = Ceil(BytesOf<Chunk>() + size + alignment - 1_Bytes, chunk_size_);

        if (auto block = memory_resource_.Allocate(chunk_size))
        {
            // Link to the previous chunk.

            auto chunk = block.Begin().As<Chunk>();

            chunk->previous_ = chunk_;
            chunk->end_ = block.End();

            chunk_ = chunk;

            // Allocate the block from the new chunk.

            auto head = (block.Begin() + BytesOf<Chunk>()).GetAligned(alignment);

            head_ = head + size;

            return { head, head_ };
        }

        return {};              // Out-of-memory.
    }

    template <typename TMemoryResource>
    inline void LinearMemoryResource<TMemoryResource>::Deallocate(const MemoryRange& block) noexcept
    {
        SYNTROPY_ASSERT(Owns(block));
    }

    template <typename TMemoryResource>
    inline void LinearMemoryResource<TMemoryResource>::Deallocate(const MemoryRange& block, Alignment /*alignment*/) noexcept
    {
        SYNTROPY_ASSERT(Owns(block));
    }

    template <typename TMemoryResource>
    inline void LinearMemoryResource<TMemoryResource>::DeallocateAll() noexcept
    {
        for (; chunk_ != nullptr;)
        {
            auto previous = chunk_->previous_;

            memory_resource_.Deallocate({ chunk_, chunk_->end_ });

            chunk_ = previous;
        }

        head_ = nullptr;
    }

    template <typename TMemoryResource>
    inline bool LinearMemoryResource<TMemoryResource>::Owns(const MemoryRange& block) const noexcept
    {
        // Can't query the underlying memory resource directly since it might be shared with other allocators.

        for (auto chunk = chunk_; chunk; chunk = chunk->previous_)
        {
            if (MemoryRange{ chunk, chunk->end_ }.Contains(block))
            {
                return true;
            }
        }

        return false;
    }

    template <typename TMemoryResource>
    inline Bytes LinearMemoryResource<TMemoryResource>::GetMaxAllocationSize() const noexcept
    {
        return memory_resource_.GetMaxAllocationSize();
    }

    template <typename TMemoryResource>
    inline void LinearMemoryResource<TMemoryResource>::Swap(LinearMemoryResource& rhs) noexcept
    {
        using std::swap;

        swap(memory_resource_, rhs.memory_resource_);
        swap(chunk_size_, rhs.chunk_size_);
        swap(chunk_, rhs.chunk_);
        swap(head_, rhs.head_);
    }

    template <typename TMemoryResource>
    inline void swap(syntropy::LinearMemoryResource<TMemoryResource>& lhs, syntropy::LinearMemoryResource<TMemoryResource>& rhs) noexcept
    {
        lhs.Swap(rhs);
    }

}