#ifndef CONTACT_LISTENER_H
#define CONTACT_LISTENER_H

#include <box2d/box2d.h>
#include <vector>
#include <utility>

enum class TipoEntidade {
    PLAYER,
    BULLET_PLAYER,
    BULLET_BANDIT,
    BANDIT,
    COW,
    FENCE,
    ROPE_TIP,
    TERRAIN
};

struct DadosEntidade {
    TipoEntidade tipo;
    int id;
};

struct ParColisao {
    DadosEntidade* a;
    DadosEntidade* b;
};

class GameContactListener : public b2ContactListener {
public:
    void BeginContact(b2Contact* contact) override;

    std::vector<ParColisao> getAndClearCollisions();

private:
    std::vector<ParColisao> colisoesPendentes;
};

#endif
