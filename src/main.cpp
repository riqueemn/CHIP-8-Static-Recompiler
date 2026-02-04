#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>

// Protótipos das funções
void escreverCabecalho(std::ofstream& out);
void inicializarAmbiente(std::ofstream& out, const std::vector<uint8_t>& buffer);
void traduzirROM(std::ofstream& out, const std::vector<uint8_t>& buffer);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Uso: tradutor <nome_da_rom.ch8>" << std::endl;
        return 1;
    }

    std::ifstream romFile(argv[1], std::ios::binary);
    if (!romFile) {
        std::cerr << "Erro ao abrir a ROM" << std::endl;
        return 1;
    }

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(romFile)), std::istreambuf_iterator<char>());
    std::ofstream out("output/rom_data.cpp");

    if (out.is_open()) {
        escreverCabecalho(out);
        inicializarAmbiente(out, buffer);
        traduzirROM(out, buffer);

        out << "\n    return 0;\n}\n";
        out.close();
        std::cout << "Recompilacao concluida com sucesso!" << std::endl;
    }

    return 0;
}

void escreverCabecalho(std::ofstream& out) {
    out << R"(#include <SFML/Graphics.hpp>
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
)";
}

void inicializarAmbiente(std::ofstream& out, const std::vector<uint8_t>& buffer) {
    out << "\n    // --- Carga da ROM ---\n";
    for (size_t i = 0; i < buffer.size(); ++i) {
        out << "    RAM[" << std::dec << (0x200 + i) << "] = 0x" << std::hex << (int)buffer[i] << ";\n";
    }
    out << "\n    goto L200; // Pulo para o inicio do codigo\n";
}

void traduzirROM(std::ofstream& out, const std::vector<uint8_t>& buffer) {
    for (size_t i = 0; i < buffer.size(); i += 2) {
        uint16_t opcode = (buffer[i] << 8) | buffer[i + 1];
        uint16_t pc = 0x200 + i;

        uint8_t nib = (opcode & 0xF000) >> 12;
        uint8_t x   = (opcode & 0x0F00) >> 8;
        uint8_t y   = (opcode & 0x00F0) >> 4;
        uint8_t n   = (opcode & 0x000F);
        uint8_t kk  = (opcode & 0x00FF);
        uint16_t nnn = (opcode & 0x0FFF);

        // Labels par e impar para seguranca de salto
        out << "    L" << std::hex << pc << ": ; L" << (pc + 1) << ": ;\n";

        // Event loop SFML
        out << "    while (const std::optional event = window.pollEvent()) {\n"
            << "        if (event->is<sf::Event::Closed>()) { window.close(); return 0; }\n"
            << "    }\n";

        // Traducao das Instrucoes
        if (nib == 0x0 && opcode == 0x00E0) out << "    for(int i=0;i<2048;i++) tela[i]=false;\n";
        else if (nib == 0x0 && opcode == 0x00EE) {
            out << "    if(sp >= 0) { uint16_t target = stack[sp--]; switch(target) {\n";
            for (size_t j = 0; j <= buffer.size(); j += 2) {
                out << "        case 0x" << std::hex << (0x200 + j) << ": goto L" << (0x200 + j) << ";\n";
            }
            out << "        default: return -1; } }\n";
        }
        else if (nib == 0x1) out << "    goto L" << std::hex << nnn << ";\n";
        else if (nib == 0x2) out << "    stack[++sp] = 0x" << std::hex << (pc + 2) << "; goto L" << nnn << ";\n";
        else if (nib == 0x3) out << "    if(V[" << std::dec << (int)x << "]==0x" << std::hex << (int)kk << ") goto L" << (pc + 4) << ";\n";
        else if (nib == 0x4) out << "    if(V[" << std::dec << (int)x << "]!=0x" << std::hex << (int)kk << ") goto L" << (pc + 4) << ";\n";
        else if (nib == 0x6) out << "    V[" << std::dec << (int)x << "] = 0x" << std::hex << (int)kk << ";\n";
        else if (nib == 0x7) out << "    V[" << std::dec << (int)x << "] += 0x" << std::hex << (int)kk << ";\n";
        else if (nib == 0x8) {
            if (n == 0x0)      out << "    V[" << std::dec << (int)x << "] = V[" << (int)y << "];\n";
            else if (n == 0x1) out << "    V[" << std::dec << (int)x << "] |= V[" << (int)y << "];\n";
            else if (n == 0x2) out << "    V[" << std::dec << (int)x << "] &= V[" << (int)y << "];\n";
            else if (n == 0x3) out << "    V[" << std::dec << (int)x << "] ^= V[" << (int)y << "];\n";
            else if (n == 0x4) out << "    { uint16_t res = V[" << std::dec << (int)x << "] + V[" << (int)y << "]; V[15] = (res > 255); V[" << (int)x << "] = res & 0xFF; }\n";
            else if (n == 0x5) out << "    { V[15] = (V[" << std::dec << (int)x << "] > V[" << (int)y << "]); V[" << (int)x << "] -= V[" << (int)y << "]; }\n";
        }
        else if (nib == 0xA) out << "    I = 0x" << std::hex << nnn << ";\n";
        else if (nib == 0xC) out << "    V[" << std::dec << (int)x << "] = (rand()%256) & 0x" << std::hex << (int)kk << ";\n";
        else if (nib == 0xD) {
            out << "    { V[15] = 0; uint8_t xp=V[" << std::dec << (int)x << "]%64, yp=V[" << std::dec << (int)y << "]%32;\n"
                << "      for(int r=0; r<" << (int)n << "; r++){ uint8_t b=RAM[I+r];\n"
                << "      for(int c=0; c<8; c++){ if((b&(0x80>>c))){\n"
                << "      int idx=(xp+c)+(yp+r)*64; if(idx<2048){ if(tela[idx]) V[15]=1; tela[idx]^=true; } } } }\n"
                << "      for(int i=0;i<2048;i++) image.setPixel({(unsigned int)(i%64),(unsigned int)(i/64)}, tela[i]?sf::Color::White:sf::Color::Black);\n"
                << "      texture.update(image); window.clear(); window.draw(sprite); window.display(); if(delayTimer>0) delayTimer--; }\n";
        }
        else if (nib == 0xE) {
            if (kk == 0x9E) out << "    if(isKeyPressed(V[" << std::dec << (int)x << "])) goto L" << std::hex << (pc + 4) << ";\n";
            if (kk == 0xA1) out << "    if(!isKeyPressed(V[" << std::dec << (int)x << "])) goto L" << std::hex << (pc + 4) << ";\n";
        }
        else if (nib == 0xF) {
            if (kk == 0x07) out << "    V[" << std::dec << (int)x << "] = delayTimer;\n";
            if (kk == 0x15) out << "    delayTimer = V[" << std::dec << (int)x << "];\n";
            if (kk == 0x1E) out << "    I += V[" << std::dec << (int)x << "];\n";
            if (kk == 0x29) out << "    I = V[" << std::dec << (int)x << "] * 5;\n";
            if (kk == 0x33) out << "    RAM[I]=V[" << std::dec << (int)x << "]/100; RAM[I+1]=(V[" << (int)x << "]/10)%10; RAM[I+2]=V[" << (int)x << "]%10;\n";
            if (kk == 0x65) out << "    for(int j=0; j<=" << std::dec << (int)x << "; j++) V[j]=RAM[I+j];\n";
        }
    }
    // Labels de fechamento
    uint16_t fim = static_cast<uint16_t>(0x200 + buffer.size());
    out << "    L" << std::hex << fim << ": ; L" << (fim + 1) << ": ;\n";
}