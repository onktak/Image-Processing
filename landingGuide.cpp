/*
 * landingGuide.c
 *
 * The main landing guide system file. Initialises the landing guide system
 */

#include "SDL/SDL.h"
#include "frame.h"
#include "video.h"


#define NUMBER_OF_FRAMES 100
#define END_DELAY 200


#define BOX_WIDTH 100
#define BOX_HEIGHT 100


SDL_Surface *frame = NULL;
SDL_Surface *screen = NULL;


int xBox = 350, yBox = 200;


//----------------my function prototypes-----------------

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination );
SDL_Surface* create_cam_img (unsigned char *pixels, int w, int h);
static void mainloop (void);

void process_video(unsigned char *pixels, unsigned char *processedPixels, int numPixels);


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

    unsigned int numPixels = sizeof(unsigned char) * 640 * 480 * 3;
    unsigned char processedPixels[numPixels];

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
            process_video(pixels, processedPixels,numPixels);

            frame = create_cam_img(processedPixels, 640, 480);

            apply_surface(0, 0, frame, screen);

            SDL_Flip(screen);
        }
    }
}
void process_video(unsigned char *pixels, unsigned char *processedPixels, int numPixels) {

    filter(pixels, processedPixels, numPixels);
}





