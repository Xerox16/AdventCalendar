#include <GL/glew.h>
#include <SDL.h>
#include <SDL_mixer.h>

#include <chrono>
#include <iostream>


using std::cerr;
using std::endl;




int main()
{
    // Initialize the SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
 {
  cerr << "SDL_Init() Failed: " << SDL_GetError() << endl;
  exit(1);
 }

 // Set the video mode
 SDL_Surface* display;
 display = SDL_SetVideoMode(450, 500, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
 if (display == NULL)
 {
  cerr << "SDL_SetVideoMode() Failed: " << SDL_GetError() << endl;
  exit(1);
 }

 // Set the title bar
 SDL_WM_SetCaption("SDL Tutorial", "SDL Tutorial");

// Load the image
 SDL_Surface* image;
 image = SDL_LoadBMP("media/background.bmp");
 if (image == NULL)
 {
     cerr << "SDL_LoadBMP() Failed: " << SDL_GetError() << endl;
     exit(1);
 }

// Load the sound file
  Mix_Music *music;
	music = Mix_LoadMUS("media/music.mp3");


    //Initialize SDL_mixer 
    if( Mix_OpenAudio( 22050, AUDIO_S16SYS, 2, 2048 ) == -1 ) {
	return false;
    } 
    // Set Volume
    Mix_VolumeMusic(50);

    if(music == NULL) {
	std::cout<<"Could not load sound file!";
	return -1;
    }
    
    //If there is no music playing 
    Mix_PlayMusic( music, 1 );

    int startTime = SDL_GetTicks();
    // Wait
    while (Mix_PlayingMusic()) {
		SDL_Delay(1000);
		std::cout << "Time: " << (SDL_GetTicks() - startTime) / 1000 << std::endl;
	} 

 // Main loop
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
  // Apply the image to the display
  if (SDL_BlitSurface(image, NULL, display, NULL) != 0)
  {
 cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
 exit(1);
  }

  //Update the display
  SDL_Flip(display);

 }
    Mix_FreeMusic( music );
    Mix_CloseAudio();


    // Tell the SDL to clean up and shut down
    SDL_Quit();
    
    return 0;    
}
