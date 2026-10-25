#include "entities/fleet.hpp"
#include <algorithm>
#include "raylib.h"

namespace space_invaders::entities {

    Fleet::Fleet() {}

    void Fleet::generate_fleet(int rows, int cols, float start_x, float start_y, float spacing_x, float spacing_y) {
        // LImpeza e reserva de memória
        invaders.clear();
        invaders.reserve(rows * cols);

        // Reset das variáveis de estado caso a frota seja regerada
        is_frozen = false;
        direction = 1;
        move_timer = 0.0f;

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                Invader ship;
                ship.x = start_x + (c * spacing_x);
                ship.y = start_y + (r * spacing_y);
                ship.is_alive = true;
                // define o tipo para linha r
                ship.type = static_cast<InvaderType>(r % 3);

                invaders.push_back(ship);
            }
        }
    }

    void Fleet::update(float screen_width, float bottom_limit) {
        // se não há mais naves ou os aliens atingiram o chão a frota não precisa se mover
        if (invaders.empty() || is_frozen) return;

        // Acumula o tempo que passou desde o último frame
        float dt = GetFrameTime();
        move_timer += dt;

        // Só vai movimentar quando der o intervalo
        if (move_timer >= move_interval) {
            move_timer = 0.0f; // Reinicia o timer para o próximo salto

            float dx = step_x * direction;
            bool hit_edge = false;
            float margin = 20.0f;
            float invader_width = invaders.front().width;
            float invader_height = invaders.front().height;

            // 1. Verifica se a frota antingiu a borda
            // direction = 1 = mover pra direita e = -1 é o mesmo que mexer pra esquerda
            for (const auto& ship : invaders) {
                if (!ship.is_alive) continue;

                if (direction == 1 && (ship.x + invader_width + dx > screen_width - margin)) {
                    hit_edge = true;
                    break;
                }
                else if (direction == -1 && (ship.x + dx < margin)) {
                    hit_edge = true;
                    break;
                }
            }

            // Salto da nave
            if (hit_edge) {
                direction *= -1;
                float lowest_y = 0.0f;
                for (const auto& ship : invaders) {
                    if (ship.is_alive && (ship.y + invader_height > lowest_y)) {
                        lowest_y = ship.y + invader_height;
                    }
                }
                if (lowest_y + drop_distance <= bottom_limit) {
                    for (auto& ship : invaders) {
                        ship.y += drop_distance;
                    }
                } else {
                    // Game over os aliens chegaram na base, a frota para de se mexer

                    is_frozen = true;
                }
            } else {
                for (auto& ship : invaders) {
                    ship.x += dx;
                }
            }
        }
    }

    void Fleet::draw() const {
        for (const auto& ship : invaders) {
            ship.draw();
        }
    }

    std::vector<Invader>& Fleet::get_invaders() {
        return invaders;
    }

    void Fleet::trigger_chain_lightning(Invader* origin, int max_targets) {
        std::vector<std::pair<float, Invader*>> distances;

        for (auto& inv : invaders) {
            if (inv.is_alive && &inv != origin) {
                float dx = inv.x - origin->x;
                float dy = inv.y - origin->y;
                distances.push_back({dx*dx + dy*dy, &inv});
            }
        }

        std::sort(distances.begin(), distances.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

        int count = 0;
        for (auto& pair : distances) {
            if (count >= max_targets) break;

            pair.second->is_alive = false;

            active_lightnings.push_back(LightningEffect{
                Vector2{origin->x + origin->width/2, origin->y + origin->height/2},
                Vector2{pair.second->x + pair.second->width/2, pair.second->y + pair.second->height/2},
                0.5f
            });
            count++;
        }
    }

    void Fleet::update_effects() {
        float dt = GetFrameTime();
        for (auto& l : active_lightnings) l.lifetime -= dt;

        active_lightnings.erase(
            std::remove_if(active_lightnings.begin(), active_lightnings.end(), 
                [](const LightningEffect& l){ return l.lifetime <= 0.0f; }),
            active_lightnings.end()
        );
    }

    void Fleet::draw_effects() const {
        for (const auto& l : active_lightnings) {
            DrawLineEx(l.start, l.end, 3.0f, Fade(BLUE, l.lifetime * 2.0f));
        }
    }
}
