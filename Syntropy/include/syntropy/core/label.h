
/// \file label.h
/// \brief This header is part of Syntropy core module. It contains classes used to handle immutable strings (labels).
///
/// \author Raffaele D. Facendola - 2016

#pragma once

#include "syntropy/core/string.h"

namespace syntropy
{
    /************************************************************************/
    /* LABEL                                                                */
    /************************************************************************/

    /// \brief Represents an immutable string optimized for fast comparison.
    /// Equal labels are guaranteed to point to the exact same location in memory.
    /// \author Raffaele D. Facendola.
    class Label
    {
    public:

        /// \brief Type of a string character.
        using TChar = String::value_type;

        /// \brief Type of the string.
        using TString = String;

        /// \brief Type of the string view.
        using TStringView = StringView;

        /// \brief Create an empty label.
        Label();

        /// \brief Copy constructor.
        Label(const Label& other) noexcept = default;

        /// \brief Create a new label from a string view.
        Label(const TStringView& string);

        /// \brief Create a new label from a null-terminated string.
        Label(const TChar* characters);

        /// \brief Default assignment operator.
        Label& operator=(const Label& other) noexcept = default;

        /// \brief Check whether the label is non-empty.
        /// \return Returns true if the string is non-empty, returns false otherwise.
        operator bool() const noexcept;

        /// \brief Get the underlying characters.
        const TChar* GetCharacters() const noexcept;

        /// \brief Swaps two label.
        void Swap(Label& other) noexcept;

    private:

        class Registry;

        /// \brief Underlying characters, shared among all labels and never deallocated.
        const TChar* characters_;

    };

    /************************************************************************/
    /* NON-MEMBER FUNCTIONS                                                 */
    /************************************************************************/

    /// \brief Equality comparison.
    bool operator==(const Label& lhs, const Label& rhs) noexcept;

    /// \brief Inequality comparison.
    bool operator!=(const Label& lhs, const Label& rhs) noexcept;

    /// \brief Get a label hash.
    std::int64_t Hash64(const Label& rhs);

    /// \brief Swaps two labels.
    void swap(Label& lhs, Label& rhs) noexcept;

    /// \brief Stream insertion for labels.
    std::ostream& operator<<(std::ostream& out, const Label& rhs);

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Label.

    inline Label::Label()
        : Label("")
    {

    }

    inline Label::Label(const Label::TChar* characters)
        : Label(Label::TStringView(characters))
    {

    }

    inline Label::operator bool() const noexcept
    {
        return (*this) != Label{};
    }

    inline const Label::TChar* Label::GetCharacters() const noexcept
    {
        return characters_;
    }

    inline void Label::Swap(Label& other) noexcept
    {
        using std::swap;

        swap(characters_, other.characters_);
    }

    // Non-member functions.

    inline bool operator==(const Label& lhs, const Label& rhs) noexcept
    {
        return lhs.GetCharacters() == rhs.GetCharacters();
    }

    inline bool operator!=(const Label& lhs, const Label& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    inline std::int64_t Hash64(const Label& rhs)
    {
        // Labels are never deallocated, hence their memory location is unique.

        return reinterpret_cast<std::intptr_t>(rhs.GetCharacters());
    }

    inline void swap(Label& lhs, Label& rhs) noexcept
    {
        lhs.Swap(rhs);
    }

    inline std::ostream& operator<<(std::ostream& out, const Label& rhs)
    {
        out << rhs.GetCharacters();

        return out;
    }

}


