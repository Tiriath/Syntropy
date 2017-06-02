
/// \file diagnostics.h
/// \brief This header is part of the syntropy diagnostic system. It contains meta definitions for the file diagnostics.h.
///
/// \author Raffaele D. Facendola - 2016

#pragma once

#include "diagnostics/diagnostics.h"
#include "diagnostics/log_channels.h"

#include "reflection/class.h"
#include "reflection/types/core_types.h"

#include "serialization/json.h"
#include "serialization/json/json_deserializer.h"

namespace syntropy
{
    namespace reflection
    {
        extern const Class& ClassOf_Severity;
        extern const Class& ClassOf_Context;

        // Reflection specialization for Context
        template<>
        struct ClassDeclaration<diagnostics::Severity>
        {
            static constexpr const char* GetName() noexcept
            {
                return "syntropy::diagnostics::Severity";
            }

            void operator()(ClassDefinitionT<diagnostics::Context>& definition) const
            {
                definition << serialization::JSONConstruct();
            }
        };

        // Reflection specialization for Context
        template<>
        struct ClassDeclaration<diagnostics::Context>
        {
            static constexpr const char* GetName() noexcept
            {
                return "syntropy::diagnostics::Context";
            }

            void operator()(ClassDefinitionT<diagnostics::Context>& definition) const
            {
                definition << serialization::JSONConstruct();
            }
        };

        extern const Class& ClassOf_LogChannel;

        // Reflection specialization for LogChannel.
        template <>
        struct ClassDeclaration<diagnostics::LogChannel>
        {
            static constexpr const char* GetName() noexcept
            {
                return "diagnostics::LogChannel";
            }
        };
    }

    namespace reflection
    {
        extern const Class& ClassOf_StreamLogChannel;
        extern const Class& ClassOf_FileLogChannel;

        // Reflection specialization for StreamLogChannel.
        template <>
        struct ClassDeclaration<diagnostics::StreamLogChannel>
        {
            static constexpr const char* GetName() noexcept
            {
                return "syntropy::diagnostics::StreamLogChannel";
            }

            void operator()(ClassDefinitionT<diagnostics::StreamLogChannel>& definition) const
            {
                definition.DefineNameAlias("StreamLogChannel");

                definition.DefineBaseClass<diagnostics::LogChannel>();
            }
        };

        // Reflection specialization for FileLogChannel.
        template <>
        struct ClassDeclaration<diagnostics::FileLogChannel>
        {
            static constexpr const char* GetName() noexcept
            {
                return "syntropy::diagnostics::FileLogChannel";
            }

            void operator()(ClassDefinitionT<diagnostics::FileLogChannel>& definition) const
            {
                definition << serialization::JSONConstruct();

                definition.DefineNameAlias("FileLogChannel");

                definition.DefineBaseClass<diagnostics::StreamLogChannel>();
            }
        };

    }

    namespace serialization
    {
        // Used to deserialize a FileLogChannel from a JSON object.
        template <>
        struct JSONDeserializerT<diagnostics::FileLogChannel>
        {
            std::optional<diagnostics::FileLogChannel> operator()(const nlohmann::json& json) const
            {
                if (json.is_object())
                {
                    std::string file;
                    std::string format;
                    std::vector<diagnostics::Context> contexts;
                    diagnostics::Severity verbosity;

                    return std::make_optional<diagnostics::FileLogChannel>(std::move(file), std::move(format), std::move(contexts), verbosity);
                }

                return std::nullopt;
            }
        };
    }

    namespace serialization
    {
        // Used to deserialize a Severity from a JSON object.
        template <>
        struct JSONDeserializerT<diagnostics::Severity>
        {
            std::optional<diagnostics::Severity> operator()(const nlohmann::json& json) const
            {
                if (json.is_string())
                {
                    auto str = json.get<std::string>();

                    if (str == "Informative")       return diagnostics::Severity::kInformative;
                    if (str == "Warning")           return diagnostics::Severity::kWarning;
                    if (str == "Error")             return diagnostics::Severity::kError;
                    if (str == "Critical")          return diagnostics::Severity::kCritical;

                }
                return std::nullopt;
            }
        };

        // Used to deserialize a Context from a JSON object.
        template <>
        struct JSONDeserializerT<diagnostics::Context>
        {
            std::optional<diagnostics::Context> operator()(const nlohmann::json& json) const
            {
                if (json.is_string())
                {
                    return diagnostics::Context(json.get<std::string>());
                }
                return std::nullopt;
            }
        };
    }
}