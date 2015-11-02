#include <GL/glew.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <chrono>
#include <iostream>
#include <math.h>

#include "../include/calendarreader.h"

using std::cerr;
using std::endl;
using std::cout;

SDL_Surface* display;

#define PITCH (display->pitch / 4)

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
            ((unsigned int*)display->pixels)[pos] = 0x007f00;
            pos += PITCH;
        }
    }
}

void newsnow()
{
    int i;
    for (i = 0; i < 8; i++)
        ((unsigned int*)display->pixels)[rand() % 1198 + 1] = 0xffffff;
}

void snowfall()
{
    int i, j;
    unsigned int *fb = (unsigned int*)display->pixels;
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

SDL_Surface* renderText(const std::string &message, TTF_Font* font,
	SDL_Color color)
{
		
	//We need to first render to a surface as that's what TTF_RenderText
//	int w, h;
//	TTF_SizeText(font, message.c_str(), &w, &h);
	SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
	if (surf == nullptr){
		cerr<<"Failed to render text"<<endl;
		return  nullptr;
	}
	return surf;
}

//int blitText(SDL_Surface* text, SDL_Surface* dest, int x, int y) {
	
	

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

//Init ttf and load font in two sizes

    if(TTF_Init() != 0) {
	cerr<<"TTF init failed: "<<SDL_GetError()<<endl;
	exit(1);
    }
    //Open the font two times
	TTF_Font *smallfont = TTF_OpenFont("media/font.ttf", 32);
	TTF_Font *largefont = TTF_OpenFont("media/font.ttf", 64);
	if (largefont == nullptr || smallfont == nullptr){
		cerr<<"Failed to load fonts!"<<endl;
		exit(1);
	}	
	
	
    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface* text = renderText("Hello World", largefont, color);

    //setup rectangles for doors of the calendar
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

//load content
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
                        if(x <  rectangles[i].x) 			inside = false;
                        if(x > (rectangles[i].x + rectangles[i].w)) 	inside = false;
                        if(y <  rectangles[i].y)			inside = false;
                        if(y > (rectangles[i].y + rectangles[i].h))	inside = false;
			if(inside) {
				activePanel = i;
				break;
			}
                    }

                    cout<<"Panel activated? : "<<activePanel<<endl;
                }
            }
        }
        // Game loop will go here...
        // Apply the background to the display
        if (SDL_BlitSurface(background, NULL, display, NULL) != 0)
        {
            cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
            exit(1);
        }
// Apply the text to the display
      if (SDL_BlitSurface(text, NULL, display, NULL) != 0)
      {
         cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
         break;
      }		

/*

         for(int i = 0; i < fields; ++i) {
             SDL_FillRect(display, &rectangles[i], 0);
         }*/

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
        SDL_UpdateRect(display, 0, 0, 1200, 805);
        //Update the display
        //SDL_Flip(display);

    }

    file.close();
    Mix_FreeMusic( music );
    Mix_CloseAudio();


    // Tell the SDL to clean up and shut down
    SDL_Quit();

    return 0;
}
