all:
	g++ -std=c++17 src/main.cpp -o tradutor
	./tradutor roms/ibm_logo.ch8
	g++ -std=c++17 -c output/game.cpp -I"D:\SFML-3.0.0\include"
	g++ -std=c++17 game.o -o game -L"D:\SFML-3.0.0\\lib" -lsfml-graphics -lsfml-window -lsfml-system
	./game