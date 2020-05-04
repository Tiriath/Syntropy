#include "syntropy/core/context.h"

#include <tuple>

#include "syntropy/containers/map.h"

#include "syntropy/allocators/linear_virtual_memory_resource.h"
#include "syntropy/allocators/fallback_memory_resource.h"
#include "syntropy/allocators/system_memory_resource.h"
#include "syntropy/allocators/memory_resource.h"

namespace syntropy
{
    using namespace Literals;

    /************************************************************************/
    /* CONTEXT :: REGISTRY                                                  */
    /************************************************************************/

    /// \brief Singleton class used to store contexts.
    /// \author Raffaele D. Facendola - May 2020.
    class Context::Registry
    {
    public:

        /// \brief Get the singleton instance.
        static Registry& GetSingleton();

        /// \brief Get a context by name.
        const Context* GetParentContext(const Context::TStringView& context_name);

    private:

        /// \brief Allocate a new entry.
        const Context* Allocate(const Context::TStringView& context_name);

        /// \brief Type of the memory resource used to store context. Contexts are never deallocated.
        /// In the unlikely event the virtual memory range is exhausted, the system memory resource is used as a last resort.
        using TMemoryResource = MemoryResourceT<FallbackMemoryResource<LinearVirtualMemoryResource, SystemMemoryResource>>;

        /// \brief Private constructor.
        Registry();

        /// \brief Memory resource used for dynamic memory allocation.
        TMemoryResource memory_resource_;

        /// \brief Context registered so far, indexed by full name.
        Map<Label, const Context*> contexts_;

    };

    /************************************************************************/

    Context::Registry& Context::Registry::GetSingleton()
    {
        static auto singleton = Registry();

        return singleton;
    }

    Context::Registry::Registry()
        : memory_resource_(std::forward_as_tuple(1_MiBytes, 64_KiBytes), kDefaultConstruct)
        , contexts_(memory_resource_)
    {
        // #TODO Configure memory resource from data.
    }

    const Context* Context::Registry::GetParentContext(const Context::TStringView& context_name)
    {
        // Find or allocate a parent context.

        if (auto separator_index = context_name.find_last_of(Context::kSeparator); separator_index != Context::TStringView::npos)
        {
            // Parent name - discard until a separator is found.

            auto parent_context_name = context_name;

            parent_context_name.remove_suffix(parent_context_name.size() - separator_index);

            auto parent_context_label = Label(parent_context_name);

            if (auto parent_context_iterator = contexts_.lower_bound(parent_context_label); parent_context_iterator != contexts_.end())
            {
                // The parent context already exists.

                return parent_context_iterator->second;
            }
            else
            {
                // The parent doesn't exist: add it now. Allocation will cause recursive parent allocation if necessary.

                auto parent_context = Allocate(parent_context_name);

                contexts_.insert(parent_context_iterator, std::make_pair(parent_context_name, parent_context));

                return parent_context;
            }
        }

        // Root contexts have no parent.

        return nullptr;
    }

    const Context* Context::Registry::Allocate(const Context::TStringView& context_name)
    {
        auto storage = memory_resource_.Allocate(BytesOf<Context>(), AlignmentOf<Context>()).Begin().As<Context>();

        new (storage) Context(context_name);

        return storage;
    }

    /************************************************************************/
    /* CONTEXT                                                              */
    /************************************************************************/

    Context::Context(const TStringView& name)
        : name_(name)
        , parent_(Registry::GetSingleton().GetParentContext(name))
    {

    }

}