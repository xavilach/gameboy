#include "screen.h"

#include "stdlib.h"

screen_t *screen_allocate(void)
{
    screen_t *p_screen = malloc(sizeof(screen_t));

    if (p_screen)
    {
        p_screen->width = 160;
        p_screen->height = 144;
        p_screen->buffer = malloc(p_screen->width * p_screen->height * 3);

        if (!p_screen->buffer)
        {
            screen_free(p_screen);
            p_screen = NULL;
        }
    }

    return p_screen;
}

void screen_free(screen_t *p_screen)
{
    if (p_screen)
    {
        if (p_screen->buffer)
        {
            free(p_screen->buffer);
            p_screen->buffer = NULL;
        }

        free(p_screen);
    }
}