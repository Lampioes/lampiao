#include "../include/Terrain.h"
#include "../include/ContactListener.h"
#include <algorithm>
#include <bit>
#include <cmath>

Terrain::Terrain(b2World& world) : StaticObject(world, 0.0f, 0.0f) {
    espacamentoPontos = static_cast<float>(NUM_ZONAS * TELA_W) / TOTAL_PONTOS;

    generateHeightmap();
    createChainFixture();
}

void Terrain::generateHeightmap() {
    const float alturasBase[NUM_ZONAS] = {
        700.0f,
        760.0f,
        840.0f,
        670.0f,
        840.0f
    };

    const float amplitude[NUM_ZONAS] = {
        40.0f,
        35.0f,
        15.0f,
        25.0f,
        15.0f
    };

    for (int altura = 0; altura <= TOTAL_PONTOS; ++altura) {
        int zona = (altura / PONTOS_POR_ZONA) % NUM_ZONAS;
        int proximaZona = (zona + 1) % NUM_ZONAS;
        float posNaZona = static_cast<float>(altura % PONTOS_POR_ZONA) / PONTOS_POR_ZONA;

        float base = alturasBase[zona] * (1.0f - posNaZona) + alturasBase[proximaZona] * posNaZona;
        float amp = amplitude[zona] * (1.0f - posNaZona) + amplitude[proximaZona] * posNaZona;

        float t = static_cast<float>(altura) / TOTAL_PONTOS;
        float variacao =
            std::sin(t * 3.14159f * 6.0f) * 0.5f +
            std::sin(t * 3.14159f * 14.0f) * 0.3f +
            std::sin(t * 3.14159f * 24.0f) * 0.2f;

        alturas[altura] = base + amp * variacao;
        alturas[altura] = std::clamp(alturas[altura], 500.0f, 1000.0f);
    }

    for (int passe = 0; passe < 3; ++passe) {
        std::array<float, TOTAL_PONTOS + 1> suavizado = alturas;
        for (int i = 1; i < TOTAL_PONTOS; ++i) {
            suavizado[i] = (alturas[i - 1] + alturas[i] * 2.0f + alturas[i + 1]) / 4.0f;
        }
        alturas = suavizado;
    }

    alturas[TOTAL_PONTOS] = alturas[0];
}

void Terrain::createChainFixture() {
    const int totalVertices = NUM_REPETICOES * TOTAL_PONTOS + 1;
    const int meioRepeticoes = NUM_REPETICOES / 2;
    const float offsetPixel = -static_cast<float>(meioRepeticoes) * TOTAL_PONTOS * espacamentoPontos;

    std::vector<b2Vec2> vertices(totalVertices);
    for (int i = 0; i < totalVertices; ++i) {
        int idxAltura = i % TOTAL_PONTOS;
        float pixelX = offsetPixel + i * espacamentoPontos;
        float pixelY = alturas[idxAltura];
        vertices[i].Set(pixelX / P2M, pixelY / P2M);
    }

    b2ChainShape correia;
    correia.CreateChain(vertices.data(), static_cast<int>(vertices.size()),
                      vertices.front(), vertices.back());

    b2FixtureDef defFixacao;
    defFixacao.shape = &correia;
    defFixacao.friction = 0.6f;

    DadosEntidade* dados = new DadosEntidade{TipoEntidade::TERRAIN, 0};
    defFixacao.userData.pointer = std::bit_cast<uintptr_t>(dados);

    corpo->CreateFixture(&defFixacao);
}

float Terrain::getHeightAt(float pixelX) const {
    const float larguraCiclo = TOTAL_PONTOS * espacamentoPontos;

    float pixelXLocal = std::fmod(pixelX, larguraCiclo);
    if (pixelXLocal < 0.0f) pixelXLocal += larguraCiclo;

    float indice = pixelXLocal / espacamentoPontos;
    int i0 = static_cast<int>(indice);
    int i1 = (i0 + 1) % TOTAL_PONTOS;
    i0 = i0 % TOTAL_PONTOS;
    float fracao = indice - std::floor(indice);

    return alturas[i0] * (1.0f - fracao) + alturas[i1] * fracao;
}

void Terrain::draw(SDL_Renderer* renderer, int cameraX) {
    float inicioX = static_cast<float>(cameraX);
    float fimX = inicioX + TELA_W;

    int indiceInicio = static_cast<int>(std::floor(inicioX / espacamentoPontos)) - 1;
    int indiceFim = static_cast<int>(std::floor(fimX / espacamentoPontos)) + 2;

    auto alturaEm = [this](int i) -> float {
        int idx = ((i % TOTAL_PONTOS) + TOTAL_PONTOS) % TOTAL_PONTOS;
        return alturas[idx];
    };

    SDL_SetRenderDrawColor(renderer, 139, 90, 43, 255);

    for (int i = indiceInicio; i < indiceFim; ++i) {
        int x1 = static_cast<int>(i * espacamentoPontos) - cameraX;
        int x2 = static_cast<int>((i + 1) * espacamentoPontos) - cameraX;
        int y1 = static_cast<int>(alturaEm(i));
        int y2 = static_cast<int>(alturaEm(i + 1));

        int yMin = std::min(y1, y2);
        int largura = x2 - x1;
        if (largura <= 0) continue;

        SDL_Rect retangulo = {x1, yMin, largura, TELA_H - yMin};
        SDL_RenderFillRect(renderer, &retangulo);

        if (y1 != y2) {
            float inclinacao = static_cast<float>(y2 - y1) / largura;
            for (int px = 0; px < largura; ++px) {
                int py = y1 + static_cast<int>(inclinacao * px);
                if (py > yMin) SDL_RenderDrawLine(renderer, x1 + px, yMin, x1 + px, py);
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 100, 65, 25, 255);
    for (int i = indiceInicio; i < indiceFim; ++i) {
        int x1 = static_cast<int>(i * espacamentoPontos) - cameraX;
        int x2 = static_cast<int>((i + 1) * espacamentoPontos) - cameraX;
        int y1 = static_cast<int>(alturaEm(i));
        int y2 = static_cast<int>(alturaEm(i + 1));
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}
