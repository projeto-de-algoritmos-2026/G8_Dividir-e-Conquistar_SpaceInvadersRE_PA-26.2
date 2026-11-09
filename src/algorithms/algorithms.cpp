#include "algorithms/algorithms.hpp"
#include <vector>

namespace space_invaders::algorithms {

    entities::Invader* find_commander(std::vector<entities::Invader>& fleet) {
        std::vector<entities::Invader*> active_ships;
        for (auto& ship : fleet) {
            if (ship.is_alive) active_ships.push_back(&ship);
        }
        
        if (active_ships.empty()) return nullptr;
        
        // Retorna a nave que está no meio das naves ativas
        int mid_index = active_ships.size() / 2;
        return active_ships[mid_index];
    }

    std::pair<entities::Invader*, entities::Invader*> find_closest_pair(std::vector<entities::Invader>& fleet) {
        std::vector<entities::Invader*> active_ships;
        for (auto& ship : fleet) {
            if (ship.is_alive) active_ships.push_back(&ship);
        }

        if (active_ships.size() < 2) return {nullptr, nullptr};
        // Garante o comportamento orgânico da mira azul.
        float min_dist_sq = 99999999.0f;
        entities::Invader* p1 = nullptr;
        entities::Invader* p2 = nullptr;

        for (size_t i = 0; i < active_ships.size(); ++i) {
            for (size_t j = i + 1; j < active_ships.size(); ++j) {
                float dx = active_ships[i]->x - active_ships[j]->x;
                float dy = active_ships[i]->y - active_ships[j]->y;
                float dist_sq = dx * dx + dy * dy;

                if (dist_sq < min_dist_sq) {
                    min_dist_sq = dist_sq;
                    p1 = active_ships[i];
                    p2 = active_ships[j];
                }
            }
        }
        return {p1, p2};
    }

}