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


#define BOX_WIDTH 10
#define BOX_HEIGHT 10

#define FRAME_HEIGHT 480
#define FRAME_WIDTH  640


SDL_Surface *frame = NULL;
SDL_Surface *screen = NULL;


int xBox = 350, yBox = 200;


//----------------my function prototypes-----------------

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination );
SDL_Surface* create_cam_img (unsigned char *pixels, int w, int h);
static void mainloop (void);

void process_video(unsigned char *pixels, unsigned char **processedPixels);


int main (int argc, char **argv) {

    dev_name = "/dev/video0";
    io = IO_METHOD_MMAP;

    pixels = (unsigned char *) malloc (sizeof(unsigned char) * 640 * 480 * 3);

    //Start SDL
    SDL_Init( SDL_INIT_EVERYTHING );

    screen = SDL_SetVideoMode(640, 480, 24, SDL_SWSURFACE);

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

static void mainloop (void) {

    int i,j;

    unsigned char **processedPixels;
    processedPixels = (unsigned char**)malloc(FRAME_HEIGHT * sizeof(unsigned char*));
    for(i = 0; i < FRAME_HEIGHT; i++) {
        processedPixels[i] = (unsigned char*)malloc(FRAME_WIDTH * sizeof(unsigned char));
    }

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

            /*
               if(count < NUMBER_OF_FRAMES - 50) {
               mean_shift(pixels, &xBox, &yBox, BOX_WIDTH, BOX_HEIGHT);
               }
               else {
               draw_box(pixels, xBox, yBox, BOX_WIDTH, BOX_HEIGHT, FALSE);
               }
             */
        
            // process the video
            process_video(pixels, processedPixels);        
            
            // convert grayscale processed pixels to an RGB format
            for(i = 0; i < FRAME_HEIGHT *  FRAME_WIDTH * 3; i+=3) {
                int shift = i / 3;            	
                int row = shift / FRAME_WIDTH;
                int col = shift % FRAME_WIDTH;

                pixels[i] = 0; // red;
                pixels[i + 1] = 0; // green
                pixels[i + 2] = processedPixels[row][col];
            }
            // process blobs
            int **labels;
            labels = (int **)malloc(sizeof(int *) * FRAME_HEIGHT);
            for(i = 0; i < FRAME_HEIGHT; i++) {
                labels[i] = (int *)malloc(sizeof(int) * FRAME_WIDTH);
                for(j = 0; j < FRAME_WIDTH; j++) {
                    labels[i][j] = -1;
                }
            }

            int numBlobs = two_pass(processedPixels, labels, FRAME_WIDTH, FRAME_HEIGHT);
            
            blob *blobs = (blob *) malloc(sizeof(blob) * numBlobs);
    		extract_blobs(blobs, numBlobs, labels, FRAME_WIDTH, FRAME_HEIGHT);
    		// done with labels
    		for(i = 0; i < FRAME_HEIGHT; i++) {
                free(labels[i]);
            }
            free(labels); 
    		
    		// remove too small/big blobs
    		numBlobs = apply_blob_size_heuristic(blobs, numBlobs);
    		
    		shape sh;
    	
    		int found = get_shape(blobs, numBlobs, &sh);           
    		if(found) {
    			draw_box(pixels, sh.head.x, sh.head.y, BOX_WIDTH, BOX_HEIGHT); 
    			draw_box(pixels, sh.center.x, sh.center.y, BOX_WIDTH, BOX_HEIGHT); 
    			draw_box(pixels, sh.tail.x, sh.tail.y, BOX_WIDTH, BOX_HEIGHT); 
    			draw_box(pixels, sh.left.x, sh.left.y, BOX_WIDTH, BOX_HEIGHT); 
    			draw_box(pixels, sh.right.x, sh.right.y, BOX_WIDTH, BOX_HEIGHT); 
    			/*
    			frame = create_cam_img(pixels, 640, 480);
            	apply_surface(0, 0, frame, screen);
            	SDL_Flip(screen); */
    		}
    		/*
    		for(i = 0; i < 9; i++) {
    			draw_box(pixels, coords[i].x, coords[i].y, BOX_WIDTH, BOX_HEIGHT); 
    		}
    		*/
            free_blobs(blobs, numBlobs);
            
            frame = create_cam_img(pixels, 640, 480);
        	apply_surface(0, 0, frame, screen);
        	SDL_Flip(screen);
           
        }
    }
}
void process_video(unsigned char *pixels, unsigned char **processedPixels) {

   filter(pixels, processedPixels, FRAME_WIDTH, FRAME_HEIGHT);
}





