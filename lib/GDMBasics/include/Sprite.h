//
// Created by elly_sparky on 02/01/24.
//

#ifndef GDMATE_SPRITE_H
#define GDMATE_SPRITE_H

#include "GDMBasics.h"
#include <string>

namespace mate
{
class Sprite : public Component
{
  private:
    sf::Texture _texture;
    std::shared_ptr<ord_sprite> _sprite;
    std::weak_ptr<Game> _game_manager;

    bool _actualize = true;

  public:
    // Constructor
    explicit Sprite(const std::weak_ptr<Element> &parent);
    ~Sprite() = default;

    // Simple methods
    void setTexture(const std::string &filename)
    {
        _texture.loadFromFile(filename);
    }

    [[maybe_unused]] void setColor(sf::Color color)
    {
        _sprite->sprite.setColor(color);
    }

    std::shared_ptr<const ord_sprite> getSprite() const
    {
        return _sprite;
    }

    [[maybe_unused]] void setColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha)
    {
        _sprite->sprite.setColor(sf::Color(red, green, blue, alpha));
    }

    [[maybe_unused]] void setSpriteDepth(unsigned int depth)
    {
        _sprite->depth = depth;
    }

    [[maybe_unused]] int getElementDepth() const
    {
        if (auto spt_parent = _parent.lock())
        {
            return spt_parent->depth;
        }
        return INT_MIN;
    }

    [[maybe_unused]] unsigned int getSpriteDepth() const
    {
        return _sprite->depth;
    }

    [[maybe_unused]] void doActualize(bool actualize)
    {
        _actualize = actualize;
    }

    // Other methods declarations
    [[maybe_unused]] void addDepth(int depth);
    void loop() override;
};
} // namespace mate

#endif // GDMATE_SPRITE_H
