#include "display.h"

#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct display_s
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
} display_t;

static void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);

int display_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        return -1;
    }

    if (TTF_Init() != 0)
    {
        return -1;
    }

    return 0;
}

display_t *display_create(void)
{
    display_t *p_display = malloc(sizeof(display_t));

    if (p_display)
    {
        p_display->window = SDL_CreateWindow(
            "GAMEBOY-EMULATOR", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 576, SDL_WINDOW_SHOWN);

        p_display->renderer = NULL;
        if (p_display->window)
        {
            p_display->renderer = SDL_CreateRenderer(p_display->window, -1, SDL_RENDERER_SOFTWARE);
        }

        p_display->font = TTF_OpenFont("FreeSans.ttf", 20);

        if (!p_display->window || !p_display->renderer || !p_display->font)
        {
            display_free(p_display);
            p_display = NULL;
        }
    }

    if (p_display)
    {
        /* Clear screen. */
        SDL_SetRenderDrawColor(p_display->renderer, 0, 0, 0, 255);
        SDL_RenderClear(p_display->renderer);
        SDL_RenderPresent(p_display->renderer);
    }

    return p_display;
}

int display_render_gb(display_t *p_display, gb_t *p_gb)
{
    screen_t *p_screen = gb_get_screen(p_gb);
    if (!p_display || !p_screen)
    {
        return -1;
    }

    SDL_SetRenderDrawColor(p_display->renderer, 255, 255, 255, 255);
    SDL_RenderClear(p_display->renderer);

    for (int y = 0; y < p_screen->height; y++)
    {
        for (int x = 0; x < p_screen->width; x++)
        {
            unsigned char pixel_r = p_screen->buffer[(x + (y * p_screen->width)) * 3 + 0];
            unsigned char pixel_g = p_screen->buffer[(x + (y * p_screen->width)) * 3 + 1];
            unsigned char pixel_b = p_screen->buffer[(x + (y * p_screen->width)) * 3 + 2];
            SDL_SetRenderDrawColor(p_display->renderer, pixel_r, pixel_g, pixel_b, 255);
            SDL_Rect rect = {x * 4, y * 4, 4, 4};
            SDL_RenderFillRect(p_display->renderer, &rect);
        }
    }

    char buffer[20];
    gb_dbg_read_mem(p_gb, 0xFF40, 20, buffer);

    for (int i = 0; i < 20; i++)
    {
        char str[40];
        snprintf(str, 40, "0x%04x:0x%02x", (uint16_t)(0xFF40 + i), (uint8_t)buffer[i]);

        display_text(p_display, 0, 40 + 20 * i, str);
    }

    return 0;
}

int display_refresh(display_t *p_display)
{
    if (!p_display)
    {
        return -1;
    }

    SDL_RenderPresent(p_display->renderer);

    return 0;
}

void display_free(display_t *p_display)
{
    if (p_display)
    {
        if (p_display->renderer)
        {
            SDL_DestroyRenderer(p_display->renderer);
            p_display->renderer = NULL;
        }

        if (p_display->window)
        {
            SDL_DestroyWindow(p_display->window);
            p_display->window = NULL;
        }

        if (p_display->font)
        {
            TTF_CloseFont(p_display->font);
            p_display->font = NULL;
        }

        free(p_display);
    }
}

void display_quit(void)
{
    TTF_Quit();

    SDL_Quit();
}

void display_text(display_t *p_display, int x, int y, char *str)
{
    if (!p_display)
    {
        return;
    }

    SDL_Rect rect;
    SDL_Texture *texture = NULL;

    get_text_and_rect(p_display->renderer, x, y, str, p_display->font, &texture, &rect);

    if (texture)
    {
        SDL_RenderCopy(p_display->renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }
}

/*****************************/

static void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect)
{
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 255};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

/*
static void display_u8(SDL_Renderer *renderer, int x, int y, TTF_Font *font, uint8_t value)
{
    SDL_Rect rect1;
    SDL_Texture *texture1;
    char str[40];

    sprintf(str, "%02x", value);
    get_text_and_rect(renderer, x, y, str, font, &texture1, &rect1);
    SDL_RenderCopy(renderer, texture1, NULL, &rect1);
    SDL_DestroyTexture(texture1);
}

static void display_u16(SDL_Renderer *renderer, int x, int y, TTF_Font *font, uint16_t value)
{
    SDL_Rect rect1;
    SDL_Texture *texture1;
    char str[40];

    sprintf(str, "%04x", value);
    get_text_and_rect(renderer, x, y, str, font, &texture1, &rect1);
    SDL_RenderCopy(renderer, texture1, NULL, &rect1);
    SDL_DestroyTexture(texture1);
}
*/