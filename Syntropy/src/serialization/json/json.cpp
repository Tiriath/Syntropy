#include "serialization/json/json.h"

#include "diagnostics/log.h"

namespace syntropy::serialization
{
    /************************************************************************/
    /* NON-MEMBER FUNCTIONS                                                 */
    /************************************************************************/

    const reflection::Class* GetClassFromJSON(const nlohmann::json& json, const reflection::Class* base_class)
    {
        auto it = json.find(kClassToken);

        if (it == json.end())
        {
            return base_class;      // No class specified: return the base class.
        }

        if (!it->is_string())
        {
            SYNTROPY_WARNING((SerializationCtx), "Expected a string value for the property '", kClassToken, "'.");
            return nullptr;
        }

        auto concrete_class = reflection::GetClass(it->get<std::string>());

        if (concrete_class == nullptr)
        {
            SYNTROPY_WARNING((SerializationCtx), "Unrecognized class '", it->get<std::string>(), "'. Did you forget to add it to the reflection system?");
            return nullptr;
        }

        if(!concrete_class->IsA(*base_class))
        {
            SYNTROPY_WARNING((SerializationCtx), "Cannot deserialize an object of type '", base_class, "' from type ", it->get<std::string>(), ".");
            return nullptr;
        }

        return concrete_class;
    }
}