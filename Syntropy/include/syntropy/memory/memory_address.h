
/// \file memory_address.h
/// \brief This header is part of the syntropy memory management system. It contains the definition of the address type and related functions.
///
/// \author Raffaele D. Facendola - August 2018

#pragma once

#include <cstdint>
#include <type_traits>

#include "syntropy/diagnostics/assert.h"
#include "syntropy/memory/bytes.h"
#include "syntropy/memory/alignment.h"

namespace syntropy
{
    /************************************************************************/
    /* MEMORY ADDRESS                                                       */
    /************************************************************************/

    /// \brief Represents an address.
    /// This type is meant to be a strongly-typed replacement for void*, uintptr_t and intptr_t.
    /// \author Raffaele D. Facendola - August 2018
    template <bool is_const>
    class MemoryAddressT
    {

    public:

        /// \brief Type alias for the underlying pointer type.
        using Pointer = std::conditional_t<is_const, const void*, void *>;

        /// \brief Create a en empty address.
        constexpr MemoryAddressT() = default;

        /// \brief Create a new address.
        constexpr MemoryAddressT(Pointer address);

        /// \brief Create a en empty address.
        constexpr MemoryAddressT(std::nullptr_t);

        /// \brief Create a new address.
        constexpr MemoryAddressT(uintptr_t address);

        /// \brief Create a new address.
        constexpr MemoryAddressT(intptr_t address);

        /// \brief Default copy-constructor.
        template <bool is_rhs_const>
        constexpr MemoryAddressT(const MemoryAddressT<is_rhs_const>& rhs);

        /// \brief Default assignment operator.
        MemoryAddressT& operator=(const MemoryAddressT&) = default;

        /// \brief Get the underlying pointer.
        /// \return Returns the underlying pointer.
        constexpr operator Pointer() const noexcept;

        /// \brief Convert the address to an unsigned numeric value.
        /// \return Returns an unsigned number that represents the underlying address.
        constexpr operator uintptr_t() const noexcept;

        /// \brief Convert the address to an signed numeric value.
        /// \return Returns an signed number that represents the underlying address.
        constexpr operator intptr_t() const noexcept;

        /// \brief Check whether the memory address points to a valid address or to nullptr.
        /// \return Returns true if the stored address is nullptr, returns false otherwise.
        constexpr operator bool() const noexcept;

        /// \brief Get the underlying pointer.
        /// \return Returns the underlying pointer.
        constexpr Pointer operator*() const noexcept;

        /// \brief Get the underlying strongly-type pointer.
        /// \return Returns the underlying strongly-type pointer.
        template <typename TType>
        constexpr std::conditional_t<is_const, const TType*, TType*> As() const noexcept;

        /// \brief Move the address forward.
        /// \return Returns a reference to this element.
        constexpr MemoryAddressT& operator+=(const Bytes& rhs) noexcept;

        /// \brief Move the address backwards.
        /// \return Returns a reference to this element.
        constexpr MemoryAddressT& operator-=(const Bytes& rhs) noexcept;

        /// \brief Check whether this address is aligned to the provided amount.
        /// \param Returns true if the address is aligned to alignment, returns false otherwise.
        constexpr bool IsAlignedTo(Alignment alignment) const noexcept;

        /// \brief Align this address up to the provided alignment.
        /// \return Returns a new address which is guaranteed to be aligned to alignment and equal or greater than this address.
        constexpr MemoryAddressT GetAligned(Alignment alignment) const noexcept;

        /// \brief Align this address down to the provided alignment.
        /// \return Returns a new address which is guaranteed to be aligned to alignment and equal or less than this address.
        constexpr MemoryAddressT GetAlignedDown(Alignment alignment) const noexcept;

    private:

        Pointer address_ = nullptr;             ///< \brief Underlying address.

    };

    /// \brief Type alias for non-const memory addresses.
    using MemoryAddress = MemoryAddressT<false>;

    /// \brief Type alias for const memory address.
    using ConstMemoryAddress = MemoryAddressT<true>;

    /// \brief Equality comparison for MemoryAddress.
    /// \return Returns true if lhs and rhs refer to the same address, returns false otherwise.
    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator==(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_lhs_const>& rhs) noexcept;

    /// \brief Inequality comparison for MemoryAddress.
    /// \return Returns true if lhs and rhs refer to different address, returns false otherwise.
    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator!=(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_lhs_const>& rhs) noexcept;

    /// \brief Greater-than comparison for MemoryAddress.
    /// \return Returns true if lhs refers to an address that is strictly greater than rhs, returns false otherwise.
    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator>(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_lhs_const>& rhs) noexcept;

    /// \brief Less-than comparison for MemoryAddress.
    /// \return Returns true if lhs refers to an address that is strictly less than rhs, returns false otherwise.
    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator<(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_lhs_const>& rhs) noexcept;

    /// \brief Greater-or-equal comparison for MemoryAddress.
    /// \return Returns true if lhs refers to an address that is equal or greater than rhs, returns false otherwise.
    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator>=(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_lhs_const>& rhs) noexcept;

    /// \brief Less-or-equal comparison for MemoryAddress.
    /// \return Returns true if lhs refers to an address that is equal or less than rhs, returns false otherwise.
    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator<=(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_lhs_const>& rhs) noexcept;

    /// \brief Move an address forward.
    /// \return Returns an address which is equal to lhs moved forward by rhs bytes.
    template <bool is_const>
    constexpr MemoryAddressT<is_const> operator+(const MemoryAddressT<is_const>& lhs, const Bytes& rhs) noexcept;

    /// \brief Move an address backward.
    /// \return Returns an address which is equal to lhs moved backward by rhs bytes.
    template <bool is_const>
    constexpr MemoryAddressT<is_const> operator-(const MemoryAddressT<is_const>& lhs, const Bytes& rhs) noexcept;

    /// \brief Get the difference of two memory addresses.
    /// \return Returns a the signed difference between lhs and rhs.
    template <bool is_lhs_const, bool is_rhs_const>
    constexpr ptrdiff_t operator-(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_rhs_const>& rhs) noexcept;

    /// \brief Stream insertion for MemoryAddress.
    template <bool is_const>
    std::ostream& operator<<(std::ostream& lhs, const MemoryAddressT<is_const>& rhs);

    /************************************************************************/
    /* MEMORY BIT ADDRESS                                                   */
    /************************************************************************/

    /// \brief Represents a bit address.
    /// This type is meant to be used when addressing non-byte aligned memory blocks.
    /// \author Raffaele D. Facendola - June 2019
    template <bool is_const>
    class MemoryBitAddressT
    {

    public:

        /// \brief Type alias for the underlying pointer type.
        using Pointer = std::conditional_t<is_const, const void*, void *>;

        /// \brief Create a en empty address.
        constexpr MemoryBitAddressT() = default;

        /// \brief Create a new address.
        /// \param address Base address.
        template <bool is_address_const>
        constexpr MemoryBitAddressT(const MemoryAddressT<is_address_const>& address);

        /// \brief Create a new address.
        /// \param address Base address.
        constexpr MemoryBitAddressT(Pointer address);

        /// \brief Create a new address.
        /// \param address Base address.
        /// \param offset Offset relative to the base address, in bits.
        template <bool is_address_const>
        constexpr MemoryBitAddressT(const MemoryAddressT<is_address_const>& address, Bits offset);

        /// \brief Create a new address.
        /// \param address Base address.
        /// \param offset Offset relative to the base address, in bits.
        constexpr MemoryBitAddressT(Pointer address, Bits offset);

        /// \brief Create a en empty address.
        constexpr MemoryBitAddressT(std::nullptr_t);

        /// \brief Default copy-constructor.
        template <bool is_rhs_const>
        constexpr MemoryBitAddressT(const MemoryBitAddressT<is_rhs_const>& rhs);

        /// \brief Default assignment operator.
        MemoryBitAddressT& operator=(const MemoryBitAddressT&) = default;

        /// \brief Get the memory base address, aligned down to byte boundary.
        constexpr const MemoryAddressT<is_const>& GetBaseAddress() const noexcept;

        /// \brief Get the bit offset, relative to the base address.
        constexpr Bits GetOffset() const noexcept;

        /// \brief Check whether the memory address points to a valid address or to nullptr.
        /// \return Returns true if the stored address is nullptr, returns false otherwise.
        constexpr operator bool() const noexcept;

        /// \brief Move the address forward.
        /// \return Returns a reference to this element.
        constexpr MemoryBitAddressT& operator+=(const Bits& rhs) noexcept;

        /// \brief Move the address backwards.
        /// \return Returns a reference to this element.
        constexpr MemoryBitAddressT& operator-=(const Bits& rhs) noexcept;

    private:

        /// \brief Underlying memory address.
        MemoryAddressT<is_const> address_;

        /// \brief Bit offset, relative to the address.
        Bits offset_;

    };

    /// \brief Type alias for non-const memory bit addresses.
    using MemoryBitAddress = MemoryBitAddressT<false>;

    /// \brief Type alias for const memory bit address.
    using ConstMemoryBitAddress = MemoryBitAddressT<true>;

    /// \brief Equality comparison for MemoryBitAddress.
    /// \return Returns true if lhs and rhs refer to the same address, returns false otherwise.
    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator==(const MemoryBitAddressT<is_lhs_const>& lhs, const MemoryBitAddressT<is_rhs_const>& rhs) noexcept;

    /// \brief Inequality comparison for MemoryBitAddress.
    /// \return Returns true if lhs and rhs refer to different address, returns false otherwise.
    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator!=(const MemoryBitAddressT<is_lhs_const>& lhs, const MemoryBitAddressT<is_rhs_const>& rhs) noexcept;

    /// \brief Greater-than comparison for MemoryBitAddress.
    /// \return Returns true if lhs refers to an address that is strictly greater than rhs, returns false otherwise.
    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator>(const MemoryBitAddressT<is_lhs_const>& lhs, const MemoryBitAddressT<is_rhs_const>& rhs) noexcept;

    /// \brief Less-than comparison for MemoryBitAddress.
    /// \return Returns true if lhs refers to an address that is strictly less than rhs, returns false otherwise.
    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator<(const MemoryBitAddressT<is_lhs_const>& lhs, const MemoryBitAddressT<is_rhs_const>& rhs) noexcept;

    /// \brief Greater-or-equal comparison for MemoryBitAddress.
    /// \return Returns true if lhs refers to an address that is equal or greater than rhs, returns false otherwise.
    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator>=(const MemoryBitAddressT<is_lhs_const>& lhs, const MemoryBitAddressT<is_rhs_const>& rhs) noexcept;

    /// \brief Less-or-equal comparison for MemoryBitAddress.
    /// \return Returns true if lhs refers to an address that is equal or less than rhs, returns false otherwise.
    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator<=(const MemoryBitAddressT<is_lhs_const>& lhs, const MemoryBitAddressT<is_rhs_const>& rhs) noexcept;

    /// \brief Move a bit address forward.
    /// \return Returns a bit address which is equal to lhs moved forward by rhs bits.
    template <bool is_const>
    constexpr MemoryBitAddressT<is_const> operator+(const MemoryBitAddressT<is_const>& lhs, const Bits& rhs) noexcept;

    /// \brief Move a bit address backward.
    /// \return Returns a bit address which is equal to lhs moved backward by rhs bits.
    template <bool is_const>
    constexpr MemoryBitAddressT<is_const> operator-(const MemoryBitAddressT<is_const>& lhs, const Bits& rhs) noexcept;

    /// \brief Stream insertion for MemoryBitAddress.
    template <bool is_const>
    std::ostream& operator<<(std::ostream& lhs, const MemoryBitAddressT<is_const>& rhs);

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Memory address.

    template <bool is_const>
    constexpr MemoryAddressT<is_const>::MemoryAddressT(MemoryAddressT::Pointer address)
        : address_(address)
    {

    }

    template <bool is_const>
    constexpr MemoryAddressT<is_const>::MemoryAddressT(std::nullptr_t)
        : MemoryAddressT()
    {

    }

    template <bool is_const>
    constexpr MemoryAddressT<is_const>::MemoryAddressT(uintptr_t address)
        : address_(reinterpret_cast<Pointer>(address))
    {

    }

    template <bool is_const>
    constexpr MemoryAddressT<is_const>::MemoryAddressT(intptr_t address)
        : address_(reinterpret_cast<Pointer>(address))
    {

    }

    template <bool is_const>
    template <bool is_rhs_const>
    constexpr MemoryAddressT<is_const>::MemoryAddressT(const MemoryAddressT<is_rhs_const>& rhs)
        : address_(*rhs)
    {

    }

    template <bool is_const>
    constexpr MemoryAddressT<is_const>::operator typename MemoryAddressT<is_const>::Pointer() const noexcept
    {
        return address_;
    }

    template <bool is_const>
    constexpr MemoryAddressT<is_const>::operator uintptr_t() const noexcept
    {
        return reinterpret_cast<uintptr_t>(address_);
    }

    template <bool is_const>
    constexpr MemoryAddressT<is_const>::operator intptr_t() const noexcept
    {
        return reinterpret_cast<intptr_t>(address_);
    }

    template <bool is_const>
    constexpr MemoryAddressT<is_const>::operator bool() const noexcept
    {
        return !!address_;
    }

    template <bool is_const>
    constexpr typename MemoryAddressT<is_const>::Pointer MemoryAddressT<is_const>::operator*() const noexcept
    {
        return address_;
    }

    template <bool is_const>
    template <typename TType>
    constexpr std::conditional_t<is_const, const TType*, TType*> MemoryAddressT<is_const>::As() const noexcept
    {
        return reinterpret_cast<std::conditional_t<is_const, const TType*, TType*>>(address_);
    }

    template <bool is_const>
    constexpr MemoryAddressT<is_const>& MemoryAddressT<is_const>::operator+=(const Bytes& rhs) noexcept
    {
        address_ = reinterpret_cast<std::conditional_t<is_const, const int8_t*, int8_t*>>(address_) + std::size_t(rhs);
        return *this;
    }

    template <bool is_const>
    constexpr MemoryAddressT<is_const>& MemoryAddressT<is_const>::operator-=(const Bytes& rhs) noexcept
    {
        address_ = reinterpret_cast<std::conditional_t<is_const, const int8_t*, int8_t*>>(address_) - std::size_t(rhs);
        return *this;
    }

    template <bool is_const>
    constexpr bool MemoryAddressT<is_const>::IsAlignedTo(Alignment alignment) const noexcept
    {
        return (uintptr_t(*this) & (std::size_t(alignment) - 1u)) == 0u;
    }

    template <bool is_const>
    constexpr MemoryAddressT<is_const> MemoryAddressT<is_const>::GetAligned(Alignment alignment) const noexcept
    {
        auto alignment_mask = std::size_t(alignment) - 1u;
        return (uintptr_t(*this) + alignment_mask) & ~alignment_mask;
    }

    template <bool is_const>
    constexpr MemoryAddressT<is_const> MemoryAddressT<is_const>::GetAlignedDown(Alignment alignment) const noexcept
    {
        auto alignment_mask = std::size_t(alignment) - 1u;

        return uintptr_t(*this) & ~alignment_mask;
    }

    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator==(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_rhs_const>& rhs) noexcept
    {
        return uintptr_t(lhs) == uintptr_t(rhs);
    }

    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator!=(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_rhs_const>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator>(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_rhs_const>& rhs) noexcept
    {
        return uintptr_t(lhs) > uintptr_t(rhs);
    }

    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator<(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_rhs_const>& rhs) noexcept
    {
        return uintptr_t(lhs) < uintptr_t(rhs);
    }

    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator>=(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_rhs_const>& rhs) noexcept
    {
        return uintptr_t(lhs) >= uintptr_t(rhs);
    }

    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator<=(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_rhs_const>& rhs) noexcept
    {
        return uintptr_t(lhs) <= uintptr_t(rhs);
    }

    template <bool is_const>
    constexpr MemoryAddressT<is_const> operator+(const MemoryAddressT<is_const>& lhs, const Bytes& rhs) noexcept
    {
        return MemoryAddressT<is_const>(lhs) += rhs;
    }

    template <bool is_const>
    constexpr MemoryAddressT<is_const> operator-(const MemoryAddressT<is_const>& lhs, const Bytes& rhs) noexcept
    {
        return MemoryAddressT<is_const>(lhs) -= rhs;
    }

    template <bool is_lhs_const, bool is_rhs_const>
    constexpr ptrdiff_t operator-(const MemoryAddressT<is_lhs_const>& lhs, const MemoryAddressT<is_rhs_const>& rhs) noexcept
    {
        return intptr_t(lhs) - intptr_t(rhs);
    }

    template <bool is_const>
    inline std::ostream& operator<<(std::ostream& lhs, const MemoryAddressT<is_const>& rhs)
    {
        return lhs << uintptr_t(rhs);
    }

    // MemoryBitAddress.

    template <bool is_const>
    template <bool is_address_const>
    constexpr MemoryBitAddressT<is_const>::MemoryBitAddressT(const MemoryAddressT<is_address_const>& address)
        : address_(address)
    {

    }

    template <bool is_const>
    constexpr MemoryBitAddressT<is_const>::MemoryBitAddressT(MemoryBitAddressT::Pointer address)
        : MemoryBitAddressT(MemoryAddressT<is_const>(address))
    {

    }

    template <bool is_const>
    template <bool is_address_const>
    constexpr MemoryBitAddressT<is_const>::MemoryBitAddressT(const MemoryAddressT<is_address_const>& address, Bits offset)
        : address_(address + ToBytesFloor(offset))
        , offset_(offset % Bits::kByte)
    {

    }

    template <bool is_const>
    constexpr MemoryBitAddressT<is_const>::MemoryBitAddressT(MemoryBitAddressT::Pointer address, Bits offset)
        : MemoryBitAddressT(MemoryAddressT<is_const>(address), offset)
    {

    }

    template <bool is_const>
    constexpr MemoryBitAddressT<is_const>::MemoryBitAddressT(std::nullptr_t)
        : MemoryBitAddressT()
    {

    }

    template <bool is_const>
    template <bool is_rhs_const>
    constexpr MemoryBitAddressT<is_const>::MemoryBitAddressT(const MemoryBitAddressT<is_rhs_const>& rhs)
        : address_(rhs.GetBaseAddress())
        , offset_(rhs.GetOffset())
    {

    }

    template <bool is_const>
    constexpr const MemoryAddressT<is_const>& MemoryBitAddressT<is_const>::GetBaseAddress() const noexcept
    {
        return address_;
    }

    template <bool is_const>
    constexpr Bits MemoryBitAddressT<is_const>::GetOffset() const noexcept
    {
        return offset_;
    }

    template <bool is_const>
    constexpr MemoryBitAddressT<is_const>::operator bool() const noexcept
    {
        return address_ || (offset_ > 0_Bits);
    }

    template <bool is_const>
    constexpr MemoryBitAddressT<is_const>& MemoryBitAddressT<is_const>::operator+=(const Bits& rhs) noexcept
    {
        offset_ += rhs;
        address_ += ToBytesFloor(offset_);
        offset_ %= Bits::kByte;
        return *this;
    }

    template <bool is_const>
    constexpr MemoryBitAddressT<is_const>& MemoryBitAddressT<is_const>::operator-=(const Bits& rhs) noexcept
    {
        offset_ = (Bits(Bits::kByte) - offset_) + rhs;
        address_ -= ToBytesFloor(offset_ - 1_Bits);
        offset_ = (Bits(Bits::kByte) - (offset_ % Bits::kByte)) % Bits::kByte;
        return *this;
    }

    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator==(const MemoryBitAddressT<is_lhs_const>& lhs, const MemoryBitAddressT<is_rhs_const>& rhs) noexcept
    {
        return (lhs.GetBaseAddress() == rhs.GetBaseAddress()) && (lhs.GetOffset() == rhs.GetOffset());
    }

    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator!=(const MemoryBitAddressT<is_lhs_const>& lhs, const MemoryBitAddressT<is_rhs_const>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator>(const MemoryBitAddressT<is_lhs_const>& lhs, const MemoryBitAddressT<is_rhs_const>& rhs) noexcept
    {
        return (lhs.GetBaseAddress() > rhs.GetBaseAddress()) ||
            (lhs.GetBaseAddress() == rhs.GetBaseAddress() && lhs.GetOffset() > rhs.GetOffset());
    }

    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator<(const MemoryBitAddressT<is_lhs_const>& lhs, const MemoryBitAddressT<is_rhs_const>& rhs) noexcept
    {
        return (lhs.GetBaseAddress() < rhs.GetBaseAddress()) ||
            (lhs.GetBaseAddress() == rhs.GetBaseAddress() && lhs.GetOffset() < rhs.GetOffset());
    }

    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator>=(const MemoryBitAddressT<is_lhs_const>& lhs, const MemoryBitAddressT<is_rhs_const>& rhs) noexcept
    {
        return (lhs.GetBaseAddress() > rhs.GetBaseAddress()) ||
            (lhs.GetBaseAddress() == rhs.GetBaseAddress() && lhs.GetOffset() >= rhs.GetOffset());
    }

    template <bool is_lhs_const, bool is_rhs_const>
    constexpr bool operator<=(const MemoryBitAddressT<is_lhs_const>& lhs, const MemoryBitAddressT<is_rhs_const>& rhs) noexcept
    {
        return (lhs.GetBaseAddress() < rhs.GetBaseAddress()) ||
            (lhs.GetBaseAddress() == rhs.GetBaseAddress() && lhs.GetOffset() <= rhs.GetOffset());
    }

    template <bool is_const>
    constexpr MemoryBitAddressT<is_const> operator+(const MemoryBitAddressT<is_const>& lhs, const Bits& rhs) noexcept
    {
        return MemoryBitAddressT<is_const>(lhs) += rhs;
    }

    template <bool is_const>
    constexpr MemoryBitAddressT<is_const> operator-(const MemoryBitAddressT<is_const>& lhs, const Bits& rhs) noexcept
    {
        return MemoryBitAddressT<is_const>(lhs) -= rhs;
    }

    template <bool is_const>
    inline std::ostream& operator<<(std::ostream& lhs, const MemoryBitAddressT<is_const>& rhs)
    {
        return lhs << rhs.GetBaseAddress() << ":" << rhs.GetOffset();
    }
}

