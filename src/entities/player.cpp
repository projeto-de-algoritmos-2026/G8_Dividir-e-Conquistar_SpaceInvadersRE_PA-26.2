#include "entities/player.hpp"

namespace space_invaders::entities {
    Player::Player(float x, float y, float width, float height, float speed)
        : position{x, y}, size{width, height}, speed(speed), color(WHITE) {}

    std::optional<Projectile> Player::update(float screen_width) {
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            position.x -= speed * dt;
        }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            position.x += speed * dt;
        }

        float margin = 20.0f;
        if (position.x < margin) {
            position.x = margin;
        }
        if (position.x + size.x > screen_width - margin) {
            position.x = screen_width - margin - size.x;
        }

        // --- Disparo (Input e Cooldown) ---
        time_since_last_shot += dt;
        // retorna o projetio para o vetor da main caso houver disparo
        if (IsKeyPressed(KEY_SPACE) && time_since_last_shot >= shoot_cooldown) {
            time_since_last_shot = 0.0f;

            Projectile p;
            p.x = position.x + (size.x / 2.0f) - (p.width / 2.0f);
            p.y = position.y;
            p.velocity_y = -600.0f;
            return p;
        }
        return std::nullopt; // se não houve projetio retorna nulo
    }

    void Player::draw() const {
        // Pisca a nave quando toma dano
        if (invulnerability_timer > 0.0f) {
            if (static_cast<int>(invulnerability_timer * 10) % 2 == 0) {
                return;
            }
        }
        DrawRectangleRec(get_rect(), color);
    }
}