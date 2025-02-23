#include "GDMBasics.h"

int main()
{
	const auto game = mate::start();
	game->gameLoop();
}

namespace mate
{

	/*
	 * The following example explains how tu implement the Trigger component. This component is by default
	 * "abstract", meaning it's not fully implemented and therefore the user must make its own implementation.
	 * This ColorTrigger class is an example of an implementation that changes the color of a sprite when superposed
	 * by another Trigger.
	 */
	class ColorTrigger : public Trigger
	{
		/*
		 * First we will use a weak_prt to store the Sprite, we should always store other components, elements or rooms
		 * as weak_ptr (You can use shared_ptr but that might lead to later issues).
		 */
		std::weak_ptr<Sprite> _sprite;
		sf::Color color;
	protected:

		/*
		 * All Trigger implementations must override the fireTransition(const std::shared_ptr<Trigger>& trigger_by)
		 * method. In this case we call the setColor method of the sprite with the specified color.
		 */
		void fireTrigger(const std::shared_ptr<Trigger>& trigger_by) override
		{
			if (const auto spt_sprite = _sprite.lock())
				spt_sprite->setColor(color);
		}
	public:
		/*
		 * Trigger implementations must always follow this structure, take only one parameter
		 * (const std::weak_ptr<Element> &parent) and pass it to Trigger (: Trigger(parent)).
		 * You can add whatever you need on the body.
		 */
		explicit ColorTrigger(const std::weak_ptr<Element> &parent) : Trigger(parent)
		{
			color = sf::Color::Green;
		}

		/*
		 * We will make one simple setter function to select the sprite that will be changed.
		 */
		void addSprite(const std::shared_ptr<Sprite> &sprite)
		{
			_sprite = sprite;
		}

		/*
		 * Lastly we add a simple setter function fot the color.
		 */
		void setColor(const sf::Color &color_)
		{
			color = color_;
		}
	};

	game_instance start()
	{
		/*
		 * We start the start() function as per usual by creating a room, a game instance and a camera.
		 */
		const auto main_room = std::make_shared<Room>();
		auto game = Game::getGame(480, 360, "MyGame", main_room);

		const auto cam_element = main_room->addElement();
		const auto camera = cam_element->addComponent<Camera>();
		camera->setScaleType(Camera::ScaleType::REVEAL);

		/*
		 * Now we will make our "player" which consists of a sprite, an EmptyTrigger and inputs to move around.
		 */
        const auto player = main_room->addElement();
		const auto sprite = player->addComponent<Sprite>();
		sprite->setTexture("resources/Circle.png");
		sprite->setColor(sf::Color::Blue);
		sprite->setSpriteDepth(5);
		/*
		 * The player inputs will move it with the arrow keys.
		 */
		const auto inputs = player->addComponent<InputActions>();
		inputs->addInput(sf::Keyboard::Key::Up, &Element::move, player, 0.0f, -5.0f);
		inputs->addInput(sf::Keyboard::Key::Down, &Element::move, player, 0.0f, 5.0f);
		inputs->addInput(sf::Keyboard::Key::Left, &Element::move, player, -5.0f, 0.0f);
		inputs->addInput(sf::Keyboard::Key::Right, &Element::move, player, 5.0f, 0.0f);
		/*
		 * Finally the player trigger will be an EmptyTrigger, which is a type of Trigger that does nothing when fired.
		 */
		const auto player_trigger = player->addComponent<EmptyTrigger>();
		/*
		 * Since the player sprite is a 64px by 64px circle we will make the Trigger fit said area setting it's
		 * dimensions and shape. You can also add an offset to the position relative to the Element with
		 * player_trigger->setPosition() but that won't be necessary here.
		 */
		player_trigger->setDimensions(64, 64);
		player_trigger->setShape(CIRCLE);

		/*
		 * Now we are going to add some extra Triggers that will change the player's color when fired.
		 */
		const auto square_one = main_room->addElement();
		const auto square_two = main_room->addElement();
		const auto circle = main_room->addElement();

		/*
		 * We will add some sprites to them.
		 */
		const auto square_sprite_one = square_one->addComponent<Sprite>();
		square_sprite_one->setTexture("resources/Square.png");
		square_sprite_one->setColor(sf::Color::Green);
		const auto square_sprite_two = square_two->addComponent<Sprite>();
		square_sprite_two->setTexture("resources/Square.png");
		square_sprite_two->setColor(sf::Color::Red);
		const auto circle_sprite = circle->addComponent<Sprite>();
		circle_sprite->setTexture("resources/Circle.png");
		circle_sprite->setColor(sf::Color::Yellow);

		/*
		 * Now we will move this elements initial positions arownd.
		 */
		square_one->move(-200, 0);
		square_two->move(200, 0);
		circle->move(0, -170);

		/*
		 * Finally we will add the triggers to all this elements.
		 */
		const auto trigger_square_one = square_one->addComponent<ColorTrigger>();
		trigger_square_one->setDimensions(64, 64);
		trigger_square_one->setShape(RECTANGLE);
		trigger_square_one->setColor(sf::Color::Green);
		trigger_square_one->addSprite(sprite); // This will turn the player's sprite green on contact

		const auto trigger_square_two = square_two->addComponent<ColorTrigger>();
		trigger_square_two->setDimensions(64, 64);
		trigger_square_two->setShape(RECTANGLE);
		trigger_square_two->setColor(sf::Color::Red);
		trigger_square_two->addSprite(sprite); // This will turn the player's sprite red on contact

		const auto trigger_circle = circle->addComponent<ColorTrigger>();
		trigger_circle->setDimensions(64, 64);
		trigger_circle->setShape(CIRCLE);
		trigger_circle->setColor(sf::Color::Yellow);
		trigger_circle->addSprite(sprite); // This will turn the player's sprite yellow on contact

		/*
		 * To enable a trigger you must call it's subscribe method, to unable it you can call it's unsubscribe method.
		 * Triggers are disabled by default so make sure you enable all of them before you return from this function.
		 */
		player_trigger->subscribe();
		trigger_square_two->subscribe();
		trigger_square_one->subscribe();
		trigger_circle->subscribe();

		/*
		 * Also don't forget to add all your sprites to the visible sprites of the camera.
		 */
		camera->addSprite(sprite);
		camera->addSprite(square_sprite_one);
		camera->addSprite(square_sprite_two);
		camera->addSprite(circle_sprite);

		/*
		 * Lastly, just to add some flavour we will add the spacebar as an input to activate and deactivate the player's
		 * trigger. For that we will use the switchActive Trigger method.
		 */
		// std::static_pointer_cast<typename std::remove_pointer<decltype(obj.get())>::type>(obj);
		const auto aux_pointer = std::static_pointer_cast<Trigger>(player_trigger);
		inputs->addInput(sf::Keyboard::Key::Space, &Trigger::switchActive, aux_pointer);
        return game;
    }
}
