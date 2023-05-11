#include "Server.hpp"

#include "Alternative.hpp"

namespace LootTable {
    namespace Entry {
        Alternative::Alternative(const nlohmann::json &entry) : Entry(entry)
        {
            // get entries
            for (const auto &child : entry["children"]) {
                std::unique_ptr<Entry> newChild = Server::getInstance()->lootTables.createEntry(child);

                const auto &it = this->_children.insert(this->_children.end(), nullptr);
                it->swap(newChild);
            }
        }

        bool Alternative::poll(LootTablePoll &poll)
        {
            for (const auto &child : this->_children) {
                for (const auto &condition : this->_conditions) {
                    if (condition->verify() == false)
                        continue;
                    child->poll(poll);
                    return (true);
                }
            }
            return (false);
        }

        std::unique_ptr<Entry> Alternative::creator(const nlohmann::json &entry)
        {
            return (std::make_unique<Alternative>(entry));
        }
    };
};
