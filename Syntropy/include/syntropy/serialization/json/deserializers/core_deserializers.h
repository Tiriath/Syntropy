
/// \file core_deserializers.h
/// \brief This header is part of the syntropy serialization system. It contains deserializers for syntropy's core types.
///
/// \author Raffaele D. Facendola - 2017

#pragma once

#include "syntropy/types/label.h"
#include "syntropy/containers/context.h"

#include "syntropy/serialization/json/json.h"

namespace syntropy::serialization
{
    /************************************************************************/
    /* HASHED STRING                                                        */
    /************************************************************************/

    template <>
    struct JSONDeserializerT<Label>
    {
        std::optional<Label> operator()(const nlohmann::json& json) const
        {
            if (json.is_string())
            {
                return Label(json.get<std::string>());
            }
            return std::nullopt;
        }
    };

    /************************************************************************/
    /* CONTEXT                                                              */
    /************************************************************************/

    template <>
    struct JSONDeserializerT<Context>
    {
        std::optional<Context> operator()(const nlohmann::json& json) const
        {
            if (json.is_string())
            {
                return Context(json.get<std::string>());
            }
            return std::nullopt;
        }
    };
}
