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

	/**
	 * @brief Superposition detection component.
	 *
	 * Triggers are a component type that executes tasks when they are superposed by another Trigger.
	 */
	class Trigger : public Component, public std::enable_shared_from_this<Trigger>
	{
	private:
		/// Used to know when a Trigger has already made it's superposition checks.
		bool checked;

		bool active;
		ShapeType shape{};

		/// List of existing subscribed (active) Triggers. This are the Triggers that will be checked for superposition.
		static std::list<std::weak_ptr<Trigger>> active_triggers;

		/**
		 * runs a loop to check superposition with all the active Triggers.
		 */
		void runChecks();

		/**
		 * Checks if there is superposition between trigger_a & trigger_b. If there is, it executes both of their
		 * fireTrigger methods.
		 * @param trigger_a Trigger different from trigger_b
		 * @param trigger_b Trigger different from trigger_a
		 */
		static void checkTrigger(const std::shared_ptr<Trigger>& trigger_a, const std::shared_ptr<Trigger>& trigger_b);
	protected:
		/**
		 * Executes a particular task that depends on the particular implementation of Trigger.
		 * @param trigger_by Trigger that's superposed with this one.
		 */
		virtual void fireTrigger(const std::shared_ptr<Trigger>& trigger_by) = 0;
	public:
		// Constructor
		explicit Trigger(const std::weak_ptr<Element> &parent);

		/// offset of the Trigger in respect to the Element that holds it.
		Bounds offset{};

		// Simple methods
		/**
		 * @return Trigger's shape type
		 */
		[[nodiscard]] ShapeType getShape() const { return shape; }
		/**
		 * shape setter method
		 * @param shape New shape of the Trigger
		 */
		void setShape(const ShapeType shape) { this->shape = shape; }
		/**
		 * checked getter
		 * @return checked
		 */
		[[nodiscard]] bool wasChecked() const { return checked; }

		// Other Methods declarations

		/**
		 * Position getter
		 * @return Element world position plus Trigger's offset
		 */
		sf::Vector2f getPosition() const;

		/**
		 * Dimension getter
		 * @return Element's world scale multiplied by Trigger's offset
		 */
		sf::Vector2f getDimensions() const;

		/**
		 * Setter for offset scale
		 * @param width
		 * @param height
		 */
		void setDimensions(float width, float height);

		/**
		 * Setter for offset position
		 * @param x
		 * @param y
		 */
		void setPosition(float x, float y);

		/**
		 * Removes the Trigger from the active Triggers list making it so fireTrigger is never executed.
		 */
		void unsubscribe();

		/**
		 * Adds the trigger to the active triggers list making it so fireTrigger is executed when in superposition with
		 * another Trigger.
		 */
		void subscribe();

		/**
		 * On loop Trigger runs checks with all other subscribed Triggers.
		 */
		void loop() override;

		/**
		 * On renderLoop Triggers clear their checked flag for the next frame.
		 */
		void renderLoop() override;
	};

	class EmptyTrigger : public Trigger
	{
	protected:
		void fireTrigger(const std::shared_ptr<Trigger>& trigger_by) override {}
	public:
		explicit EmptyTrigger(const std::weak_ptr<Element> &parent) : Trigger(parent) {
		}
	};
}

#endif //TRIGGER_H
