#ifndef CUBIC_SERVER_LOOT_TABLES_FUNCTIONS_ENCHANTRANDOMLY_HPP
#define CUBIC_SERVER_LOOT_TABLES_FUNCTIONS_ENCHANTRANDOMLY_HPP

#include "Function.hpp"

namespace LootTable {
    namespace Function {
        class EnchantRandomly : public Function {
        public:
            EnchantRandomly(const nlohmann::json &function);
            ~EnchantRandomly() = default;

            void apply(void) override;

            static std::unique_ptr<Function> create(const nlohmann::json &function);
        };
    };
};

#endif //CUBIC_SERVER_LOOT_TABLES_FUNCTIONS_ENCHANTRANDOMLY_HPP
