/* motion test. (c) 2001 Fabrice Bellard. */

/**
 * @file motion_test.c
 * motion test.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "dsputil.h"

#ifdef HAVE_MMX
#include "i386/mmx.h"
#endif

#undef printf
#undef fprintf

int pix_abs16_mmx(void *v, uint8_t *blk1, uint8_t *blk2, int lx, int h);
int pix_abs16_mmx1(void *v, uint8_t *blk1, uint8_t *blk2, int lx, int h);
int pix_abs16_c(void *v, uint8_t *blk1, uint8_t *blk2, int lx, int h);
int pix_abs16_x2_mmx(void *v, uint8_t *blk1, uint8_t *blk2, int lx, int h);
int pix_abs16_x2_mmx1(void *v, uint8_t *blk1, uint8_t *blk2, int lx, int h);
int pix_abs16_x2_c(void *v, uint8_t *blk1, uint8_t *blk2, int lx, int h);
int pix_abs16_y2_mmx(void *v, uint8_t *blk1, uint8_t *blk2, int lx, int h);
int pix_abs16_y2_mmx1(void *v, uint8_t *blk1, uint8_t *blk2, int lx, int h);
int pix_abs16_y2_c(void *v, uint8_t *blk1, uint8_t *blk2, int lx, int h);
int pix_abs16_xy2_mmx(void *v, uint8_t *blk1, uint8_t *blk2, int lx, int h);
int pix_abs16_xy2_mmx1(void *v, uint8_t *blk1, uint8_t *blk2, int lx, int h);
int pix_abs16_xy2_c(void *v, uint8_t *blk1, uint8_t *blk2, int lx, int h);

typedef int motion_func(void *v, uint8_t *blk1, uint8_t *blk2, int lx, int h);

#define WIDTH 64
#define HEIGHT 64

uint8_t img1[WIDTH * HEIGHT];
uint8_t img2[WIDTH * HEIGHT];

void fill_random(uint8_t *tab, int size)
{
    int i;
    for(i=0;i<size;i++) {
#if 1
        tab[i] = random() % 256;
#else
        tab[i] = i;
#endif
    }
}

void help(void)
{
    printf("motion-test [-h]\n"
           "test motion implementations\n");
    exit(1);
}

int64_t gettime(void)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

#define NB_ITS 500

int dummy;

void test_motion(const char *name,
                 motion_func *test_func, motion_func *ref_func)
{
    int x, y, d1, d2, it;
    uint8_t *ptr;
    int64_t ti;
    printf("testing '%s'\n", name);

    /* test correctness */
    for(it=0;it<20;it++) {

        fill_random(img1, WIDTH * HEIGHT);
        fill_random(img2, WIDTH * HEIGHT);
        
        for(y=0;y<HEIGHT-17;y++) {
            for(x=0;x<WIDTH-17;x++) {
                ptr = img2 + y * WIDTH + x; 
                d1 = test_func(NULL, img1, ptr, WIDTH, 1);
                d2 = ref_func(NULL, img1, ptr, WIDTH, 1);
                if (d1 != d2) {
                    printf("error: mmx=%d c=%d\n", d1, d2);
                }
            }
        }
    }
#ifdef HAVE_MMX
    emms();
#endif
    
    /* speed test */
    ti = gettime();
    d1 = 0;
    for(it=0;it<NB_ITS;it++) {
        for(y=0;y<HEIGHT-17;y++) {
            for(x=0;x<WIDTH-17;x++) {
                ptr = img2 + y * WIDTH + x; 
                d1 += test_func(NULL, img1, ptr, WIDTH, 1);
            }
        }
    }
#ifdef HAVE_MMX
    emms();
#endif
    dummy = d1; /* avoid optimisation */
    ti = gettime() - ti;
    
    printf("  %0.0f kop/s\n", 
           (double)NB_ITS * (WIDTH - 16) * (HEIGHT - 16) / 
           (double)(ti / 1000.0));
}


int main(int argc, char **argv)
{
    int n;
#ifdef HAVE_MMX
    AVCodecContext *ctx;
    DSPContext c, mmx;
#endif
    
    for(;;) {
        n = getopt(argc, argv, "h");
        if (n == -1)
            break;
        switch(n) {
        case 'h':
            help();
            break;
        }
    }
               
    printf("ffmpeg motion test\n");

#ifdef HAVE_MMX
    ctx = avcodec_alloc_context();
    mm_flags = 0;
    dsputil_init(&c, ctx);
    mm_flags = MM_MMX;
    dsputil_init(&mmx, ctx);

    test_motion("mmx/16",     mmx.pix_abs[0][0], c.pix_abs[0][0]);
    test_motion("mmx_x2/16",  mmx.pix_abs[0][1], c.pix_abs[0][1]);
    test_motion("mmx_y2/16",  mmx.pix_abs[0][2], c.pix_abs[0][2]);
    test_motion("mmx_xy2/16", mmx.pix_abs[0][3], c.pix_abs[0][3]);

    test_motion("mmx/8",      mmx.pix_abs[1][0], c.pix_abs[1][0]);
    test_motion("mmx_x2/8",   mmx.pix_abs[1][1], c.pix_abs[1][1]);
    test_motion("mmx_y2/8",   mmx.pix_abs[1][2], c.pix_abs[1][2]);
    test_motion("mmx_xy2/8",  mmx.pix_abs[1][3], c.pix_abs[1][3]);

    av_free(ctx);
#endif

    return 0;
}
