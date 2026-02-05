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
    sf::RenderWindow window(sf::VideoMode({640, 320}), "Chip-8 Recompilado - Pong");
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
    
    sf::Clock timerClock;

    uint8_t fontes[] = {
        0xF0,0x90,0x90,0x90,0xF0, 0x20,0x60,0x20,0x20,0x70, 0xF0,0x10,0xF0,0x80,0xF0,
        0xF0,0x10,0xF0,0x10,0xF0, 0x90,0x90,0xF0,0x10,0x10, 0xF0,0x80,0xF0,0x10,0xF0,
        0xF0,0x80,0xF0,0x90,0xF0, 0xF0,0x10,0x20,0x40,0x40, 0xF0,0x90,0xF0,0x90,0xF0,
        0xF0,0x90,0xF0,0x10,0xF0
    };
    for(int i=0; i<50; i++) RAM[i] = fontes[i];

    // Mapeamento de Teclas personalizado para Pong
    auto isKeyPressed = [](int key) -> bool {
        switch(key) {
            case 0x1: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::W);     // P1 Cima
            case 0x4: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::S);     // P1 Baixo
            case 0x2: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up);    // P2 Cima
            case 0x8: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down);  // P2 Baixo
            
            case 0x3: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Num3);
            case 0xC: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up);
            case 0x5: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Q);
            case 0x6: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::E);
            case 0xD: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down);
            case 0x7: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A);
            case 0x9: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D);
            case 0xE: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::F);
            case 0xA: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Z);
            case 0x0: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::X);
            case 0xB: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::C);
            case 0xF: return sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::V);
            default: return false;
        }
    };

    // Carga da ROM na RAM
    RAM[512] = 0x6a;
    RAM[513] = 0x2;
    RAM[514] = 0x6b;
    RAM[515] = 0xc;
    RAM[516] = 0x6c;
    RAM[517] = 0x3f;
    RAM[518] = 0x6d;
    RAM[519] = 0xc;
    RAM[520] = 0xa2;
    RAM[521] = 0xea;
    RAM[522] = 0xda;
    RAM[523] = 0xb6;
    RAM[524] = 0xdc;
    RAM[525] = 0xd6;
    RAM[526] = 0x6e;
    RAM[527] = 0x0;
    RAM[528] = 0x22;
    RAM[529] = 0xd4;
    RAM[530] = 0x66;
    RAM[531] = 0x3;
    RAM[532] = 0x68;
    RAM[533] = 0x2;
    RAM[534] = 0x60;
    RAM[535] = 0x60;
    RAM[536] = 0xf0;
    RAM[537] = 0x15;
    RAM[538] = 0xf0;
    RAM[539] = 0x7;
    RAM[540] = 0x30;
    RAM[541] = 0x0;
    RAM[542] = 0x12;
    RAM[543] = 0x1a;
    RAM[544] = 0xc7;
    RAM[545] = 0x17;
    RAM[546] = 0x77;
    RAM[547] = 0x8;
    RAM[548] = 0x69;
    RAM[549] = 0xff;
    RAM[550] = 0xa2;
    RAM[551] = 0xf0;
    RAM[552] = 0xd6;
    RAM[553] = 0x71;
    RAM[554] = 0xa2;
    RAM[555] = 0xea;
    RAM[556] = 0xda;
    RAM[557] = 0xb6;
    RAM[558] = 0xdc;
    RAM[559] = 0xd6;
    RAM[560] = 0x60;
    RAM[561] = 0x1;
    RAM[562] = 0xe0;
    RAM[563] = 0xa1;
    RAM[564] = 0x7b;
    RAM[565] = 0xfe;
    RAM[566] = 0x60;
    RAM[567] = 0x4;
    RAM[568] = 0xe0;
    RAM[569] = 0xa1;
    RAM[570] = 0x7b;
    RAM[571] = 0x2;
    RAM[572] = 0x60;
    RAM[573] = 0x1f;
    RAM[574] = 0x8b;
    RAM[575] = 0x2;
    RAM[576] = 0xda;
    RAM[577] = 0xb6;
    RAM[578] = 0x60;
    RAM[579] = 0xc;
    RAM[580] = 0xe0;
    RAM[581] = 0xa1;
    RAM[582] = 0x7d;
    RAM[583] = 0xfe;
    RAM[584] = 0x60;
    RAM[585] = 0xd;
    RAM[586] = 0xe0;
    RAM[587] = 0xa1;
    RAM[588] = 0x7d;
    RAM[589] = 0x2;
    RAM[590] = 0x60;
    RAM[591] = 0x1f;
    RAM[592] = 0x8d;
    RAM[593] = 0x2;
    RAM[594] = 0xdc;
    RAM[595] = 0xd6;
    RAM[596] = 0xa2;
    RAM[597] = 0xf0;
    RAM[598] = 0xd6;
    RAM[599] = 0x71;
    RAM[600] = 0x86;
    RAM[601] = 0x84;
    RAM[602] = 0x87;
    RAM[603] = 0x94;
    RAM[604] = 0x60;
    RAM[605] = 0x3f;
    RAM[606] = 0x86;
    RAM[607] = 0x2;
    RAM[608] = 0x61;
    RAM[609] = 0x1f;
    RAM[610] = 0x87;
    RAM[611] = 0x12;
    RAM[612] = 0x46;
    RAM[613] = 0x2;
    RAM[614] = 0x12;
    RAM[615] = 0x78;
    RAM[616] = 0x46;
    RAM[617] = 0x3f;
    RAM[618] = 0x12;
    RAM[619] = 0x82;
    RAM[620] = 0x47;
    RAM[621] = 0x1f;
    RAM[622] = 0x69;
    RAM[623] = 0xff;
    RAM[624] = 0x47;
    RAM[625] = 0x0;
    RAM[626] = 0x69;
    RAM[627] = 0x1;
    RAM[628] = 0xd6;
    RAM[629] = 0x71;
    RAM[630] = 0x12;
    RAM[631] = 0x2a;
    RAM[632] = 0x68;
    RAM[633] = 0x2;
    RAM[634] = 0x63;
    RAM[635] = 0x1;
    RAM[636] = 0x80;
    RAM[637] = 0x70;
    RAM[638] = 0x80;
    RAM[639] = 0xb5;
    RAM[640] = 0x12;
    RAM[641] = 0x8a;
    RAM[642] = 0x68;
    RAM[643] = 0xfe;
    RAM[644] = 0x63;
    RAM[645] = 0xa;
    RAM[646] = 0x80;
    RAM[647] = 0x70;
    RAM[648] = 0x80;
    RAM[649] = 0xd5;
    RAM[650] = 0x3f;
    RAM[651] = 0x1;
    RAM[652] = 0x12;
    RAM[653] = 0xa2;
    RAM[654] = 0x61;
    RAM[655] = 0x2;
    RAM[656] = 0x80;
    RAM[657] = 0x15;
    RAM[658] = 0x3f;
    RAM[659] = 0x1;
    RAM[660] = 0x12;
    RAM[661] = 0xba;
    RAM[662] = 0x80;
    RAM[663] = 0x15;
    RAM[664] = 0x3f;
    RAM[665] = 0x1;
    RAM[666] = 0x12;
    RAM[667] = 0xc8;
    RAM[668] = 0x80;
    RAM[669] = 0x15;
    RAM[670] = 0x3f;
    RAM[671] = 0x1;
    RAM[672] = 0x12;
    RAM[673] = 0xc2;
    RAM[674] = 0x60;
    RAM[675] = 0x20;
    RAM[676] = 0xf0;
    RAM[677] = 0x18;
    RAM[678] = 0x22;
    RAM[679] = 0xd4;
    RAM[680] = 0x8e;
    RAM[681] = 0x34;
    RAM[682] = 0x22;
    RAM[683] = 0xd4;
    RAM[684] = 0x66;
    RAM[685] = 0x3e;
    RAM[686] = 0x33;
    RAM[687] = 0x1;
    RAM[688] = 0x66;
    RAM[689] = 0x3;
    RAM[690] = 0x68;
    RAM[691] = 0xfe;
    RAM[692] = 0x33;
    RAM[693] = 0x1;
    RAM[694] = 0x68;
    RAM[695] = 0x2;
    RAM[696] = 0x12;
    RAM[697] = 0x16;
    RAM[698] = 0x79;
    RAM[699] = 0xff;
    RAM[700] = 0x49;
    RAM[701] = 0xfe;
    RAM[702] = 0x69;
    RAM[703] = 0xff;
    RAM[704] = 0x12;
    RAM[705] = 0xc8;
    RAM[706] = 0x79;
    RAM[707] = 0x1;
    RAM[708] = 0x49;
    RAM[709] = 0x2;
    RAM[710] = 0x69;
    RAM[711] = 0x1;
    RAM[712] = 0x60;
    RAM[713] = 0x4;
    RAM[714] = 0xf0;
    RAM[715] = 0x18;
    RAM[716] = 0x76;
    RAM[717] = 0x1;
    RAM[718] = 0x46;
    RAM[719] = 0x40;
    RAM[720] = 0x76;
    RAM[721] = 0xfe;
    RAM[722] = 0x12;
    RAM[723] = 0x6c;
    RAM[724] = 0xa2;
    RAM[725] = 0xf2;
    RAM[726] = 0xfe;
    RAM[727] = 0x33;
    RAM[728] = 0xf2;
    RAM[729] = 0x65;
    RAM[730] = 0xf1;
    RAM[731] = 0x29;
    RAM[732] = 0x64;
    RAM[733] = 0x14;
    RAM[734] = 0x65;
    RAM[735] = 0x0;
    RAM[736] = 0xd4;
    RAM[737] = 0x55;
    RAM[738] = 0x74;
    RAM[739] = 0x15;
    RAM[740] = 0xf2;
    RAM[741] = 0x29;
    RAM[742] = 0xd4;
    RAM[743] = 0x55;
    RAM[744] = 0x0;
    RAM[745] = 0xee;
    RAM[746] = 0x80;
    RAM[747] = 0x80;
    RAM[748] = 0x80;
    RAM[749] = 0x80;
    RAM[750] = 0x80;
    RAM[751] = 0x80;
    RAM[752] = 0x80;
    RAM[753] = 0x0;
    RAM[754] = 0x0;
    RAM[755] = 0x0;
    RAM[756] = 0x0;
    RAM[757] = 0x0;

    goto L200;
    L200: ; L201: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[10] = 0x2;
    L202: ; L203: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[11] = 0xc;
    L204: ; L205: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[12] = 0x3f;
    L206: ; L207: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[13] = 0xc;
    L208: ; L209: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x2ea;
    L20a: ; L20b: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[10]%64, yp=V[11]%32;
      for(int r=0; r<6; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); }
    L20c: ; L20d: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[12]%64, yp=V[13]%32;
      for(int r=0; r<6; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); }
    L20e: ; L20f: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[14] = 0x0;
    L210: ; L211: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    stack[++sp] = 0x212; goto L2d4;
    L212: ; L213: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[6] = 0x3;
    L214: ; L215: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[8] = 0x2;
    L216: ; L217: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = 0x60;
    L218: ; L219: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    delayTimer = V[0];
    L21a: ; L21b: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = delayTimer;
    L21c: ; L21d: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[0]==0x0) goto L220;
    L21e: ; L21f: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L21a;
    L220: ; L221: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[7] = (rand()%256) & 0x17;
    L222: ; L223: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[7] += 0x8;
    L224: ; L225: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[9] = 0xff;
    L226: ; L227: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x2f0;
    L228: ; L229: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[6]%64, yp=V[7]%32;
      for(int r=0; r<1; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); }
    L22a: ; L22b: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x2ea;
    L22c: ; L22d: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[10]%64, yp=V[11]%32;
      for(int r=0; r<6; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); }
    L22e: ; L22f: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[12]%64, yp=V[13]%32;
      for(int r=0; r<6; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); }
    L230: ; L231: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = 0x1;
    L232: ; L233: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(!isKeyPressed(V[0])) goto L236;
    L234: ; L235: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[11] += 0xfe;
    L236: ; L237: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = 0x4;
    L238: ; L239: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(!isKeyPressed(V[0])) goto L23c;
    L23a: ; L23b: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[11] += 0x2;
    L23c: ; L23d: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = 0x1f;
    L23e: ; L23f: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[11] &= V[0];
    L240: ; L241: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[10]%64, yp=V[11]%32;
      for(int r=0; r<6; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); }
    L242: ; L243: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = 0xc;
    L244: ; L245: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(!isKeyPressed(V[0])) goto L248;
    L246: ; L247: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[13] += 0xfe;
    L248: ; L249: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = 0xd;
    L24a: ; L24b: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(!isKeyPressed(V[0])) goto L24e;
    L24c: ; L24d: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[13] += 0x2;
    L24e: ; L24f: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = 0x1f;
    L250: ; L251: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[13] &= V[0];
    L252: ; L253: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[12]%64, yp=V[13]%32;
      for(int r=0; r<6; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); }
    L254: ; L255: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x2f0;
    L256: ; L257: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[6]%64, yp=V[7]%32;
      for(int r=0; r<1; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); }
    L258: ; L259: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { uint16_t res = (uint16_t)V[6] + V[8]; V[15] = (res > 255); V[6] = res & 0xFF; }
    L25a: ; L25b: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { uint16_t res = (uint16_t)V[7] + V[9]; V[15] = (res > 255); V[7] = res & 0xFF; }
    L25c: ; L25d: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = 0x3f;
    L25e: ; L25f: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[6] &= V[0];
    L260: ; L261: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[1] = 0x1f;
    L262: ; L263: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[7] &= V[1];
    L264: ; L265: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[6]!=0x2) goto L268;
    L266: ; L267: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L278;
    L268: ; L269: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[6]!=0x3f) goto L26c;
    L26a: ; L26b: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L282;
    L26c: ; L26d: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[7]!=0x1f) goto L270;
    L26e: ; L26f: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[9] = 0xff;
    L270: ; L271: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[7]!=0x0) goto L274;
    L272: ; L273: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[9] = 0x1;
    L274: ; L275: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[6]%64, yp=V[7]%32;
      for(int r=0; r<1; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); }
    L276: ; L277: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L22a;
    L278: ; L279: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[8] = 0x2;
    L27a: ; L27b: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[3] = 0x1;
    L27c: ; L27d: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = V[7];
    L27e: ; L27f: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = (V[0] >= V[11]); V[0] -= V[11]; }
    L280: ; L281: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L28a;
    L282: ; L283: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[8] = 0xfe;
    L284: ; L285: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[3] = 0xa;
    L286: ; L287: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = V[7];
    L288: ; L289: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = (V[0] >= V[13]); V[0] -= V[13]; }
    L28a: ; L28b: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[15]==0x1) goto L28e;
    L28c: ; L28d: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L2a2;
    L28e: ; L28f: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[1] = 0x2;
    L290: ; L291: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = (V[0] >= V[1]); V[0] -= V[1]; }
    L292: ; L293: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[15]==0x1) goto L296;
    L294: ; L295: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L2ba;
    L296: ; L297: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = (V[0] >= V[1]); V[0] -= V[1]; }
    L298: ; L299: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[15]==0x1) goto L29c;
    L29a: ; L29b: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L2c8;
    L29c: ; L29d: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = (V[0] >= V[1]); V[0] -= V[1]; }
    L29e: ; L29f: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[15]==0x1) goto L2a2;
    L2a0: ; L2a1: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L2c2;
    L2a2: ; L2a3: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = 0x20;
    L2a4: ; L2a5: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L2a6: ; L2a7: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    stack[++sp] = 0x2a8; goto L2d4;
    L2a8: ; L2a9: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { uint16_t res = (uint16_t)V[14] + V[3]; V[15] = (res > 255); V[14] = res & 0xFF; }
    L2aa: ; L2ab: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    stack[++sp] = 0x2ac; goto L2d4;
    L2ac: ; L2ad: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[6] = 0x3e;
    L2ae: ; L2af: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[3]==0x1) goto L2b2;
    L2b0: ; L2b1: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[6] = 0x3;
    L2b2: ; L2b3: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[8] = 0xfe;
    L2b4: ; L2b5: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[3]==0x1) goto L2b8;
    L2b6: ; L2b7: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[8] = 0x2;
    L2b8: ; L2b9: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L216;
    L2ba: ; L2bb: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[9] += 0xff;
    L2bc: ; L2bd: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[9]!=0xfe) goto L2c0;
    L2be: ; L2bf: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[9] = 0xff;
    L2c0: ; L2c1: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L2c8;
    L2c2: ; L2c3: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[9] += 0x1;
    L2c4: ; L2c5: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[9]!=0x2) goto L2c8;
    L2c6: ; L2c7: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[9] = 0x1;
    L2c8: ; L2c9: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = 0x4;
    L2ca: ; L2cb: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L2cc: ; L2cd: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[6] += 0x1;
    L2ce: ; L2cf: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(V[6]!=0x40) goto L2d2;
    L2d0: ; L2d1: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[6] += 0xfe;
    L2d2: ; L2d3: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    goto L26c;
    L2d4: ; L2d5: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = 0x2f2;
    L2d6: ; L2d7: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    RAM[I]=V[14]/100; RAM[I+1]=(V[14]/10)%10; RAM[I+2]=V[14]%10;
    L2d8: ; L2d9: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    for(int j=0; j<=2; j++) V[j]=RAM[I+j];
    L2da: ; L2db: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = V[1] * 5;
    L2dc: ; L2dd: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[4] = 0x14;
    L2de: ; L2df: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[5] = 0x0;
    L2e0: ; L2e1: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[4]%64, yp=V[5]%32;
      for(int r=0; r<5; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); }
    L2e2: ; L2e3: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[4] += 0x15;
    L2e4: ; L2e5: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    I = V[2] * 5;
    L2e6: ; L2e7: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    { V[15] = 0; uint8_t xp=V[4]%64, yp=V[5]%32;
      for(int r=0; r<5; r++){ uint8_t b=RAM[I+r];
      for(int c=0; c<8; c++){ if((b&(0x80>>c))){
      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }
      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);
      texture.update(image); window.clear(); window.draw(sprite); window.display(); }
    L2e8: ; L2e9: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    if(sp >= 0) { uint16_t target = stack[sp--]; switch(target) {
        case 0x200: goto L200;
        case 0x202: goto L202;
        case 0x204: goto L204;
        case 0x206: goto L206;
        case 0x208: goto L208;
        case 0x20a: goto L20a;
        case 0x20c: goto L20c;
        case 0x20e: goto L20e;
        case 0x210: goto L210;
        case 0x212: goto L212;
        case 0x214: goto L214;
        case 0x216: goto L216;
        case 0x218: goto L218;
        case 0x21a: goto L21a;
        case 0x21c: goto L21c;
        case 0x21e: goto L21e;
        case 0x220: goto L220;
        case 0x222: goto L222;
        case 0x224: goto L224;
        case 0x226: goto L226;
        case 0x228: goto L228;
        case 0x22a: goto L22a;
        case 0x22c: goto L22c;
        case 0x22e: goto L22e;
        case 0x230: goto L230;
        case 0x232: goto L232;
        case 0x234: goto L234;
        case 0x236: goto L236;
        case 0x238: goto L238;
        case 0x23a: goto L23a;
        case 0x23c: goto L23c;
        case 0x23e: goto L23e;
        case 0x240: goto L240;
        case 0x242: goto L242;
        case 0x244: goto L244;
        case 0x246: goto L246;
        case 0x248: goto L248;
        case 0x24a: goto L24a;
        case 0x24c: goto L24c;
        case 0x24e: goto L24e;
        case 0x250: goto L250;
        case 0x252: goto L252;
        case 0x254: goto L254;
        case 0x256: goto L256;
        case 0x258: goto L258;
        case 0x25a: goto L25a;
        case 0x25c: goto L25c;
        case 0x25e: goto L25e;
        case 0x260: goto L260;
        case 0x262: goto L262;
        case 0x264: goto L264;
        case 0x266: goto L266;
        case 0x268: goto L268;
        case 0x26a: goto L26a;
        case 0x26c: goto L26c;
        case 0x26e: goto L26e;
        case 0x270: goto L270;
        case 0x272: goto L272;
        case 0x274: goto L274;
        case 0x276: goto L276;
        case 0x278: goto L278;
        case 0x27a: goto L27a;
        case 0x27c: goto L27c;
        case 0x27e: goto L27e;
        case 0x280: goto L280;
        case 0x282: goto L282;
        case 0x284: goto L284;
        case 0x286: goto L286;
        case 0x288: goto L288;
        case 0x28a: goto L28a;
        case 0x28c: goto L28c;
        case 0x28e: goto L28e;
        case 0x290: goto L290;
        case 0x292: goto L292;
        case 0x294: goto L294;
        case 0x296: goto L296;
        case 0x298: goto L298;
        case 0x29a: goto L29a;
        case 0x29c: goto L29c;
        case 0x29e: goto L29e;
        case 0x2a0: goto L2a0;
        case 0x2a2: goto L2a2;
        case 0x2a4: goto L2a4;
        case 0x2a6: goto L2a6;
        case 0x2a8: goto L2a8;
        case 0x2aa: goto L2aa;
        case 0x2ac: goto L2ac;
        case 0x2ae: goto L2ae;
        case 0x2b0: goto L2b0;
        case 0x2b2: goto L2b2;
        case 0x2b4: goto L2b4;
        case 0x2b6: goto L2b6;
        case 0x2b8: goto L2b8;
        case 0x2ba: goto L2ba;
        case 0x2bc: goto L2bc;
        case 0x2be: goto L2be;
        case 0x2c0: goto L2c0;
        case 0x2c2: goto L2c2;
        case 0x2c4: goto L2c4;
        case 0x2c6: goto L2c6;
        case 0x2c8: goto L2c8;
        case 0x2ca: goto L2ca;
        case 0x2cc: goto L2cc;
        case 0x2ce: goto L2ce;
        case 0x2d0: goto L2d0;
        case 0x2d2: goto L2d2;
        case 0x2d4: goto L2d4;
        case 0x2d6: goto L2d6;
        case 0x2d8: goto L2d8;
        case 0x2da: goto L2da;
        case 0x2dc: goto L2dc;
        case 0x2de: goto L2de;
        case 0x2e0: goto L2e0;
        case 0x2e2: goto L2e2;
        case 0x2e4: goto L2e4;
        case 0x2e6: goto L2e6;
        case 0x2e8: goto L2e8;
        case 0x2ea: goto L2ea;
        case 0x2ec: goto L2ec;
        case 0x2ee: goto L2ee;
        case 0x2f0: goto L2f0;
        case 0x2f2: goto L2f2;
        case 0x2f4: goto L2f4;
        default: return -1; } }
    L2ea: ; L2eb: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = V[8];
    L2ec: ; L2ed: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = V[8];
    L2ee: ; L2ef: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = V[8];
    L2f0: ; L2f1: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    V[0] = V[0];
    L2f2: ; L2f3: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L2f4: ; L2f5: ;
    sf::sleep(sf::microseconds(1500));
    if (timerClock.getElapsedTime().asMicroseconds() > 16666) {
        if (delayTimer > 0) delayTimer--;
        timerClock.restart();
    }
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }
    }
    L2f6: ; L2f7: ;

    return 0;
}
