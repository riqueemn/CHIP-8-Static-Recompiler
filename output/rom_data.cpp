#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdint>
#include <vector>
#include <optional>
#include <cstdlib>
#include <ctime>

int main() {
    srand(static_cast<unsigned>(time(NULL)));
    sf::RenderWindow window(sf::VideoMode({640, 320}), "Chip-8 Recompilado SFML 3");
    window.setFramerateLimit(60);

    uint8_t V[16] = {0};
    uint16_t I = 0;
    uint8_t RAM[4096] = {0};
    uint8_t delayTimer = 0;
    uint16_t stack[16];
    int sp = -1;
    bool tela[64 * 32] = {false};

    sf::Image image;
    image.resize({64, 32}, sf::Color::Black);
    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);
    sprite.setScale({10.f, 10.f});

    uint8_t fontes[] = {
        0xF0,0x90,0x90,0x90,0xF0, 0x20,0x60,0x20,0x20,0x70, 0xF0,0x10,0xF0,0x80,0xF0,
        0xF0,0x10,0xF0,0x10,0xF0, 0x90,0x90,0xF0,0x10,0x10, 0xF0,0x80,0xF0,0x10,0xF0,
        0xF0,0x80,0xF0,0x90,0xF0, 0xF0,0x10,0x20,0x40,0x40, 0xF0,0x90,0xF0,0x90,0xF0,
        0xF0,0x90,0xF0,0x10,0xF0
    };
    for(int i=0; i<50; i++) RAM[i] = fontes[i];

    auto isKeyPressed = [](int key) -> bool {
        switch(key) {
            case 0x1: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Num1);
            case 0x2: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Num2);
            case 0x3: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Num3);
            case 0xC: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Num4);
            case 0x4: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Q);
            case 0x5: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W);
            case 0x6: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::E);
            case 0xD: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::R);
            case 0x7: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A);
            case 0x8: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S);
            case 0x9: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D);
            case 0xE: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::F);
            case 0xA: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Z);
            case 0x0: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::X);
            case 0xB: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::C);
            case 0xF: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::V);
            default: return false;
        }
    };

    // --- Carga da ROM ---
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

    goto L200; // Pulo para o inicio do codigo
    L200: ; L201: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    for(int i=0;i<2048;i++) tela[i]=false;
    L202: ; L203: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x22a;
    L204: ; L205: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = 0xc;
    L206: ; L207: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[1] = 0x8;
    L208: ; L209: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[0]%64, yp=V[1]%32;
      for(int r=0; r<15; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); if(delayTimer>0) delayTimer--; }
    L20a: ; L20b: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] += 0x9;
    L20c: ; L20d: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x239;
    L20e: ; L20f: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[0]%64, yp=V[1]%32;
      for(int r=0; r<15; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); if(delayTimer>0) delayTimer--; }
    L210: ; L211: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x248;
    L212: ; L213: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] += 0x8;
    L214: ; L215: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[0]%64, yp=V[1]%32;
      for(int r=0; r<15; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); if(delayTimer>0) delayTimer--; }
    L216: ; L217: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] += 0x4;
    L218: ; L219: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x257;
    L21a: ; L21b: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[0]%64, yp=V[1]%32;
      for(int r=0; r<15; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); if(delayTimer>0) delayTimer--; }
    L21c: ; L21d: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] += 0x8;
    L21e: ; L21f: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x266;
    L220: ; L221: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[0]%64, yp=V[1]%32;
      for(int r=0; r<15; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); if(delayTimer>0) delayTimer--; }
    L222: ; L223: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] += 0x8;
    L224: ; L225: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x275;
    L226: ; L227: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[0]%64, yp=V[1]%32;
      for(int r=0; r<15; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); if(delayTimer>0) delayTimer--; }
    L228: ; L229: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L228;
    L22a: ; L22b: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L22c: ; L22d: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L22e: ; L22f: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[12]==0x0) goto L232;
    L230: ; L231: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[12]==0x0) goto L234;
    L232: ; L233: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[12]==0x0) goto L236;
    L234: ; L235: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[12]==0x0) goto L238;
    L236: ; L237: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L238: ; L239: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L23a: ; L23b: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0xff não implementada.
    L23c: ; L23d: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x38 não implementada.
    L23e: ; L23f: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x3f não implementada.
    L240: ; L241: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x3f não implementada.
    L242: ; L243: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x38 não implementada.
    L244: ; L245: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0xff não implementada.
    L246: ; L247: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0xff não implementada.
    L248: ; L249: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = V[0];
    L24a: ; L24b: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L24c: ; L24d: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L24e: ; L24f: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = V[0];
    L250: ; L251: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = V[0];
    L252: ; L253: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L254: ; L255: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L256: ; L257: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L258: ; L259: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0xfc não implementada.
    L25a: ; L25b: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x3e não implementada.
    L25c: ; L25d: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x3f não implementada.
    L25e: ; L25f: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x3b não implementada.
    L260: ; L261: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x39 não implementada.
    L262: ; L263: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0xf8 não implementada.
    L264: ; L265: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0xf8 não implementada.
    L266: ; L267: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x300 não implementada.
    L268: ; L269: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x700 não implementada.
    L26a: ; L26b: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0xf00 não implementada.
    L26c: ; L26d: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto *(&&L200 + V[0] + 0xf00); // Exemplo conceitual
    L26e: ; L26f: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L270: ; L271: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L272: ; L273: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L274: ; L275: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[3]!=0xe0) goto L278;
    L276: ; L277: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    for(int i=0;i<2048;i++) tela[i]=false;
    L278: ; L279: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x80 não implementada.
    L27a: ; L27b: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x80 não implementada.
    L27c: ; L27d: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x80 não implementada.
    L27e: ; L27f: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    // Instrução 0x80 não implementada.
    L280: ; L281: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    for(int i=0;i<2048;i++) tela[i]=false;
    L282: ; L283: ;
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    for(int i=0;i<2048;i++) tela[i]=false;
    L284: ; L285: ;

    return 0;
}
