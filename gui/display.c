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
} display_t;

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

        if (!p_display->renderer)
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

int display_refresh(display_t *p_display, gb_t *p_gb)
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
            unsigned char pixel = p_screen->buffer[x + (y * p_screen->width)];
            SDL_SetRenderDrawColor(p_display->renderer, pixel, pixel, pixel, 255);
            SDL_Rect rect = {x * 4, y * 4, 4, 4};
            SDL_RenderFillRect(p_display->renderer, &rect);
        }
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

        free(p_display);
    }
}

void display_quit(void)
{
    TTF_Quit();

    SDL_Quit();
}

/*****************************/

static void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text, TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect)
{
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {0, 0, 0, 255};

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