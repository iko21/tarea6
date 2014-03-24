/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include <string>
#include <fstream>

using namespace std;


const int SIZE_X=5;
const int SIZE_Y=5;

char tablero[SIZE_X][SIZE_Y]={{'_','_','_','#','#'},
                              {'O','#','_','#','#'},
                              {'O','O','_','#','#'},
                              {'#','#','#','#','#'},
                              {'#','#','#','#','#'}
                             };

//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The surfaces
SDL_Surface *background = NULL;
SDL_Surface *upMessage = NULL;
SDL_Surface *downMessage = NULL;
SDL_Surface *leftMessage = NULL;
SDL_Surface *rightMessage = NULL;
SDL_Surface *message = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *camino = NULL;
SDL_Surface *obstaculo = NULL;
SDL_Surface *pared = NULL;
SDL_Surface *verde = NULL;

void readLaberi(){
    char syms;
    ifstream maze("laberi.txt");

    for(int x = 0; x<5; x++){
        for(int y = 0; y<5; y++){
            maze>>syms;
            tablero[x][y] = syms;
        }
    }

    maze.close();
}


//The event structure
SDL_Event event;

//The font
TTF_Font *font = NULL;

//The color of the font
SDL_Color textColor = { 0, 0, 0 };

SDL_Surface *load_image( std::string filename )
{
    return IMG_Load( filename.c_str() );
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "Press an Arrow Key", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the background image
    background = load_image( "background.png" );

    //Open the font
    font = TTF_OpenFont( "lazy.ttf", 72 );

    //If there was a problem in loading the background
    if( background == NULL )
    {
        return false;
    }

    //If there was an error in loading the font
    if( font == NULL )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Free the surfaces
    SDL_FreeSurface( background );
    SDL_FreeSurface( upMessage );
    SDL_FreeSurface( downMessage );
    SDL_FreeSurface( leftMessage );
    SDL_FreeSurface( rightMessage );

    //Close the font
    TTF_CloseFont( font );

    //Quit SDL_ttf
    TTF_Quit();

    //Quit SDL
    SDL_Quit();
}


void imprimirTablero()
{
    for(int x=0;x<SIZE_X;x++)
    {
        for(int y=0;y<SIZE_X;y++)
        {
            if(tablero[y][x]=='#')
                apply_surface(x*64,y*64,pared,screen,NULL);
            if(tablero[y][x]=='_')
                apply_surface(x*64,y*64,camino,screen,NULL);
            if(tablero[y][x]=='O')
                apply_surface(x*64,y*64,obstaculo,screen,NULL);
        }
    }
}

bool puedoLLegar(int x_inicio,int y_inicio,
                 int x_final, int y_final,
                 char tablero[SIZE_Y][SIZE_X],
                 int pasos)
{
    imprimirTablero();

    apply_surface(x_inicio*64,y_inicio*64,verde,screen,NULL);
    SDL_Flip(screen);
    SDL_Delay(500);


    if(pasos<0)
        return false;

    if(tablero[y_inicio][x_inicio]=='O')
        pasos--;

    if(tablero[y_final][x_final]=='#'
       || tablero[y_inicio][x_inicio]=='#')
        return false;

    if(x_inicio<0 || x_inicio>=SIZE_X
       || y_inicio<0 || y_inicio>=SIZE_Y)
       return false;


    if(x_final<0 || x_final>=SIZE_X
       || y_final<0 || y_final>=SIZE_Y)
       return false;

    if(x_inicio==x_final && y_inicio==y_final)
        return true;

    if(puedoLLegar(x_inicio+1,y_inicio,x_final,y_final,tablero,pasos-1))
        return true;
    if(puedoLLegar(x_inicio-1,y_inicio,x_final,y_final,tablero,pasos-1))
        return true;
    if(puedoLLegar(x_inicio,y_inicio+1,x_final,y_final,tablero,pasos-1))
        return true;
    if(puedoLLegar(x_inicio,y_inicio-1,x_final,y_final,tablero,pasos-1))
        return true;

    return false;
}


int main( int argc, char* args[] )
{
    readLaberi();
    camino = load_image("tiles/camino.png");
    obstaculo = load_image("tiles/obstaculo.png");
    pared = load_image("tiles/pared.png");
    verde = load_image("tiles/verde.png");
    //Quit flag
    bool quit = false;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }

    //Generate the message surfaces
    upMessage = TTF_RenderText_Solid( font, "Up was pressed.", textColor );
    downMessage = TTF_RenderText_Solid( font, "Down was pressed.", textColor );
    leftMessage = TTF_RenderText_Solid( font, "Left was pressed", textColor );
    rightMessage = TTF_RenderText_Solid( font, "Right was pressed", textColor );

    //Apply the background
    apply_surface( 0, 0, background, screen );

    puedoLLegar(0,0,
                 1,2,
                 tablero,
                 3);

//    //While the user hasn't quit
//    while( quit == false )
//    {
//        //If there's an event to handle
//        if( SDL_PollEvent( &event ) )
//        {
//            //If a key was pressed
//            if( event.type == SDL_KEYDOWN )
//            {
//                //Set the proper message surface
//                switch( event.key.keysym.sym )
//                {
//                    case SDLK_UP: message = upMessage; break;
//                    case SDLK_DOWN: message = downMessage; break;
//                    case SDLK_LEFT: message = leftMessage; break;
//                    case SDLK_RIGHT: message = rightMessage; break;
//                }
//            }
//
//            //If the user has Xed out the window
//            else if( event.type == SDL_QUIT )
//            {
//                //Quit the program
//                quit = true;
//            }
//        }
//
//        //If a message needs to be displayed
//        if( message != NULL )
//        {
//            //Apply the background to the screen
//            apply_surface( 0, 0, background, screen );
//
//            //Apply the message centered on the screen
//            apply_surface( ( SCREEN_WIDTH - message->w ) / 2, ( SCREEN_HEIGHT - message->h ) / 2, message, screen );
//
//            //Null the surface pointer
//            message = NULL;
//        }
//
//        for(int x=0;x<SIZE_X;x++)
//        {
//            for(int y=0;y<SIZE_X;y++)
//            {
//                if(tablero[y][x]=='#')
//                    apply_surface(x*64,y*64,pared,screen,NULL);
//                if(tablero[y][x]=='_')
//                    apply_surface(x*64,y*64,camino,screen,NULL);
//                if(tablero[y][x]=='O')
//                    apply_surface(x*64,y*64,obstaculo,screen,NULL);
//            }
//        }
//
//        apply_surface(0*64,0*64,verde,screen,NULL);
//
//        //Update the screen
//        if( SDL_Flip( screen ) == -1 )
//        {
//            return 1;
//        }
//    }
//
//    puedoLLegar(0,0,
//                 1,2,
//                 tablero,
//                 3);
    //Clean up
    clean_up();

    return 0;
}
