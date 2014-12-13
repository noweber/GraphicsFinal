run-game: test
	./test

test: test.cpp Player.cpp Camera.cpp Level.cpp modelIndex.cpp
	g++ -o test test.cpp Player.cpp Camera.cpp Level.cpp modelIndex.cpp -lGLEW -framework SDL2 -framework OpenGL

clean:
	rm -Rf test
