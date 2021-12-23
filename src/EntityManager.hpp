#ifndef _ENTITY_MANAGER_HPP_
#define _ENTITY_MANAGER_HPP_

#include "Common.hpp"
#include "Entity.hpp"

using EntityVec = std::vector<std::shared_ptr<Entity>>;
using EntityMap = std::map<std::string, EntityVec>;

class EntityManager
{
    EntityVec m_entities;
    EntityVec m_entitiesToAdd;
    EntityMap m_entityMap;
    size_t m_totalEntities = 0;

    void removeDeadEntities(EntityVec& v);

public:

    EntityManager();

    void update();

    std::shared_ptr<Entity> addEntity(const std::string& tag);

    const EntityVec& getEntities() const;
    const EntityVec& getEntities(const std::string& tag);
};

#endif