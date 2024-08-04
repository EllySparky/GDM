#include "GDMBasics.h"

//Example of trigger implementation
namespace mate{
    class ColorTrigger : public Trigger {
        // Al destruirse el elemento va a seguir existiendo el sprite por esta referencia, debería ser un weak_ptr
        // No es muy importante porque solo es un código de test, pero esta bueno saberlo.
        // A futuro debería crear Factories de los componentes.
        std::weak_ptr<Sprite> _sprite;
    public:
        explicit ColorTrigger(const std::weak_ptr<Element>& parent) : Trigger(parent, true) {}

        void addSprite(const std::shared_ptr<Sprite>& sprite){
            _sprite = sprite;
        }

        void TriggerIn() override{
            if(auto spt_sprite = _sprite.lock())
                spt_sprite->setColor(sf::Color::Green);
        }
    };
}

void print(){
    auto win = mate::Game::getGame()->getWindow();
    win->setSize(sf::Vector2u (960, 360));
}

int main(){
    auto game = mate::Start();
    game->gameLoop();
}

namespace mate{
    game_instance Start()
    {
        auto mainRoom = std::make_shared<mate::Room>();
        auto game = mate::Game::getGame(480, 360, "MyGame", mainRoom);

        auto camElem = mainRoom->AddElement();
        //*camera component
        auto camera = camElem->addComponent<mate::Camera>();
        camera->setScaleType(mate::Camera::ScaleType::REVEAL);

        auto sptElem0 = mainRoom->AddElement();
        //*sprite component
        auto sprite1 = sptElem0->addComponent<mate::Sprite>();
        sprite1->setColor(sf::Color::Red);
        //*input action component
        auto input1 = sptElem0->addComponent<mate::InputActions>();
        input1->AddInput(sf::Keyboard::Right, &mate::Element::move, sptElem0, 5.f, 0.f);
        input1->AddInput(sf::Keyboard::Left, &mate::Element::move, sptElem0, -5.f, 0.f);
        input1->AddInput(sf::Keyboard::Up, &mate::Element::move, sptElem0, 0.f, -5.f);
        input1->AddInput(sf::Keyboard::Down, &mate::Element::move, sptElem0, 0.f, 5.f);
        //TriggerShooter
        auto shooter = sptElem0->addComponent<mate::TriggerShooter>();
        shooter->shape = mate::RECTANGLE;
        shooter->setDimensionOffset(64, 64);

        auto child0 = sptElem0->AddChild();
        child0->setDepth(-1);
        child0->setPosition(-20, 0);
        child0->setScale(0.5, 0.5);
        child0->addComponent<mate::Sprite>();

        auto child1 = sptElem0->AddChild();
        child1->setDepth(-1);
        child1->setPosition(55, 0);
        child1->setScale(0.5, 0.5);
        child1->addComponent<mate::Sprite>();

        auto sptElem1 = mainRoom->AddElement();
        sptElem1->setPosition(0, 0);
        sptElem1->setScale(2, 2);
        sptElem1->setDepth(-2);
        //Sprite component
        auto sprite2 = sptElem1->addComponent<mate::Sprite>();
        sprite2->setTexture("../Circle.png");
        sprite2->setColor(sf::Color::Magenta);
        //sprite2->setDepth(-10);
        //ColorTrigger
        auto color = std::make_unique<ColorTrigger>(sptElem1);
        //input1->AddInput(sf::Keyboard::W, &mate::Game::RemoveTrigger, game, color->getID());
        color->addSprite(sprite2);
        color->shape = mate::CIRCLE;
        color->setDimensionOffset(64, 64);
        game->AddTrigger(std::move(color));


        input1->AddInput(sf::Keyboard::W, &mate::Element::Destroy, sptElem1);
        input1->AddInput(sf::Keyboard::A, &mate::Element::Destroy, child0);
        input1->AddInput(sf::Keyboard::D, &mate::Element::Destroy, child1);
        input1->AddInput(sf::Keyboard::S, &print);
        input1->AddInput(sf::Keyboard::Space, &mate::Sprite::setColor, sprite2, sf::Color::Magenta);


        return game;
    }
}
