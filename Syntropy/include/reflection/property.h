
/// \file property.h
/// \brief This header is part of the syntropy reflection system. It contains classes used to handle class properties.
///
/// \author Raffaele D. Facendola - 2016

#pragma once

#include <tuple>

#include "type_traits.h"

#include "containers/hashed_string.h"
#include "containers/interface_container.h"

#include "reflection/type.h"
#include "reflection/any.h"
#include "reflection/interfaces/property_interfaces.h"

#include "diagnostics/log.h"

namespace syntropy::reflection
{
    /************************************************************************/
    /* PROPERTY                                                             */
    /************************************************************************/

    /// \brief Describes a class property.
    /// A property can either be a member field, a getter method (for read-only properties) or a getter\setter method pairs.
    /// \author Raffaele D. Facendola - 2016
    class Property
    {
        template <typename... TAccessors>
        friend class PropertyDefinitionT;

    public:

        /// \brief Create a new property from member field.
        /// \param name Name of the property.
        /// \param field Pointer to a member field.
        template <typename TClass, typename TField>
        Property(const HashedString& name, TField(TClass::* field))
            : name_(name)
            , type_(TypeOf<TField>())
        {
            if constexpr(std::is_copy_constructible_v<std::remove_cv_t<TField>>)
            {
                interfaces_.AddInterface<Readable>(field);
            }

            if constexpr(std::is_copy_assignable_v<TField>)
            {
                interfaces_.AddInterface<Writeable>(field);
            }
        }

        /// \brief Create a new property from a read-only getter.
        /// \param name Name of the property.
        /// \param getter Pointer to the member function used as property getter.
        template <typename TClass, typename TProperty>
        Property(const HashedString& name, TProperty(TClass::* getter)() const)
            : name_(name)
            , type_(TypeOf<std::remove_cvref<TProperty>>())
        {
            if constexpr(std::is_copy_constructible_v<std::remove_cvref<TProperty>>)
            {
                interfaces_.AddInterface<Readable>(getter);
            }
        }

        /// \brief Create a new property from a getter\setter pair.
        /// \param name Name of the property.
        /// \param getter Pointer to the member function used as property getter.
        /// \param setter Pointer to the member function used as property setter.
        template <typename TClass, typename TPropertyGetter, typename TPropertySetter>
        Property(const HashedString& name, TPropertyGetter(TClass::* getter)() const, void (TClass::* setter)(TPropertySetter))
            : name_(name)
            , type_(TypeOf<std::remove_cvref<TPropertyGetter>>())
        {
            static_assert(std::is_same_v<std::remove_cvref<TPropertyGetter>, std::remove_cvref<TPropertySetter>>, "TPropertyGetter and TPropertySetter must refer to the same underlying type (regardless of reference and qualifiers)");

            if constexpr(std::is_copy_constructible_v<std::remove_cvref<TPropertyGetter>>)
            {
                interfaces_.AddInterface<Readable>(getter);
            }

            if constexpr(std::is_copy_constructible_v<std::remove_cvref<TPropertySetter>>)
            {
                interfaces_.AddInterface<Writeable>(setter);
            }
        }

        /// \brief Create a new property from a getter\setter pair.
        /// \param name Name of the property.
        /// \param getter Pointer to the member function used as const accessor to the property.
        /// \param setter Pointer to the member function used as non-const accessor to the property.
        template <typename TClass, typename TProperty>
        Property(const HashedString& name, const TProperty&(TClass::* getter)() const, TProperty& (TClass::* setter)())
            : name_(name)
            , type_(TypeOf<TProperty>())
        {
            if constexpr(std::is_copy_constructible_v<TProperty>)
            {
                interfaces_.AddInterface<Readable>(getter);
            }

            if constexpr(std::is_copy_assignable_v<TProperty>)
            {
                interfaces_.AddInterface<Writeable>(setter);
            }
        }

        /// \brief Move constructor.
        Property(Property&& other);

        /// \brief Default destructor.
        ~Property() = default;

        /// \brief Get the property name.
        /// \return Returns the property name.
        const HashedString& GetName() const noexcept;

        /// \brief Get the property type.
        /// \brief Returns the property type.
        const Type& GetType() const noexcept;
            
        /// \brief Query the property for an interface of type TInterface.
        /// \return If an interface of type TInterface was previously added via AddInterface(.), returns a pointer to that interface, otherwise returns nullptr.
        /// \remarks This method doesn't account for polymorphism. If a class of type Foo derived from Bar is added to the property, GetInterface<Bar>() will return nullptr even if a conversion exists.
        template <typename TInterface>
        const TInterface* GetInterface() const
        {
            return interfaces_.GetInterface<TInterface>();
        }

        /// \brief Add a new interface to the property.
        /// The method creates an instance of TConcrete using TArgs as construction parameters. Only one interface of type TInterface can be added per property.
        /// TConcrete must be equal to or derive from TInterface.
        /// \param arguments Arguments to pass to the constructor of TInterface.
        template <typename TInterface, typename TConcrete = TInterface, typename... TArgs>
        void AddInterface(TArgs&&... arguments)
        {
            if (interfaces_.AddInterface<TInterface, TConcrete>(std::forward<TArgs>(arguments)...) == nullptr)
            {
                SYNTROPY_ERROR((ReflectionCtx), "An interface '", typeid(TInterface).name(), "' was already added to the property '", name_, "'. The new interface has been ignored.");
            }
        }

    private:

        HashedString name_;                                             ///< \brief Property name.

        const Type& type_;                                              ///< \brief Underlying property type.

        InterfaceContainer<> interfaces_;                               ///< \brief Interfaces assigned to the property.

    };

    /************************************************************************/
    /* PROPERTY DEFINITION T                                                */
    /************************************************************************/

    /// \brief Concrete property definition.
    /// This class is used to define additional property interfaces.
    /// \author Raffaele D. Facendola - April 2017
    template <typename... TAccessors>
    class PropertyDefinitionT
    {
    public:

        /// \brief Apply a functor to this property definition.
        /// \param property_definition Target property definition.
        /// \param functor Functor to apply.
        /// \return Returns the property definition.
        template <typename TFunctor>
        PropertyDefinitionT& operator<<(TFunctor&& functor)
        {
            std::apply(std::forward<TFunctor>(functor), std::tuple_cat(std::tie(*this), accessors_));
            return *this;
        }

        /// \brief Create a new property definition.
        /// \param accessor Accessors to the property.
        PropertyDefinitionT(Property& subject, TAccessors... accessors)
            : property_(subject)
            , accessors_(std::forward<TAccessors>(accessors)...)
        {

        }

        /// \brief Add a new interface to the property.
        /// The method creates an instance of TConcrete using TArgs as construction parameters. Only one interface of type TInterface can be added per property.
        /// TConcrete must be equal to or derive from TInterface.
        /// \param arguments Arguments to pass to the constructor of TInterface.
        template <typename TInterface, typename TConcrete = TInterface, typename... TArgs>
        PropertyDefinitionT& AddInterface(TArgs&&... arguments)
        {
            property_.AddInterface<TInterface, TConcrete>(std::forward<TArgs>(arguments)...);
            return *this;
        }

    private:

        Property& property_;                            ///< \brief Property this definition refers to.

        std::tuple<TAccessors...> accessors_;           ///< \brief Concrete accessors to the property.
    };
}