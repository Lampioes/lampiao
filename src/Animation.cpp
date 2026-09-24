#include "../include/Animation.h"

void Animation::addFrame(const Sprite& sprite) {
    quadros.push_back(&sprite);
}

void Animation::tick(float dt) {
    if (quadros.size() < 2 || duracaoFrame <= 0.0f) return;
    tempoAcumulado += dt;
    while (tempoAcumulado >= duracaoFrame) {
        tempoAcumulado -= duracaoFrame;
        frameAtual = (frameAtual + 1) % static_cast<int>(quadros.size());
    }
}

void Animation::setFrame(int index) {
    if (quadros.empty()) return;
    frameAtual = index % static_cast<int>(quadros.size());
    if (frameAtual < 0) frameAtual += static_cast<int>(quadros.size());
    tempoAcumulado = 0.0f;
}

void Animation::draw(SDL_Renderer* renderer, const SDL_Rect& dst, SDL_RendererFlip flip) const {
    if (quadros.empty()) return;
    quadros[frameAtual]->draw(renderer, dst, flip);
}
