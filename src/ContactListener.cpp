#include "../include/ContactListener.h"
#include <bit>

void GameContactListener::BeginContact(b2Contact* contact) {
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    auto ponteiroa = fixtureA->GetUserData().pointer;
    auto ponteirob = fixtureB->GetUserData().pointer;

    DadosEntidade* entidadeA = std::bit_cast<DadosEntidade*>(ponteiroa);
    DadosEntidade* entidadeB = std::bit_cast<DadosEntidade*>(ponteirob);

    colisoesPendentes.push_back({entidadeA, entidadeB});
}

std::vector<ParColisao> GameContactListener::getAndClearCollisions() {
    std::vector<ParColisao> resultado = std::move(colisoesPendentes);
    colisoesPendentes.clear();
    return resultado;
}
