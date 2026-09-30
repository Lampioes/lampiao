#include "../include/SpriteCatalog.h"

namespace {
const char* CAMINHOS[SpriteCatalog::TOTAL_SPRITES] = {
    "../sprites/vaca.png",
    "../sprites/bullet.png",
    "../sprites/output_ixt5mp.gif",
    "../sprites/run-1.png",
    "../sprites/run-2-e-parado.png",
    "../sprites/run-3.png",
    "../sprites/Chapeu lampiões.png",
    "../sprites/montanhas.jpg",
    "../sprites/transicao-montanhas-deserto.jpg",
    "../sprites/calica-deserto.png",
    "../sprites/cidade-deserto.jpeg",
};
}

void SpriteCatalog::loadAll(SDL_Renderer* renderer) {
    for (int i = 0; i < TOTAL_SPRITES; ++i) {
        sprites[i].load(renderer, CAMINHOS[i]);
        if (!sprites[i].valid()) SDL_Log("Nao consegui carregar a sprite %s", CAMINHOS[i]);
    }
}

void SpriteCatalog::clear() {
    for (auto& s : sprites) s = Sprite{};
}
