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

    std::vector<space_invaders::entities::Projectile> player_projectiles;
    std::vector<space_invaders::entities::Projectile> enemy_projectiles;

    GameState current_state = GameState::PLAYING;
    int score = 0;

    // NOVO: Temporizador de desativação do escudo (10 segundos)
    float shield_disabled_timer = 0.0f;

    while (!WindowShouldClose()) {

        // --- 1. Leitura de Dados Partilhados ---
        auto& invaders = enemy_fleet.get_invaders();
        auto closest_pair = space_invaders::algorithms::find_closest_pair(invaders);
        float fleet_bottom_limit = player.get_position().y - 10.0f;

        int active_ships = static_cast<int>(std::ranges::count_if(invaders, [](const auto& ship) {
            return ship.is_alive;
        }));

        // --- 2. Atualização ---
        if (current_state == GameState::PLAYING) {
            float dt = GetFrameTime();

            // Atualiza temporizador do escudo
            if (shield_disabled_timer > 0.0f) {
                shield_disabled_timer -= dt;
                if (shield_disabled_timer < 0.0f) shield_disabled_timer = 0.0f;
            }

            // O Comandante só existe e ativa o escudo se o temporizador estiver zerado
            space_invaders::entities::Invader* commander = nullptr;
            bool shield_active = false;

            if (shield_disabled_timer <= 0.0f) {
                commander = space_invaders::algorithms::find_commander(invaders);
                shield_active = (commander != nullptr && commander->is_alive);
            }

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

            // Atualiza projéteis
            for (auto& p : player_projectiles) p.update(dt);
            std::erase_if(player_projectiles, [](const auto& p) { return !p.is_active; });

            for (auto& p : enemy_projectiles) p.update(dt);
            std::erase_if(enemy_projectiles, [](const auto& p) { return !p.is_active; });

            // Atualiza Frota e efeitos
            enemy_fleet.update(static_cast<float>(screen_width), fleet_bottom_limit);
            enemy_fleet.update_effects();

            // --- Colisões 1: Tiros do Jogador vs Naves ---
            for (auto& p : player_projectiles) {
                if (!p.is_active) continue;

                for (auto& invader : invaders) {
                    if (!invader.is_alive) continue;

                    Rectangle proj_rect = {p.x, p.y, p.width, p.height};
                    Rectangle inv_rect = {invader.x, invader.y, invader.width, invader.height};

                    if (CheckCollisionRecs(proj_rect, inv_rect)) {
                        bool is_commander = (commander != nullptr && &invader == commander);
                        bool is_closest_pair = (&invader == closest_pair.first || &invader == closest_pair.second);

                        // Destrói a nave se o escudo estiver OFF, se for o Comandante ou Par Mais Próximo
                        if (!shield_active || is_commander || is_closest_pair) {
                            invader.is_alive = false;
                            p.is_active = false; // Consome o tiro

                            if (is_commander) {
                                score += 100;
                                shield_disabled_timer = 10.0f; // NOVO: Gatilho dos 10 segundos sem escudo!
                            } else {
                                score += 10;
                            }

                            if (is_closest_pair) {
                                int chained_kills = enemy_fleet.trigger_chain_lightning(&invader, 3);
                                score += (chained_kills * 50);
                            }

                            break;
                        }
                    }
                }
            }

            // --- Colisões 2: Tiros Inimigos vs Jogador ---
            for (auto& p : enemy_projectiles) {
                if (!p.is_active) continue;

                Rectangle proj_rect = {p.x, p.y, p.width, p.height};
                if (CheckCollisionRecs(proj_rect, player.get_rect())) {
                    player.take_damage();
                    p.is_active = false;
                }
            }

            // --- Condições de Vitória e Derrota ---
            int ships_alive_now = static_cast<int>(std::ranges::count_if(invaders, [](const auto& ship) { return ship.is_alive; }));

            if (ships_alive_now == 0) {
                current_state = GameState::VICTORY;
            }
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

        // --- 3. Renderização ---
        BeginDrawing();
        ClearBackground(BLACK);

        if (current_state == GameState::PLAYING) {
            enemy_fleet.draw();
            enemy_fleet.draw_effects();

            player.draw();
            for (const auto& p : player_projectiles) p.draw();
            for (const auto& p : enemy_projectiles) p.draw();

            // Desenha o Comandante (apenas se o escudo estiver ativo)
            space_invaders::entities::Invader* commander = (shield_disabled_timer <= 0.0f) ? space_invaders::algorithms::find_commander(invaders) : nullptr;
            if (commander != nullptr && commander->is_alive) {
                DrawRectangleLinesEx(Rectangle{commander->x - 5, commander->y - 5, commander->width + 10, commander->height + 10}, 2.0f, PURPLE);
                DrawText("ALVO: COMANDANTE", static_cast<int>(commander->x) - 30, static_cast<int>(commander->y) - 15, 9, PURPLE);
            }

            // Desenha o Par Mais Próximo (continua sempre visível)
            if (closest_pair.first != nullptr && closest_pair.second != nullptr) {
                auto* p1 = closest_pair.first;
                auto* p2 = closest_pair.second;
                DrawRectangleLinesEx(Rectangle{p1->x - 5, p1->y - 5, p1->width + 10, p1->height + 10}, 2.0f, BLUE);
                DrawRectangleLinesEx(Rectangle{p2->x - 5, p2->y - 5, p2->width + 10, p2->height + 10}, 2.0f, BLUE);
                DrawLineEx(Vector2{p1->x + p1->width/2, p1->y + p1->height/2}, Vector2{p2->x + p2->width/2, p2->y + p2->height/2}, 2.0f, BLUE);
                DrawText("PAR MAIS PROXIMO", static_cast<int>(p1->x), static_cast<int>(p1->y) - 15, 9, BLUE);
            }

            // Interface / HUD
            DrawText(TextFormat("Naves Ativas: %d", active_ships), 10, 10, 20, LIME);
            DrawText(TextFormat("Vidas: %d", player.get_lives()), 10, 40, 20, RED);

            // NOVO: Alerta visual quando o escudo é quebrado
            if (shield_disabled_timer > 0.0f) {
                DrawText(TextFormat("ESCUDO DESATIVADO: %.1fs", shield_disabled_timer), 10, 70, 20, YELLOW);
            } else {
                DrawText("ESCUDO ATIVO - ALVEJE O COMANDANTE OU O PAR AZUL", 10, 70, 14, GRAY);
            }

            DrawText(TextFormat("SCORE: %06d", score), screen_width - 250, 10, 20, GOLD);

        } else if (current_state == GameState::GAME_OVER) {
            const char* text = "GAME OVER";
            int font_size = 60;
            int text_width = MeasureText(text, font_size);
            DrawText(text, (screen_width - text_width) / 2, screen_height / 2 - 30, font_size, RED);
            DrawText(TextFormat("SCORE FINAL: %06d", score), (screen_width - MeasureText(TextFormat("SCORE FINAL: %06d", score), 30)) / 2, screen_height / 2 + 50, 30, GOLD);

        } else if (current_state == GameState::VICTORY) {
            const char* text = "VITORIA!";
            int font_size = 60;
            int text_width = MeasureText(text, font_size);
            DrawText(text, (screen_width - text_width) / 2, screen_height / 2 - 30, font_size, GREEN);
            DrawText(TextFormat("SCORE FINAL: %06d", score), (screen_width - MeasureText(TextFormat("SCORE FINAL: %06d", score), 30)) / 2, screen_height / 2 + 50, 30, GOLD);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}