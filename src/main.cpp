#include <raylib.h>
#include <algorithm>
#include "entities/fleet.hpp"
#include "algorithms/algorithms.hpp"
#include "entities/player.hpp"

int main() {
    constexpr int screen_width = 1280;
    constexpr int screen_height = 720;

    InitWindow(screen_width, screen_height, "Space Invaders: Dividir e Conquistar");
    SetTargetFPS(60);

    space_invaders::entities::Fleet enemy_fleet;
    enemy_fleet.generate_fleet(10, 25, 100.0f, 60.0f, 45.0f, 35.0f);

    space_invaders::entities::Player player((screen_width / 2.0f) - 25.0f, static_cast<float>(screen_height) - 80.0f, 50.0f, 25.0f, 400.0f);
    std::vector<space_invaders::entities::Projectile> player_projectiles;

    while (!WindowShouldClose()) {

        // --- Atualização ---

        auto new_shot = player.update(static_cast<float>(screen_width));
        // Garante que não serão inseridos elementos nulos
        if (new_shot.has_value()) {
            player_projectiles.push_back(new_shot.value());
        }

        // Atualização do cooldown dos projéteis
        float dt = GetFrameTime();
        for (auto& p : player_projectiles) {
            p.update(dt);
        }
        std::erase_if(player_projectiles, [](const auto& p) { return !p.is_active; });
        float fleet_bottom_limit = player.get_position().y - 10.0f;
        enemy_fleet.update(static_cast<float>(screen_width), fleet_bottom_limit);

        // Obtém a referência do vetor para passar aos algoritmos
        auto& invaders = enemy_fleet.get_invaders();

        for (auto& p : player_projectiles) {
            if (!p.is_active) continue; // Ignora tiros já desativados nesta frame

            for (auto& invader : invaders) {
                if (!invader.is_alive) continue; // Ignora naves já destruídas

                // Cria os retângulos de colisão
                Rectangle proj_rect = {p.x, p.y, p.width, p.height};
                Rectangle inv_rect = {invader.x, invader.y, invader.width, invader.height};

                if (CheckCollisionRecs(proj_rect, inv_rect)) {
                    invader.is_alive = false; // Destrói a nave
                    p.is_active = false;      // Destrói o tiro
                    break; // O tiro já atingiu um alvo, avança para o próximo tiro
                }
            }
        }

        // Executa os algoritmos de Divisão e Conquista
        auto* commander = space_invaders::algorithms::find_commander(invaders);
        auto closest_pair = space_invaders::algorithms::find_closest_pair(invaders);

        // --- Renderização (Desenho) ---
        BeginDrawing();
        ClearBackground(BLACK);
        enemy_fleet.draw();

        player.draw();
        for (const auto& p : player_projectiles) {
            p.draw();
        }

        // Destaca a nave comandante (se encontrada e viva)
        if (commander != nullptr && commander->is_alive) {
            // Desenha um quadrado de mira em volta da nave
            DrawRectangleLinesEx(
                Rectangle{commander->x - 5, commander->y - 5, commander->width + 10, commander->height + 10},
                2.0f,
                PURPLE
            );
            // apenas para testar depois remover e deixar apenas o contorno
            DrawText("ALVO: COMANDANTE", static_cast<int>(commander->x) - 30, static_cast<int>(commander->y) - 15, 9, PURPLE);
        }


        // Destaca as duas naves do Par Mais Próximo (se existirem e estiverem vivas)
        if (closest_pair.first != nullptr && closest_pair.second != nullptr) {
            auto* p1 = closest_pair.first;
            auto* p2 = closest_pair.second;

            // Caixas de mira em ciano para as duas naves
            DrawRectangleLinesEx(Rectangle{p1->x - 5, p1->y - 5, p1->width + 10, p1->height + 10}, 2.0f, BLUE);
            DrawRectangleLinesEx(Rectangle{p2->x - 5, p2->y - 5, p2->width + 10, p2->height + 10}, 2.0f, BLUE);

            // Desenha uma linha a conectar os centros das duas naves
            DrawLineEx(
                Vector2{p1->x + p1->width/2, p1->y + p1->height/2},
                Vector2{p2->x + p2->width/2, p2->y + p2->height/2},
                2.0f,
                BLUE
            );
            // apenas para testar depois remover e deixar apenas o contorno
            DrawText("PAR MAIS PROXIMO", static_cast<int>(p1->x), static_cast<int>(p1->y) - 15, 9, BLUE);
        }

        // Calcula dinamicamente quantas naves continuam com is_alive = true
        int active_ships = static_cast<int>(std::ranges::count_if(invaders, [](const auto& ship) {
            return ship.is_alive;
        }));

        DrawText(TextFormat("Naves Ativas: %d", active_ships), 10, 10, 20, LIME);
        DrawText("USE AS SETAS OU 'A'/'D' PARA MOVER A NAVE", 10, 40, 14, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}