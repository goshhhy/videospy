#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sndfile.h>

int rWidth = 32, rHeight = 22;
int phase = 0;
int hBlank = 0, vBlank = 4;
int intensity = 128;

char* fileName = "radio.wav";

void ParseArgs ( int argc, char** argv ) {
	int i;

	for ( i = 1; i < argc; i++ ) {
		if ( !strcmp( argv[i], "-p" ) ) {
			phase = sscanf( argv[++i], "%i" );
		} else {
			fileName = argv[++i];
		}
	}
}


int main( int argc, char** argv ) {
	SNDFILE* file;
	SF_INFO sfinfo;
	int count, x = 0, y = 0;
	short samples[4096];

	memset ( &sfinfo, 0, sizeof (sfinfo) ) ;

	ParseArgs( argc, argv );
	file = sf_open( fileName, SFM_READ , &sfinfo );
	if ( !file ) {
		fprintf( stderr, "error opening file %s\n", fileName );
		return 1;
	}
	
	if ( VID_Init() != 0 ) {
		fprintf( stderr, "error initializing video\n" );
		return 1;
	}

	if ( sf_seek( file, phase * 2, SEEK_SET ) == -1 ) {
		fprintf( stderr, "error while seeking\n" );
		return 1;
	} 

	while ( 1 ) {
		VID_StartTick();
		
		for ( y = 0; y < rHeight + hBlank; y++ ) {
			for ( x = 0; x < rWidth + vBlank; x++ ) {
				if ( sf_read_short( file, &samples, 2 ) < 2 ) {
					printf( "no more samples\n" );
					exit(0);
				}
				int color = samples[ 1 ];
				color = color + 16384;
				color = color / ( 256 - intensity );
				if ( color > 256 )
					color = 256;
				if ( color < 0 )
					color = 0;
				DrawPixel( x, y, color, color, color );
			}
		}
		VID_Flush();
		VID_EndTick();
	}

	return 0;
}
