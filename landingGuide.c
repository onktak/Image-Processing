/*
 * landingGuide.c
 *
 * The main landing guide system file. Initialises the landing guide system
 */

#include "SDL/SDL.h"
#include "frame.h"
#include "video.h"
#include "two_pass.h"

#define END_DELAY 200





SDL_Surface *frame = NULL;
SDL_Surface *screen = NULL;


int xBox = 350, yBox = 200;


//----------------my function prototypes-----------------

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination );
SDL_Surface* create_cam_img (unsigned char *pixels, int w, int h);
static void mainloop (void);

void process_video(unsigned char *pixels, unsigned char processedPixels[][FRAME_WIDTH]);


int main (int argc, char **argv) {

    dev_name = "/dev/video0";
    io = IO_METHOD_MMAP;

    pixels = (unsigned char *) malloc (sizeof(unsigned char) * FRAME_HEIGHT * FRAME_WIDTH * 3);

    //Start SDL
    SDL_Init( SDL_INIT_EVERYTHING );

    screen = SDL_SetVideoMode(FRAME_WIDTH, FRAME_HEIGHT, 24, SDL_SWSURFACE);

    //init_tracker();

    open_device ();

    init_device ();

    start_capturing ();

    mainloop ();

    stop_capturing ();

    uninit_device ();

    close_device ();

    SDL_Delay(END_DELAY);
    SDL_FreeSurface(frame);
    //Quit SDL
    SDL_Quit();
    free(pixels);
    //cleanup_tracker();
    exit (EXIT_SUCCESS);

    return 0;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
    //Make a temporary rectangle to hold the offsets
    SDL_Rect offset;

    //Give the offsets to the rectangle
    offset.x = x;
    offset.y = y;

    //Blit the surface
    SDL_BlitSurface( source, NULL, destination, &offset );
}

SDL_Surface* create_cam_img (unsigned char *pixels, int w,int h)
{
    int bpp,pitch;
    Uint32 rmask, gmask, bmask, amask;

    bpp=24;
    pitch = w * 3;

    amask = 0;
    bmask = 0xff0000;
    gmask = 0x00ff00;
    rmask = 0x0000ff;

    return SDL_CreateRGBSurfaceFrom (pixels,w,h,bpp,pitch,rmask,gmask,bmask,amask);

}
void draw_collinear(unsigned char *pixels, collinear co) {

	coord p1 = co.point1;
	coord p2 = co.point2;
	coord p3 = co.point3;

	draw_box(pixels, p1.x, p1.y, BOX_WIDTH, BOX_HEIGHT); 
	draw_box(pixels, p2.x, p2.y, BOX_WIDTH, BOX_HEIGHT); 
	draw_box(pixels, p3.x, p3.y, BOX_WIDTH, BOX_HEIGHT);
}

static void mainloop (void) {

    int i,j;

    unsigned char processedPixels[FRAME_HEIGHT][FRAME_WIDTH];
 

    for (;;) {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO (&fds);
        FD_SET (fd, &fds);

        /* Timeout. */
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        r = select (fd + 1, &fds, NULL, NULL, &tv);

        if (-1 == r) {
            if (EINTR == errno)
                continue;

            errno_exit ("select");
        }

        if (0 == r) {
            fprintf (stderr, "select timeout\n");
            exit (EXIT_FAILURE);
        }

        if (read_frame ()) {
        
            // process the video
            process_video(pixels, processedPixels);        
           
            // convert grayscale processed pixels to an RGB format
 			convert_grayscale_to_rgb(pixels, processedPixels,FRAME_WIDTH, FRAME_HEIGHT);
 			
            // process blobs
            int labels[FRAME_HEIGHT][FRAME_WIDTH];
            for(i = 0; i < FRAME_HEIGHT; i++) {               
                for(j = 0; j < FRAME_WIDTH; j++) {
                    labels[i][j] = -1;
                }
            }

            int numBlobs = two_pass(processedPixels, labels, FRAME_WIDTH, FRAME_HEIGHT);
         
            
            blob blobs[MAX_BLOBS];
    		extract_blobs(blobs, numBlobs, labels, FRAME_WIDTH, FRAME_HEIGHT);
			
			
    		
    		// remove too small/big blobs
    		numBlobs = apply_blob_size_heuristic(blobs, numBlobs);
    		// get all center points for blobs
    		coord centerCoords[MAX_BLOBS];// = (coord*)malloc(sizeof(coord) * numBlobs);
    		get_blob_centers(blobs, numBlobs, centerCoords);
    	
    		    		
    		collinear points[MAX_BLOBS];
    		int shortSegments = get_more_straight_sides(centerCoords, numBlobs, points);
    		
    		coord shapeCoords[5];
    		int shapeFound = 0;
    	    		
    		for(i = 0; i < shortSegments; i++) {    			
						
				coord p1 = points[i].point1;
				coord p2 = points[i].point2;
				coord p3 = points[i].point3;			
	
				double m1 = fabs(distance(p1, p2));
				double m2 = fabs(distance(p2, p3));				
				double m3 =	fabs(distance(p1, p3));
				
								
				if(is_long_side(points[i])) {
					collinear shortSide;
					int shortSideFound = get_short_side(centerCoords, numBlobs, points[i], &shortSide);
					if(shortSideFound) {			
						draw_collinear(pixels, shortSide);
						draw_collinear(pixels, points[i]);
						
						/*print_point(p1);
						print_point(p2);
						print_point(p3); */
						//printf(" -------------------- SHORT SIDE \n");					
						
						/*	
						print_point(shortSide.point1);
						print_point(shortSide.point2);
						print_point(shortSide.point3);*/
						printf("\n");	
						
						shapeCoords[0] = p1;
						shapeCoords[1] = p2;
						shapeCoords[2] = p3;
						shapeCoords[3] = shortSide.point1;
						shapeCoords[4] = shortSide.point3;
						
						shapeFound = 1;
						break;
					}
				} else if(is_short_side(points[i])) {
					collinear longSide;
					int longSideFound = get_long_side(centerCoords, numBlobs, points[i], &longSide);
					if(longSideFound) {					
						draw_collinear(pixels, longSide);
						draw_collinear(pixels, points[i]);
						
						/*
						print_point(p1);
						print_point(p2);
						print_point(p3); */
						//printf(" ======================== LONG SIDE\n");	
						/*
						print_point(longSide.point1);
						print_point(longSide.point2);
						print_point(longSide.point3); */
						printf("\n");	
						
						shapeCoords[0] = p1;
						shapeCoords[1] = p2;
						shapeCoords[2] = p3;
						shapeCoords[3] = longSide.point1;
						shapeCoords[4] = longSide.point3;
						
						shapeFound = 1;
						break;
					}
				} 
    		}
    		if(shapeFound) {
    			int imageCenterX = FRAME_WIDTH / 2;
				int imageCenterY = FRAME_HEIGHT / 2;	
	
				coord center;
				center.x = imageCenterX;
				center.y = imageCenterY;
				double distance =  distance_from_center(shapeCoords[1]);
				double angle = quadrant_angle(shapeCoords[1]);
				if(distance > 100) {
					 printf("MOVE TOWARDS CENTER  ::  ANGLE : %f  :: DISTANCE : %f(pixels)\n", 
    				angle, distance);	
    				//print_direction(angle); 
				} else {
					printf("MOVE DOWN HEIGHT : ?\n");
				}
    			print_image(centerCoords, numBlobs, shapeCoords, 5);
    		}
			
            free_blobs(blobs, numBlobs);
        
                      
            frame = create_cam_img(pixels, FRAME_WIDTH, FRAME_HEIGHT);
        	apply_surface(0, 0, frame, screen);
        	SDL_Flip(screen);
           
        }
    }
}
void process_video(unsigned char *pixels, unsigned char processedPixels[][FRAME_WIDTH]) {

   filter(pixels, processedPixels, FRAME_WIDTH, FRAME_HEIGHT);
}





