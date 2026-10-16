#include <FrenchieCoreECSTests.hpp>
#include <FrenchieCoreECS.hpp>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

using namespace Frenchie::Core::ECS;

namespace
{
    void require(bool _Condition, const std::string& _Message)
    {
        if(!_Condition)
            throw std::runtime_error("Entity registry test failed: " + _Message);
    }

    struct RegistryTestPosition
    {
        explicit RegistryTestPosition(int _Value) : value(_Value) {}
        int value;
    };

    struct RegistryTestName
    {
        explicit RegistryTestName(std::string _Value) : value(std::move(_Value)) {}
        std::string value;
    };

    struct RegistryTestMoveOnly
    {
        explicit RegistryTestMoveOnly(std::unique_ptr<int> _Value) : value(std::move(_Value)) {}
        std::unique_ptr<int> value;
    };

    struct RegistryTestThrowing
    {
        explicit RegistryTestThrowing(int _Value) : value(_Value)
        {
            if(_Value < 0)
                throw std::runtime_error("requested test construction failure");
        }

        int value;
    };
}

void Frenchie::Core::Tests::frenchie_core_ecs_entity_lifecycle_test()
{
    std::cout << "entity registry entity lifecycle test...\n";

    {
        Registry registry;
        const Entity first = registry.create_entity();
        const Entity second = registry.create_entity();
        const Entity third = registry.create_entity();

        require(first.identifier() == 0 && second.identifier() == 1 && third.identifier() == 2,
            "new entities should receive sequential slot indices");
        require(first.version() == 0 && second.version() == 0 && third.version() == 0,
            "new slots should start at generation zero");
        require(registry.is_not_null(first) && registry.is_not_null(second) && registry.is_not_null(third),
            "newly created entities should be alive");
    }

    {
        Registry registry;
        const Entity removed = registry.create_entity();
        registry.remove_entity(removed);

        require(!registry.is_not_null(removed), "removed entity should no longer be alive");
        registry.remove_entity(removed);
        require(!registry.is_not_null(removed), "removing a stale entity again should have no effect");

        const Entity reused = registry.create_entity();
        require(reused.identifier() == removed.identifier(),
            "the most recently removed slot should be reused");
        require(reused.version() == removed.version() + 1,
            "reused slot should have an incremented generation");
        require(registry.is_not_null(reused) && !registry.is_not_null(removed),
            "reusing a slot should invalidate the old entity handle");

        registry.remove_entity(removed);
        require(registry.is_not_null(reused),
            "removing an old handle after slot reuse should not remove the new entity");
    }

    {
        Registry registry;
        const Entity first = registry.create_entity();
        const Entity second = registry.create_entity();
        const Entity third = registry.create_entity();
        const Entity fourth = registry.create_entity();

        registry.remove_entity(second);
        registry.remove_entity(fourth);
        registry.remove_entity(third);

        const Entity reusedThird = registry.create_entity();
        const Entity reusedFourth = registry.create_entity();
        const Entity reusedSecond = registry.create_entity();

        require(reusedThird.identifier() == third.identifier() &&
                reusedFourth.identifier() == fourth.identifier() &&
                reusedSecond.identifier() == second.identifier(),
            "freed slots should be reused in last-in-first-out order");
        require(registry.is_not_null(first) && registry.is_not_null(reusedThird) &&
                registry.is_not_null(reusedFourth) && registry.is_not_null(reusedSecond),
            "reusing multiple freed slots should preserve all live entities");
        require(!registry.is_not_null(second) && !registry.is_not_null(third) && !registry.is_not_null(fourth),
            "old handles should remain invalid after multiple slots are reused");
    }

    {
        Registry registry;
        const Entity first = registry.create_entity();
        const Entity second = registry.create_entity();

        registry.remove_entity(first);
        registry.remove_entity(first);

        const Entity reused = registry.create_entity();
        const Entity appended = registry.create_entity();
        require(reused.identifier() == first.identifier(),
            "double removal should leave the slot reusable exactly once");
        require(appended.identifier() == 2,
            "double removal must not put the same slot into the free list twice");
        require(registry.is_not_null(reused) && registry.is_not_null(second) && registry.is_not_null(appended),
            "double removal must preserve all live entities");
    }
}

void Frenchie::Core::Tests::frenchie_core_ecs_components_test()
{
    std::cout << "entity registry components test...\n";

    Registry registry;
    const Entity first = registry.create_entity();
    const Entity second = registry.create_entity();
    const Entity third = registry.create_entity();

    registry.create_component<RegistryTestPosition>(first, 10);
    registry.create_component<RegistryTestPosition>(second, 20);
    registry.create_component<RegistryTestPosition>(third, 30);
    registry.create_component<RegistryTestName>(first, std::string("first"));
    registry.create_component<RegistryTestName>(second, std::string("second"));
    registry.create_component<RegistryTestName>(third, std::string("third"));

    auto firstPosition = registry.request_component<RegistryTestPosition>(first);
    auto secondPosition = registry.request_component<RegistryTestPosition>(second);
    auto thirdPosition = registry.request_component<RegistryTestPosition>(third);
    require(firstPosition != nullptr && firstPosition->value == 10,
        "component lookup should return the first entity's component");
    require(secondPosition != nullptr && secondPosition->value == 20,
        "component lookup should return the second entity's component");
    require(thirdPosition != nullptr && thirdPosition->value == 30,
        "component lookup should return the third entity's component");

    auto firstName = registry.request_component<RegistryTestName>(first);
    auto secondName = registry.request_component<RegistryTestName>(second);
    auto thirdName = registry.request_component<RegistryTestName>(third);
    require(firstName != nullptr && firstName->value == "first" &&
            secondName != nullptr && secondName->value == "second" &&
            thirdName != nullptr && thirdName->value == "third",
        "different component types should have independent per-entity storage");

    registry.create_component<RegistryTestPosition>(first, 99);
    firstPosition = registry.request_component<RegistryTestPosition>(first);
    require(firstPosition != nullptr && firstPosition->value == 10,
        "creating a duplicate component should not replace the existing component");

    registry.create_component<RegistryTestMoveOnly>(first, std::make_unique<int>(42));
    std::unique_ptr<int> duplicateArgument = std::make_unique<int>(7);
    registry.create_component<RegistryTestMoveOnly>(first, std::move(duplicateArgument));
    auto moveOnlyDuplicate = registry.request_component<RegistryTestMoveOnly>(first);
    require(duplicateArgument != nullptr && *duplicateArgument == 7 &&
            moveOnlyDuplicate != nullptr && moveOnlyDuplicate->value != nullptr &&
            *moveOnlyDuplicate->value == 42,
        "duplicate insertion should neither replace the component nor consume an unused rvalue argument");

    registry.remove_component<RegistryTestPosition>(second);
    require(registry.request_component<RegistryTestPosition>(second) == nullptr,
        "removing a component should make it unavailable on that entity");
    registry.remove_component<RegistryTestPosition>(second);
    registry.remove_component<RegistryTestName>(third);
    registry.remove_component<RegistryTestName>(third);
    require(registry.request_component<RegistryTestName>(third) == nullptr,
        "removing absent components repeatedly should be harmless");
    firstPosition = registry.request_component<RegistryTestPosition>(first);
    thirdPosition = registry.request_component<RegistryTestPosition>(third);
    require(firstPosition != nullptr && firstPosition->value == 10 &&
            thirdPosition != nullptr && thirdPosition->value == 30,
        "removing a component should preserve other entities' components after packed storage moves");

    auto moveOnly = registry.request_component<RegistryTestMoveOnly>(first);
    require(moveOnly != nullptr && moveOnly->value != nullptr && *moveOnly->value == 42,
        "component creation should forward move-only constructor arguments");

    {
        Registry compactingRegistry;
        const Entity compactFirst = compactingRegistry.create_entity();
        const Entity compactMiddle = compactingRegistry.create_entity();
        const Entity compactLast = compactingRegistry.create_entity();

        compactingRegistry.create_component<RegistryTestPosition>(compactFirst, 1);
        compactingRegistry.create_component<RegistryTestPosition>(compactMiddle, 2);
        compactingRegistry.create_component<RegistryTestPosition>(compactLast, 3);
        compactingRegistry.create_component<RegistryTestName>(compactFirst, std::string("one"));
        compactingRegistry.create_component<RegistryTestName>(compactMiddle, std::string("two"));
        compactingRegistry.create_component<RegistryTestName>(compactLast, std::string("three"));

        compactingRegistry.remove_entity(compactMiddle);
        auto compactFirstPosition = compactingRegistry.request_component<RegistryTestPosition>(compactFirst);
        auto compactLastPosition = compactingRegistry.request_component<RegistryTestPosition>(compactLast);
        auto compactFirstName = compactingRegistry.request_component<RegistryTestName>(compactFirst);
        auto compactLastName = compactingRegistry.request_component<RegistryTestName>(compactLast);
        require(compactingRegistry.request_component<RegistryTestPosition>(compactMiddle) == nullptr &&
                compactingRegistry.request_component<RegistryTestName>(compactMiddle) == nullptr,
            "removing an entity should remove each of its components");
        require(compactFirstPosition != nullptr && compactFirstPosition->value == 1 &&
                compactLastPosition != nullptr && compactLastPosition->value == 3 &&
                compactFirstName != nullptr && compactFirstName->value == "one" &&
                compactLastName != nullptr && compactLastName->value == "three",
            "entity removal should preserve the entities moved during packed storage compaction");
    }

    {
        Registry moveOnlyRegistry;
        const Entity moveFirst = moveOnlyRegistry.create_entity();
        const Entity moveMiddle = moveOnlyRegistry.create_entity();
        const Entity moveLast = moveOnlyRegistry.create_entity();
        moveOnlyRegistry.create_component<RegistryTestMoveOnly>(moveFirst, std::make_unique<int>(1));
        moveOnlyRegistry.create_component<RegistryTestMoveOnly>(moveMiddle, std::make_unique<int>(2));
        moveOnlyRegistry.create_component<RegistryTestMoveOnly>(moveLast, std::make_unique<int>(3));

        moveOnlyRegistry.remove_component<RegistryTestMoveOnly>(moveMiddle);
        auto moveFirstComponent = moveOnlyRegistry.request_component<RegistryTestMoveOnly>(moveFirst);
        auto moveLastComponent = moveOnlyRegistry.request_component<RegistryTestMoveOnly>(moveLast);
        require(moveOnlyRegistry.request_component<RegistryTestMoveOnly>(moveMiddle) == nullptr &&
                moveFirstComponent != nullptr && moveFirstComponent->value != nullptr &&
                *moveFirstComponent->value == 1 &&
                moveLastComponent != nullptr && moveLastComponent->value != nullptr &&
                *moveLastComponent->value == 3,
            "packed storage compaction should support move-only component types");
    }

    {
        Registry throwingRegistry;
        const Entity throwingEntity = throwingRegistry.create_entity();
        throwingRegistry.create_component<RegistryTestThrowing>(throwingEntity, -1);
        require(throwingRegistry.request_component<RegistryTestThrowing>(throwingEntity) == nullptr,
            "a component whose constructor throws should not appear in the registry");

        throwingRegistry.create_component<RegistryTestThrowing>(throwingEntity, 5);
        auto recoveredComponent = throwingRegistry.request_component<RegistryTestThrowing>(throwingEntity);
        require(recoveredComponent != nullptr && recoveredComponent->value == 5,
            "the component pool should remain usable after a constructor failure");
    }

    registry.remove_entity(third);
    require(registry.request_component<RegistryTestPosition>(third) == nullptr &&
            registry.request_component<RegistryTestName>(third) == nullptr,
        "removing an entity should remove all of its components");

    registry.create_component<RegistryTestPosition>(third, 50);
    registry.remove_component<RegistryTestPosition>(third);
    require(registry.request_component<RegistryTestPosition>(third) == nullptr,
        "stale entity handles should not create or remove components");

    const Entity reused = registry.create_entity();
    require(reused.identifier() == third.identifier() && reused.version() != third.version(),
        "removed entity slots should be reused with a new generation");
    require(registry.request_component<RegistryTestPosition>(reused) == nullptr,
        "a reused entity slot should not inherit components from its previous entity");

    registry.remove_component<RegistryTestPosition>(third);
    require(registry.is_not_null(reused) &&
            registry.request_component<RegistryTestPosition>(reused) == nullptr,
        "removing a component through a stale handle must not affect the reused entity");

    registry.create_component<RegistryTestPosition>(reused, 40);
    auto reusedPosition = registry.request_component<RegistryTestPosition>(reused);
    require(reusedPosition != nullptr && reusedPosition->value == 40 &&
            registry.request_component<RegistryTestPosition>(third) == nullptr,
        "component lookup should distinguish a reused entity from its stale handle");

    const Entity invalid(0, 9999);
    require(registry.request_component<RegistryTestPosition>(invalid) == nullptr,
        "component lookup on an invalid entity handle should return null");
    registry.create_component<RegistryTestPosition>(invalid, 60);
    registry.remove_component<RegistryTestPosition>(invalid);
    reusedPosition = registry.request_component<RegistryTestPosition>(reused);
    require(registry.request_component<RegistryTestPosition>(invalid) == nullptr &&
            reusedPosition != nullptr && reusedPosition->value == 40,
        "component operations on invalid handles should have no effect on live entities");
}

void Frenchie::Core::Tests::frenchie_core_ecs_view_test()
{
    std::cout << "entity registry view test...\n";

    Registry registry;
    const Entity first = registry.create_entity();
    const Entity second = registry.create_entity();
    const Entity third = registry.create_entity();

    registry.create_component<RegistryTestPosition>(first, 10);
    registry.create_component<RegistryTestPosition>(second, 20);
    registry.create_component<RegistryTestPosition>(third, 30);
    registry.create_component<RegistryTestName>(first, std::string("first"));
    registry.create_component<RegistryTestName>(third, std::string("third"));

    std::size_t matchingEntities = 0;
    int positionTotal = 0;
    for(auto [entity, position, name] : registry.view<RegistryTestPosition, RegistryTestName>())
    {
        require((entity == first && name.value == "first") ||
                (entity == third && name.value == "third"),
            "a multi-component view should yield only entities with every requested component");
        position.value += 1;
        positionTotal += position.value;
        ++matchingEntities;
    }

    require(matchingEntities == 2 && positionTotal == 42,
        "a multi-component view should iterate matching entities and expose mutable component references");
    require(registry.request_component<RegistryTestPosition>(first)->value == 11 &&
            registry.request_component<RegistryTestPosition>(third)->value == 31,
        "mutations through a view should update the stored components");

    std::size_t singleComponentEntities = 0;
    for(auto [entity, position] : registry.view<RegistryTestPosition>())
    {
        (void)entity;
        (void)position;
        ++singleComponentEntities;
    }
    require(singleComponentEntities == 3,
        "a single-component view should visit all entities in that component pool");

    std::size_t emptyViewEntities = 0;
    for(auto [entity, component] : registry.view<RegistryTestThrowing>())
    {
        (void)entity;
        (void)component;
        ++emptyViewEntities;
    }
    require(emptyViewEntities == 0,
        "a view over an empty component pool should have no elements");
}

void Frenchie::Core::Tests::frenchie_core_ecs_entity_iteration_test()
{
    std::cout << "entity registry entity iteration test...\n";

    Registry registry;
    const Entity first = registry.create_entity();
    const Entity removed = registry.create_entity();
    const Entity third = registry.create_entity();
    registry.remove_entity(removed);
    const Entity reused = registry.create_entity();

    std::size_t entityCount = 0;
    bool foundFirst = false;
    bool foundThird = false;
    bool foundReused = false;
    for(const Entity entity : registry.each())
    {
        foundFirst = foundFirst || entity == first;
        foundThird = foundThird || entity == third;
        foundReused = foundReused || entity == reused;
        require(entity != removed, "entity iteration should skip removed entity handles");
        ++entityCount;
    }

    require(entityCount == 3 && foundFirst && foundThird && foundReused,
        "entity iteration should include every live entity, including entities without components");
}
