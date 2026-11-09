#include "algorithms/algorithms.hpp"
#include <vector>
#include <algorithm>
#include <cmath>
#include <cfloat>

namespace space_invaders::algorithms {

    entities::Invader* find_commander(std::vector<entities::Invader>& fleet) {
        std::vector<entities::Invader*> active_ships;
        for (auto& ship : fleet) {
            if (ship.is_alive) active_ships.push_back(&ship);
        }

        if (active_ships.empty()) return nullptr;

        int mid_index = active_ships.size() / 2;
        return active_ships[mid_index];
    }

    // --- FUNÇÕES AUXILIARES PARA O PAR MAIS PRÓXIMO ---

    // Calcula a distância ao quadrado para evitar a operação pesada de sqrt()
    float dist_sq(entities::Invader* p1, entities::Invader* p2) {
        float dx = p1->x - p2->x;
        float dy = p1->y - p2->y;
        return dx * dx + dy * dy;
    }

    // Função Recursiva Central (O(N log N) na prática com pré-ordenação)
    std::pair<entities::Invader*, entities::Invader*> closest_pair_recursive(std::vector<entities::Invader*>& ships_x, int left, int right) {
        // CASO BASE: Se houver 3 ou menos naves neste bloco, usa Força Bruta O(N^2)
        if (right - left <= 3) {
            float min_d = FLT_MAX;
            std::pair<entities::Invader*, entities::Invader*> best_pair = {nullptr, nullptr};
            for (int i = left; i < right; ++i) {
                for (int j = i + 1; j < right; ++j) {
                    float d = dist_sq(ships_x[i], ships_x[j]);
                    if (d < min_d) {
                        min_d = d;
                        best_pair = {ships_x[i], ships_x[j]};
                    }
                    // NOVO: Desempate pseudoaleatório ancorado ao vetor para não piscar
                    else if (d == min_d && min_d != FLT_MAX) {
                        if (((ships_x.size() * 37) + i * 13 + j * 7) % 2 == 0) {
                            best_pair = {ships_x[i], ships_x[j]};
                        }
                    }
                }
            }
            return best_pair;
        }

        // DIVISÃO: Encontra a nave do meio
        int mid = left + (right - left) / 2;
        entities::Invader* mid_point = ships_x[mid];

        // CONQUISTA: Recursão para a metade esquerda e metade direita
        auto pair_left = closest_pair_recursive(ships_x, left, mid);
        auto pair_right = closest_pair_recursive(ships_x, mid, right);

        // Descobre qual das duas metades tem a menor distância
        float d_left = (pair_left.first != nullptr) ? dist_sq(pair_left.first, pair_left.second) : FLT_MAX;
        float d_right = (pair_right.first != nullptr) ? dist_sq(pair_right.first, pair_right.second) : FLT_MAX;

        float min_d = std::min(d_left, d_right);
        auto best_pair = (d_left < d_right) ? pair_left : pair_right;

        // COMBINAÇÃO: Criar uma "faixa" (strip) ao redor da linha central
        std::vector<entities::Invader*> strip;
        for (int i = left; i < right; ++i) {
            float dx = ships_x[i]->x - mid_point->x;
            if (dx * dx < min_d) {
                strip.push_back(ships_x[i]);
            }
        }

        // Ordenar a faixa pelo eixo Y
        std::sort(strip.begin(), strip.end(), [](entities::Invader* a, entities::Invader* b) {
            return a->y < b->y;
        });

        // Procurar o par mais próximo dentro da faixa central
        for (size_t i = 0; i < strip.size(); ++i) {
            for (size_t j = i + 1; j < strip.size(); ++j) {
                float dy = strip[j]->y - strip[i]->y;
                if (dy * dy > min_d) break; // Tem de ser > e não >= para permitir processar empates

                float d = dist_sq(strip[i], strip[j]);
                if (d < min_d) {
                    min_d = d;
                    best_pair = {strip[i], strip[j]};
                }
                // NOVO: Desempate pseudoaleatório cruzado entre os dois lados da faixa
                else if (d == min_d) {
                    if (((strip.size() * 37) + i * 13 + j * 7) % 2 == 0) {
                        best_pair = {strip[i], strip[j]};
                    }
                }
            }
        }

        return best_pair;
    }

    std::pair<entities::Invader*, entities::Invader*> find_closest_pair(std::vector<entities::Invader>& fleet) {
        std::vector<entities::Invader*> active_ships;
        for (auto& ship : fleet) {
            if (ship.is_alive) active_ships.push_back(&ship);
        }

        if (active_ships.size() < 2) return {nullptr, nullptr};

        // 1. PRÉ-PROCESSAMENTO: Ordenação inicial pelo eixo X (necessário para a recursão)
        std::sort(active_ships.begin(), active_ships.end(), [](entities::Invader* a, entities::Invader* b) {
            return a->x < b->x;
        });

        // 2. Executa a Divisão e Conquista
        return closest_pair_recursive(active_ships, 0, active_ships.size());
    }

}