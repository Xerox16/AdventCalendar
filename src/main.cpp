#include <GL/glew.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <chrono>
#include <iostream>
#include <math.h>
#include <ctime>
#include <sstream>

#include "calendarreader.h"
#include "printstring.h"

using std::cerr;
using std::endl;
using std::cout;

SDL_Surface* display;
SDL_Surface* snow,* text;

#define PITCH (snow->pitch / 4)

void initsnow()
{
    int i, j, pos;
    for (i = 0; i < 1200; i++)
    {
        int p = (int)((sin((i + 3247) * 0.02) * 0.3 +
                       sin((i + 2347) * 0.04) * 0.1 +
                       sin((i + 4378) * 0.01) * 0.6) * 100 + 705);
        pos = p * PITCH + i;
        for (j = p; j < 805; j++)
        {
            ((unsigned int*)snow->pixels)[pos] = 0x007f00;
            pos += PITCH;
        }
    }
}

void newsnow()
{
    int i;
    for (i = 0; i < 8; i++)
        ((unsigned int*)snow->pixels)[rand() % 1198 + 1] = 0xffffff;
}

void snowfall()
{
    int i, j;
    unsigned int *fb = (unsigned int*)snow->pixels;
    for (j = 803; j >= 0; j--)
    {
        int ypos = j * PITCH;
        for (i = 1; i < 1159; i++)
        {
            if (fb[ypos + i] == 0xffffff)
            {
                if (fb[ypos + i + PITCH] == 0)
                {
                    if (fb[ypos + i + PITCH] == 0)
                    {
                        fb[ypos + i + PITCH] = 0xffffff;
                        fb[ypos + i] = 0;
                    }
                    else if (fb[ypos + i + PITCH - 1] == 0)
                    {
                        fb[ypos + i + PITCH - 1] = 0xffffff;
                        fb[ypos + i] = 0;
                    }
                    else if (fb[ypos + i + PITCH + 1] == 0)
                    {
                        fb[ypos + i + PITCH + 1] = 0xffffff;
                        fb[ypos + i] = 0;
                    }
                }
            }
        }
    }
}

SDL_Surface* createSurface( int width , int height )
{
    uint32_t rmask , gmask , bmask , amask ;

    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x00000000;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0x00000000;
#endif

    SDL_Surface* surface = SDL_CreateRGBSurface( 0 , width , height , 32 , rmask , gmask , bmask , amask ) ;
    if( surface == NULL )
    {
        ( void )fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError() );
        exit(1);
    }

    return surface ;
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

//Get time for calendar functionality and for setting according title
    std::time_t currentTime = std::time(nullptr);
    struct std::tm * now = localtime(&currentTime);
    int month = now->tm_mon + 1, date = now->tm_mday - 1;
    month = 12;
    std::stringstream title;
    title << "Calendrier de l'Advent: " << now->tm_mday << ". " << month << " --> Encore " << 24 - now->tm_mday + (12 - month) * 31<<" jours jusqu'a Noel!";

// Set the title bar
    SDL_WM_SetCaption(title.str().c_str(), title.str().c_str());

// Load the background and the door
    SDL_Surface* background,* door,* lockTemp,* lock;
    background = SDL_LoadBMP("media/kalender.bmp");
    door       = SDL_LoadBMP("media/background.bmp");
    lockTemp   = SDL_LoadBMP("media/lock.bmp");
    if (background == NULL || door == NULL || lockTemp == NULL)
    {
        cerr << "SDL_LoadBMP() Failed: " << SDL_GetError() << endl;
        exit(1);
    }
//Setup rectangles for doors of the calendar
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


//Set colorkey so that lock does not cover the doors
   SDL_SetAlpha(lockTemp, 0, 0);
   Uint32 colorkey = SDL_MapRGB(display->format, 255, 0, 255);
   SDL_SetColorKey(lockTemp, SDL_SRCCOLORKEY, colorkey);

    lock = createSurface(190,190);
    SDL_SetColorKey(lock, SDL_SRCCOLORKEY, colorkey);
    SDL_BlitSurface(lockTemp, NULL, lock, NULL);

//Draw locks on background
    for(int i = (month != 12 ? 0 : now->tm_mday); i < 24 ; ++i) {
        SDL_BlitSurface(lock, NULL, background, &rectangles[i]);
    }
    
    SDL_FreeSurface(lockTemp);
    SDL_FreeSurface(lock);

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
//    Mix_PlayMusic( music, 1 );

//Init ttf and load font in two sizes
    if(TTF_Init() != 0) {
        cerr<<"TTF init failed: "<<SDL_GetError()<<endl;
        exit(1);
    }

//Open the font two times
    TTF_Font *smallfont = TTF_OpenFont("media/font.ttf", 32);
    TTF_Font *largefont = TTF_OpenFont("media/font.ttf", 48);
    if (largefont == nullptr || smallfont == nullptr) {
        cerr<<"Failed to load fonts!"<<endl;
        exit(1);
    }


//Set rectangle and color for panel
    SDL_Rect panelRect   = {150,100,900,600};
    SDL_Rect textRect    = {30,30,840,540};
    Uint32 panelColorI   = SDL_MapRGB(display->format, 255, 255, 255);
    SDL_Color panelColor = {255, 255, 255, 0};

//Set rectangle and color for text
    SDL_Color textColor = { 55, 55, 55, 0 };


//Load content
    std::ifstream file;
    file.open("media/content.txt");
    if(!file.is_open()) {
        cerr << "Failed to open content file at '../media/content.txt'" << endl;
        exit(1);
    }

    CalendarReader creader;
    if(creader.read(file) != 0) {
        cerr << "Failed to read content file" << endl;
        exit(1);
    }

    cout<<"Content debug print:"<<endl;

    for(int i = 0; i < 24; ++i) {
        cout<<"Description "<<i<<" : "<<creader.getDescription(i)<<endl;
        cout<<"Content: "<<creader.getContent(i)<<endl;
    }

//Create semi-transparent snow and text surface
    snow = createSurface(1200,805);
    SDL_SetAlpha(snow, SDL_SRCALPHA , 128);
    SDL_SetColorKey(snow, SDL_SRCCOLORKEY, colorkey);

    text = createSurface(900, 600);
    SDL_SetAlpha(text, SDL_SRCALPHA, 200);
    SDL_FillRect(text, NULL, SDL_MapRGB(text->format, 255, 255, 255));


//Enable Cursor
    SDL_ShowCursor(SDL_ENABLE);

    int activePanel = -1;

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
            } else if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(activePanel != -1) {
                    cout<<"Panel "<<activePanel<<" closed!"<<endl;
                    activePanel = -1;
                } else {
                    int x, y;
                    SDL_GetMouseState( &x,&y);

                    for(int i = 0; i < 24; ++i) {
                        bool inside = true;
                        if(x <  rectangles[i].x)                        inside = false;
                        if(x > (rectangles[i].x + rectangles[i].w))     inside = false;
                        if(y <  rectangles[i].y)                        inside = false;
                        if(y > (rectangles[i].y + rectangles[i].h))     inside = false;
                        if(month != 12 || i > date)                     inside = false;


                        if(inside) {
                            activePanel = i;
                            break;
                        }
                    }

                    cout<<"Panel activated? : "<<activePanel<<endl;
                }
            }
        }
// Reset snow surface
        SDL_FillRect(snow, NULL, SDL_MapRGB(display->format, 255,0,255));

// Lock surface if needed
        if (SDL_MUSTLOCK(snow))
            if (SDL_LockSurface(snow) < 0)
                return -1;

        newsnow();
        snowfall();

// Unlock if needed
        if (SDL_MUSTLOCK(snow))
            SDL_UnlockSurface(snow);

// Apply the background to the display
        if (SDL_BlitSurface(background, NULL, display, NULL) != 0)
        {
            cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
            exit(1);
        }
// Apply snowfall to the display
        if(SDL_BlitSurface(snow, NULL, display, NULL) != 0)
        {
            cerr << "SDL_BlitSurface() Failed: " <<SDL_GetError() << endl;
            exit(1);
        }


        if (activePanel != -1) {
        printString(text, largefont, creader.getDescription(activePanel) + "++" + creader.getContent(activePanel), textRect, textColor, panelColor, 0);
             
// Apply the text to the display
            if (SDL_BlitSurface(text, NULL, display, &panelRect) != 0)
            {
                cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
                break;
            }
            
           }




        SDL_Flip(display);
    }

    file.close();
    Mix_FreeMusic( music );
    Mix_CloseAudio();
    
    SDL_FreeSurface(text);
    SDL_FreeSurface(background);
    SDL_FreeSurface(snow);

    TTF_CloseFont(smallfont);
    TTF_CloseFont(largefont);

// Tell the SDL to clean up and shut down
    SDL_Quit();

    return 0;
}
