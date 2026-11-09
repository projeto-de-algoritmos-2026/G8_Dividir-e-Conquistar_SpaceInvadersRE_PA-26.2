#include "entities/projectile.hpp"

namespace space_invaders::entities {

    void Projectile::update(float dt) {
        if (!is_active) return;
        
        y += velocity_y * dt; // velocidade diminui conforme avança
        
        //Se sair da tela será deletado
        if (y + height < -50.0f || y > 1000.0f) {
            is_active = false;
        }
    }

    void Projectile::draw() const {
        if (!is_active) return;
        DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(height), color);
    }

}