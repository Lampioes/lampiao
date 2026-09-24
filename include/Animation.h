#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL.h>
#include <vector>

#include "Sprite.h"

class Animation {
public:
    Animation() = default;
    explicit Animation(float frameDuration) : duracaoFrame(frameDuration) {}

    // recebe a sprite ja carregada (a SpriteCatalog e a dona dela)
    void addFrame(const Sprite& sprite);

    void tick(float dt);
    void reset() { tempoAcumulado = 0.0f; frameAtual = 0; }
    void setFrame(int index);
    void setFrameDuration(float d) { duracaoFrame = d; }

    void draw(SDL_Renderer* renderer, const SDL_Rect& dst, SDL_RendererFlip flip = SDL_FLIP_NONE) const;

    int currentFrame() const { return frameAtual; }
    int frameCount() const { return static_cast<int>(quadros.size()); }
    bool empty() const { return quadros.empty(); }

private:
    std::vector<const Sprite*> quadros;
    int frameAtual = 0;
    float tempoAcumulado = 0.0f;
    float duracaoFrame = 0.15f;
};

#endif
