#include "EntityManager.hpp"

EntityManager::EntityManager()
{
}

void EntityManager::update()
{
    for (auto entity: m_entitiesToAdd) {
        m_entities.push_back(entity);
        m_entityMap[entity->tag()].push_back(entity);
    }

    removeDeadEntities(m_entities);

    for (auto& [tag, entityVec] : m_entityMap) {
        removeDeadEntities(entityVec);
    }

    m_entitiesToAdd.clear();
}

void EntityManager::removeDeadEntities(EntityVec& v)
{
    auto end = std::remove_if(v.begin(), v.end(),
                    [](std::shared_ptr<Entity> e) { return !e->isActive(); });
    v.erase(end, v.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
    auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_entitiesToAdd.push_back(entity);
    return entity;
}

const EntityVec& EntityManager::getEntities() const
{
    return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
    return m_entityMap[tag];
}
