FLAGS=-O2 `pkg-config --cflags opencv`
DEFS= -D USE_OPEN_GL -D MACOS

LIBS= -framework OpenGL -framework GLUT -lm -L/usr/local/lib `pkg-config --libs opencv`

UNAME= $(shell uname)
ifeq ($(UNAME), Linux)
LIBS= -lGL -lGLU -lglut `pkg-config --libs opencv`
DEFS= -D USE_OPEN_GL
endif


all:path_planner path_planner_opencv

path_planner: Dstar.h Dstar.cpp path_planner.cpp
	g++ ${FLAGS} Dstar.cpp path_planner.cpp -o path_planner ${LIBS} ${DEFS}

path_planner_opencv: Dstar.h Dstar.cpp path_planner_opencv.cpp
	g++ ${FLAGS} Dstar.cpp path_planner_opencv.cpp -o path_planner_opencv ${LIBS} ${DEFS}

clean:
	rm -f path_planner path_planner_opencv
