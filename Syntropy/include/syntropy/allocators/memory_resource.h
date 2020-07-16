
/// \file memory_resource.h
/// \brief This header is part of the Syntropy allocators module. It contains definitions and interfaces for memory resource.
///
/// \author Raffaele D. Facendola - 2020

#pragma once

#include "syntropy/syntropy.h"
#include "syntropy/core/types.h"
#include "syntropy/memory/bytes.h"
#include "syntropy/memory/alignment.h"
#include "syntropy/memory/byte_span.h"
#include "syntropy/allocators/system_memory_resource.h"

namespace syntropy
{
    /************************************************************************/
    /* NON-MEMBER FUNCTIONS                                                 */
    /************************************************************************/

    class MemoryResource;

    /// \brief Get a memory resource that uses the global operator new and operator delete to allocate memory.
    MemoryResource& GetSystemMemoryResource() noexcept;

    /// \brief Get the thread-local default memory resource.
    /// \remarks The local default memory resource is used by certain facilities when an explicit memory resource is not supplied.
    MemoryResource& GetDefaultMemoryResource() noexcept;

    /// \brief Set the thread-local default memory resource.
    /// \return Returns the previous value of the local default memory resource.
    /// \remarks The local default memory resource is used by certain facilities when an explicit memory resource is not supplied.
    MemoryResource& SetDefaultMemoryResource(MemoryResource& memory_resource) noexcept;

    /************************************************************************/
    /* MEMORY RESOURCE                                                      */
    /************************************************************************/

    /// \brief Represents an abstract interface to an unbounded set of classes encapsulating memory resources.
    /// This class mimics and replaces standard's std::memory_resource. Standard's global default memory resoures are deemed
    /// to be harmful since it may cause non-coherent allocation behavior in the same scope.
    /// \author Raffaele D. Facendola - April 2020.
    class MemoryResource
    {
        friend MemoryResource& GetSystemMemoryResource() noexcept;
        friend MemoryResource& GetDefaultMemoryResource() noexcept;
        friend MemoryResource& SetDefaultMemoryResource(MemoryResource& memory_resource) noexcept;

    public:

        /// \brief Default virtual destructor.
        virtual ~MemoryResource() = default;

        /// \brief Allocate a new memory block.
        /// The returned storage, if any, is aligned to a specified amount if supported or to MaxAlignmentOf otherwise.
        /// \param size Size of the memory block to allocate.
        /// \param alignment Block alignment.
        /// \return Returns a range representing the requested aligned memory block. If no allocation could be performed returns an empty range.
        virtual RWByteSpan Allocate(Bytes size, Alignment alignment = MaxAlignmentOf()) noexcept = 0;

        /// \brief Deallocate a memory block.
        /// \param block Block to deallocate. Must refer to any allocation performed via Allocate(size, alignment).
        /// \param alignment Block alignment.
        /// \remarks The behavior of this function is undefined unless the provided block was returned by a previous call to Allocate(size, alignment).
        virtual void Deallocate(const RWByteSpan& block, Alignment alignment = MaxAlignmentOf()) = 0;

        /// \brief Check whether this memory resource owns the provided memory block.
        /// \param block Block to check the ownership of.
        /// \return Returns true if the provided memory range was allocated by this memory resource, returns false otherwise.
        virtual Bool Owns(const ByteSpan& block) const noexcept = 0;

    private:

        /// \brief Get the active memory resource in the current scope.
        static MemoryResource*& GetScopeMemoryResource() noexcept;

    };

    /************************************************************************/
    /* MEMORY RESOURCE T <TMEMORY RESOURCE>                                 */
    /************************************************************************/

    /// \brief Tier Omega memory resource used to forward calls to an underlying, type-erased, memory resource.
    /// \author Raffaele D. Facendola - April 2020
    template <typename TMemoryResource>
    class MemoryResourceT : public MemoryResource
    {
    public:

        /// \brief Create a new memory resource.
        template <typename... TArguments>
        MemoryResourceT(TArguments&&... arguments) noexcept;

        /// \brief Default destructor.
        virtual ~MemoryResourceT() = default;

        virtual RWByteSpan Allocate(Bytes size, Alignment alignment = MaxAlignmentOf()) noexcept override;

        virtual void Deallocate(const RWByteSpan& block, Alignment alignment = MaxAlignmentOf()) override;

        virtual Bool Owns(const ByteSpan& block) const noexcept override;

        /// \brief Get the underlying memory resource.
        TMemoryResource& GetMemoryResource();

        /// \brief Get the underlying memory resource.
        const TMemoryResource& GetMemoryResource() const;

    private:

        /// \brief Underlying memory resource.
        TMemoryResource memory_resource_;

    };

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Non-member functions.

    inline MemoryResource& GetSystemMemoryResource() noexcept
    {
        static auto system_memory_resource = MemoryResourceT<SystemMemoryResource>{};

        return system_memory_resource;
    }

    inline MemoryResource& GetDefaultMemoryResource() noexcept
    {
        return *MemoryResource::GetScopeMemoryResource();
    }

    inline MemoryResource& SetDefaultMemoryResource(MemoryResource& memory_resource) noexcept
    {
        auto& previous_memory_resource = GetDefaultMemoryResource();

        MemoryResource::GetScopeMemoryResource() = &memory_resource;

        return previous_memory_resource;
    }

    // MemoryResource.

    inline MemoryResource*& MemoryResource::GetScopeMemoryResource() noexcept
    {
        static thread_local MemoryResource* default_memory_resource_ = &GetSystemMemoryResource();

        return default_memory_resource_;
    }

    // MemoryResourceT<TMemoryResource>.

    template <typename TMemoryResource>
    template <typename... TArguments>
    inline MemoryResourceT<TMemoryResource>::MemoryResourceT(TArguments&&... arguments) noexcept
        : memory_resource_(std::forward<TArguments>(arguments)...)
    {

    }

    template <typename TMemoryResource>
    inline RWByteSpan MemoryResourceT<TMemoryResource>::Allocate(Bytes size, Alignment alignment) noexcept
    {
        return memory_resource_.Allocate(size, alignment);
    }

    template <typename TMemoryResource>
    inline void MemoryResourceT<TMemoryResource>::Deallocate(const RWByteSpan& block, Alignment alignment)
    {
        memory_resource_.Deallocate(block, alignment);
    }

    template <typename TMemoryResource>
    inline Bool MemoryResourceT<TMemoryResource>::Owns(const ByteSpan& block) const noexcept
    {
        return memory_resource_.Owns(block);
    }

    template <typename TMemoryResource>
    inline TMemoryResource& MemoryResourceT<TMemoryResource>::GetMemoryResource()
    {
        return memory_resource_;
    }

    template <typename TMemoryResource>
    inline const TMemoryResource& MemoryResourceT<TMemoryResource>::GetMemoryResource() const
    {
        return memory_resource_;
    }

}
