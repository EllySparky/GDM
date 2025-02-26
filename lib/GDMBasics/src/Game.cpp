//
// Created by elly_sparky on 23/01/24.
//

#include "Basics.h"

namespace mate
{
std::shared_ptr<Game> Game::_instance = nullptr;

void Game::setWindowView(const sf::View& view_, const u_int id_) const
{
    if (id_ == 0)
    {
        _main_render_target.target->setView(view_);
        return;
    }
    for (const auto &target : _secondary_targets)
    {
        if (target.id == id_)
        {
            target.target->setView(view_);
            return;
        }
    }
}

sf::Vector2i Game::getWindowPosition(const uint id_) const
{
    if (id_ == 0)
    {
        return _main_render_target.target->getPosition();
    }
    for (const auto &target : _secondary_targets)
    {
        if (target.id == id_)
        {
            return target.target->getPosition();
        }
    }
    return {0, 0};
}

void Game::setWindowPosition(const int x, const int y, const uint id_) const
{
    if (id_ == 0)
    {
        _main_render_target.target->setPosition(sf::Vector2i(x, y));
        return;
    }
    for (const auto &target : _secondary_targets)
    {
        if (target.id == id_)
        {
            target.target->setPosition(sf::Vector2i(x, y));
            return;
        }
    }
}

sf::Vector2u Game::getWindowSize(const uint id_) const
{
    if (id_ == 0)
    {
        return _main_render_target.target->getSize();
    }
    for (const auto &target : _secondary_targets)
    {
        if (target.id == id_)
        {
            return target.target->getSize();
        }
    }
    return {0, 0};
}

void Game::setWindowSize(const int x, const int y, const uint id_) const
{
    if (id_ == 0)
    {
        _main_render_target.target->setSize(sf::Vector2u(x, y));
        return;
    }
    for (const auto &target : _secondary_targets)
    {
        if (target.id == id_)
        {
            target.target->setSize(sf::Vector2u(x, y));
            return;
        }
    }
}

std::shared_ptr<Game> Game::getGame()
{
    if (!_instance)
    {
        _instance = std::shared_ptr<Game>(new Game());
        _instance->addRoom(std::move(std::make_shared<Room>()));
    }
    return _instance;
}

std::shared_ptr<Game> Game::getGame(const int win_width_, const int win_height_, const std::string &game_name_,
                                    std::shared_ptr<Room> main_room_)
{
    if (!_instance)
    {
        _instance = std::shared_ptr<Game>(new Game());
    }
    _instance->_main_render_target.target->setSize(sf::Vector2u(win_width_, win_height_));
    _instance->_main_render_target.target->setTitle(game_name_);
    _instance->_rooms.push_back(main_room_);
    _instance->_active_room = std::move(main_room_);
    return _instance;
}

[[maybe_unused]] std::shared_ptr<Game> Game::getGame(const int win_width_, const int win_height_, const std::string &game_name_,
                                                     std::list<std::shared_ptr<Room>> &rooms_list_)
{
    if (!_instance)
    {
        _instance = std::shared_ptr<Game>(new Game());
    }
    _instance->_main_render_target.target->setSize(sf::Vector2u(win_width_, win_height_));
    _instance->_main_render_target.target->setTitle(game_name_);
    _instance->_rooms.merge(rooms_list_);
    if (!rooms_list_.empty())
    {
        _instance->_active_room = rooms_list_.front();
    }
    return _instance;
}

void Game::draw(const std::shared_ptr<const ord_sprite> &sprite_, const u_int id_) const
{
    if (id_ == 0)
    {
        _main_render_target.target->draw(sprite_->sprite);
    }
    else
    {
        for (const auto &target : _secondary_targets)
        {
            target.target->draw(sprite_->sprite);
        }
    }
}

u_int Game::addSecondaryTarget(const sf::View& view_, const std::string &title)
{
    render_target new_target;
    u_int id = new_target.id;
    new_target.target = std::make_unique<sf::RenderWindow>(sf::VideoMode(800, 400), title);
    new_target.target->setView(view_);
    _secondary_targets.push_back(std::move(new_target));
    return id;
}

[[maybe_unused]] void Game::switchRoom(const int position_)
{
    if (position_ < _rooms.size())
    {
        auto it = _rooms.begin();
        std::advance(it, position_);
        _active_room = *it;
    }
}

[[noreturn]] void Game::gameLoop() const
{
    _main_render_target.target->setFramerateLimit(60);
    do
    {
        runSingleFrame();
    } while (_main_render_target.target->isOpen());
    exit(0);
}

void Game::runSingleFrame() const
{
    // Event Pooling
    sf::Event event{};
    while (_main_render_target.target->pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            exit(EXIT_SUCCESS);
        case sf::Event::Resized:
            _active_room->windowResizeEvent();
            break;
        default:
            break;
        }
    }
    for (const auto &target : _secondary_targets)
    {
        sf::Event event{};
        while (target.target->pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                target.target->setVisible(false);
            case sf::Event::Resized:
                _active_room->windowResizeEvent();
                break;
            default:
                break;
            }
        }
    }

    _active_room->loop();

    _main_render_target.target->clear();
    for (const auto &target : _secondary_targets)
    {
        target.target->clear();
    }

    _active_room->renderLoop();

    for (const auto &target : _secondary_targets)
    {
        target.target->display();
    }
    _main_render_target.target->display();
}
} // namespace mate
