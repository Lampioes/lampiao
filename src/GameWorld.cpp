#include "../include/GameWorld.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "../include/Bandit.h"
#include "../include/Bullet.h"
#include "../include/Cow.h"

GameWorld::GameWorld(const ContextoJogo& contexto) : Scene(contexto) {
    mundo = std::make_unique<b2World>(b2Vec2(0.0f, GRAVIDADE));
    mundo->SetContactListener(&ouvinteContato);

    loadSounds();
    setupLevel();
    updateCamera();
}

GameWorld::~GameWorld() {
    for (auto& o : objetos) o->destroyBody(*mundo);
    objetos.clear();
    paraAdicionar.clear();
    jogador = nullptr;
    terreno = nullptr;

    if (somPulo) {
        Mix_FreeChunk(somPulo);
        somPulo = nullptr;
    }
    if (somTiro) {
        Mix_FreeChunk(somTiro);
        somTiro = nullptr;
    }
}

void GameWorld::loadSounds() {
    somPulo = Mix_LoadWAV("../audio/jump.wav");
    somTiro = Mix_LoadWAV("../audio/freesound.wav");

    if (somPulo) Mix_VolumeChunk(somPulo, MIX_MAX_VOLUME / 2);
}

void GameWorld::setupLevel() {
    auto terrenoPtr = std::make_unique<Terrain>(*mundo);
    terreno = terrenoPtr.get();
    objetos.push_back(std::move(terrenoPtr));

    const float jogadorInicioX = 300.0f;
    const float jogadorInicioY = terreno->getHeightAt(jogadorInicioX) - 80.0f;
    auto jogadorPtr = std::make_unique<Player>(*mundo, *contexto.sprites, b2Vec2(jogadorInicioX, jogadorInicioY));
    jogador = jogadorPtr.get();
    objetos.push_back(std::move(jogadorPtr));

    const float larguraCiclo = static_cast<float>(Terrain::NUM_ZONAS * Terrain::TELA_W);
    const float espacoEntreVacas = larguraCiclo / NUMERO_VACAS;
    for (int i = 0; i < NUMERO_VACAS; ++i) {
        const float aleatoridadeVcas = static_cast<float>((i * 137) % 200) - 100.0f;
        const float vacaX = espacoEntreVacas * (i + 0.5f) + aleatoridadeVcas;
        const float vacaY = terreno->getHeightAt(vacaX) - 25.0f;
        objetos.push_back(std::make_unique<Cow>(*mundo, contexto.sprites->get(SpriteId::VACA),
                                                b2Vec2(vacaX, vacaY), i));
    }
}

void GameWorld::handleEvent(const SDL_Event& evento) {
    
    
    if (evento.type == SDL_KEYDOWN && evento.key.keysym.sym == SDLK_ESCAPE) {
        pedido = PedidoCena::empilhar(TipoCena::PAUSA);
        return;
    }

    
    if (terminado) {
        if (evento.type == SDL_KEYDOWN) pedido = PedidoCena::trocar(TipoCena::MENU);
        return;
    }

    for (auto& o : objetos) o->handleEvent(evento);
}

void GameWorld::update(float dt) {
    if (terminado) return;

    mundo->Step(dt, 6, 2);

    for (auto& o : objetos) {
        o->update(dt);
    }

    handlePlayerActions();

    const float jogadorPx = jogador->getBody()->GetPosition().x * PIXELSPORMETRO;
    forEach<Bandit>([&](Bandit& bandido) {
        if (bandido.shouldShoot(dt, jogadorPx)) {
            const float bx = bandido.getBody()->GetPosition().x * PIXELSPORMETRO;
            const float by = bandido.getBody()->GetPosition().y * PIXELSPORMETRO;
            const float dirX = bandido.getShootDirX(jogadorPx);
            paraAdicionar.push_back(std::make_unique<Bullet>(
                *mundo, contexto.sprites->get(SpriteId::BALA),
                b2Vec2(bx + dirX * 30.0f, by), b2Vec2(dirX, 0.0f), false));
        }
    });

    temporizadorSpawnBandido += dt;
    if (temporizadorSpawnBandido >= intervaloSpawnBandido) {
        int bandidosVivos = 0;
        forEach<Bandit>([&](Bandit& bandido) {
            if (bandido.isAlive()) bandidosVivos++;
        });
        if (bandidosVivos < MAX_BANDIDOS_VIVOS) {
            spawnBandit();
            temporizadorSpawnBandido = 0.0f;
        } else {
            temporizadorSpawnBandido = intervaloSpawnBandido;
        }
    }

    processCollisions();
    cleanupDead();
    flushSpawns();
    updateCamera();

    if (!jogador->isAlive() || jogador->hasWon()) {
        terminado = true;
        venceu = jogador->hasWon();
    }
}

void GameWorld::handlePlayerActions() {
    if (jogador->consumeJumped() && somPulo) {
        Mix_PlayChannel(-1, somPulo, 0);
    }

    if (jogador->consumeShot()) {
        const float posicaoX = jogador->getBody()->GetPosition().x * PIXELSPORMETRO;
        const float posicaoY = jogador->getBody()->GetPosition().y * PIXELSPORMETRO;
        const float direcao = jogador->getShootDirX();
        const float spawnX = posicaoX + direcao * 80.0f;

        if (somTiro) Mix_PlayChannel(-1, somTiro, 0);
        paraAdicionar.push_back(std::make_unique<Bullet>(
            *mundo, contexto.sprites->get(SpriteId::BALA),
            b2Vec2(spawnX, posicaoY), b2Vec2(direcao, 0.0f), true));
    }
}

void GameWorld::updateCamera() {
    const int jogadorPixelX = static_cast<int>(jogador->getBody()->GetPosition().x * PIXELSPORMETRO);
    camera.setPosition(static_cast<float>(jogadorPixelX - contexto.telaLargura / 2), 0.0f);
}

void GameWorld::spawnBandit() {
    const float jogadorPx = jogador->getBody()->GetPosition().x * PIXELSPORMETRO;
    const float lado = (std::rand() % 2 == 0) ? 1.0f : -1.0f;
    const float spawnX = jogadorPx + lado * (contexto.telaLargura * 0.6f);
    const float spawnY = terreno->getHeightAt(spawnX) - 150.0f;

    paraAdicionar.push_back(std::make_unique<Bandit>(
        *mundo, contexto.sprites->get(SpriteId::BANDIDO), b2Vec2(spawnX, spawnY), proxIdBandido++));
}

void GameWorld::flushSpawns() {
    for (auto& o : paraAdicionar) {
        objetos.push_back(std::move(o));
    }
    paraAdicionar.clear();
}

void GameWorld::processCollisions() {
    auto colisoes = ouvinteContato.getAndClearCollisions();

    for (auto& col : colisoes) {
        DadosEntidade* a = col.a;
        DadosEntidade* b = col.b;
        if (!a || !b) continue;

        if (a->tipo > b->tipo) std::swap(a, b);

        if (a->tipo == TipoEntidade::BULLET_PLAYER && b->tipo == TipoEntidade::BANDIT) {
            forEach<Bullet>([](Bullet& bala) {
                if (bala.isFromPlayer() && bala.isAlive()) bala.kill();
            });
            forEach<Bandit>([&](Bandit& bandido) {
                if (bandido.getId() == b->id && bandido.isAlive()) {
                    bandido.takeDamage();
                    if (!bandido.isAlive()) pontuacao += 100;
                }
            });
        }

        if (a->tipo == TipoEntidade::PLAYER && b->tipo == TipoEntidade::BULLET_BANDIT) {
            jogador->takeDamage();
            forEach<Bullet>([](Bullet& bala) {
                if (!bala.isFromPlayer() && bala.isAlive()) bala.kill();
            });
        }

        if (a->tipo == TipoEntidade::PLAYER && b->tipo == TipoEntidade::TERRAIN) jogador->setOnGround(true);

        if (b->tipo == TipoEntidade::TERRAIN && (a->tipo == TipoEntidade::BULLET_PLAYER || a->tipo == TipoEntidade::BULLET_BANDIT)) {
            forEach<Bullet>([](Bullet& bala) {
                if (bala.isAlive()) bala.kill();
            });
        }

        if (a->tipo == TipoEntidade::PLAYER && b->tipo == TipoEntidade::COW) {
            forEach<Cow>([&](Cow& vaca) {
                if (vaca.getId() == b->id && vaca.isAlive()) {
                    vaca.kill();
                    jogador->captureCow();
                }
            });
        }
    }
}

void GameWorld::cleanupDead() {
    objetos.erase(std::remove_if(objetos.begin(), objetos.end(),
        [this](std::unique_ptr<GameObject>& o) {
            if (o.get() == jogador) return false;
            if (!o->isAlive()) {
                o->destroyBody(*mundo);
                return true;
            }
            return false;
        }),
        objetos.end());
}

void GameWorld::draw() {
    renderBackgrounds();

    for (auto& o : objetos) o->draw(contexto.renderizacao, camera);

    renderHUD();

    if (terminado) renderFimDeJogo();
}

void GameWorld::renderBackgrounds() {
    static const SpriteId FUNDOS[NUMERO_BACKGROUNDS] = {
        SpriteId::FUNDO_MONTANHAS,
        SpriteId::FUNDO_TRANSICAO,
        SpriteId::FUNDO_CALICA,
        SpriteId::FUNDO_CIDADE,
        SpriteId::FUNDO_CALICA,
    };

    const int telaLargura = contexto.telaLargura;
    const int zonaInicial = static_cast<int>(std::floor(static_cast<float>(camera.x()) / telaLargura));

    for (int z = zonaInicial; z <= zonaInicial + 2; ++z) {
        const int telaX = z * telaLargura - camera.x();
        if (telaX + telaLargura < 0 || telaX > telaLargura) continue;

        const int indiceFundo = ((z % NUMERO_BACKGROUNDS) + NUMERO_BACKGROUNDS) % NUMERO_BACKGROUNDS;
        SDL_Rect destino = {telaX, -camera.y(), telaLargura, contexto.telaAltura};

        contexto.sprites->get(FUNDOS[indiceFundo]).draw(contexto.renderizacao, destino);
    }
}

void GameWorld::renderHUD() {
    SDL_Renderer* renderizacao = contexto.renderizacao;

    SDL_SetRenderDrawColor(renderizacao, 220, 30, 30, 255);
    for (int i = 0; i < jogador->getHealth(); ++i) {
        SDL_Rect coracao = {20 + i * 30, 20, 24, 24};
        SDL_RenderFillRect(renderizacao, &coracao);
    }

    SDL_SetRenderDrawColor(renderizacao, 255, 255, 255, 255);
    forEach<Cow>([&](Cow& vaca) {
        if (vaca.isAlive()) {
            SDL_Rect vaquinhas_coletadas = {800 + vaca.getId() * 30, 10, 14, 14};
            SDL_RenderFillRect(renderizacao, &vaquinhas_coletadas);
        }
    });

    SDL_SetRenderDrawColor(renderizacao, 255, 215, 0, 255);
    const int larguraBarraPontuacao = std::min(pontuacao, 500);
    SDL_Rect barraPontuacao = {20, 55, larguraBarraPontuacao, 10};
    SDL_RenderFillRect(renderizacao, &barraPontuacao);

    const SDL_Color branco = {240, 240, 240, 255};
    drawTexto(contexto.fonteHud, "PONTOS: " + std::to_string(pontuacao), 20, 80, branco);
    drawTexto(contexto.fonteHud, "VACAS: " + std::to_string(jogador->getCapturedCows()) + "/" +
                                 std::to_string(jogador->getCowsToWin()), 20, 115, branco);
}

void GameWorld::renderFimDeJogo() {
    SDL_Renderer* renderizacao = contexto.renderizacao;
    const int centroX = contexto.telaLargura / 2;

    SDL_Rect telaInteira = {0, 0, contexto.telaLargura, contexto.telaAltura};
    SDL_SetRenderDrawBlendMode(renderizacao, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderizacao, 0, 0, 0, 170);
    SDL_RenderFillRect(renderizacao, &telaInteira);
    SDL_SetRenderDrawBlendMode(renderizacao, SDL_BLENDMODE_NONE);

    const SDL_Color dourado = {255, 215, 0, 255};
    const SDL_Color vermelho = {220, 60, 60, 255};
    const SDL_Color branco = {240, 240, 240, 255};

    if (venceu) {
        drawTextoCentralizado(contexto.fonteTitulo, "VOCÊ VENCEU!", centroX, contexto.telaAltura / 2 - 140, dourado);
    } else {
        drawTextoCentralizado(contexto.fonteTitulo, "VOCÊ MORREU", centroX, contexto.telaAltura / 2 - 140, vermelho);
    }

    drawTextoCentralizado(contexto.fonteMenu, "PONTOS: " + std::to_string(pontuacao), centroX,
                          contexto.telaAltura / 2 + 20, branco);
    drawTextoCentralizado(contexto.fonteHud, "APERTE QUALQUER TECLA PRA VOLTAR AO MENU", centroX,
                          contexto.telaAltura / 2 + 140, branco);
}
