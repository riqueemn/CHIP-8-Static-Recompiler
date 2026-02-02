#include <iostream>
#include <cstdint>
#include <cstring>
#include <thread>
#include <chrono>

int main() {
    uint8_t V[16] = {0};
    uint16_t I = 0;

    uint8_t RAM[4096] = {0};
    bool tela[64*32] = {false};

    // Carregando a ROM para a RAM virtual
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

    L200: std::cout << "[Limpar Tela]" << std::endl;
    L202:     I = 0x22a;
    L204: V[0] = 0x12;
    L206: V[1] = 0x8;
    L208:     {
        uint8_t x_pos = V[0] % 64;
        uint8_t y_pos = V[1] % 32;
        V[0xF] = 0;
        for (int row = 0; row < 15; row++) {
            uint8_t sprite_byte = RAM[I + row];
            for (int col = 0; col < 8; col++) {
                if ((sprite_byte & (0x80 >> col)) != 0) {
                    int idx = (x_pos + col) + ((y_pos + row) * 64);
                    if (idx < 64 * 32) {
                        if (tela[idx]) V[0xF] = 1; // Colisão!
                        tela[idx] ^= true; // XOR
                    }
                }
            }
        }
        std::cout << "\033[H"; // Move o cursor para o topo do terminal
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                std::cout << (tela[x + y * 64] ? "#" : " ");
            }
            std::cout << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    L20a: V[0] += 0x9;
    L20c:     I = 0x239;
    L20e:     {
        uint8_t x_pos = V[0] % 64;
        uint8_t y_pos = V[1] % 32;
        V[0xF] = 0;
        for (int row = 0; row < 15; row++) {
            uint8_t sprite_byte = RAM[I + row];
            for (int col = 0; col < 8; col++) {
                if ((sprite_byte & (0x80 >> col)) != 0) {
                    int idx = (x_pos + col) + ((y_pos + row) * 64);
                    if (idx < 64 * 32) {
                        if (tela[idx]) V[0xF] = 1; // Colisão!
                        tela[idx] ^= true; // XOR
                    }
                }
            }
        }
        std::cout << "\033[H"; // Move o cursor para o topo do terminal
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                std::cout << (tela[x + y * 64] ? "#" : " ");
            }
            std::cout << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    L210:     I = 0x248;
    L212: V[0] += 0x8;
    L214:     {
        uint8_t x_pos = V[0] % 64;
        uint8_t y_pos = V[1] % 32;
        V[0xF] = 0;
        for (int row = 0; row < 15; row++) {
            uint8_t sprite_byte = RAM[I + row];
            for (int col = 0; col < 8; col++) {
                if ((sprite_byte & (0x80 >> col)) != 0) {
                    int idx = (x_pos + col) + ((y_pos + row) * 64);
                    if (idx < 64 * 32) {
                        if (tela[idx]) V[0xF] = 1; // Colisão!
                        tela[idx] ^= true; // XOR
                    }
                }
            }
        }
        std::cout << "\033[H"; // Move o cursor para o topo do terminal
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                std::cout << (tela[x + y * 64] ? "#" : " ");
            }
            std::cout << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    L216: V[0] += 0x4;
    L218:     I = 0x257;
    L21a:     {
        uint8_t x_pos = V[0] % 64;
        uint8_t y_pos = V[1] % 32;
        V[0xF] = 0;
        for (int row = 0; row < 15; row++) {
            uint8_t sprite_byte = RAM[I + row];
            for (int col = 0; col < 8; col++) {
                if ((sprite_byte & (0x80 >> col)) != 0) {
                    int idx = (x_pos + col) + ((y_pos + row) * 64);
                    if (idx < 64 * 32) {
                        if (tela[idx]) V[0xF] = 1; // Colisão!
                        tela[idx] ^= true; // XOR
                    }
                }
            }
        }
        std::cout << "\033[H"; // Move o cursor para o topo do terminal
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                std::cout << (tela[x + y * 64] ? "#" : " ");
            }
            std::cout << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    L21c: V[0] += 0x8;
    L21e:     I = 0x266;
    L220:     {
        uint8_t x_pos = V[0] % 64;
        uint8_t y_pos = V[1] % 32;
        V[0xF] = 0;
        for (int row = 0; row < 15; row++) {
            uint8_t sprite_byte = RAM[I + row];
            for (int col = 0; col < 8; col++) {
                if ((sprite_byte & (0x80 >> col)) != 0) {
                    int idx = (x_pos + col) + ((y_pos + row) * 64);
                    if (idx < 64 * 32) {
                        if (tela[idx]) V[0xF] = 1; // Colisão!
                        tela[idx] ^= true; // XOR
                    }
                }
            }
        }
        std::cout << "\033[H"; // Move o cursor para o topo do terminal
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                std::cout << (tela[x + y * 64] ? "#" : " ");
            }
            std::cout << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    L222: V[0] += 0x8;
    L224:     I = 0x275;
    L226:     {
        uint8_t x_pos = V[0] % 64;
        uint8_t y_pos = V[1] % 32;
        V[0xF] = 0;
        for (int row = 0; row < 15; row++) {
            uint8_t sprite_byte = RAM[I + row];
            for (int col = 0; col < 8; col++) {
                if ((sprite_byte & (0x80 >> col)) != 0) {
                    int idx = (x_pos + col) + ((y_pos + row) * 64);
                    if (idx < 64 * 32) {
                        if (tela[idx]) V[0xF] = 1; // Colisão!
                        tela[idx] ^= true; // XOR
                    }
                }
            }
        }
        std::cout << "\033[H"; // Move o cursor para o topo do terminal
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                std::cout << (tela[x + y * 64] ? "#" : " ");
            }
            std::cout << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    L228: goto L228;
    L22a: // Opcode nao implementado: 0xff00
    L22c: // Opcode nao implementado: 0xff00
    L22e:     if (V[12] == 0x0) goto L232;
    L230:     if (V[12] == 0x0) goto L234;
    L232:     if (V[12] == 0x0) goto L236;
    L234:     if (V[12] == 0x0) goto L238;
    L236: // Opcode nao implementado: 0xff00
    L238: // Opcode nao implementado: 0xffff
    L23a: // Opcode nao implementado: 0xff
    L23c: // Opcode nao implementado: 0x38
    L23e: // Opcode nao implementado: 0x3f
    L240: // Opcode nao implementado: 0x3f
    L242: // Opcode nao implementado: 0x38
    L244: // Opcode nao implementado: 0xff
    L246: // Opcode nao implementado: 0xff
    L248: // Opcode nao implementado: 0x8000
    L24a: // Opcode nao implementado: 0xe000
    L24c: // Opcode nao implementado: 0xe000
    L24e: // Opcode nao implementado: 0x8000
    L250: // Opcode nao implementado: 0x8000
    L252: // Opcode nao implementado: 0xe000
    L254: // Opcode nao implementado: 0xe000
    L256: // Opcode nao implementado: 0x80f8
    L258: // Opcode nao implementado: 0xfc
    L25a: // Opcode nao implementado: 0x3e
    L25c: // Opcode nao implementado: 0x3f
    L25e: // Opcode nao implementado: 0x3b
    L260: // Opcode nao implementado: 0x39
    L262: // Opcode nao implementado: 0xf8
    L264: // Opcode nao implementado: 0xf8
    L266: // Opcode nao implementado: 0x300
    L268: // Opcode nao implementado: 0x700
    L26a: // Opcode nao implementado: 0xf00
    L26c: // Opcode nao implementado: 0xbf00
    L26e: // Opcode nao implementado: 0xfb00
    L270: // Opcode nao implementado: 0xf300
    L272: // Opcode nao implementado: 0xe300
    L274: // Opcode nao implementado: 0x43e0
    L276: std::cout << "[Limpar Tela]" << std::endl;
    L278: // Opcode nao implementado: 0x80
    L27a: // Opcode nao implementado: 0x80
    L27c: // Opcode nao implementado: 0x80
    L27e: // Opcode nao implementado: 0x80
    L280: std::cout << "[Limpar Tela]" << std::endl;
    L282: std::cout << "[Limpar Tela]" << std::endl;

   return 0;
}
