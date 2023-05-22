#ifndef CUBICSERVER_RECIPES_SMELTING_HPP
#define CUBICSERVER_RECIPES_SMELTING_HPP

#include "Recipes.hpp"

#include "Server.hpp"

namespace Recipe {
    class Smelting : public Recipe {
    public:
        Smelting(const nlohmann::json &recipe);
        ~Smelting() = default;

        void dump(void) const override;
        void insertToPayload(std::vector<uint8_t> &payload) const override;

        static std::unique_ptr<Recipe> create(const nlohmann::json &recipe);
    private:
        ItemId _ingredient; // what to cook
        ItemId _result; // what is obtained
        double _experience; // experience generated by the smelting
        uint64_t _cookingTime; // time it takes to cook
    };
};

#endif // CUBICSERVER_RECIPES_SMELTING_HPP
