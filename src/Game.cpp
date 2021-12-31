#include "Game.hpp"

const float PI = 2 * acosf(0.0f);

Game::Game(const std::string& config_file)
{
    init(config_file);
}

void Game::init(const std::string& path)
{
    std::ifstream fin(path);
    std::string section;

    assert(fin.is_open());

    fin >> section;
    assert(section == "Window");
    int wWidth, wHeight, frLimit, fullScreenMode;
    fin >> wWidth >> wHeight >> frLimit >> fullScreenMode;

    fin >> section;
    assert(section == "Font");
    std::string fontFile;
    int fontSize, fr, fg, fb;
    fin >> fontFile >> fontSize >> fr >> fg >> fb;
    sf::Color fontColor(fr, fg, fb);

    if (!m_font.loadFromFile(fontFile)) {
        std::cerr << "Error loading font!\n";
        exit(-1);
    }
    m_text.setFont(m_font);
    m_text.setCharacterSize(fontSize);
    m_text.setFillColor(fontColor);
    m_text.setPosition(0, 0);
    m_text.setString("Score 0");

    fin >> section;
    assert(section == "Player");
    fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S;
    fin >> m_playerConfig.FR >> m_playerConfig.FG >> m_playerConfig.FB;
    fin >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB;
    fin >> m_playerConfig.OT >> m_playerConfig.V;

    fin >> section;
    assert(section == "Enemy");
    fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX;
    fin >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT;
    fin >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;

    fin >> section;
    assert(section == "Bullet");
    fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S;
    fin >> m_bulletConfig.FR >> m_bulletConfig.FG >> m_bulletConfig.FB;
    fin >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB;
    fin >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;


    m_window.create(sf::VideoMode(wWidth, wHeight), "Polygon Wars");
    m_window.setFramerateLimit(frLimit);

    spawnPlayer();
}

void Game::spawnPlayer()
{
    auto entity = m_entities.addEntity("player");
    entity->cTransform = std::make_shared<CTransform>(
                            Vec2(m_window.getSize().x/2, m_window.getSize().y/2),
                            Vec2(1.0f, 1.0f), 0.0f);
    entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V,
                        sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
                        sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
                        m_playerConfig.OT);
    entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);
    entity->cInput = std::make_shared<CInput>();
    m_player = entity;

    for (auto p: m_entities.getEntities("player"))
        if (p != m_player)
            p->destroy();
}

void Game::spawnEnemy()
{
    static RandInt v_rnd(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
    static RandInt s_rnd(m_enemyConfig.SMIN, m_enemyConfig.SMAX);

    static RandInt x_rnd(m_enemyConfig.CR, m_window.getSize().x - m_enemyConfig.CR);
    static RandInt y_rnd(m_enemyConfig.CR, m_window.getSize().y - m_enemyConfig.CR);

    static RandInt clr_rnd(0, 255);

    int vertices = v_rnd();
    int speed = s_rnd();

    auto entity = m_entities.addEntity("enemy");
    entity->cTransform = std::make_shared<CTransform>(Vec2(x_rnd(), y_rnd()),
                            Vec2(speed, speed), 0.0f);
    entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, vertices,
                        sf::Color(clr_rnd(), clr_rnd(), clr_rnd()),
                        sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
                        m_enemyConfig.OT);
    entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
    entity->cScore = std::make_shared<CScore>(vertices * 100);

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> enemy)
{
    int vertices = enemy->cShape->circle.getPointCount();

    for (int i = 0; i < vertices; ++i) {
        float angle = ((360.0f/vertices) * (i + 1)) * PI / 180.0f;
        auto entity = m_entities.addEntity("small");
        entity->cTransform = std::make_shared<CTransform>(enemy->cTransform->pos,
                                Vec2(cosf(angle), sinf(angle)).scale(abs(enemy->cTransform->velocity.x)),
                                0.0f);
        entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR/2, vertices,
                            enemy->cShape->circle.getFillColor(),
                            enemy->cShape->circle.getOutlineColor(),
                            m_enemyConfig.OT);
        entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR/2);
        entity->cScore = std::make_shared<CScore>(enemy->cScore->score * 2);
        entity->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
    }
}

void Game::spawnBullet(std::shared_ptr<Entity> player, const Vec2& mousePos)
{
    Vec2 bulletVelocity = mousePos - player->cTransform->pos;
    bulletVelocity.normalize();

    auto entity = m_entities.addEntity("bullet");
    entity->cTransform = std::make_shared<CTransform>(player->cTransform->pos,
                            bulletVelocity.scale(m_bulletConfig.S), 0.0f);
    entity->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V,
                        sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
                        sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
                        m_bulletConfig.OT);
    entity->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    entity->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
    if (m_specialWeaponCount <= 0) return;

    const int numBullets = m_playerConfig.V;
    for (int i = 0; i < numBullets; ++i) {
        float angle = ((360.0f/numBullets) * (i + 1)) * PI / 180.0f;
        spawnBullet(entity, Vec2(cosf(angle), sinf(angle)).scale(1000000));
    }
    --m_specialWeaponCount;
}

// ===== Systems =====

void Game::sMovement()
{
    Vec2 playerVelocity;
    if (m_player->cInput->up)
        playerVelocity.y -= m_playerConfig.S; 
    if (m_player->cInput->down)
        playerVelocity.y += m_playerConfig.S;
    if (m_player->cInput->left)
        playerVelocity.x -= m_playerConfig.S;
    if (m_player->cInput->right)
        playerVelocity.x += m_playerConfig.S;
    m_player->cTransform->velocity = playerVelocity;

    for (auto e : m_entities.getEntities())
        e->cTransform->pos += e->cTransform->velocity;
}

void Game::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
            m_running = false;
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::W)
                m_player->cInput->up = true;
            if (event.key.code == sf::Keyboard::S)
                m_player->cInput->down = true;
            if (event.key.code == sf::Keyboard::A)
                m_player->cInput->left = true;
            if (event.key.code == sf::Keyboard::D)
                m_player->cInput->right = true;
            if (event.key.code == sf::Keyboard::Space)
                m_paused = !m_paused;
        }
        if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::W)
                m_player->cInput->up = false;
            if (event.key.code == sf::Keyboard::S)
                m_player->cInput->down = false;
            if (event.key.code == sf::Keyboard::A)
                m_player->cInput->left = false;
            if (event.key.code == sf::Keyboard::D)
                m_player->cInput->right = false;
        }
        if (event.type == sf::Event::MouseButtonPressed && !m_paused) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                Vec2 mousePos(event.mouseButton.x, event.mouseButton.y);
                spawnBullet(m_player, mousePos);
            }
            if (event.mouseButton.button == sf::Mouse::Right) {
                spawnSpecialWeapon(m_player);
            }
        }
    }
}

void Game::sLifespan()
{
    for (auto e : m_entities.getEntities()) {
        if (!e->cLifespan) continue;
        if (e->cLifespan->rem > 0)  --e->cLifespan->rem;
        if (e->isActive()) {
            auto color = e->cShape->circle.getFillColor();
            color.a *= e->cLifespan->rem/(float)e->cLifespan->total;
            e->cShape->circle.setFillColor(color);

            color = e->cShape->circle.getOutlineColor();
            color.a *= e->cLifespan->rem/(float)e->cLifespan->total;
            e->cShape->circle.setOutlineColor(color);

            if (e->cLifespan == 0) e->destroy();
        }
    }
}

void Game::sRender()
{
    m_window.clear();

    // Render all entities
    for (auto e : m_entities.getEntities()) {
        e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
        e->cTransform->angle += 1.0f;
        e->cShape->circle.setRotation(e->cTransform->angle);
        m_window.draw(e->cShape->circle);
    }

    // Render player score
    std::stringstream ss;
    ss << "Score " << m_score << "  SpecialAmmo " << m_specialWeaponCount;
    m_text.setString(ss.str());
    m_window.draw(m_text);

    m_window.display();
}

void Game::sEnemySpawner()
{
    if (m_currentFrame - m_lastEnemySpawnTime > m_enemyConfig.SI)
        spawnEnemy();
}

void Game::sCollision()
{
    for (auto p : m_entities.getEntities("player")) {
        for (auto e : m_entities.getEntities("enemy")) {
            float dist = p->cTransform->pos.dist(e->cTransform->pos);
            if (dist < p->cCollision->radius + e->cCollision->radius) {
                p->destroy();
                spawnPlayer();
                m_score /= 2;
            }
        }
    }

    for (auto p : m_entities.getEntities("player")) {
        for (auto e : m_entities.getEntities("small")) {
            float dist = p->cTransform->pos.dist(e->cTransform->pos);
            if (dist < p->cCollision->radius + e->cCollision->radius) {
                p->destroy();
                spawnPlayer();
                m_score /= 2;
            }
        }
    }

    for (auto b : m_entities.getEntities("bullet")) {
        for (auto e : m_entities.getEntities("enemy")) {
            float dist = b->cTransform->pos.dist(e->cTransform->pos);
            if (dist < b->cCollision->radius + e->cCollision->radius) {
                b->destroy();
                e->destroy();
                spawnSmallEnemies(e);
                m_score += e->cScore->score;
            }
        }
    }

    for (auto b : m_entities.getEntities("bullet")) {
        for (auto e : m_entities.getEntities("small")) {
            float dist = b->cTransform->pos.dist(e->cTransform->pos);
            if (dist < b->cCollision->radius + e->cCollision->radius) {
                b->destroy();
                e->destroy();
                m_score += e->cScore->score;
            }
        }
    }

    int wWidth = m_window.getSize().x;
    int wHeight = m_window.getSize().y;
    for (auto e : m_entities.getEntities()) {
        if (e->tag() != "player" && e->tag() != "enemy") continue;
        Vec2 pos = e->cTransform->pos;
        if (pos.x - e->cCollision->radius < 0) e->cTransform->velocity.x *= -1.0f;
        if (pos.y - e->cCollision->radius < 0) e->cTransform->velocity.y *= -1.0f;
        if (pos.x + e->cCollision->radius > wWidth) e->cTransform->velocity.x *= -1.0f;
        if (pos.y + e->cCollision->radius > wHeight) e->cTransform->velocity.y *= -1.0f;
    }

    Vec2& ppos = m_player->cTransform->pos;
    if (ppos.x - m_player->cCollision->radius < 0)
        ppos.x = m_player->cCollision->radius;
    if (ppos.y - m_player->cCollision->radius < 0)
        ppos.y = m_player->cCollision->radius;
    if (ppos.x + m_player->cCollision->radius > wWidth)
        ppos.x = wWidth - m_player->cCollision->radius;
    if (ppos.y + m_player->cCollision->radius > wHeight)
        ppos.y = wHeight - m_player->cCollision->radius;
}

void Game::run()
{
    while (m_running) {
        m_entities.update();

        if (!m_paused) {
            sEnemySpawner();
            sMovement();
            sLifespan();
            sCollision();
        }
        sUserInput();
        sRender();

        m_currentFrame++;
    }
}