#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include <string>
#include <vector>

#include "Scene.h"
#include "ScoreHistory.h"



class MenuScene : public Scene {
public:
    MenuScene(const ContextoJogo& contexto, TipoCena tipo);

    void handleEvent(const SDL_Event& evento) override;
    void update(float dt) override;
    void draw() override;

    bool transparente() const override { return tipo != TipoCena::MENU; }

private:
    struct Opcao {
        std::string rotulo;
        PedidoCena pedido;
    };

    TipoCena tipo;
    std::string titulo;
    std::string subtitulo;
    std::string rodape;
    std::vector<Opcao> opcoes;
    std::vector<std::string> linhas;  

    int opcaoSelecionada = 0;
    float tempo = 0.0f;
    ScoreHistory historico;
    int paginaHistorico = 0;
    static constexpr int REGISTROS_POR_PAGINA = 6;

    void montarMenuPrincipal();
    void montarPausa();
    void montarControles();
    void montarHistorico();

    void desenharFundo();
    void desenharOpcoes(int inicioY);
};

#endif
