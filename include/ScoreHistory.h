#ifndef SCORE_HISTORY_H
#define SCORE_HISTORY_H

#include <string>
#include <vector>

struct ScoreEntry {
    std::string nome;
    int pontos = 0;
};

class ScoreHistory {
public:
    explicit ScoreHistory(const std::string& caminho) : caminho(caminho) {}

    bool carregar();
    bool salvar(const std::string& nome, int pontos);
    const std::vector<ScoreEntry>& getRegistros() const { return registros; }
    const std::string& getErro() const { return erro; }
    static std::string limparNome(const std::string& nome);

private:
    std::string caminho;
    std::vector<ScoreEntry> registros;
    std::string erro;
};

#endif
