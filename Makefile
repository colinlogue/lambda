build:
	g++ -std=c++2a -Ilib -Isrc main.cpp lib/lang_tools/utils/utils.cpp src/*.cpp -o repl

run:
	./repl
