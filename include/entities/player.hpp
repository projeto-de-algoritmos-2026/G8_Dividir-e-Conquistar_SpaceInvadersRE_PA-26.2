#pragma once
#include <raylib.h>
#include <vector>
#include <optional>
#include "projectile.hpp"

namespace space_invaders::entities {

class Player {
private:
    Vector2 position;
    Vector2 size;
    float speed;
    Color color;
    // Cooldown de disparo do player
    float shoot_cooldown = 0.4f;
    float time_since_last_shot = 0.0f;
    // vida e tempo de invulnerabilidade
    int lives = 3;
    float invulnerability_timer = 0.0f;

public:
    Player(float x, float y, float width, float height, float speed);

    std::optional<Projectile> update(float screen_width);
    void draw() const;

    Vector2 get_position() const { return position;}
    Rectangle get_rect() const { return { position.x, position.y, size.x, size.y }; }
    int get_lives() const { return lives; }
    void take_damage();
};

} 