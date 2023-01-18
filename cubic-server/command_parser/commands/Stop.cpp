#include "Stop.hpp"
#include "Server.hpp"
#include "World.hpp"

using namespace command_parser;

void Stop::autocomplete(std::vector<std::string>& args) const {
    logging::Logger::get_instance()->info("autocomplete stop");
}

void Stop::execute(std::vector<std::string>& args) const {
    auto server = Server::getInstance();

    server->getWorldGroup("default")->getWorld("default")->save();
    server->stop();
}

void Stop::help(std::vector<std::string>& args) const {
    logging::Logger::get_instance()->info("/stop");
}
