#include "ScoreHistory.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    // O chamador fornece uma pasta temporaria exclusiva para este teste.
    assert(argc == 2);
    const std::filesystem::path pasta = argv[1];
    std::filesystem::create_directories(pasta);
    const auto arquivo = pasta / "pontuacoes.txt";
    assert(!std::filesystem::exists(arquivo));
    ScoreHistory historico(arquivo.string());
    assert(historico.carregar());
    assert(historico.getRegistros().empty());
    assert(!historico.salvar("   ", 100));
    assert(!historico.salvar("Lucas", -1));
    assert(historico.salvar("  Jo\xC3\xA3o Silva  ", 300));
    assert(historico.salvar("Ana \"Lampiao\"", 0));
    // Simula fechar e abrir o jogo usando uma nova instancia.
    ScoreHistory reaberto(arquivo.string());
    assert(reaberto.carregar());
    assert(reaberto.getErro().empty());
    assert(reaberto.getRegistros().size() == 2);
    assert(reaberto.getRegistros()[0].nome == "Jo\xC3\xA3o Silva");
    assert(reaberto.getRegistros()[0].pontos == 300);
    assert(reaberto.getRegistros()[1].nome == "Ana \"Lampiao\"");
    assert(reaberto.getRegistros()[1].pontos == 0);
    assert(reaberto.carregar());
    assert(reaberto.getRegistros().size() == 2); // Recarregar nao duplica.
    {
        std::ofstream corrompido(arquivo, std::ios::app);
        corrompido << "\nlinha quebrada\n-10 \"Invalido\"\n50 \"Extra\" lixo";
    }
    assert(reaberto.carregar());
    assert(reaberto.getRegistros().size() == 2);
    assert(!reaberto.getErro().empty());
    assert(reaberto.salvar("Depois do erro", 500));
    assert(reaberto.carregar());
    assert(reaberto.getRegistros().size() == 3);
    ScoreHistory indisponivel("");
    assert(!indisponivel.carregar());
    assert(!indisponivel.salvar("Lucas", 100));
    ScoreHistory pastaComoArquivo(pasta.string());
    assert(!pastaComoArquivo.salvar("Lucas", 100));
    std::cout << "Testes do historico passaram.\n";
}
