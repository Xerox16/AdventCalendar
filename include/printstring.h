#ifndef _PRINT_STRING_H_
#define _PRINT_STRING_H_

#include <SDL.h>
#include <vector>
#include <string>

const int PS_ALIGN_LEFT =  1;

const int PS_ALIGN_CENTER =  2;

const int PS_ALIGN_RIGHT =  4;

const int PS_CREATE_SURFACE =  8;

const int PS_TRANSPARENT_BG =  16;

const int PS_BLENDED =    32;

bool operator==(const SDL_Color&lhs, const SDL_Color& rhs) {
    return (lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b);
}

SDL_Color BLACK = {0,0,0,255}, WHITE = {255,255,255,255};

SDL_Surface* printString( SDL_Surface *dest, TTF_Font *font,

                           std::string str, SDL_Rect &rc, SDL_Color clrFg,

                           SDL_Color clrBg, int flags ) {

/* This function prints "str" with font "font" and color "clrFg"

 * onto a rectangle of color "clrBg".

 * It does not pad the text.

 * If CREATE_SURFACE is NOT passed, the function returns NULL,

 *otherwise, it returns an SDL_Surface * pointer.

 */

  // If there's nothing to draw, return NULL:

  if( str.length() == 0 || font == NULL ) {

    return( NULL );

  }



  // This is the surface that everything is printed to.

  SDL_Surface *sText = NULL;

  int lineSkip = TTF_FontLineSkip( font );

  int width = 10, height = 10;

  std::vector<std::string> vLines;

  

  // Break the std::string into its lines:

  int n = 0;

  while( n != -1 ) {

    // Get until either "\n" or "\0":

    std::string strSub;

    n = str.find( "+", 0 );

    strSub = str.substr( 0,n );

    if( n != -1 ) {

      str = str.substr( n+1, -1 );

    }

    vLines.push_back(strSub);

    

    int w = 0;

    // Get the size of the rendered text.

    TTF_SizeText( font, strSub.c_str(), &w,&height );

    if( w > width ) {  width = w;  }

  }

  

  // vLines.size() == Number of Lines.

  // we assume that height is the same for all lines.

  height = (vLines.size()-1) * lineSkip + height;

  

  // dest CAN'T be NULL if you're creating the surface!

  if( dest != NULL && (flags&PS_CREATE_SURFACE) ) {

    printf("dest != NULL with PS_CREATE_SURFACE!\n");

    return(NULL);

  }

  if( dest == NULL ) {

    if( flags&PS_CREATE_SURFACE ) {

      // Create a "dest" to which to print:

      dest = SDL_CreateRGBSurface( SDL_SWSURFACE, width,

          height, 32, 255,255,255,0 );

    } else {

      printf("There was no surface.\n(Exiting function...)\n");

      return(NULL);

    }

  }

  

  sText = SDL_CreateRGBSurface( SDL_SWSURFACE, width,height,

    dest->format->BitsPerPixel, dest->format->Rmask,dest->format->Gmask,

    dest->format->Bmask,dest->format->Amask );



  // Color in the rectangle on the destination:

  if( flags&PS_TRANSPARENT_BG ) {

    // If the fg & bg colors are the same, we need to fix it:

    if( clrFg == clrBg ) {

      if( clrFg == BLACK ) {

        clrBg = WHITE;

      } else {

        clrBg = BLACK;

      }

    }

    SDL_FillRect( sText,NULL,

      SDL_MapRGB(sText->format,clrBg.r,clrBg.g,clrBg.b) );

    SDL_SetColorKey( sText, SDL_SRCCOLORKEY|SDL_RLEACCEL,

         SDL_MapRGB(sText->format,clrBg.r,clrBg.g,clrBg.b) );

  } else {

    SDL_FillRect( sText,NULL, SDL_MapRGB(sText->format, clrBg.r, clrBg.g, clrBg.b) );

  }

  

  // Actually render the text:

  SDL_Surface *sTemp = NULL;

  for( size_t i = 0; i < vLines.size(); i++ ) {

    // The rendered text:

    if( flags & PS_BLENDED ) {

      sTemp = TTF_RenderText_Blended( font, vLines[i].c_str(), clrFg );

    } else {

      sTemp = TTF_RenderText_Solid( font, vLines[i].c_str(), clrFg );

    }

    

    // Put it on the surface:

    if( (flags&PS_ALIGN_LEFT) ||

         !(flags&PS_ALIGN_CENTER ||

         flags&PS_ALIGN_RIGHT) ) {

      // If it's specifically LEFT or none of the others:
        SDL_Rect temp = {0, static_cast<short int>(i*lineSkip), 0, 0};
      SDL_BlitSurface( sTemp,NULL, sText,&temp);

    }

    if( flags & PS_ALIGN_CENTER ) {

      int w = 0, h = 0; TTF_SizeText( font, vLines[i].c_str(), &w,&h );
      SDL_Rect temp = {static_cast<short int>(width/2-w/2),static_cast<short int>(i*lineSkip), 0, 0};
      SDL_BlitSurface( sTemp,NULL, sText,&temp);

    }

    if( flags & PS_ALIGN_RIGHT ) {

      printf("ERROR: Printstd::strings()::PS_ALIGN_RIGHT:"

          "Oops, this hasn't been implemented yet\n");

    }

    // Clean up:

    SDL_FreeSurface( sTemp );

  }

  

  if( flags & PS_CREATE_SURFACE ) {

    SDL_FreeSurface( dest );

    return( sText );

  }

  

  // Draw the text on top of that:

  SDL_BlitSurface( sText,NULL, dest,&rc );

  SDL_FreeSurface( sText );

  return(NULL);

}


#endif
