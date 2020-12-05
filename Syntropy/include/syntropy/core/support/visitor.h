
/// \file visitor.h
/// \brief This header is part of the Syntropy core module. It contains definitions for visitors and visitable objects.
///
/// \author Raffaele D. Facendola - 2019

#pragma once

#include <typeinfo>

#include "syntropy/language/foundation/foundation.h"
#include "syntropy/language/templates/type_traits.h"

// ===========================================================================

namespace Syntropy
{
    /************************************************************************/
    /* VISITOR                                                              */
    /************************************************************************/

    /// \brief Base class for generic visitors.
    /// Based on: https://foonathan.net/blog/2017/12/21/visitors.html.
    /// \author Raffaele D. Facendola - August 2019.
    /// \remarks This class doesn't support polymorphism.
    class Visitor
    {
    public:

        /// \brief Default virtual destructor.
        virtual ~Visitor() noexcept = default;

        /// \brief Visit an element.
        template <typename TVisitable>
        void Visit(TVisitable visitable) const noexcept;

    protected:

        /// \brief Attempt to visit an element via a visitor functor.
        /// \return Returns true if the visit was successful, returns false otherwise.
        template <typename TFunction, typename TVisitor>
        Bool TryVisit(Ref<TVisitor> visitor, TypelessPtr visitable, Ref<std::type_info> type) const noexcept;

    private:

        /// \brief Visit an element.
        virtual void VirtualVisit(TypelessPtr visitable, Ref<std::type_info> type) const noexcept = 0;

    };

    /************************************************************************/
    /* NON-MEMBER FUNCTIONS                                                 */
    /************************************************************************/

    /// \brief Create a new visitor that responds to different types specified by a list of lambdas.
    /// \usage auto visitor = MakeVistor([](Int element){...}, [](Float element){...}, [](Bool element){...});
    ///        visitor.Visit(42).
    template <typename... TFunctions>
    auto MakeVisitor(TFunctions... functions) noexcept;

}

// ===========================================================================

namespace Syntropy
{

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // Visitor.
    // ========

    template <typename TVisitable>
    inline void Visitor::Visit(TVisitable visitable) const noexcept
    {
        if constexpr (!Templates::IsPolymorphic<TVisitable> || Templates::IsFinal<TVisitable>)
        {
            VirtualVisit(ToPtr(visitable), typeid(visitable));
        }
        else
        {
            VirtualVisit(dynamic_cast<TypelessPtr>(ToPtr(visitable)), typeid(visitable));       // Downcast to the most derived class since typeid will return the dynamic type of visitable.
        }
    }

    template <typename TFunction, typename TVisitor>
    inline Bool Visitor::TryVisit(Ref<TVisitor> visitor, TypelessPtr visitable, Ref<std::type_info> type) const noexcept
    {
        using TArgument = Templates::FunctionArgumentsElement<0, TFunction>;
        using TVisitable = Templates::RemoveReference<TArgument>;

        if (type == typeid(TVisitable))
        {
            visitor(*FromTypeless<TVisitable>(visitable));
            return true;
        }

        return false;
    }

    // Non-member functions.
    // =====================

    template <typename... TFunctions>
    inline auto MakeVisitor(TFunctions... functions) noexcept
    {
        struct LambdaVisitor : public Visitor, public TFunctions...
        {
            // Gather many lambdas in a single callable object.
            LambdaVisitor(TFunctions... functions)
                : TFunctions(Move(functions))...
            {

            }

            // Exposes one operator() for each element to handle.
            using TFunctions::operator()...;

            // Attempt to visit with each of the lambdas.
            void VirtualVisit(TypelessPtr visitable, Ref<std::type_info> type) const noexcept override
            {
                (TryVisit<TFunctions>(*this, visitable, type) || ...);
            }
        };

        return LambdaVisitor(Move(functions)...);
    }

}

// ===========================================================================