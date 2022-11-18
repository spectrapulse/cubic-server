#include "ConfigHandler.hpp"
#include <iostream>
#include <yaml-cpp/yaml.h>

namespace Configuration
{
    ConfigHandler::ConfigHandler()
    {
        _log = logging::Logger::get_instance();
    }

    void ConfigHandler::parse(const std::string &path) {
        YAML::Node config;
        try {
            _baseNode = YAML::LoadFile(path);
            _ip = _baseNode["network"]["ip"].as<std::string>();
            _port = _baseNode["network"]["port"].as<uint16_t>();
            _maxPlayers = _baseNode["general"]["max_players"].as<uint32_t>();
            _motd = _baseNode["general"]["motd"].as<std::string>();
        }
        catch (const std::exception &e) {
            LERROR("Config parsing failed, exiting now!");
            LERROR(e.what());
            exit(1); // TODO: Use an exception
        }
    }

    const std::string &ConfigHandler::getIP() const
    {
        return _ip;
    }

    uint16_t ConfigHandler::getPort() const
    {
        return _port;
    }

    uint32_t ConfigHandler::getMaxPlayers() const
    {
        return _maxPlayers;
    }

    const std::string &ConfigHandler::getMotd() const
    {
        return _motd;
    }
}