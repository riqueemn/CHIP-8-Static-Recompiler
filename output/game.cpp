#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cstdint>
#include <vector>
#include <optional>
#include <cstdlib>
#include <ctime>

int main() {
    srand(static_cast<unsigned>(time(NULL)));
    
    // Ajuste da janela para pixels de 10x10 + 2px de espaçamento (Total 12px por pixel)
    // 64 * 12 = 768 | 32 * 12 = 384
    sf::RenderWindow window(sf::VideoMode({768, 384}), "Chip-8 Recompilado - roms/ibm_logo.ch8");
    window.setFramerateLimit(60); 

    uint8_t V[16] = {0};
    uint16_t I = 0;
    uint8_t RAM[4096] = {0};
    uint8_t delayTimer = 0;
    uint16_t stack[16];
    int sp = -1;
    bool tela[64 * 32] = {false};

    // Configuração do "Pixel Físico" com espaçamento
    sf::RectangleShape pixelShape(sf::Vector2f(10.f, 10.f)); 
    pixelShape.setFillColor(sf::Color::White);
    float step = 12.f; // 10px do pixel + 2px de espaço negro
    
    sf::Clock timerClock;

    uint8_t fontes[] = {
        0xF0,0x90,0x90,0x90,0xF0, 0x20,0x60,0x20,0x20,0x70, 0xF0,0x10,0xF0,0x80,0xF0,
        0xF0,0x10,0xF0,0x10,0xF0, 0x90,0x90,0xF0,0x10,0x10, 0xF0,0x80,0xF0,0x10,0xF0,
        0xF0,0x80,0xF0,0x90,0xF0, 0xF0,0x10,0x20,0x40,0x40, 0xF0,0x90,0xF0,0x90,0xF0,
        0xF0,0x90,0xF0,0x10,0xF0
    };
    for(int i=0; i<50; i++) RAM[i] = fontes[i];

    auto isKeyPressed = [](int key) -> bool {
        switch(key) {
            case 0x1: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W);
            case 0x4: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S);
            case 0x2: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up);
            case 0x8: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down);
            case 0x5: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Q);
            case 0x6: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::E);
            case 0x7: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A);
            case 0x9: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D);
            default: return false;
        }
    };

    // Carga da ROM na RAM
    RAM[512] = 0x0;
    RAM[513] = 0xe0;
    RAM[514] = 0xa2;
    RAM[515] = 0x2a;
    RAM[516] = 0x60;
    RAM[517] = 0xc;
    RAM[518] = 0x61;
    RAM[519] = 0x8;
    RAM[520] = 0xd0;
    RAM[521] = 0x1f;
    RAM[522] = 0x70;
    RAM[523] = 0x9;
    RAM[524] = 0xa2;
    RAM[525] = 0x39;
    RAM[526] = 0xd0;
    RAM[527] = 0x1f;
    RAM[528] = 0xa2;
    RAM[529] = 0x48;
    RAM[530] = 0x70;
    RAM[531] = 0x8;
    RAM[532] = 0xd0;
    RAM[533] = 0x1f;
    RAM[534] = 0x70;
    RAM[535] = 0x4;
    RAM[536] = 0xa2;
    RAM[537] = 0x57;
    RAM[538] = 0xd0;
    RAM[539] = 0x1f;
    RAM[540] = 0x70;
    RAM[541] = 0x8;
    RAM[542] = 0xa2;
    RAM[543] = 0x66;
    RAM[544] = 0xd0;
    RAM[545] = 0x1f;
    RAM[546] = 0x70;
    RAM[547] = 0x8;
    RAM[548] = 0xa2;
    RAM[549] = 0x75;
    RAM[550] = 0xd0;
    RAM[551] = 0x1f;
    RAM[552] = 0x12;
    RAM[553] = 0x28;
    RAM[554] = 0xff;
    RAM[555] = 0x0;
    RAM[556] = 0xff;
    RAM[557] = 0x0;
    RAM[558] = 0x3c;
    RAM[559] = 0x0;
    RAM[560] = 0x3c;
    RAM[561] = 0x0;
    RAM[562] = 0x3c;
    RAM[563] = 0x0;
    RAM[564] = 0x3c;
    RAM[565] = 0x0;
    RAM[566] = 0xff;
    RAM[567] = 0x0;
    RAM[568] = 0xff;
    RAM[569] = 0xff;
    RAM[570] = 0x0;
    RAM[571] = 0xff;
    RAM[572] = 0x0;
    RAM[573] = 0x38;
    RAM[574] = 0x0;
    RAM[575] = 0x3f;
    RAM[576] = 0x0;
    RAM[577] = 0x3f;
    RAM[578] = 0x0;
    RAM[579] = 0x38;
    RAM[580] = 0x0;
    RAM[581] = 0xff;
    RAM[582] = 0x0;
    RAM[583] = 0xff;
    RAM[584] = 0x80;
    RAM[585] = 0x0;
    RAM[586] = 0xe0;
    RAM[587] = 0x0;
    RAM[588] = 0xe0;
    RAM[589] = 0x0;
    RAM[590] = 0x80;
    RAM[591] = 0x0;
    RAM[592] = 0x80;
    RAM[593] = 0x0;
    RAM[594] = 0xe0;
    RAM[595] = 0x0;
    RAM[596] = 0xe0;
    RAM[597] = 0x0;
    RAM[598] = 0x80;
    RAM[599] = 0xf8;
    RAM[600] = 0x0;
    RAM[601] = 0xfc;
    RAM[602] = 0x0;
    RAM[603] = 0x3e;
    RAM[604] = 0x0;
    RAM[605] = 0x3f;
    RAM[606] = 0x0;
    RAM[607] = 0x3b;
    RAM[608] = 0x0;
    RAM[609] = 0x39;
    RAM[610] = 0x0;
    RAM[611] = 0xf8;
    RAM[612] = 0x0;
    RAM[613] = 0xf8;
    RAM[614] = 0x3;
    RAM[615] = 0x0;
    RAM[616] = 0x7;
    RAM[617] = 0x0;
    RAM[618] = 0xf;
    RAM[619] = 0x0;
    RAM[620] = 0xbf;
    RAM[621] = 0x0;
    RAM[622] = 0xfb;
    RAM[623] = 0x0;
    RAM[624] = 0xf3;
    RAM[625] = 0x0;
    RAM[626] = 0xe3;
    RAM[627] = 0x0;
    RAM[628] = 0x43;
    RAM[629] = 0xe0;
    RAM[630] = 0x0;
    RAM[631] = 0xe0;
    RAM[632] = 0x0;
    RAM[633] = 0x80;
    RAM[634] = 0x0;
    RAM[635] = 0x80;
    RAM[636] = 0x0;
    RAM[637] = 0x80;
    RAM[638] = 0x0;
    RAM[639] = 0x80;
    RAM[640] = 0x0;
    RAM[641] = 0xe0;
    RAM[642] = 0x0;
    RAM[643] = 0xe0;

    goto L200;
    L0: ; L1: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L2: ; L3: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L4: ; L5: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L6: ; L7: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L8: ; L9: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    La: ; Lb: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lc: ; Ld: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Le: ; Lf: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L10: ; L11: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L12: ; L13: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L14: ; L15: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L16: ; L17: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L18: ; L19: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1a: ; L1b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1c: ; L1d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1e: ; L1f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L20: ; L21: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L22: ; L23: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L24: ; L25: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L26: ; L27: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L28: ; L29: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L2a: ; L2b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L2c: ; L2d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L2e: ; L2f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L30: ; L31: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L32: ; L33: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L34: ; L35: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L36: ; L37: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L38: ; L39: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L3a: ; L3b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L3c: ; L3d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L3e: ; L3f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L40: ; L41: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L42: ; L43: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L44: ; L45: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L46: ; L47: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L48: ; L49: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L4a: ; L4b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L4c: ; L4d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L4e: ; L4f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L50: ; L51: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L52: ; L53: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L54: ; L55: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L56: ; L57: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L58: ; L59: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L5a: ; L5b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L5c: ; L5d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L5e: ; L5f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L60: ; L61: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L62: ; L63: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L64: ; L65: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L66: ; L67: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L68: ; L69: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L6a: ; L6b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L6c: ; L6d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L6e: ; L6f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L70: ; L71: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L72: ; L73: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L74: ; L75: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L76: ; L77: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L78: ; L79: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L7a: ; L7b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L7c: ; L7d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L7e: ; L7f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L80: ; L81: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L82: ; L83: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L84: ; L85: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L86: ; L87: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L88: ; L89: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L8a: ; L8b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L8c: ; L8d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L8e: ; L8f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L90: ; L91: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L92: ; L93: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L94: ; L95: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L96: ; L97: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L98: ; L99: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L9a: ; L9b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L9c: ; L9d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L9e: ; L9f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    La0: ; La1: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    La2: ; La3: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    La4: ; La5: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    La6: ; La7: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    La8: ; La9: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Laa: ; Lab: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lac: ; Lad: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lae: ; Laf: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lb0: ; Lb1: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lb2: ; Lb3: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lb4: ; Lb5: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lb6: ; Lb7: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lb8: ; Lb9: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lba: ; Lbb: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lbc: ; Lbd: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lbe: ; Lbf: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lc0: ; Lc1: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lc2: ; Lc3: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lc4: ; Lc5: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lc6: ; Lc7: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lc8: ; Lc9: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lca: ; Lcb: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lcc: ; Lcd: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lce: ; Lcf: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Ld0: ; Ld1: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Ld2: ; Ld3: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Ld4: ; Ld5: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Ld6: ; Ld7: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Ld8: ; Ld9: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lda: ; Ldb: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Ldc: ; Ldd: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lde: ; Ldf: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Le0: ; Le1: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Le2: ; Le3: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Le4: ; Le5: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Le6: ; Le7: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Le8: ; Le9: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lea: ; Leb: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lec: ; Led: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lee: ; Lef: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lf0: ; Lf1: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lf2: ; Lf3: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lf4: ; Lf5: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lf6: ; Lf7: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lf8: ; Lf9: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lfa: ; Lfb: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lfc: ; Lfd: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    Lfe: ; Lff: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L100: ; L101: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L102: ; L103: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L104: ; L105: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L106: ; L107: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L108: ; L109: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L10a: ; L10b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L10c: ; L10d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L10e: ; L10f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L110: ; L111: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L112: ; L113: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L114: ; L115: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L116: ; L117: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L118: ; L119: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L11a: ; L11b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L11c: ; L11d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L11e: ; L11f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L120: ; L121: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L122: ; L123: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L124: ; L125: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L126: ; L127: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L128: ; L129: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L12a: ; L12b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L12c: ; L12d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L12e: ; L12f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L130: ; L131: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L132: ; L133: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L134: ; L135: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L136: ; L137: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L138: ; L139: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L13a: ; L13b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L13c: ; L13d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L13e: ; L13f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L140: ; L141: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L142: ; L143: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L144: ; L145: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L146: ; L147: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L148: ; L149: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L14a: ; L14b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L14c: ; L14d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L14e: ; L14f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L150: ; L151: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L152: ; L153: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L154: ; L155: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L156: ; L157: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L158: ; L159: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L15a: ; L15b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L15c: ; L15d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L15e: ; L15f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L160: ; L161: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L162: ; L163: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L164: ; L165: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L166: ; L167: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L168: ; L169: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L16a: ; L16b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L16c: ; L16d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L16e: ; L16f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L170: ; L171: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L172: ; L173: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L174: ; L175: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L176: ; L177: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L178: ; L179: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L17a: ; L17b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L17c: ; L17d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L17e: ; L17f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L180: ; L181: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L182: ; L183: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L184: ; L185: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L186: ; L187: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L188: ; L189: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L18a: ; L18b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L18c: ; L18d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L18e: ; L18f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L190: ; L191: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L192: ; L193: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L194: ; L195: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L196: ; L197: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L198: ; L199: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L19a: ; L19b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L19c: ; L19d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L19e: ; L19f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1a0: ; L1a1: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1a2: ; L1a3: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1a4: ; L1a5: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1a6: ; L1a7: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1a8: ; L1a9: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1aa: ; L1ab: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1ac: ; L1ad: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1ae: ; L1af: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1b0: ; L1b1: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1b2: ; L1b3: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1b4: ; L1b5: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1b6: ; L1b7: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1b8: ; L1b9: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1ba: ; L1bb: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1bc: ; L1bd: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1be: ; L1bf: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1c0: ; L1c1: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1c2: ; L1c3: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1c4: ; L1c5: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1c6: ; L1c7: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1c8: ; L1c9: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1ca: ; L1cb: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1cc: ; L1cd: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1ce: ; L1cf: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1d0: ; L1d1: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1d2: ; L1d3: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1d4: ; L1d5: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1d6: ; L1d7: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1d8: ; L1d9: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1da: ; L1db: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1dc: ; L1dd: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1de: ; L1df: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1e0: ; L1e1: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1e2: ; L1e3: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1e4: ; L1e5: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1e6: ; L1e7: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1e8: ; L1e9: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1ea: ; L1eb: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1ec: ; L1ed: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1ee: ; L1ef: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1f0: ; L1f1: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1f2: ; L1f3: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1f4: ; L1f5: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1f6: ; L1f7: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1f8: ; L1f9: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1fa: ; L1fb: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1fc: ; L1fd: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L1fe: ; L1ff: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    L200: ; L201: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    for(int i=0;i<2048;i++) tela[i]=false; window.clear(); window.display();
    L202: ; L203: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x22a;
    L204: ; L205: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = 0xc;
    L206: ; L207: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[1] = 0x8;
    L208: ; L209: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[0]%64, yp=V[1]%32;
      for(int r=0; r<15; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++) if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } }
      window.clear();
      for(int i=0; i<2048; i++) if(tela[i]) {
          pixelShape.setPosition(sf::Vector2f((i%64)*step, (i/64)*step));
          window.draw(pixelShape);
      }
      window.display(); }
    L20a: ; L20b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] += 0x9;
    L20c: ; L20d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x239;
    L20e: ; L20f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[0]%64, yp=V[1]%32;
      for(int r=0; r<15; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++) if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } }
      window.clear();
      for(int i=0; i<2048; i++) if(tela[i]) {
          pixelShape.setPosition(sf::Vector2f((i%64)*step, (i/64)*step));
          window.draw(pixelShape);
      }
      window.display(); }
    L210: ; L211: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x248;
    L212: ; L213: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] += 0x8;
    L214: ; L215: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[0]%64, yp=V[1]%32;
      for(int r=0; r<15; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++) if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } }
      window.clear();
      for(int i=0; i<2048; i++) if(tela[i]) {
          pixelShape.setPosition(sf::Vector2f((i%64)*step, (i/64)*step));
          window.draw(pixelShape);
      }
      window.display(); }
    L216: ; L217: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] += 0x4;
    L218: ; L219: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x257;
    L21a: ; L21b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[0]%64, yp=V[1]%32;
      for(int r=0; r<15; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++) if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } }
      window.clear();
      for(int i=0; i<2048; i++) if(tela[i]) {
          pixelShape.setPosition(sf::Vector2f((i%64)*step, (i/64)*step));
          window.draw(pixelShape);
      }
      window.display(); }
    L21c: ; L21d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] += 0x8;
    L21e: ; L21f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x266;
    L220: ; L221: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[0]%64, yp=V[1]%32;
      for(int r=0; r<15; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++) if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } }
      window.clear();
      for(int i=0; i<2048; i++) if(tela[i]) {
          pixelShape.setPosition(sf::Vector2f((i%64)*step, (i/64)*step));
          window.draw(pixelShape);
      }
      window.display(); }
    L222: ; L223: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] += 0x8;
    L224: ; L225: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x275;
    L226: ; L227: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[0]%64, yp=V[1]%32;
      for(int r=0; r<15; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++) if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } }
      window.clear();
      for(int i=0; i<2048; i++) if(tela[i]) {
          pixelShape.setPosition(sf::Vector2f((i%64)*step, (i/64)*step));
          window.draw(pixelShape);
      }
      window.display(); }
    L228: ; L229: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L228;
    L22a: ; L22b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L22c: ; L22d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L22e: ; L22f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[12]==0x0) goto L232;
    L230: ; L231: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[12]==0x0) goto L234;
    L232: ; L233: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[12]==0x0) goto L236;
    L234: ; L235: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[12]==0x0) goto L238;
    L236: ; L237: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L238: ; L239: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L23a: ; L23b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L23c: ; L23d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L23e: ; L23f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L240: ; L241: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L242: ; L243: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L244: ; L245: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L246: ; L247: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L248: ; L249: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = V[0];
    L24a: ; L24b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L24c: ; L24d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L24e: ; L24f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = V[0];
    L250: ; L251: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = V[0];
    L252: ; L253: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L254: ; L255: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L256: ; L257: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L258: ; L259: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L25a: ; L25b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L25c: ; L25d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L25e: ; L25f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L260: ; L261: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L262: ; L263: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L264: ; L265: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L266: ; L267: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L268: ; L269: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L26a: ; L26b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L26c: ; L26d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L26e: ; L26f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L270: ; L271: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L272: ; L273: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L274: ; L275: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[3]!=0xe0) goto L278;
    L276: ; L277: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    for(int i=0;i<2048;i++) tela[i]=false; window.clear(); window.display();
    L278: ; L279: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L27a: ; L27b: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L27c: ; L27d: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L27e: ; L27f: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L280: ; L281: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    for(int i=0;i<2048;i++) tela[i]=false; window.clear(); window.display();
    L282: ; L283: ;
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    for(int i=0;i<2048;i++) tela[i]=false; window.clear(); window.display();

    return 0;
}
