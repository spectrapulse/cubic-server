#include "BlockStateProperties.hpp"

namespace LootTable {
    namespace Condition {
        BlockStateProperties::BlockStateProperties(const nlohmann::json &condition)
        {
            (void)condition;

            this->setValidity(true);
        }

        bool BlockStateProperties::verify(void) const
        {
            return (true);
        }

        std::unique_ptr<Condition> BlockStateProperties::create(const nlohmann::json &condition)
        {
            return (std::make_unique<BlockStateProperties>(condition));
        }
    };
};