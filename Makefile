LFLAGS=-g -Wall `sdl-config --cflags`  `sdl-config --libs`

all : landingGuide

landingGuide:landingGuide.o landingGuide.o frame.o video.o
	g++ ${LFLAGS} -o landingGuide landingGuide.o frame.o video.o
	rm *.o

landingGuide.o : landingGuide.cpp frame.h
	g++ -c landingGuide.cpp

frame.o: frame.cpp frame.h
	g++ -c frame.cpp


video.o: video.cpp video.h
	g++ -c video.cpp
	
clean:
	rm *.o

