#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "../gb/gb.h"

typedef struct display_s display_t;

int display_init(void);

display_t *display_create(void);

int display_render_gb(display_t *p_display, gb_t *p_gb);

int display_refresh(display_t *p_display);

void display_text(display_t *p_display, int x, int y, char *str);

void display_free(display_t *p_display);

void display_quit(void);

#endif /*DISPLAY_H_*/