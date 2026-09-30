#ifndef SCENE_H
#define SCENE_H

#include <string>

#include <SDL.h>
#include <SDL_ttf.h>

#include "SpriteCatalog.h"

enum class TipoCena {
    MENU,
    JOGO,
    PAUSA,
    CONTROLES
};

enum class AcaoCena {
    NENHUMA,
    EMPILHAR,
    DESEMPILHAR,
    TROCAR,
    SAIR
};

struct PedidoCena {
    AcaoCena acao = AcaoCena::NENHUMA;
    TipoCena cena = TipoCena::MENU;

    static PedidoCena nenhum() { return {}; }
    static PedidoCena empilhar(TipoCena cena) { return {AcaoCena::EMPILHAR, cena}; }
    static PedidoCena desempilhar() { return {AcaoCena::DESEMPILHAR, TipoCena::MENU}; }
    static PedidoCena trocar(TipoCena cena) { return {AcaoCena::TROCAR, cena}; }
    static PedidoCena sair() { return {AcaoCena::SAIR, TipoCena::MENU}; }
};

struct ContextoJogo {
    SDL_Renderer* renderizacao = nullptr;
    const SpriteCatalog* sprites = nullptr;
    TTF_Font* fonteTitulo = nullptr;
    TTF_Font* fonteMenu = nullptr;
    TTF_Font* fonteHud = nullptr;
    int telaLargura = 0;
    int telaAltura = 0;
};

class Scene {
public:
    explicit Scene(const ContextoJogo& contexto) : contexto(contexto) {}
    virtual ~Scene() = default;

    virtual void handleEvent(const SDL_Event& /*evento*/) {}
    virtual void update(float /*dt*/) {}
    virtual void draw() = 0;

    virtual bool transparente() const { return false; }

    PedidoCena getPedido() const { return pedido; }
    void limparPedido() { pedido = PedidoCena::nenhum(); }

protected:
    ContextoJogo contexto;
    PedidoCena pedido;

    void drawTexto(TTF_Font* fonte, const std::string& texto, int x, int y, SDL_Color cor, bool centralizado = false) const {
        if (!fonte || texto.empty()) return;

        SDL_Surface* superficie = TTF_RenderUTF8_Blended(fonte, texto.c_str(), cor);
        if (!superficie) return;

        SDL_Texture* textura = SDL_CreateTextureFromSurface(contexto.renderizacao, superficie);
        SDL_Rect destino = {centralizado ? x - superficie->w / 2 : x, y, superficie->w, superficie->h};
        SDL_FreeSurface(superficie);

        if (!textura) return;
        SDL_RenderCopy(contexto.renderizacao, textura, nullptr, &destino);
        SDL_DestroyTexture(textura);
    }

    void drawTextoCentralizado(TTF_Font* fonte, const std::string& texto, int centroX, int y, SDL_Color cor) const {
        drawTexto(fonte, texto, centroX, y, cor, true);
    }

    int larguraTexto(TTF_Font* fonte, const std::string& texto) const {
        int largura = 0;
        int altura = 0;
        if (fonte) TTF_SizeUTF8(fonte, texto.c_str(), &largura, &altura);
        return largura;
    }
};

#endif
