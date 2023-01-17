#include "DefaultWorld.hpp"
#include "../world_storage/GZIP.hpp"
#include <iostream>

#define CHUNK 0xFF

DefaultWorld::DefaultWorld(WorldGroup *worldGroup)
    : World(worldGroup)
{
    this->_dimensions.emplace("end", std::make_shared<TheEnd>(reinterpret_cast<World *>(this)));
    this->_dimensions.emplace("nether", std::make_shared<TheNether>(reinterpret_cast<World *>(this)));
    this->_dimensions.emplace("overworld", std::make_shared<Overworld>(reinterpret_cast<World *>(this)));
    for (auto [_, dim] : this->_dimensions)
        _processingThreads.push_back(new std::thread([dim](){
            while (1) {
                dim->dimensionLock.acquire();
                dim->tick();
            }
        }));
}

void DefaultWorld::tick()
{
    World::tick();
    for (auto [_, dim] : this->_dimensions)
        dim->dimensionLock.release();
}

void DefaultWorld::initialize()
{
    for (auto &dim : this->_dimensions)
        dim.second->initialize();
}

// TODO: When _data will be initialized don't forget to remove this phony data and put the function to const !
void DefaultWorld::save() {
    nbt::Compound *_data = new nbt::Compound("Data", {
            new nbt::Byte("allowCommands", 0),
            new nbt::Double("BorderCenterX", 0),
            new nbt::Double("BorderCenterZ", 0),
            new nbt::Double("BorderCenterX", 0),
            new nbt::Double("BorderDamagePerBlock", 0.2),
            new nbt::Double("BorderSize", 60000000),
            new nbt::Double("BorderSafeZone", 5),
            new nbt::Double("BorderSizeLerpTarget", 60000000),
            new nbt::Long("BorderSizeLerpTime", 0),
            new nbt::Double("BorderWarningBlocks", 5),
            new nbt::Double("BorderWarningTime", 15),
            new nbt::Int("ClearWeatherTime", 0),
            new nbt::Compound("CustomBossEvents", {}),
            new nbt::Compound("DataPacks", {}),
            new nbt::Int("DataVersion", 3120),
            new nbt::Long("DayTime", 0),
            new nbt::Byte("Difficulty", 0),
            new nbt::Byte("DifficultyLocked", 0),
            new nbt::Compound("DimensionData", {
                    new nbt::Compound("1", {
                            new nbt::Compound("DragonFight", {
                                    new nbt::Compound("ExitPortalLocation", {
                                            new nbt::Byte("X", 0),
                                            new nbt::Byte("Y", 0),
                                            new nbt::Byte("Z", 0),
                                    }),
                                    new nbt::List("Gateways", {}),
                                    new nbt::Byte("DragonKilled", 0),
                                    new nbt::Long("DragonUUIDLeast", 0),
                                    new nbt::Long("DragonUUIDMost", 0),
                                    new nbt::Byte("PreviouslyKilled", 0),
                            })
                    })
            }),
            new nbt::Compound("GameRules", {}),
            new nbt::Int("GameType", 0),
            new nbt::Byte("hardcore", 0),
            new nbt::Byte("initialized", 0),
            new nbt::Long("LastPlayed", 0),
            new nbt::String("LevelName", "World"),
            new nbt::Byte("MapFeatures", 0),
            new nbt::Compound("Player", {}),
            new nbt::Byte("raining", 0),
            new nbt::Int("rainTime", 0),
            new nbt::Long("RandomSeed", 0),
            new nbt::Long("SizeOnDisk", 0),
            new nbt::Int("SpawnX", 0),
            new nbt::Int("SpawnY", 0),
            new nbt::Int("SpawnZ", 0),
            new nbt::Byte("thundering", 0),
            new nbt::Int("thunderTime", 0),
            new nbt::Long("Time", 0),
            new nbt::Int("Time", 19133 ),
            new nbt::Compound("Version", {
                    new nbt::Int("Id", 3120),
                    new nbt::String("Name", "1.19.2"),
                    new nbt::String("Series", "main"),
                    new nbt::Byte("Snapshot", 0)
            }),
            new nbt::IntArray("WanderingTraderId", {0, 0, 0, 0}),
            new nbt::Int("WanderingTraderSpawnChance", 10),
            new nbt::Int("WanderingTraderSpawnDelay", 100000),
            new nbt::Byte("WasModded", 0)
    });
    std::vector<uint8_t> buffer = _data->serialize();

    GZIP gzip;

    gzip.compress(buffer, "level.dat");
}