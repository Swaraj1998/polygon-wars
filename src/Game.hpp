#ifndef _GAME_HPP_
#define _GAME_HPP_

#include "Common.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"

/*
 *  =================================
 *  Config Attributes
 *  =================================
 *  
 *  SR          -> Shape Radius
 *  CR          -> Collision Radius
 *  S           -> Speed
 *  FR, FG, FB  -> Fill Color
 *  OR, OG, OB  -> Outline Color
 *  OT          -> Outline Thickness
 *  V           -> Shape Vertices
 * 
 *  SMIN, SMAX  -> Min/Max Speed
 *  VMIN, VMAX  -> Min/Max Vertices
 *  L           -> Small Enemy Lifespan
 *  SI          -> Spawn Interval  
 */ 

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; };
struct EnemyConfig  { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
    sf::RenderWindow    m_window;
    EntityManager       m_entities;
    sf::Font            m_font;
    sf::Text            m_text;
    PlayerConfig        m_playerConfig;
    EnemyConfig         m_enemyConfig;
    BulletConfig        m_bulletConfig;
    int                 m_score = 0;
    int                 m_currentFrame = 0;
    int                 m_lastEnemySpawnTime = 0;
    bool                m_paused = false;
    bool                m_running = true;

    std::shared_ptr<Entity> m_player;

    void init(const std::string& config);
    void setPaused(bool paused);

    // ===== Systems =====
    void sMovement();
    void sUserInput();
    void sLifespan();
    void sRender();
    void sEnemySpawner();
    void sCollision();
    // ==================

    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);
    void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);
    void spawnSpecialWeapon(std::shared_ptr<Entity> entity);

public:

    Game(const std::string& config_file);

    void run();
};

#endif