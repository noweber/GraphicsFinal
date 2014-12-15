run-game: test
	./test

test: test.cpp Player.cpp Camera.cpp Level.cpp modelIndex.cpp GameLane.cpp
	g++ -o test test.cpp Player.cpp Camera.cpp Level.cpp modelIndex.cpp GameLane.cpp -lGLEW -framework SDL2 -framework OpenGL

clean:
	rm -Rf test
