#include <raylib.h>
#include <algorithm>
#include <vector>
#include "entities/fleet.hpp"
#include "algorithms/algorithms.hpp"
#include "entities/player.hpp"

enum class GameState {
    PLAYING,
    GAME_OVER,
    VICTORY
};

int main() {
    constexpr int screen_width = 1280;
    constexpr int screen_height = 720;

    InitWindow(screen_width, screen_height, "Space Invaders: Dividir e Conquistar");
    SetTargetFPS(60);

    space_invaders::entities::Fleet enemy_fleet;
    enemy_fleet.generate_fleet(10, 25, 100.0f, 60.0f, 45.0f, 35.0f);

    space_invaders::entities::Player player((screen_width / 2.0f) - 25.0f, static_cast<float>(screen_height) - 80.0f, 50.0f, 25.0f, 400.0f);

    // Vetores de projéteis separados (Inversão de Controlo)
    std::vector<space_invaders::entities::Projectile> player_projectiles;
    std::vector<space_invaders::entities::Projectile> enemy_projectiles; // NOVO: Tiros da frota

    GameState current_state = GameState::PLAYING;

    while (!WindowShouldClose()) {

        // --- 1. Leitura de Dados Partilhados (Física e Desenho) ---
        auto& invaders = enemy_fleet.get_invaders();
        auto* commander = space_invaders::algorithms::find_commander(invaders);
        auto closest_pair = space_invaders::algorithms::find_closest_pair(invaders);
        float fleet_bottom_limit = player.get_position().y - 10.0f;

        int active_ships = static_cast<int>(std::ranges::count_if(invaders, [](const auto& ship) {
            return ship.is_alive;
        }));

        // --- 2. Atualização (Apenas corre se o jogo estiver ativo) ---
        if (current_state == GameState::PLAYING) {
            float dt = GetFrameTime();

            // Atualiza Disparos do Jogador
            auto new_shot = player.update(static_cast<float>(screen_width));
            if (new_shot.has_value()) {
                player_projectiles.push_back(new_shot.value());
            }

            // Atualiza Disparos dos Inimigos
            auto enemy_shot = enemy_fleet.try_shoot(dt);
            if (enemy_shot.has_value()) {
                enemy_projectiles.push_back(enemy_shot.value());
            }

            // Atualiza balística e limpa memória de ambos os vetores
            for (auto& p : player_projectiles) p.update(dt);
            std::erase_if(player_projectiles, [](const auto& p) { return !p.is_active; });

            for (auto& p : enemy_projectiles) p.update(dt);
            std::erase_if(enemy_projectiles, [](const auto& p) { return !p.is_active; });

            // Atualiza Frota
            enemy_fleet.update(static_cast<float>(screen_width), fleet_bottom_limit);
            enemy_fleet.update_effects();

            // Verifica se o escudo está ativo (Comandante vivo)
            bool shield_active = (commander != nullptr && commander->is_alive);

            // --- Colisões 1: Tiros do Jogador vs Naves ---
            for (auto& p : player_projectiles) {
                if (!p.is_active) continue;

                for (auto& invader : invaders) {
                    if (!invader.is_alive) continue;

                    Rectangle proj_rect = {p.x, p.y, p.width, p.height};
                    Rectangle inv_rect = {invader.x, invader.y, invader.width, invader.height};

                    if (CheckCollisionRecs(proj_rect, inv_rect)) {
                        p.is_active = false; // O tiro é absorvido

                        bool is_commander = (&invader == commander);
                        bool is_closest_pair = (&invader == closest_pair.first || &invader == closest_pair.second);

                        // A nave só morre se o escudo estiver desativado OU se for um dos alvos de exceção
                        if (!shield_active || is_commander || is_closest_pair) {
                            invader.is_alive = false;

                            // Gatilho do Raio em Cadeia
                            if (is_closest_pair) {
                                enemy_fleet.trigger_chain_lightning(&invader, 3);
                            }
                        }
                        break;
                    }
                }
            }

            // --- Colisões 2: Tiros Inimigos vs Jogador ---
            for (auto& p : enemy_projectiles) {
                if (!p.is_active) continue;

                Rectangle proj_rect = {p.x, p.y, p.width, p.height};
                if (CheckCollisionRecs(proj_rect, player.get_rect())) {
                    player.take_damage(); // Aplica dano e ativa invulnerabilidade
                    p.is_active = false;  // Destrói o tiro inimigo
                }
            }

            // --- Condições de Vitória e Derrota ---
            int ships_alive_now = static_cast<int>(std::ranges::count_if(invaders, [](const auto& ship) { return ship.is_alive; }));

            if (ships_alive_now == 0) {
                current_state = GameState::VICTORY;
            }
            // Derrota se o jogador ficar sem vidas
            else if (player.get_lives() <= 0) {
                current_state = GameState::GAME_OVER;
            }
            else {
                float danger_zone = player.get_position().y - 40.0f;
                for (const auto& invader : invaders) {
                    if (invader.is_alive && (invader.y + invader.height >= danger_zone)) {
                        current_state = GameState::GAME_OVER;
                        break;
                    }
                }
            }
        }

        // --- 3. Renderização (Desenho) ---
        BeginDrawing();
        ClearBackground(BLACK);

        if (current_state == GameState::PLAYING) {
            enemy_fleet.draw();
            enemy_fleet.draw_effects();

            player.draw();
            for (const auto& p : player_projectiles) p.draw();

            // Desenha os tiros dos inimigos
            for (const auto& p : enemy_projectiles) p.draw();

            if (commander != nullptr && commander->is_alive) {
                DrawRectangleLinesEx(Rectangle{commander->x - 5, commander->y - 5, commander->width + 10, commander->height + 10}, 2.0f, PURPLE);
                DrawText("ALVO: COMANDANTE", static_cast<int>(commander->x) - 30, static_cast<int>(commander->y) - 15, 9, PURPLE);
            }

            if (closest_pair.first != nullptr && closest_pair.second != nullptr) {
                auto* p1 = closest_pair.first;
                auto* p2 = closest_pair.second;
                DrawRectangleLinesEx(Rectangle{p1->x - 5, p1->y - 5, p1->width + 10, p1->height + 10}, 2.0f, BLUE);
                DrawRectangleLinesEx(Rectangle{p2->x - 5, p2->y - 5, p2->width + 10, p2->height + 10}, 2.0f, BLUE);
                DrawLineEx(Vector2{p1->x + p1->width/2, p1->y + p1->height/2}, Vector2{p2->x + p2->width/2, p2->y + p2->height/2}, 2.0f, BLUE);
                DrawText("PAR MAIS PROXIMO", static_cast<int>(p1->x), static_cast<int>(p1->y) - 15, 9, BLUE);
            }

            DrawText(TextFormat("Naves Ativas: %d", active_ships), 10, 10, 20, LIME);
            DrawText(TextFormat("Vidas: %d", player.get_lives()), 10, 40, 20, RED);
            DrawText("USE AS SETAS OU 'A'/'D' PARA MOVER E 'ESPACO' PARA DISPARAR", 10, 70, 14, GRAY);

        } else if (current_state == GameState::GAME_OVER) {
            const char* text = "GAME OVER";
            int font_size = 60;
            int text_width = MeasureText(text, font_size);
            DrawText(text, (screen_width - text_width) / 2, screen_height / 2 - 30, font_size, RED);

        } else if (current_state == GameState::VICTORY) {
            const char* text = "VITORIA!";
            int font_size = 60;
            int text_width = MeasureText(text, font_size);
            DrawText(text, (screen_width - text_width) / 2, screen_height / 2 - 30, font_size, GREEN);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}