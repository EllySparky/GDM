/**
 * @brief Trigger class methods definitions
 * @file Trigger.cpp
*/

#include "Trigger.h"
#include <cmath>

namespace mate
{
	/**
	 * Checks superposition between two rectangle shapes
	 * @param rect1_pos position of the first rectangle's left top corner
	 * @param rect1_dim scale of the first rectangle
	 * @param rect2_pos position of the second rectangle's left top corner
	 * @param rect2_dim scale of the second rectangle
	 * @return true if superposed, false otherwise
	 */
	bool rectangleToRectangleCheck(sf::Vector2f rect1_pos, sf::Vector2f rect1_dim, sf::Vector2f rect2_pos, sf::Vector2f rect2_dim);

	/**
	 * Checks superposition between a rectangle shape and a circle shape
	 * @param circ_pos center of the circle
	 * @param circ_dim scale of the circle (only the biggest of it's two dimensions will be used)
	 * @param rect_pos position of the rectangle's left top corner
	 * @param rect_dim scale of the rectangle
	 * @return true if superposed, false otherwise
	 */
	bool circleToRectangleCheck(sf::Vector2f circ_pos, sf::Vector2f circ_dim, sf::Vector2f rect_pos, sf::Vector2f rect_dim);

	/**
	 * Checks superposition between two circle shapes
	 * @param circ1_pos center of the first circle
	 * @param circ1_dim scale of the first circle (only the biggest dimension will be used)
	 * @param circ2_pos center of the second circle
	 * @param circ2_dim scale of the second circle (only the biggest dimension will be used)
	 * @return true if superposed, false otherwise
	 */
	bool circleToCircleCheck(sf::Vector2f circ1_pos, sf::Vector2f circ1_dim, sf::Vector2f circ2_pos, sf::Vector2f circ2_dim);

	std::list<std::weak_ptr<Trigger>> Trigger::active_triggers = {};

	Trigger::Trigger(const std::weak_ptr<Element> &parent) : Component(parent)
	{
		checked = false;
		active = false;
	}

	void Trigger::runChecks()
	{
		for (auto it = active_triggers.begin(); it != active_triggers.end();)
		{
			if (it->expired())
			{
				it = active_triggers.erase(it);
				continue;
			}
			if (auto trigger = it->lock();
				!trigger->wasChecked() && trigger != shared_from_this())
			{
				checkTrigger(trigger, shared_from_this());
			}
			++it;
		}
		checked = true;
	}

	void Trigger::subscribe()
	{
		if (!active) {
			active = true;
			active_triggers.push_back(shared_from_this());
		}
	}

	void Trigger::unsubscribe()
	{
		if (active)
		{
			active = false;
			active_triggers.remove_if([this](const std::weak_ptr<Trigger>& wp) {
				if (const auto sp = wp.lock()) {
					return sp == shared_from_this();
				}
				return false;
			});
		}
	}

	void Trigger::switchActive()
	{
		if (active) { unsubscribe(); }
		else { subscribe(); }
	}


	void Trigger::loop()
	{
		if (active)
		{
			runChecks();
		}
	}

	void Trigger::renderLoop()
	{
		checked = false;
	}

	sf::Vector2f Trigger::getPosition() const
	{
		if (const auto spt_parent = _parent.lock())
		{
			return offset.getPositionBounds(spt_parent->getWorldPosition());
		}
		return {0, 0};
	}

	sf::Vector2f Trigger::getDimensions() const
	{
		if (const auto spt_parent = _parent.lock())
		{
			return offset.getDimensionBounds(spt_parent->getWorldScale());
		}
		return {0, 0};
	}

	void Trigger::setDimensions(const float width, const float height)
	{
		offset.rect_bounds.width = width;
		offset.rect_bounds.height = height;
	}


	void Trigger::setPosition(const float x, const float y)
	{
		offset.rect_bounds.left = x;
		offset.rect_bounds.top = y;
	}

	// Todo: Rotated rectangles
	// Todo: Add depth to triggers
	void Trigger::checkTrigger(const std::shared_ptr<Trigger>& trigger_a, const std::shared_ptr<Trigger>& trigger_b)
	{
		const sf::Vector2f position_a = trigger_a->getPosition();
		const sf::Vector2f dimensions_a = trigger_a->getDimensions();

		const sf::Vector2 position_b = trigger_b->getPosition();
		const sf::Vector2 dimensions_b = trigger_b->getDimensions();

		bool collide = false;
		switch (trigger_a->getShape())
	        {
	        case RECTANGLE:
	            switch (trigger_b->getShape())
	            {
	            case RECTANGLE:
	                // Collision between two rectangles
	                collide = rectangleToRectangleCheck(position_a, dimensions_a, position_b, dimensions_b);
	                break;
	            case CIRCLE:
	                // B is a circle, A is a rectangle
	            	collide = circleToRectangleCheck(position_b, dimensions_b, position_a, dimensions_a);
	                break;
	            }
	            break;
	        case CIRCLE:
	            switch (trigger_b->getShape())
	            {
	            case RECTANGLE:
	            	// A is a circle, B is a rectangle
	            	collide = circleToRectangleCheck(position_a, dimensions_a, position_b, dimensions_b);
	                break;
	            case CIRCLE:
	                // Collision between circles
	                collide = circleToCircleCheck(position_a, dimensions_a, position_b, dimensions_b);
	                break;
	            }
	            break;
	        }

		if (collide){
			trigger_b->fireTrigger(trigger_a);
			trigger_a->fireTrigger(trigger_b);
		}
	}

	bool rectangleToRectangleCheck(const sf::Vector2f rect1_pos, const sf::Vector2f rect1_dim, const sf::Vector2f rect2_pos, const sf::Vector2f rect2_dim)
	{
		return rect1_pos.x + rect1_dim.x > rect2_pos.x && rect1_pos.x < rect2_pos.x + rect2_dim.x &&
		       rect1_pos.y < rect2_pos.y + rect2_dim.y && rect1_pos.y + rect1_dim.y > rect2_pos.y;
	}

	bool circleToCircleCheck(sf::Vector2f circ1_pos, const sf::Vector2f circ1_dim, sf::Vector2f circ2_pos, const sf::Vector2f circ2_dim)
	{
		const float circ1_rad = std::max(circ1_dim.x, circ1_dim.y) / 2;
		circ1_pos.x += circ1_rad;
		circ1_pos.y += circ1_rad;

		const float circ2_rad = std::max(circ2_dim.x, circ2_dim.y) / 2;
		circ2_pos.x += circ2_rad;
		circ2_pos.y += circ2_rad;

		const double distance = std::sqrt(std::pow(circ1_pos.x - circ2_pos.x, 2) + std::pow(circ1_pos.y - circ2_pos.y, 2));
		return circ1_rad + circ2_rad > distance;
	}


	bool circleToRectangleCheck(sf::Vector2f circ_pos, const sf::Vector2f circ_dim, const sf::Vector2f rect_pos, const sf::Vector2f rect_dim)
	{
		// Use the center of the circle instead of the corner, also stores the radius
		const float radius = std::max(circ_dim.x, circ_dim.y) / 2;
		circ_pos.x += radius;
		circ_pos.y += radius;

		double distance_x = rect_pos.x + rect_dim.x / 2 - circ_pos.x;
		short sign = std::signbit(distance_x);
		// If distance_x is positive means circle is on the left, negative means it's on the right
		// Checks if the circle is inside the rectangle using it's nearest horizontal sides
		if (circ_pos.x + ((distance_x >= 0) - (distance_x < 0)) * radius >= rect_pos.x + sign * rect_dim.x != sign &&
			circ_pos.y <= rect_pos.y + rect_dim.y && circ_pos.y >= rect_pos.y)
		{
			return true;
		}

		double distance_y = rect_pos.y + rect_dim.y / 2 - circ_pos.y;
		sign = std::signbit(distance_y);
		// If distance_y is positive means circle is above, negative means it's below
		// Same as before but using the vertical sides
		if (circ_pos.y + ((distance_y >= 0) - (distance_y < 0)) * radius >= rect_pos.y + sign * rect_dim.y != sign &&
			circ_pos.x <= rect_pos.x + rect_dim.x && circ_pos.x >= rect_pos.x)
		{
			return true;
		}

		// Finally checks if the nearest corner of the rectangle is inside the circle
		//  (Which can happen even when both the previous conditions are false)
		// This is checked last to reduce the use of the sqrt function
		distance_x += ((distance_x < 0) - (distance_x >= 0)) * rect_dim.x / 2;
		distance_y += ((distance_y < 0) - (distance_y >= 0)) * rect_dim.y / 2;

		const double distance = std::sqrt(std::pow(distance_y, 2) + std::pow(distance_x, 2));
		return distance < radius;
	}

} // namespace mate
