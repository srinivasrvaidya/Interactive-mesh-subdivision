output: loop_main.o Utility.o
	gcc -o "$@" loop_main.o Utility.o -lGL -lGLU -lglut -lc -lm -lstdc++ -std=c++11 -DGL_GLEXT_PROTOTYPES
loop_main.o: loop_main.cpp
	gcc -c loop_main.cpp -lGL -lGLU -lglut -lc -lm -lstdc++ -std=c++11 -DGL_GLEXT_PROTOTYPES
Utility.o: Utility.cpp
	g++ -c Utility.cpp -lglut -lGL -lGLU -lc -lm -lstdc++ -std=c++11 -DGL_GLEXT_PROTOTYPES
clean:
	rm *.o
