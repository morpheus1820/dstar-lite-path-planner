FLAGS=-O2 `pkg-config --cflags opencv`
DEFS= -D USE_OPEN_GL -D MACOS

LIBS= -framework OpenGL -framework GLUT -lm -L/usr/local/lib `pkg-config --libs opencv`

UNAME= $(shell uname)
ifeq ($(UNAME), Linux)
LIBS= -lGL -lGLU -lglut `pkg-config --libs opencv`
DEFS= -D USE_OPEN_GL
endif


all:path_planner

path_planner: Dstar.h Dstar.cpp path_planner.cpp
	g++ ${FLAGS} Dstar.cpp path_planner.cpp -o path_planner ${LIBS} ${DEFS}

clean:
	rm -f dstar
