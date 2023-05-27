#include <cstdint>
#include <memory>
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Server.hpp"
#include "World.hpp"

#include "Chat.hpp"
#include "Client.hpp"
#include "Dimension.hpp"
#include "Player.hpp"
#include "WorldGroup.hpp"
#include "command_parser/commands/Gamemode.hpp"
#include "default/DefaultWorldGroup.hpp"
#include "logging/logging.hpp"

using boost::asio::ip::tcp;

Server::Server():
    _running(false),
    _sockfd(-1),
    _config()
{
    // _config.load("./config.yml");
    // _config.parse("./config.yml");
    // _config.parse(2, (const char * const *){"./CubicServer", "--nogui"});
    // _host = _config.getIP();
    // _port = _config.getPort();
    // _maxPlayer = _config.getMaxPlayers();
    // _motd = _config.getMotd();
    // _enforceWhitelist = _config.getEnforceWhitelist();

    _commands.reserve(11);
    _commands.emplace_back(std::make_unique<command_parser::Help>());
    _commands.emplace_back(std::make_unique<command_parser::QuestionMark>());
    _commands.emplace_back(std::make_unique<command_parser::Stop>());
    _commands.emplace_back(std::make_unique<command_parser::Seed>());
    _commands.emplace_back(std::make_unique<command_parser::DumpChunk>());
    _commands.emplace_back(std::make_unique<command_parser::Log>());
    _commands.emplace_back(std::make_unique<command_parser::Op>());
    _commands.emplace_back(std::make_unique<command_parser::Deop>());
    _commands.emplace_back(std::make_unique<command_parser::Reload>());
    _commands.emplace_back(std::make_unique<command_parser::Time>());
    _commands.emplace_back(std::make_unique<command_parser::Gamemode>());
}

Server::~Server() { }

void Server::launch(const configuration::ConfigHandler &config)
{
    this->_config = config;

    // Initialize the global palette
    _globalPalette.initialize(std::string("blocks-") + MC_VERSION + ".json");
    LINFO("GlobalPalette initialized");

    // Initialize the item converter
    _itemConverter.initialize(std::string("registries-") + MC_VERSION + ".json");
    LINFO("ItemConverter initialized");

    // Initialize default world group
    auto defaultChat = std::make_shared<Chat>();
    _worldGroups.emplace("default", new DefaultWorldGroup(defaultChat));
    _worldGroups.at("default")->initialize();

    // TODO(huntears): Deal with this
    // Initialize default recipes
    // this->_recipes.initialize();

    this->_running = true;

    LINFO("Starting server on {}:{}", _config["ip"], _config["port"]);

    _acceptor = std::make_unique<boost::asio::ip::tcp::acceptor>(_io_context, tcp::endpoint(tcp::v4(), _config["port"].as<uint16_t>()));

    _doAccept();

    _io_context.run();
}

void Server::_doAccept()
{
    _acceptor->async_accept([this](boost::system::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::shared_ptr<Client> _cli(new Client(std::move(socket)));
            _clients.push_back(_cli);
            _cli->run();
        }

        _doAccept();
    });
}

void Server::stop()
{
    static auto num = 0;
    LINFO("Server has received a stop command");
    this->_running = false;
    if (num++ >= 5) {
        exit(1); // Mash that Ctrl-C xd
    }
}

void Server::_acceptLoop()
{
    // struct pollfd pollSet[1];

    // pollSet[0].fd = _sockfd;
    // pollSet[0].events = POLLIN;
    // while (this->_running) {
    //     poll(pollSet, 1, 50);
    //     if (pollSet[0].revents & POLLIN) {
    //         struct sockaddr_in6 clientAddr { };
    //         socklen_t clientAddrSize = sizeof(clientAddr);
    //         int clientFd = accept(_sockfd, reinterpret_cast<struct sockaddr *>(&clientAddr), &clientAddrSize);
    //         if (clientFd == -1) {
    //             throw std::runtime_error(strerror(errno));
    //         }
    //         // Add accepted client to the vector of clients
    //         // auto cli = std::make_shared<Client>(clientFd, clientAddr);
    //         // _clients.push_back(cli);
    //         {
    //             std::lock_guard _(this->clientsMutex);
    //             _clients.push_back(std::make_shared<Client>(clientFd, clientAddr));
    //         }

    //         // Emplace_back is not working, I don't know why
    //         // _clients.emplace_back(clientFd, clientAddr);

    //         // That line is kinda borked, but I'll check one day how to fix it
    //         // auto *cliThread = new std::thread(&Client::networkLoop, &(*cli));
    //         // That is 99.99% a data race, but aight, it will probably
    //         // never happen
    //         // cli->setRunningThread(cliThread);
    //     }

    //     std::lock_guard _(this->clientsMutex);
    //     _clients.erase(
    //         std::remove_if(
    //             _clients.begin(), _clients.end(),
    //             [](const std::shared_ptr<Client> &cli) {
    //                 return cli->isDisconnected();
    //             }
    //         ),
    //         _clients.end()
    //     );
    // }
}

void Server::_stop()
{
    // Disconect all clients
    clientsMutex.lock();
    for (auto &client : _clients)
        client->stop("Server Closed");

    // Needed because the client will not be destroyed if a reference still exists
    this->_clients.clear();

    for (auto &[name, worldGroup] : _worldGroups) {
        worldGroup->stop();
        worldGroup.reset();
    }
    if (this->_sockfd != -1)
        close(this->_sockfd);
    LINFO("Server stopped");
}

/*
**  Reloads the config if no error within the new file
*/
void Server::_reloadConfig()
{
    try {
        _config.load("./config.yml");
    } catch (const configuration::ConfigurationError &e) {
        LERROR(e.what());
        return;
    }
}

/*
**  Reloads the whitelist if no error within the new file
*/
void Server::_reloadWhitelist()
{
    try {
        if (isWhitelistEnabled()) {
            WhitelistHandling::Whitelist whitelistReloaded = WhitelistHandling::Whitelist();
            _whitelist = whitelistReloaded;
        }
    } catch (const std::exception &e) {
        LERROR(e.what());
    }
}

/*
**  Reloads the server. Used in the /reload command.
**  More details in *Reload.hpp*.
*/
void Server::reload()
{
    _reloadConfig();
    _reloadWhitelist();
    _enforceWhitelistOnReload();
    /* Reload datapacks + plugins */
}

/*
**  If the server gets a /reload, players not on the whitelist
**  must be kicked out from the server if enforce-whitelist is
**   true & the whitelist is in effect
*/
void Server::_enforceWhitelistOnReload()
{
    if (!isWhitelistEnabled() || !isWhitelistEnforce())
        return;
    for (auto [_, worldGroup] : _worldGroups) {
        for (auto [_, world] : worldGroup->getWorlds()) {
            for (auto [_, dim] : world->getDimensions()) {
                for (auto player : dim->getPlayers()) {
                    if (!_whitelist.isPlayerWhitelisted(player->getUuid(), player->getUsername()).first) {
                        player->disconnect("You are not whitelisted on this server.");
                    }
                }
            }
        }
    }
}

std::unordered_map<std::string_view, std::shared_ptr<WorldGroup>> &Server::getWorldGroups() { return _worldGroups; }

const std::unordered_map<std::string_view, std::shared_ptr<WorldGroup>> &Server::getWorldGroups() const { return _worldGroups; }
Recipes &Server::getRecipeSystem(void) noexcept { return (this->_recipes); }
