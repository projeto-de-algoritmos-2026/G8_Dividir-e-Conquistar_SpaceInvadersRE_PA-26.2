#pragma once
#include <raylib.h>

namespace space_invaders::entities {

    struct Projectile {
        float x;
        float y;
        float width = 4.0f;
        float height = 15.0f;
        float velocity_y = -600.0f; // negativo = para cima e positivo = para baixo
        Color color = YELLOW;
        bool is_active = true;

        void update(float dt);
        void draw() const;
    };

}