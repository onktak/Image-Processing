LFLAGS=-g -Wall `sdl-config --cflags`  `sdl-config --libs`

all : landingGuide

landingGuide:landingGuide.o landingGuide.o frame.o video.o two_pass.o
	g++ ${LFLAGS} -o landingGuide landingGuide.o frame.o video.o two_pass.o
	rm *.o

landingGuide.o : landingGuide.cpp frame.h
	g++ -c landingGuide.cpp

frame.o: frame.cpp frame.h
	g++ -c frame.cpp


video.o: video.cpp video.h
	g++ -c video.cpp
        
two_pass.o: two_pass.cpp two_pass.h union_find.h
	g++ -c two_pass.cpp
	
clean:
	rm *.o

