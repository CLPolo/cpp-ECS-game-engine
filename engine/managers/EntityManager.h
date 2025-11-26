/**
 * @file EntityManager.h
 * @brief Core ECS entity and component management system.
 */

#pragma once

#include <array>
#include <cassert>
#include <iostream>
#include <limits>
#include <tuple>
#include <type_traits>
#include <vector>

#include "../core/ComponentStorage.h"
#include "../core/pack.h"

namespace ECSEngine {

// Entity type definitions
using EntityID = size_t;

/**
 * @struct Entity
 * @brief Represents a game entity with a unique ID.
 */
struct Entity {
    EntityID id;
    std::string name;

    Entity()
        : id(0)
        , name("")
    {
    }

    explicit Entity(EntityID entityId, std::string entityName = "")
        : id(entityId)
        , name(std::move(entityName))
    {
    }
};

/**
 * @class EntityManager
 * @brief Manages entities and their components in the ECS architecture.
 * @details This is the core of the ECS engine, providing efficient entity creation,
 * deletion, and component management. Uses a free list for entity reuse and maintains
 * separate storage for each component type.
 *
 * RESOURCE LIFETIME:
 * - Component references (from GetComponent()) are valid until the component is
 *   removed or the entity is destroyed. Do not store component references long-term.
 *
 * - ComponentStorage references (from GetComponentStorage()) are valid for the lifetime
 *   of the EntityManager.
 *
 * - Entity iterators are invalidated when entities are created or removed.
 *
 * - EntityIDs remain stable. Use ValidEntity() to check if an EntityID is still valid.
 */
template <typename... Components> class EntityManager {
public:
    /**
     * @brief Creates a new entity with the given name.
     * @param name Descriptive name for the entity (for debugging)
     * @return Stable EntityID that remains valid until entity is removed
     */
    [[nodiscard]] EntityID CreateEntity(const std::string& name);

    /**
     * @brief Checks if an EntityID is currently valid.
     * @param entity The EntityID to check
     * @return true if the entity exists and is valid
     */
    bool ValidEntity(EntityID entity) const;

    /**
     * @brief Returns an entity name.
     * @param id The EntityID
     * @return A string representing the entity's name.
     */
    const std::string& GetEntityName(EntityID id);

    /**
     * @brief Removes an entity and all its components.
     * @param entity The EntityID to remove
     */
    void RemoveEntity(EntityID entity);

    /**
     * @brief Adds a component to an entity.
     * @param entity The EntityID to add the component to
     * @param component The component to add (passed by value, will be moved)
     */
    template <typename T> void AddComponent(EntityID entity, T component)
    {
        static constexpr size_t COMP_TYPE_ID = Pack<Components...>::template index<T>;
        static_assert(COMP_TYPE_ID != -1);
        assert(ValidEntity(entity) && "AddComp");

        auto& registry = std::get<COMP_TYPE_ID>(mRegistries);
        const size_t compID = mEntityToComponentIdx[entity][COMP_TYPE_ID];
        // Entity should NOT already have this component
        assert(compID == INVALID_COMPONENT_INDEX);
        const size_t newCompID = registry.store(std::forward<T>(component));
        mEntityToComponentIdx[entity][COMP_TYPE_ID] = newCompID;
    }

    /**
     * @brief Removes a component from an entity.
     * @param entity The EntityID to remove the component from
     */
    template <typename T> void RemoveComponent(EntityID entity)
    {
        // Can't remove components from an invalid entity
        if (!ValidEntity(entity) && "RemoveComp")
            return;

        static constexpr size_t COMP_TYPE_ID = Pack<Components...>::template index<T>;
        static_assert(COMP_TYPE_ID != -1);

        // Get the component ID for this entity
        const size_t compID = mEntityToComponentIdx[entity][COMP_TYPE_ID];

        // Only remove if the component exists
        if (compID != INVALID_COMPONENT_INDEX) {
            auto& registry = std::get<COMP_TYPE_ID>(mRegistries);
            registry.remove(compID);
            mEntityToComponentIdx[entity][COMP_TYPE_ID] = INVALID_COMPONENT_INDEX;
        }
    }

    /**
     * @brief Checks if an entity has a specific component.
     * @param entity The EntityID to check
     * @return true if the entity has the component
     */
    template <typename T> bool HasComponent(EntityID entity) const
    {
        static constexpr size_t COMP_TYPE_ID = Pack<Components...>::template index<T>;
        static_assert(COMP_TYPE_ID != -1);
        assert(ValidEntity(entity) && "HasComp");
        return mEntityToComponentIdx[entity][COMP_TYPE_ID] != INVALID_COMPONENT_INDEX;
    }

    /**
     * @brief Gets a reference to a component on an entity.
     * @param entity The EntityID to get the component from
     * @return Reference to the component (see RESOURCE LIFETIME in class docs)
     */
    template <typename T> T& GetComponent(EntityID entity)
    {
        static constexpr size_t COMP_TYPE_ID = Pack<Components...>::template index<T>;
        static_assert(COMP_TYPE_ID != -1);
        assert(ValidEntity(entity) && "GetComp");

        auto& registry = std::get<COMP_TYPE_ID>(mRegistries);
        const size_t compID = mEntityToComponentIdx[entity][COMP_TYPE_ID];
        assert(registry.valid(compID));
        return registry[compID];
    }

    /**
     * @brief Gets direct access to component storage for efficient iteration.
     * @return Reference to the ComponentStorage for type T
     */
    template <typename T> ComponentStorage<T>& GetComponentStorage()
    {
        static constexpr size_t COMP_TYPE_ID = Pack<Components...>::template index<T>;
        static_assert(COMP_TYPE_ID != -1);
        return std::get<COMP_TYPE_ID>(mRegistries);
    }

    using tEntity = std::vector<Entity>;
    using const_iterator = tEntity::const_iterator;
    const_iterator cbegin() const { return mEntities.begin(); }
    const_iterator cend() const { return mEntities.end(); }
    const_iterator begin() { return mEntities.begin(); }
    const_iterator end() { return mEntities.end(); }

private:
    void RemoveComponentByIndex(size_t compTypeID, size_t compID);

    std::vector<Entity> mEntities;
    static constexpr size_t INVALID_COMPONENT_INDEX = std::numeric_limits<size_t>::max();
    static constexpr size_t NUM_COMPONENTS = Pack<Components...>::size;

    std::tuple<ComponentStorage<Components>...> mRegistries;
    std::vector<std::array<EntityID, sizeof...(Components)>>
        mEntityToComponentIdx; // [entID][compID]
    std::vector<EntityID> mFreeList; // free ent ids
    std::vector<bool> mValid; // valid ent ids
};

// Template implementations (must be in header for templates)

template <typename... Components>
EntityID EntityManager<Components...>::CreateEntity(const std::string& name)
{
    EntityID ret;

    // Check if there are any free entity IDs from previously removed entities
    if (!mFreeList.empty()) {

        // Reuse a free entity ID
        ret = mFreeList.back();
        mFreeList.pop_back();
        mValid[ret] = true;

        // Update the entity ID and name in the existing entity slot
        mEntities[ret].id = ret;
        mEntities[ret].name = name;

    } else {

        // No free IDs available, create a new entity at the end
        ret = mEntityToComponentIdx.size();
        mValid.push_back(true);

        // Initialize component index mapping for this entity
        mEntityToComponentIdx.push_back({});
        for (size_t compTypeID = 0; compTypeID < NUM_COMPONENTS; compTypeID++) {
            mEntityToComponentIdx.back()[compTypeID] = INVALID_COMPONENT_INDEX;
        }

        // Add entity to the vector for iteration with its ID and name
        mEntities.push_back(Entity { ret, name });
    }

    return ret;
}

template <typename... Components> bool EntityManager<Components...>::ValidEntity(EntityID id) const
{
    return id < mValid.size() && mValid[id];
}

template <typename... Components>
const std::string& EntityManager<Components...>::GetEntityName(EntityID id)
{
    assert(ValidEntity(id) && "GetName");
    return mEntities[id].name;
}

template <typename... Components>
void EntityManager<Components...>::RemoveComponentByIndex(size_t compTypeID, size_t compID)
{
    /* Helper function to remove component from the correct registry at runtime
     * Uses C++20 templated lambda to map runtime index to compile-time tuple access
     * The lambda takes a template parameter pack Is (indices 0, 1, 2, ..., NUM_COMPONENTS-1)
     * and uses a fold expression to check each compile-time index against the runtime compTypeID
     */
    // Used Claude to figure out this very C++ three lines. For my own knowledge,
    // I'm breaking down the syntax:
    //
    // [&] - capture clause:
    //      Captures all variables from surrounding scope by reference
    //      i.e. this, compTypeID, and compID
    // <size_t... Is>
    //      makes the expression a templated lambda
    // (std::index_sequence<Is...>)
    //      type of parameter this lambda will take
    // || ... - fold expression:
    //      this allows the lambda to iterate through Is... when Is != compTypeID
    // (std::make_index_sequence<NUM_COMPONENTS>{})
    //      this calls the lambda and passes the idx sequence returned
    //      by the std::... call through to it
    [&]<size_t... Is>(std::index_sequence<Is...>) {
        // Fold expression: for each Is, if it matches compTypeID, remove the component
        ((Is == compTypeID ? (std::get<Is>(mRegistries).remove(compID), true) : false) || ...);
    }(std::make_index_sequence<NUM_COMPONENTS> {});
}

template <typename... Components> void EntityManager<Components...>::RemoveEntity(EntityID entity)
{
    // Check if entity is valid
    if (!ValidEntity(entity) && "Remove")
        return;

    // Remove all components from this entity
    for (size_t compTypeID = 0; compTypeID < NUM_COMPONENTS; compTypeID++) {
        const size_t compID = mEntityToComponentIdx[entity][compTypeID];

        if (compID != INVALID_COMPONENT_INDEX) {
            // Remove the component from its storage
            RemoveComponentByIndex(compTypeID, compID);
            mEntityToComponentIdx[entity][compTypeID] = INVALID_COMPONENT_INDEX;
        }
    }

    // Clear data so iterators see an invalid slot
    mEntities[entity].name.clear();
    mEntities[entity].id = 0;

    // Mark entity as invalid
    mValid[entity] = false;

    // Add to free list for reuse
    mFreeList.push_back(entity);
}

}
