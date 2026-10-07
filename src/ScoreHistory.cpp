#include "../include/ScoreHistory.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace {
std::filesystem::path caminhoUtf8(const std::string& caminho) {
    return std::filesystem::path(std::u8string(caminho.begin(), caminho.end()));
}
}

std::string ScoreHistory::limparNome(const std::string& nome) {
    std::string resultado;
    for (unsigned char letra : nome) {
        if (letra >= 32 && letra != 127) resultado += static_cast<char>(letra);
    }
    const auto inicio = resultado.find_first_not_of(' ');
    if (inicio == std::string::npos) return "";
    const auto fim = resultado.find_last_not_of(' ');
    return resultado.substr(inicio, fim - inicio + 1);
}

bool ScoreHistory::carregar() {
    registros.clear();
    erro.clear();
    if (caminho.empty()) {
        erro = "Pasta do historico indisponivel.";
        return false;
    }
    std::ifstream arquivo(caminhoUtf8(caminho));
    if (!arquivo) {
        std::error_code codigo;
        const bool existe = std::filesystem::exists(caminhoUtf8(caminho), codigo);
        if (!existe && !codigo) return true; // Primeira execucao: ainda nao ha partidas.
        erro = "Nao foi possivel ler o historico.";
        return false;
    }

    std::string linha;
    while (std::getline(arquivo, linha)) {
        if (linha.find_first_not_of(" \t\r") == std::string::npos) continue;
        std::istringstream leitura(linha);
        ScoreEntry registro;
        if (leitura >> registro.pontos >> std::quoted(registro.nome)) {
            leitura >> std::ws;
            registro.nome = limparNome(registro.nome);
            if (leitura.eof() && registro.pontos >= 0 && !registro.nome.empty()) {
                registros.push_back(registro);
                continue;
            }
        }
        erro = "Alguns registros invalidos foram ignorados.";
    }
    if (arquivo.bad()) {
        erro = "Erro durante a leitura do historico.";
        return false;
    }
    return true;
}

bool ScoreHistory::salvar(const std::string& nome, int pontos) {
    erro.clear();
    const std::string nomeLimpo = limparNome(nome);
    if (nomeLimpo.empty() || pontos < 0) {
        erro = "Digite um nome para salvar.";
        return false;
    }
    if (caminho.empty()) {
        erro = "Pasta do historico indisponivel.";
        return false;
    }
    std::ofstream arquivo(caminhoUtf8(caminho), std::ios::app);
    if (!arquivo) {
        erro = "Nao foi possivel abrir o arquivo para salvar.";
        return false;
    }
    arquivo << '\n' << pontos << ' ' << std::quoted(nomeLimpo);
    arquivo.close();
    if (!arquivo) {
        erro = "Falha ao gravar a pontuacao. Tente novamente.";
        return false;
    }
    return true;
}
