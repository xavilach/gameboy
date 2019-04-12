#ifndef SCREEN_H_
#define SCREEN_H_

typedef struct screen_s
{
    int width;
    int height;
    unsigned char *buffer;
} screen_t;

screen_t *screen_allocate(void);

void screen_free(screen_t *p_screen);

#endif /*SCREEN_H_*/
