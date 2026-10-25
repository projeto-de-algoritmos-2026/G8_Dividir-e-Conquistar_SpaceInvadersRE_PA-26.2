#pragma once
#include <vector>
#include <utility>
#include "raylib.h"
#include "entities/invader.hpp"

namespace space_invaders::entities {

    struct LightningEffect {
        Vector2 start;
        Vector2 end;
        float lifetime;
    };

    class Fleet {
    private:
        std::vector<Invader> invaders;
        int direction = 1; // 1 é para direita e -1 é para esquerda
        float step_x = 15.0f;         // Quantidade de pixels que a frota salta para o lado
        float drop_distance = 20.0f;  // Quantidade de pixels que a frota desce ao bater na borda

        float move_timer = 0.0f;      // Quanto tempo ja passou desde o último intervalo
        float move_interval = 0.5f;   // Intervalo entre cada salto
        bool is_frozen = false; // verificador para travar a frota quando ela chega no limite vertical
        
        std::vector<LightningEffect> active_lightnings;

    public:
        Fleet();
        ~Fleet() = default;
        void generate_fleet(int rows, int cols, float start_x, float start_y, float spacing_x, float spacing_y);

        void update(float screen_width, float bottom_limit);

        void draw() const;
        std::vector<Invader>& get_invaders();

        void trigger_chain_lightning(Invader* origin, int max_targets = 3);
        void update_effects();
        void draw_effects() const;
    };

}

