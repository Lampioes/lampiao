#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include <string>
#include <vector>

#include "Scene.h"



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

    void montarMenuPrincipal();
    void montarPausa();
    void montarControles();

    void desenharFundo();
    void desenharOpcoes(int inicioY);
};

#endif
