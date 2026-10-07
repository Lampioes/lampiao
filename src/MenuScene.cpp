#include "../include/MenuScene.h"

#include <cmath>

MenuScene::MenuScene(const ContextoJogo& contexto, TipoCena tipo)
    : Scene(contexto), tipo(tipo), historico(contexto.caminhoHistorico) {
    switch (tipo) {
    case TipoCena::PAUSA:
        montarPausa();
        break;
    case TipoCena::CONTROLES:
        montarControles();
        break;
    case TipoCena::HISTORICO:
        historico.carregar();
        montarHistorico();
        break;
    default:
        montarMenuPrincipal();
        break;
    }
}

void MenuScene::montarMenuPrincipal() {
    titulo = "LAMPIÕES";
    subtitulo = "JOGO DO VELHO OESTE";
    rodape = "ENTER PARA ESCOLHER";
    opcoes = {
        {"JOGAR", PedidoCena::trocar(TipoCena::JOGO)},
        {"CONTROLES", PedidoCena::empilhar(TipoCena::CONTROLES)},
        {"HISTÓRICO", PedidoCena::empilhar(TipoCena::HISTORICO)},
        {"SAIR", PedidoCena::sair()},
    };
}

void MenuScene::montarPausa() {
    titulo = "PAUSADO";
    subtitulo = "";
    rodape = "ESC PARA VOLTAR AO JOGO";
    opcoes = {
        {"CONTINUAR", PedidoCena::desempilhar()},
        {"CONTROLES", PedidoCena::empilhar(TipoCena::CONTROLES)},
        {"VOLTAR AO MENU", PedidoCena::trocar(TipoCena::MENU)},
        {"SAIR", PedidoCena::sair()},
    };
}

void MenuScene::montarControles() {
    titulo = "CONTROLES";
    subtitulo = "";
    rodape = "ESC OU ENTER PARA VOLTAR";
    linhas = {
        "SETAS ESQUERDA / DIREITA: ANDAR",
        "ESPAÇO: PULAR",
        "Z: ATIRAR",
        "ESC: PAUSAR O JOGO",
    };
    opcoes = {
        {"VOLTAR", PedidoCena::desempilhar()},
    };
}

void MenuScene::handleEvent(const SDL_Event& evento) {
    if (evento.type != SDL_KEYDOWN) return;

    if (tipo == TipoCena::HISTORICO) {
        const int total = static_cast<int>(historico.getRegistros().size());
        if (evento.key.keysym.sym == SDLK_LEFT && paginaHistorico > 0) --paginaHistorico;
        if (evento.key.keysym.sym == SDLK_RIGHT &&
            (paginaHistorico + 1) * REGISTROS_POR_PAGINA < total) ++paginaHistorico;
        montarHistorico();
    }

    const int total = static_cast<int>(opcoes.size());
    switch (evento.key.keysym.sym) {
    case SDLK_UP:
        opcaoSelecionada = (opcaoSelecionada - 1 + total) % total;
        break;
    case SDLK_DOWN:
        opcaoSelecionada = (opcaoSelecionada + 1) % total;
        break;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
    case SDLK_SPACE:
        pedido = opcoes[opcaoSelecionada].pedido;
        break;
    case SDLK_ESCAPE:
        pedido = (tipo == TipoCena::MENU) ? PedidoCena::sair() : PedidoCena::desempilhar();
        break;
    }
}

void MenuScene::update(float dt) {
    tempo += dt;
}

void MenuScene::montarHistorico() {
    titulo = "HISTÓRICO";
    rodape = "ESQUERDA / DIREITA: PAGINAS | ENTER OU ESC: VOLTAR";
    opcoes = {{"VOLTAR", PedidoCena::desempilhar()}};
    linhas.clear();
    const auto& registros = historico.getRegistros();
    const int total = static_cast<int>(registros.size());
    if (total == 0) linhas.push_back("Nenhuma pontuacao salva.");
    // Mais recentes primeiro, sem excluir pontuacoes antigas.
    for (int i = paginaHistorico * REGISTROS_POR_PAGINA;
         i < total && i < (paginaHistorico + 1) * REGISTROS_POR_PAGINA; ++i) {
        const ScoreEntry& registro = registros[total - 1 - i];
        linhas.push_back(registro.nome + "  -  " + std::to_string(registro.pontos) + " pontos");
    }
    if (!historico.getErro().empty()) linhas.push_back(historico.getErro());
    const int paginas = total == 0 ? 1 : (total + REGISTROS_POR_PAGINA - 1) / REGISTROS_POR_PAGINA;
    rodape = "PAGINA " + std::to_string(paginaHistorico + 1) + "/" + std::to_string(paginas) +
             " | " + rodape;
}

void MenuScene::desenharFundo() {
    SDL_Renderer* renderizacao = contexto.renderizacao;
    SDL_Rect telaInteira = {0, 0, contexto.telaLargura, contexto.telaAltura};

    if (transparente()) {
        SDL_SetRenderDrawBlendMode(renderizacao, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderizacao, 0, 0, 0, 160);
        SDL_RenderFillRect(renderizacao, &telaInteira);
        return;
    }

    const Sprite& fundo = contexto.sprites->get(SpriteId::FUNDO_MONTANHAS);
    if (fundo.valid()) {
        fundo.draw(renderizacao, telaInteira);
    } else {
        SDL_SetRenderDrawColor(renderizacao, 60, 45, 35, 255);
        SDL_RenderFillRect(renderizacao, &telaInteira);
    }

    SDL_SetRenderDrawBlendMode(renderizacao, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderizacao, 0, 0, 0, 130);
    SDL_RenderFillRect(renderizacao, &telaInteira);

    const Sprite& chapeu = contexto.sprites->get(SpriteId::CHAPEU);
    if (chapeu.valid()) {
        SDL_Rect ondecolocar = {contexto.telaLargura / 2 - 150, 130, 300, 200};
        chapeu.draw(renderizacao, ondecolocar);
    }
}

void MenuScene::desenharOpcoes(int inicioY) {
    SDL_Renderer* renderizacao = contexto.renderizacao;
    const int centroX = contexto.telaLargura / 2;
    const SDL_Color dourado = {255, 215, 0, 255};
    const SDL_Color cinza = {150, 150, 150, 255};
    const int espacamento = 90;

    for (int i = 0; i < static_cast<int>(opcoes.size()); ++i) {
        const bool selecionada = (i == opcaoSelecionada);
        const int y = inicioY + i * espacamento;
        drawTextoCentralizado(contexto.fonteMenu, opcoes[i].rotulo, centroX, y,
                              selecionada ? dourado : cinza);

        if (selecionada) {
            const int piscada = static_cast<int>((std::sin(tempo * 6.0f) + 1.0f) * 60.0f);
            SDL_SetRenderDrawColor(renderizacao, 255, 215, 0, static_cast<Uint8>(120 + piscada));
            const int larguraRotulo = larguraTexto(contexto.fonteMenu, opcoes[i].rotulo);
            SDL_Rect marcador = {centroX - larguraRotulo / 2 - 60, y + 20, 26, 26};
            SDL_RenderFillRect(renderizacao, &marcador);
        }
    }
}

void MenuScene::draw() {
    SDL_Renderer* renderizacao = contexto.renderizacao;
    const int centroX = contexto.telaLargura / 2;

    desenharFundo();

    const SDL_Color dourado = {255, 215, 0, 255};
    const SDL_Color branco = {240, 240, 240, 255};
    const SDL_Color cinza = {150, 150, 150, 255};

    if (tipo == TipoCena::MENU) {
        drawTextoCentralizado(contexto.fonteTitulo, titulo, centroX, 360, dourado);
        drawTextoCentralizado(contexto.fonteHud, subtitulo, centroX, 500, branco);
        desenharOpcoes(620);
    } else {
        drawTextoCentralizado(contexto.fonteTitulo, titulo, centroX, 180, dourado);

        int y = 380;
        for (const std::string& linha : linhas) {
            drawTextoCentralizado(contexto.fonteHud, linha, centroX, y, branco);
            y += 60;
        }
        desenharOpcoes(linhas.empty() ? 400 : y + 40);
    }

    drawTextoCentralizado(contexto.fonteHud, rodape, centroX, contexto.telaAltura - 80, cinza);

    SDL_SetRenderDrawBlendMode(renderizacao, SDL_BLENDMODE_NONE);
}
