#include <GL/glew.h>
#include <SDL.h>
#include <SDL_mixer.h>

#include <chrono>
#include <iostream>
#include <math.h>

using std::cerr;
using std::endl;

SDL_Surface* display;

#define PITCH (display->pitch / 4)

void init()
{
    int i, j, pos;
    for (i = 0; i < 640; i++)
    {
        int p = (int)((sin((i + 3247) * 0.02) * 0.3 +
                       sin((i + 2347) * 0.04) * 0.1 +
                       sin((i + 4378) * 0.01) * 0.6) * 100 + 380);
        pos = p * PITCH + i;
        for (j = p; j < 480; j++)
        {
            ((unsigned int*)display->pixels)[pos] = 0x007f00;
            pos += PITCH;
        }
    }
}

void newsnow()
{
    int i;
    for (i = 0; i < 8; i++)
        ((unsigned int*)display->pixels)[rand() % 638 + 1] = 0xffffff;
}

void snowfall()
{
    int i, j;
    unsigned int *fb = (unsigned int*)display->pixels;
    for (j = 478; j >= 0; j--)
    {
        int ypos = j * PITCH;
        for (i = 1; i < 639; i++)
        {
            if (fb[ypos + i] == 0xffffff)
            {
                if (fb[ypos + i + PITCH] == 0)
                {
                    fb[ypos + i + PITCH] = 0xffffff;
                    fb[ypos + i] = 0;
                }
            }
        }
    }
}


int main()
{
    // Initialize the SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        cerr << "SDL_Init() Failed: " << SDL_GetError() << endl;
        exit(1);
    }

// Set the video mode
    display = SDL_SetVideoMode(1200, 805, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (display == NULL)
    {
        cerr << "SDL_SetVideoMode() Failed: " << SDL_GetError() << endl;
        exit(1);
    }

// Set the title bar
    SDL_WM_SetCaption("Adventskalendar", "Adventskalendar");

// Load the background and the door
    SDL_Surface* background,* door;
    background = SDL_LoadBMP("media/kalender.bmp");
    door       = SDL_LoadBMP("media/background.bmp");
    if (background == NULL || door == NULL)
    {
        cerr << "SDL_LoadBMP() Failed: " << SDL_GetError() << endl;
        exit(1);
    }

// Load the sound file
    Mix_Music *music;
    music = Mix_LoadMUS("media/music.mp3");
    if (music == NULL)
    {
        cerr << "Mix_LoadMUS() Failed: " << SDL_GetError() << endl;
        exit(1);
    }

    //Initialize SDL_mixer
    if( Mix_OpenAudio( 22050, AUDIO_S16SYS, 2, 2048 ) == -1 )
    {
        cerr << "Mix_OpenAudio() Failed: " << SDL_GetError() << endl;
        exit(1);
    }
    // Set Volume
    Mix_VolumeMusic(50);
    // Play Music
    Mix_PlayMusic( music, 1 );

    int columns = 6, rows = 4, width = 190, height = 190, xOffset = 15, yOffset = 10, xDistance = 6, yDistance = 6;
    int fields = rows * columns;

    SDL_Rect rectangles[fields];
    for(int y = 0; y < rows; ++y) {
        for(int x = 0; x < columns; ++x) {
            rectangles[y * columns + x].w = width;
            rectangles[y * columns + x].h = height;
            rectangles[y * columns + x].x = xOffset + (width + xDistance) * x;
            rectangles[y * columns + x].y = yOffset + (height+ yDistance) * y;
        }
    }

        SDL_Event event;
        while(1)
        {
            // Check for messages
            if (SDL_PollEvent(&event))
            {
                // Check for the quit message
                if (event.type == SDL_QUIT)
                {
                    // Quit the program
                    break;
                }
            }
            // Game loop will go here...
            // Apply the background to the display
            if (SDL_BlitSurface(background, NULL, display, NULL) != 0)
            {
                cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
                exit(1);
            }

            for(int i = 0; i < fields; ++i) {
                SDL_FillRect(display, &rectangles[i], 0);
            }

            // Lock surface if needed
            if (SDL_MUSTLOCK(display))
                if (SDL_LockSurface(display) < 0)
                    return -1;

            // Ask SDL for the time in milliseconds
            SDL_GetTicks();

            newsnow();
            snowfall();

            // Unlock if needed
            if (SDL_MUSTLOCK(display))
                SDL_UnlockSurface(display);

            //Update the display
            SDL_Flip(display);

        }
        Mix_FreeMusic( music );
        Mix_CloseAudio();


        // Tell the SDL to clean up and shut down
        SDL_Quit();

        return 0;
    }
