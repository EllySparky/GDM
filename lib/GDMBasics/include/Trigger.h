/**
 * @brief Trigger class declaration
 * @file Trigger.h
 */

#ifndef TRIGGER_H
#define TRIGGER_H

#include "Basics.h"
#include <list>
#include <memory>

namespace mate {

	/// Shapes supported for trigger detection
	enum ShapeType
	{
		RECTANGLE, ///< Calculated from the top left corner + height/width
		CIRCLE ///< Calculated from center + radius [max dimension]
	};

	class Trigger : public Component, public std::enable_shared_from_this<Trigger>
	{
	private:
		bool checked, active;
		ShapeType shape{};
		static std::list<std::weak_ptr<Trigger>> active_triggers;
		void runChecks();
		static void checkTrigger(const std::shared_ptr<Trigger>& trigger_a, const std::shared_ptr<Trigger>& trigger_b);
	protected:
		virtual void fireTrigger(const std::shared_ptr<Trigger>& trigger_by) = 0;
	public:
		Bounds offset{};
		explicit Trigger(const std::weak_ptr<Element> &parent);
		[[nodiscard]] ShapeType getShape() const { return shape; }
		void setShape(const ShapeType shape) { this->shape = shape; }
		[[nodiscard]] bool wasChecked() const { return checked; }
		sf::Vector2f getPosition() const;
		sf::Vector2f getDimensions() const;
		void setDimensions(float width, float height);
		void setPosition(float x, float y);
		void unsubscribe();
		void subscribe();
		void loop() override;
		void renderLoop() override;
	};

}

#endif //TRIGGER_H
