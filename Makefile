FLAGS=-O2 `pkg-config --cflags opencv`
DEFS= -D USE_OPEN_GL -D MACOS

LIBS= -framework OpenGL -framework GLUT -lm -L/usr/local/lib `pkg-config --libs opencv`

UNAME= $(shell uname)
ifeq ($(UNAME), Linux)
LIBS= -lGL -lGLU -lglut
DEFS= -D USE_OPEN_GL
endif


all:dstar

dstar: Dstar.h Dstar.cpp DstarDraw.cpp
	g++ ${FLAGS} Dstar.cpp DstarDraw.cpp -o dstar ${LIBS} ${DEFS}

clean:
	rm -f dstar
