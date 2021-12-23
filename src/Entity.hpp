#ifndef _ENTITY_HPP_
#define _ENTITY_HPP_

#include "Common.hpp"
#include "Components.hpp"

class Entity
{
    friend class EntityManager;

    bool        m_active = true;
    size_t      m_id = 0;
    std::string m_tag = "default";

    Entity(size_t id, const std::string& tag);

public:

    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CShape>     cShape;
    std::shared_ptr<CCollision> cCollision;
    std::shared_ptr<CInput>     cInput;
    std::shared_ptr<CScore>     cScore;
    std::shared_ptr<CLifespan>  cLifespan;

    bool isActive() const;
    const std::string& tag() const;
    size_t id() const;
    void destroy();
};

#endif