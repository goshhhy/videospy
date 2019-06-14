/****************************************************************************\
Part of the Candle game engine
Copyright Emily Ingalls 2017
All Rights Reserved
******************************************************************************
 VID_sdl.c: SDL video, input, audio interface. Also handles FS tasks.
\****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

SDL_Window 	*window;
SDL_Surface *wsurface;
SDL_Surface *rsurface;
SDL_Surface *pixel;

Mix_Chunk *music = NULL;
Mix_Chunk *sfx = NULL;

int ticks;

int renderWidth = 36;
int renderHeight = 22;

/* SDL stuff */
void VID_Deinit( void ) {
	SDL_DestroyWindow( window );
	window = NULL;
	wsurface = NULL;
	SDL_FreeSurface( rsurface );
	rsurface = NULL;
	SDL_Quit();
	printf( "SDL backend deinitialized\n" );
}

void InitRenderSurface( void ) {
	if ( rsurface ) 
		SDL_FreeSurface( rsurface );

	rsurface = SDL_CreateRGBSurface( 0, renderWidth, renderHeight, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 );
	if ( !rsurface )
		fprintf( stderr, "Couldn't create SDL2 render surface: %s", SDL_GetError() );
}

int VID_Init( void ) {
	atexit( VID_Deinit );
	
	if( SDL_Init( SDL_INIT_VIDEO ) != 0 ) {
		fprintf( stderr, "Couldn't initialize SDL2: %s", SDL_GetError() );
        return 1;
    }

	window = SDL_CreateWindow( "videospy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 540, 0 );
	if ( !window ) {
		fprintf( stderr, "Couldn't create SDL2 window: %s", SDL_GetError() );
        return 1;
    }

	wsurface = SDL_GetWindowSurface( window );
	if ( !wsurface ) {
		fprintf( stderr, "Couldn't get SDL2 window surface: %s", SDL_GetError() );
        return 1;
    }
	InitRenderSurface();

	pixel = SDL_CreateRGBSurface( 0, 1, 1, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 );
	if ( !rsurface ) {
		fprintf( stderr, "Couldn't create SDL2 pixel surface: %s", SDL_GetError() );
        return 1;
    }

	VID_ClearScreen();
	VID_Flush();

	printf( "SDL backend initialized\n" );

	return 0;
}
 
void VID_StartTick( void ) {
	SDL_Event e;
	while ( SDL_PollEvent( &e ) != 0 ) {
		switch( e.type ) {
			case SDL_QUIT:
				exit(0);
				break;
			default:
				break;
		}
	}
	ticks = SDL_GetTicks();
}

void VID_EndTick( void ) {
	int endTicks = SDL_GetTicks();
	if ( endTicks - ticks < 16 ) {
		SDL_Delay( 16 - ( endTicks - ticks ) );
	}
}

void VID_ClearScreen( void ) {
	SDL_FillRect( rsurface, NULL, SDL_MapRGB( rsurface->format, 0, 0, 0 ) );
}

void VID_Flush( void ) {
	if ( SDL_BlitScaled( rsurface, NULL, wsurface, NULL ) )
		fprintf( stderr, "Flush: Scale failed: %s", SDL_GetError() );
	if ( SDL_UpdateWindowSurface( window ) ) {
		fprintf( stderr, "Flush: Update failed: %s", SDL_GetError() );
		printf( "Reloading window surface\n\n" );
		wsurface = SDL_GetWindowSurface( window );
		if ( !wsurface )
			fprintf( stderr, "Couldn't get SDL2 window surface: %s", SDL_GetError() );
	}
}

void VID_SetRenderResolution( int width, int height ) {
	renderWidth = width;
	renderHeight = height;
	InitRenderSurface();
}

int VID_GetRenderWidth( void ) {
	return renderWidth;
}

int VID_GetRenderHeight( void ) {
	return renderHeight;
}

void DrawPixel( short x, short y, short r, short g, short b ) {
    SDL_Rect srcrect;
	SDL_Rect dstrect;

    srcrect.x = 0;
	srcrect.y = 0;
    dstrect.x = x;
	dstrect.y = y;
    srcrect.w = dstrect.w = 1;
    srcrect.h = dstrect.h = 1;

    SDL_FillRect( pixel, NULL, SDL_MapRGB(pixel->format, r, g, b) );
    SDL_BlitSurface( pixel, &srcrect, rsurface, &dstrect );
}

void DrawLine( short x1, short y1,  short x2, short y2, short r, short g, short b ) {
	SDL_Rect srcrect;
	SDL_Rect dstrect;
	float xt, yt;
	float dx;
	float dy;
	float derr;
	float err;

	if ( x2 < x1 ) {
		xt = x2;
		x2 = x1;
		x1 = xt;
	}
	if ( y2 < y1 ) {
		yt = y2;
		y2 = y1;
		y1 = yt;
	}

	SDL_FillRect( pixel, NULL, SDL_MapRGB(pixel->format, r, g, b) );
	srcrect.x = 0;
	srcrect.y = 0;
	dstrect.w = srcrect.w = 1;
	dstrect.h = srcrect.h = 1;

	dx = x2 - x1;
	if ( dx == 0 ) {
		for( ; y1 < y2; y1++ ) {
			dstrect.x = x1;
			dstrect.y = y1;
			SDL_BlitSurface( pixel, &srcrect, rsurface, &dstrect );
		}
	} else {
		dy = y2 - y1;
		derr = fabs( dy / dx );
		err = derr - 0.5f;

		for ( ; x1 < x2; x1++ ) {
				dstrect.x = x1;
				dstrect.y = y1;
				SDL_BlitSurface( pixel, &srcrect, rsurface, &dstrect );
				err = err + derr;
				if ( err >= 0.5f ) {
					y1++;
					err = err - 1.0f;
				}
		}
	}
}

void DrawFill( short x1, short y1,  short x2, short y2, short r, short g, short b ) {
	for( ; x1 != x2; x1 < x2 ? x1++ : x1-- ) {
		DrawLine( x1, y1, x1, y2, r, g, b );
	}
}