#include "GDMBasics.h"
#include <gtest/gtest.h>

/**
 * Tests Triggers coordinates by moving it and the element that holds it
 */
TEST(TriggersTest, TriggerCoords)
{
    std::shared_ptr<mate::EmptyTrigger> empty_trigger;
    {
        auto room = std::make_shared<mate::Room>();
        room->setPosition(0, 0);
        room->setScale(1, 1);

        auto element = room->addElement();
        element->setPosition(0, 0);
        element->setScale(1, 1);

        empty_trigger = element->addComponent<mate::EmptyTrigger>();
        empty_trigger->setPosition(5, -5);
        empty_trigger->setDimensions(2, 0.5f);

        EXPECT_EQ(empty_trigger->getPosition(), sf::Vector2f (5, -5));
        EXPECT_EQ(empty_trigger->getDimensions(), sf::Vector2f (2, 0.5f));

        element->setPosition(-6, 5);
        element->setScale(0.1f, 2);

        EXPECT_EQ(empty_trigger->getPosition(), sf::Vector2f (-1, 0));
        EXPECT_EQ(empty_trigger->getDimensions(), sf::Vector2f (0.2f, 1));
    }
    EXPECT_EQ(empty_trigger->getPosition(), sf::Vector2f (0, 0));
    EXPECT_EQ(empty_trigger->getDimensions(), sf::Vector2f (0, 0));
}

namespace mate{
    class TestTrigger : public Trigger{
        public:
        explicit TestTrigger(const std::weak_ptr<Element> &parent) : Trigger(parent){}
        static int count;
        void fireTrigger(const std::shared_ptr<Trigger>& trigger_by) override
        {
            count++;
        }
    };

    int TestTrigger::count = 0;
}

TEST(TriggersTest, TriggerActivation){
    int starting_count = mate::TestTrigger::count;

    auto room = std::make_shared<mate::Room>();
    auto game = mate::Game::getGame(400, 400, "", room);
    room->setScale(1, 1);
    room->setPosition(0, 0);

    // CIRCLE TestTrigger on 5, 5
    auto element_a = room->addElement();
    auto test_trigger_a = element_a->addComponent<mate::TestTrigger>();
    test_trigger_a->setDimensions(2, 2);
    test_trigger_a->setPosition(5, 5);
    test_trigger_a->setShape(mate::ShapeType::CIRCLE);
    test_trigger_a->subscribe();

    // RECTANGLE TestTrigger on -5, 5
    auto element_b = room->addElement();
    auto test_trigger_b = element_b->addComponent<mate::TestTrigger>();
    test_trigger_b->setDimensions(2, 2);
    test_trigger_b->setPosition(-5, 5);
    test_trigger_b->setShape(mate::ShapeType::RECTANGLE);
    test_trigger_b->subscribe();

    ASSERT_EQ(mate::TestTrigger::count, starting_count);

    // CIRCLE EmptyTrigger on -5, -5
    auto element_c = room->addElement();
    auto empty_trigger_a = element_c->addComponent<mate::EmptyTrigger>();
    empty_trigger_a->setDimensions(2, 2);
    element_c->setScale(1, 1);
    empty_trigger_a->setPosition(0, 0);
    element_c->setPosition(-5, -5);
    empty_trigger_a->setShape(mate::ShapeType::CIRCLE);
    empty_trigger_a->subscribe();

    // RECTANGLE EmptyTrigger on 5, -5
    auto element_d = room->addElement();
    auto empty_trigger_b = element_d->addComponent<mate::EmptyTrigger>();
    empty_trigger_b->setDimensions(2, 2);
    element_d->setScale(1, 1);
    empty_trigger_b->setPosition(0, 0);
    element_d->setPosition(5, -5);
    empty_trigger_b->setShape(mate::ShapeType::RECTANGLE);
    empty_trigger_b->subscribe();

    room->loop();
    room->renderLoop();
    EXPECT_EQ(mate::TestTrigger::count, starting_count);

    // RECTANGLE moving to CIRCLE
    element_d->setPosition(5, 5);
    room->loop();
    room->renderLoop();
    EXPECT_EQ(mate::TestTrigger::count, starting_count+1);

    element_d->setPosition(4, 4);
    room->loop();
    room->renderLoop();
    EXPECT_EQ(mate::TestTrigger::count, starting_count+2);

    element_d->setPosition(3.001, 3.001);
    room->loop();
    room->renderLoop();
    EXPECT_EQ(mate::TestTrigger::count, starting_count+2);

    // RECTANGLE moving to RECTANGLE
    element_d->setPosition(-5, 5);
    room->loop();
    room->renderLoop();
    EXPECT_EQ(mate::TestTrigger::count, starting_count+3);

    element_d->setPosition(-6.999, 3.001);
    room->loop();
    room->renderLoop();
    EXPECT_EQ(mate::TestTrigger::count, starting_count+4);

    // RECTANGLE moving to CIRCLE (Empty)
    element_d->setPosition(0, 0);
    element_c->setPosition(0, 0);

    room->loop();
    room->renderLoop();
    EXPECT_EQ(mate::TestTrigger::count, starting_count+4);
    room->loop();
    room->renderLoop();
    EXPECT_EQ(mate::TestTrigger::count, starting_count+4);

    // CIRCLE moving to CIRCLE
    element_c->setPosition(5, 5);
    room->loop();
    room->renderLoop();
    EXPECT_EQ(mate::TestTrigger::count, starting_count+5);

    element_c->setPosition(4, 4);
    room->loop();
    room->renderLoop();
    EXPECT_EQ(mate::TestTrigger::count, starting_count+6);

    element_c->setPosition(3.001, 3.001);
    room->loop();
    room->renderLoop();
    EXPECT_EQ(mate::TestTrigger::count, starting_count+6);
}

