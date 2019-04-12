#include "cpu.h"
#include "ppu.h"
#include "mmu.h"
#include "log.h"

#include "cpu_base.h"

#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef struct shared_data_s
{
	mmu_t *p_mmu;
	cpu_t *p_cpu;
	ppu_t *p_ppu;
	lcd_t *p_lcd;
	pthread_mutex_t mutex;

	int cycles;
} shared_data_t;

static void *thread_cpu(void *arg);
static void *thread_timers(void *arg);
static void *thread_graphics(void *arg);

int main(int argc, char *argv[])
{
	freopen("CON", "w", stdout);
	freopen("con", "w", stderr);

	if (argc < 3)
	{
		ERROR_PRINT("Missing argument.\n");
		return -1;
	}

	cpu_init();

	lcd_t lcd;

	mmu_t *p_mmu = mmu_allocate();
	if (!p_mmu)
	{
		ERROR_PRINT("mmu_allocate failed.\n");
		return -1;
	}

	if (0 != mmu_load_boot(p_mmu, argv[1]))
	{
		ERROR_PRINT("mmu_load_boot failed.\n");
		return -1;
	}

	if (0 != mmu_load_rom(p_mmu, argv[2]))
	{
		ERROR_PRINT("mmu_load_rom failed.\n");
		return -1;
	}

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		ERROR_PRINT("SDL_Init failed.\n");
		return -1;
	}

	if (TTF_Init() != 0)
	{
		ERROR_PRINT("TTF_Init failed.\n");
		return -1;
	}

	cpu_t *p_cpu = cpu_allocate(p_mmu);
	if (!p_cpu)
	{
		ERROR_PRINT("cpu_allocate failed.\n");
		return -1;
	}

	ppu_t *p_ppu = ppu_allocate(p_mmu, &lcd);
	if (!p_ppu)
	{
		ERROR_PRINT("ppu_allocate failed.\n");
		return -1;
	}

	shared_data_t shared_data;
	shared_data.p_mmu = p_mmu;
	shared_data.p_cpu = p_cpu;
	shared_data.p_ppu = p_ppu;
	shared_data.p_lcd = &lcd;
	shared_data.cycles = 0;

	pthread_mutex_init(&(shared_data.mutex), NULL);

	pthread_t pth_cpu, pth_graphics, pth_timers, pth_debug;

	(void)pthread_create(&pth_cpu, NULL, thread_cpu, &shared_data);
	(void)pthread_create(&pth_timers, NULL, thread_timers, &shared_data);
	(void)pthread_create(&pth_graphics, NULL, thread_graphics, &shared_data);

	(void)pthread_join(pth_graphics, NULL);

	(void)pthread_cancel(pth_cpu);
	(void)pthread_cancel(pth_timers);

	TTF_Quit();

	SDL_Quit();

	return 0;
}

static void *thread_cpu(void *arg)
{
	shared_data_t *data = (shared_data_t *)arg;

	while (1)
	{
		for (int i = 0; i < 500; i++)
		{
			(void)pthread_mutex_lock(&(data->mutex));

			int cycles = cpu_run(data->p_cpu);

			for (int cycle = 0; cycle < cycles; cycle++)
			{
				ppu_cycle(data->p_ppu);
			}

			data->cycles += cycles;

			(void)pthread_mutex_unlock(&(data->mutex));
		}

		SDL_Delay(1);
	}
}

static void *thread_timers(void *arg)
{
	shared_data_t *data = (shared_data_t *)arg;

	while (1)
	{
		(void)pthread_mutex_lock(&(data->mutex));
		(void)pthread_mutex_unlock(&(data->mutex));

		SDL_Delay((int)(1000.0 / 60.0));
	}
}

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

static void *thread_graphics(void *arg)
{
	shared_data_t *data = (shared_data_t *)arg;

	int screen = 0;

	SDL_Window *window = SDL_CreateWindow("GAMEBOY-EMULATOR",
										  SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 576,
										  SDL_WINDOW_SHOWN);
	if (!window)
	{
		ERROR_PRINT("SDL_CreateWindow failed.\n");
		pthread_exit(NULL);
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (!renderer)
	{
		ERROR_PRINT("SDL_CreateRenderer failed.\n");
		pthread_exit(NULL);
	}

	/* Clear screen. */
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	int quit = 0;
	while (!quit)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		(void)pthread_mutex_lock(&(data->mutex));

		for (int y = 0; y < 144; y++)
		{
			for (int x = 0; x < 160; x++)
			{
				uint8_t pixel = data->p_lcd->pixels[x + (y * 160)];
				SDL_SetRenderDrawColor(renderer, pixel, pixel, pixel, 255);
				SDL_Rect rect = {x * 4, y * 4, 4, 4};
				SDL_RenderFillRect(renderer, &rect);
			}
		}

		for (int y = 0; y < 512; y++)
		{
			for (int x = 0; x < 128; x++)
			{
				uint8_t pixel = data->p_mmu->mem[x + (y * 128)];
				SDL_SetRenderDrawColor(renderer, pixel, pixel, pixel, 255);
				SDL_Rect rect = {x + 640, y + 2, 1, 1};
				SDL_RenderFillRect(renderer, &rect);
			}
		}

		for (int y = 0; y < 2; y++)
		{
			for (int x = 0; x < 128; x++)
			{
				uint8_t pixel = data->p_mmu->boot[x + (y * 128)];
				SDL_SetRenderDrawColor(renderer, pixel, pixel, pixel, 255);
				SDL_Rect rect = {x + 640, y, 1, 1};
				SDL_RenderFillRect(renderer, &rect);
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_Rect crect = {data->p_lcd->last_x * 4, data->p_lcd->last_y * 4, 4, 4};

		(void)pthread_mutex_unlock(&(data->mutex));

		SDL_RenderFillRect(renderer, &crect);

		SDL_RenderPresent(renderer);

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				quit = 1;
				break;
			}
		}

		SDL_Delay((int)(1000.0 / 60.0));
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	pthread_exit(NULL);
}
