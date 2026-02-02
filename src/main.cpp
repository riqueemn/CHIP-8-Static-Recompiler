#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

//uint8_t V[16];
//uint16_t I;

int main(int argc, char* argv[]){

    if (argc < 2){
        std::cout << "Uso: tradutor <nome_da_rom.ch8>" << std::endl;
        return 1;
    }

    std::string arquivoEntrada = argv[1];


    // Abrir ROM
    std::ifstream romFile(arquivoEntrada, std::ios::binary);
    if(!romFile){
        std::cerr << "Erro ao abrir a ROM: " << arquivoEntrada << std::endl;
        return 1;
    }

    // Carregar os bytes da ROM para um vetor
    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(romFile)), std::istreambuf_iterator<char>());

    // Criar o arquivo C++ de saída
    std::ofstream out("output/rom_data.cpp");

    // Cabeçalho básico do jogo
    out << "#include <iostream>\n";
    out << "#include <cstdint>\n";
    out << "#include <cstring>\n";
    out << "#include <thread>\n";
    out << "#include <chrono>\n\n";
    out << "int main() {\n";
    out << "    uint8_t V[16] = {0};\n";
    out << "    uint16_t I = 0;\n\n";
    out << "    uint8_t RAM[4096] = {0};\n";
    out << "    bool tela[64*32] = {false};\n\n";

    out << "    // Carregando a ROM para a RAM virtual\n";

    for (size_t i = 0; i < buffer.size(); ++i) {
        out << "    RAM[" << std::dec << (0x200 + i) << "] = 0x" << std::hex << (int)buffer[i] << ";\n";
    }

    out << "\n";

    // Loop de Tradução Simples (Teste)
    for (size_t i = 0; i < buffer.size(); i += 2)
    {
        uint16_t opcode = (buffer[i] << 8) | buffer[i + 1];
        uint16_t pc = 0x200 + i; // Endereço inicial típico para ROMs CHIP-8

        uint8_t  nibble  = (opcode & 0xF000) >> 12; // Primeiro dígito (Tipo)
        uint8_t  x       = (opcode & 0x0F00) >> 8;  // Segundo dígito (Registrador VX)
        uint8_t  y       = (opcode & 0x00F0) >> 4;  // Terceiro dígito (Registrador VY)
        uint8_t  n       = (opcode & 0x000F);       // Quarto dígito (4-bit constante)
        uint8_t  kk      = (opcode & 0x00FF);       // Últimos 8 bits (8-bit constante)
        uint16_t nnn     = (opcode & 0x0FFF);       // Últimos 12 bits (Endereço)

        out << "    L" << std::hex << pc << ": ";

        // 1. Comando de Pulo (1NNN)
        if (nibble == 0x1) {
            out << "goto L" << std::hex << nnn << ";\n";
        }
        // 2. Definir Registro (6XKK)
        else if (nibble == 0x6) {
            out << "V[" << std::dec << int(x) << "] = 0x" << (int)kk << ";\n";
        }
        // 3. Adicionar ao Registro (7XKK)
        else if (nibble == 0x7) {
            out << "V[" << std::dec << int(x) << "] += 0x" << (int)kk << ";\n";
        }
        // 4. Limpar Tela (00E0)
        else if (opcode == 0x00E0) {
            out << "std::cout << \"[Limpar Tela]\" << std::endl;\n";
        }
        // 5. SE (3XKK) - Pula se Vx == kk
        else if (nibble == 0x3) {
            uint16_t proximoEndereco = pc + 4;
            out << "    if (V[" << std::dec << (int)x << "] == 0x" << std::hex << (int)kk << ") ";
            out << "goto L" << proximoEndereco << ";\n";
        }
        // 6. ANNN: Define o registrador I com o valor nnn
        else if (nibble == 0xA) {
            out << "    I = 0x" << std::hex << nnn << ";\n";
        }
        // 7. FX1E: Soma VX ao registrador I
        else if (nibble == 0xF && kk == 0x1E) {
            out << "    I += V[" << std::dec << (int)x << "];\n";
        }
        // FX55: Salva V0 a VX na memória a partir de I
        else if (nibble == 0xF && kk == 0x55) {
            out << "    for(int idx = 0; idx <= " << std::dec << (int)x << "; ++idx) ";
            out << "RAM[I + idx] = V[idx];\n";
        }
        // FX65: Preenche V0 a VX com valores da memória a partir de I
        else if (nibble == 0xF && kk == 0x65) {
            out << "    for(int idx = 0; idx <= " << std::dec << (int)x << "; ++idx) ";
            out << "V[idx] = RAM[I + idx];\n";
        }
        // DXYN: Desenha um sprite na tela
        else if (nibble == 0xD) {
            out << "    {\n";
            out << "        uint8_t x_pos = V[" << std::dec << (int)x << "] % 64;\n";
            out << "        uint8_t y_pos = V[" << (int)y << "] % 32;\n";
            out << "        V[0xF] = 0;\n";
            out << "        for (int row = 0; row < " << (int)n << "; row++) {\n";
            out << "            uint8_t sprite_byte = RAM[I + row];\n";
            out << "            for (int col = 0; col < 8; col++) {\n";
            out << "                if ((sprite_byte & (0x80 >> col)) != 0) {\n";
            out << "                    int idx = (x_pos + col) + ((y_pos + row) * 64);\n";
            out << "                    if (idx < 64 * 32) {\n";
            out << "                        if (tela[idx]) V[0xF] = 1; // Colisão!\n";
            out << "                        tela[idx] ^= true; // XOR\n";
            out << "                    }\n";
            out << "                }\n";
            out << "            }\n";
            out << "        }\n";
            
            // Desenhar o resultado no terminal após cada sprite
            out << "        std::cout << \"\\033[H\"; // Move o cursor para o topo do terminal\n";
            out << "        for (int y = 0; y < 32; y++) {\n";
            out << "            for (int x = 0; x < 64; x++) {\n";
            out << "                std::cout << (tela[x + y * 64] ? \"#\" : \" \");\n";
            out << "            }\n";
            out << "            std::cout << \"\\n\";\n";
            out << "        }\n";

            out << "        std::this_thread::sleep_for(std::chrono::milliseconds(16));\n";
            out << "    }\n";
        }
        // FX33: Armazena a representação BCD de VX na memória (I, I+1, I+2)
        else if (nibble == 0xF && kk == 0x33) {
            out << "    RAM[I] = V[" << std::dec << (int)x << "] / 100;\n";
            out << "    RAM[I+1] = (V[" << std::dec << (int)x << "] / 10) % 10;\n";
            out << "    RAM[I+2] = V[" << std::dec << (int)x << "] % 10;\n";
        }
        else {
            out << "// Opcode nao implementado: 0x" << std::hex << opcode << "\n";
        }
        

        
    }

    out << "\n   return 0;\n";
    out << "}\n";

    out.close();
    std::cout << "Traducao concluida. Arquivo gerado em output/rom_data.cpp" << std::endl;
    

    return 0;
}