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
        
        // Retorna a primeira e a última nave ativa para evitar erros de índice
        return {active_ships.front(), active_ships.back()};
    }

}