#include "ApplyBonus.hpp"

namespace LootTable {
    namespace Function {
        ApplyBonus::ApplyBonus(const nlohmann::json &function):
            Function(function)
        {
            this->setValidity(true);
        }

        void ApplyBonus::apply(void)
        {}

        std::unique_ptr<Function> ApplyBonus::create(const nlohmann::json &function)
        {
            return (std::make_unique<ApplyBonus>(function));
        }
    };
};