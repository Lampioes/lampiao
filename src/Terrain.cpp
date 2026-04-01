#include "../include/Terrain.h"
#include "../include/ContactListener.h"
#include <cmath>
#include <algorithm>

Terrain::Terrain(b2World& world) {
    pointSpacing = static_cast<float>(NUM_ZONES * SCREEN_W) / TOTAL_POINTS;

    generateHeightmap();
    createPhysicsBody(world);
}

Terrain::~Terrain() {}

void Terrain::generateHeightmap() {
    // Alturas base alinhadas com a linha do chão em cada imagem de fundo
    const float baseHeights[NUM_ZONES] = {
        700.0f,  // montanhas - chão alto
        760.0f,  // transicao montanhas-deserto - desce
        840.0f,  // calica deserto - chão baixo
        670.0f,  // cidade deserto - chão alto (plataformas)
        840.0f   // calica deserto novamente
    };

    const float amplitude[NUM_ZONES] = {
        40.0f,   // montanhas - variação moderada
        35.0f,   // transicao - variação moderada
        15.0f,   // deserto - quase plano
        25.0f,   // cidade - alguma variação
        15.0f    // deserto - quase plano
    };

    for (int i = 0; i <= TOTAL_POINTS; ++i) {
        // Posição fracionária dentro da zona atual
        int zone = std::min(i / POINTS_PER_ZONE, NUM_ZONES - 1);
        int nextZone = std::min(zone + 1, NUM_ZONES - 1);
        float posInZone = static_cast<float>(i % POINTS_PER_ZONE) / POINTS_PER_ZONE;

        // Interpola suavemente entre zonas para evitar saltos bruscos
        float base = baseHeights[zone] * (1.0f - posInZone) + baseHeights[nextZone] * posInZone;
        float amp = amplitude[zone] * (1.0f - posInZone) + amplitude[nextZone] * posInZone;

        float t = static_cast<float>(i) / TOTAL_POINTS;
        float variation =
            std::sin(t * 3.14159f * 6.0f) * 0.5f +
            std::sin(t * 3.14159f * 14.0f) * 0.3f +
            std::sin(t * 3.14159f * 25.0f) * 0.2f;

        heights[i] = base + amp * variation;
        heights[i] = std::clamp(heights[i], 500.0f, 1000.0f);
    }

    for (int pass = 0; pass < 3; ++pass) {
        std::array<float, TOTAL_POINTS + 1> smoothed = heights;
        for (int i = 1; i < TOTAL_POINTS; ++i) {
            smoothed[i] = (heights[i - 1] + heights[i] * 2.0f + heights[i + 1]) / 4.0f;
        }
        heights = smoothed;
    }
}

void Terrain::createPhysicsBody(b2World& world) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(0.0f, 0.0f);
    groundBody = world.CreateBody(&bodyDef);

    std::vector<b2Vec2> vertices(TOTAL_POINTS + 1);
    for (int i = 0; i <= TOTAL_POINTS; ++i) {
        float pixelX = i * pointSpacing;
        float pixelY = heights[i];
        vertices[i].Set(pixelX / P2M, pixelY / P2M);
    }

    b2ChainShape chain;
    chain.CreateChain(vertices.data(), static_cast<int>(vertices.size()),
                      vertices.front(), vertices.back());

    b2FixtureDef fixDef;
    fixDef.shape = &chain;
    fixDef.friction = 0.6f;

    EntityData* data = new EntityData{EntityType::TERRAIN, 0};
    fixDef.userData.pointer = reinterpret_cast<uintptr_t>(data);

    groundBody->CreateFixture(&fixDef);
}

float Terrain::getHeightAt(float pixelX) const {
    float totalWidth = TOTAL_POINTS * pointSpacing;
    pixelX = std::clamp(pixelX, 0.0f, totalWidth);

    float index = pixelX / pointSpacing;
    int i0 = static_cast<int>(index);
    int i1 = std::min(i0 + 1, TOTAL_POINTS);
    float frac = index - i0;

    return heights[i0] * (1.0f - frac) + heights[i1] * frac;
}

void Terrain::draw(SDL_Renderer* renderer, int cameraX) {
    float startX = static_cast<float>(cameraX);
    float endX = startX + SCREEN_W;

    int startIdx = std::max(0, static_cast<int>(startX / pointSpacing) - 1);
    int endIdx = std::min(TOTAL_POINTS, static_cast<int>(endX / pointSpacing) + 2);

    SDL_SetRenderDrawColor(renderer, 139, 90, 43, 255);

    for (int i = startIdx; i < endIdx; ++i) {
        int x1 = static_cast<int>(i * pointSpacing) - cameraX;
        int x2 = static_cast<int>((i + 1) * pointSpacing) - cameraX;
        int y1 = static_cast<int>(heights[i]);
        int y2 = static_cast<int>(heights[i + 1]);

        int minY = std::min(y1, y2);
        int w = x2 - x1;
        if (w <= 0) continue;

        SDL_Rect rect = {x1, minY, w, SCREEN_H - minY};
        SDL_RenderFillRect(renderer, &rect);

        if (y1 != y2) {
            float slope = static_cast<float>(y2 - y1) / w;
            for (int px = 0; px < w; ++px) {
                int py = y1 + static_cast<int>(slope * px);
                if (py > minY) {
                    SDL_RenderDrawLine(renderer, x1 + px, minY, x1 + px, py);
                }
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 100, 65, 25, 255);
    for (int i = startIdx; i < endIdx; ++i) {
        int x1 = static_cast<int>(i * pointSpacing) - cameraX;
        int x2 = static_cast<int>((i + 1) * pointSpacing) - cameraX;
        int y1 = static_cast<int>(heights[i]);
        int y2 = static_cast<int>(heights[i + 1]);
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}
