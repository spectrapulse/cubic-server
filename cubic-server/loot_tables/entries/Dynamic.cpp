#include "Server.hpp"

#include "Dynamic.hpp"

namespace LootTable {
    namespace Entry {
        Dynamic::Dynamic(const nlohmann::json &entry) : Entry(entry)
        {
            (void)entry;
        }

        bool Dynamic::poll(LootTablePoll &poll)
        {
            (void)poll;
            return (true);
        }

        std::unique_ptr<Entry> Dynamic::creator(const nlohmann::json &entry)
        {
            return (std::make_unique<Dynamic>(entry));
        }
    };
};
