#include "Constant.hpp"

namespace LootTable {
    namespace Roll {
        Constant::Constant(const nlohmann::json &roll)
        {
            if (roll.is_number_unsigned())
                this->_rolls = roll.get<nlohmann::json::number_unsigned_t>();
            else
                this->_rolls = roll["value"].get<nlohmann::json::number_unsigned_t>();
        }

        const RollResult Constant::poll(void)
        {
            return (RollResult(this->_rolls, 1.0));
        }

        std::unique_ptr<Roll> Constant::creator(const nlohmann::json &roll)
        {
            return (std::make_unique<Constant>(roll));
        }

        bool Constant::isOfType(const nlohmann::json &roll)
        {
            return ( // roll is just a number, or
                roll.is_number_unsigned() || \
                ( // roll is an object containing "type":"minecraft:constant" and "value":a-number
                    roll.is_object() && \
                    roll.contains("type") && \
                    roll.contains("value") && \
                    roll["type"].is_string() && \
                    roll["value"].is_number_unsigned() && \
                    roll["type"].get<std::string>() == "minecraft:constant"
                )
            );
        }
    };
};