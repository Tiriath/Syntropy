#include "reflection/interfaces/class_interfaces.h"

namespace syntropy
{
    namespace reflection
    {

        /************************************************************************/
        /* ENUMERATION                                                          */
        /************************************************************************/

        Any Enumeration::GetValueByName(const HashedString& value_name) const
        {
            auto it = values_.find(value_name);

            return it != values_.end() ?
                it->second :
                Any();
        }

    }
}