#include "gb/gb.h"
#include "gui/display.h"

#include "SDL2/SDL.h"

#include "log.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		ERROR_PRINT("Missing argument.\n");
		return -1;
	}

	gb_init();

	gb_t *p_gb = gb_allocate();
	if (!p_gb)
	{
		ERROR_PRINT("gb_allocate failed.\n");
		return -1;
	}

	if (0 != gb_load_program(p_gb, argv[1], argv[2]))
	{
		ERROR_PRINT("mmu_load_boot failed.\n");
		return -1;
	}

	if (0 != display_init())
	{
		ERROR_PRINT("display_init failed.\n");
		return -1;
	}

	display_t *p_display = display_create();
	if (!p_display)
	{
		ERROR_PRINT("display_create failed.\n");
		return -1;
	}

	int quit = 0;
	while (!quit)
	{
		unsigned int startTime = SDL_GetTicks();

		(void)gb_execute(p_gb, 1000.0 / 60.0);

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

		unsigned int delta = SDL_GetTicks() - startTime;
		unsigned int frameTime = (int)(1000.0 / 60.0);

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
