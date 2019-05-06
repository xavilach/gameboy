#include "gb/gb.h"
#include "gui/display.h"

#include "SDL2/SDL.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Missing argument.\n");
		return -1;
	}

	gb_init();

	gb_t *p_gb = gb_allocate();
	if (!p_gb)
	{
		printf("gb_allocate failed.\n");
		return -1;
	}

	if (0 != gb_load_program(p_gb, argv[1], argv[2]))
	{
		printf("mmu_load_boot failed.\n");
		return -1;
	}

	if (0 != display_init())
	{
		printf("display_init failed.\n");
		return -1;
	}

	display_t *p_display = display_create();
	if (!p_display)
	{
		printf("display_create failed.\n");
		return -1;
	}

	dbg_registers_t regs = 0;

	int quit = 0;
	while (!quit)
	{
		unsigned int time_start = SDL_GetTicks();

		(void)gb_execute(p_gb, 1000.0 / 60.0);

		unsigned int delta_gb = SDL_GetTicks() - time_start;

		time_start = SDL_GetTicks();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				quit = 1;
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.scancode)
				{
				case SDL_SCANCODE_ESCAPE:
					quit = 1;
					break;

				case SDL_SCANCODE_N:
					regs = (regs + 1) % DBG_REGISTERS_MAX;
					break;

				case SDL_SCANCODE_P:
					regs = ((regs - 1) + DBG_REGISTERS_MAX) % DBG_REGISTERS_MAX;
					break;

				default:
					break;
				}
				break;

			default:
				break;
			}
		}

		display_render_gb(p_display, p_gb);
		display_dbg_registers(p_display, p_gb, regs);

		unsigned int delta_display = SDL_GetTicks() - time_start;

		char str[40];
		(void)snprintf(str, 40, "Delta %d %d", delta_gb, delta_display);
		display_text(p_display, 0, 0, str);

		display_refresh(p_display);

		unsigned int delta = delta_gb + delta_display;
		const unsigned int frameTime = (int)(1000.0 / 60.0);
		if (delta < frameTime)
		{
			SDL_Delay(frameTime - delta);
		}
	}

	display_free(p_display);
	p_display = NULL;

	display_quit();

	gb_free(p_gb);
	p_gb = NULL;

	return 0;
}
