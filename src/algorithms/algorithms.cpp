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

        // Proteção contra Segfault: garante que há pelo menos 2 naves
        // e impede divisão por zero no módulo (n - 1)
        if (active_ships.size() < 2) return {nullptr, nullptr};

        size_t n = active_ships.size();

        // Fórmula pseudoaleatória ancorada ao tamanho da frota ativa.
        // O valor 37 é um número primo arbitrário para gerar dispersão.
        // O módulo (n - 1) garante que o índice máximo gerado é (n - 2).
        size_t pseudo_random_index = (n * 37) % (n - 1);

        // Retorna o par gerado e o seu adjacente imediato.
        // Como o índice máximo é (n - 2), pseudo_random_index + 1 será no máximo (n - 1),
        // que é exatamente a última posição válida do vetor, garantindo acesso seguro à memória.
        return {active_ships[pseudo_random_index], active_ships[pseudo_random_index + 1]};
    }

}