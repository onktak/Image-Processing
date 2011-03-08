/*
 * frame.h
 *
 * Access video frames from the camera.
 */
#ifndef FRAME_H
#define FRAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <math.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

typedef enum {
    IO_METHOD_READ,
    IO_METHOD_MMAP,
    IO_METHOD_USERPTR,
} io_method;

struct buffer {
    void *                  start;
    size_t                  length;
};

// TODO : documentation

extern unsigned char *pixels;
extern int fd;
extern char *dev_name;
extern io_method io;

/*
 * close the video device
 */
void close_device(void);
/*
 * open the video device
 */
void open_device (void);
void errno_exit(const char *s);
int xioctl (int fd, int request, void *arg);
int read_frame (void);
void stop_capturing (void);
void start_capturing (void);
void uninit_device (void);
void init_mmap (void);
void init_device (void);

#endif

