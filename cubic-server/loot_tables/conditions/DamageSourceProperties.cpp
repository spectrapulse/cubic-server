#include "DamageSourceProperties.hpp"

namespace LootTable {
    namespace Condition {
        DamageSourceProperties::DamageSourceProperties(const nlohmann::json &condition)
        {
            (void)condition;
        }

        bool DamageSourceProperties::verify(void)
        {
            return (true);
        }

        std::unique_ptr<Condition> DamageSourceProperties::create(const nlohmann::json &condition)
        {
            return (std::make_unique<DamageSourceProperties>(condition));
        }
    };
};