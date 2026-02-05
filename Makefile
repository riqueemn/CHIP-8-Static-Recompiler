all:
	g++ -std=c++17 src/main.cpp -o tradutor
	./tradutor roms/ibm_logo.ch8
	g++ -std=c++17 -c output/rom_data.cpp -I"D:\SFML-3.0.0\include"
	g++ -std=c++17 rom_data.o -o jogo -L"D:\SFML-3.0.0\\lib" -lsfml-graphics -lsfml-window -lsfml-system
	./jogo