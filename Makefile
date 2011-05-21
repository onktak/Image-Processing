LFLAGS=-g -Wall `sdl-config --cflags`  `sdl-config --libs`

all : landingGuide

landingGuide:landingGuide.o landingGuide.o frame.o video.o two_pass.o union_find.o
	gcc ${LFLAGS} -o landingGuide landingGuide.o frame.o video.o two_pass.o union_find.o
	rm *.o

landingGuide.o : landingGuide.c frame.h video.h two_pass.h
	gcc -c landingGuide.c

frame.o: frame.c frame.h
	gcc -c frame.c


video.o: video.c video.h
	gcc -c video.c

union_find.o: union_find.c union_find.h
	gcc -c union_find.c
        
two_pass.o: two_pass.c two_pass.h union_find.h
	gcc -c two_pass.c	
	
clean:
	rm *.o

