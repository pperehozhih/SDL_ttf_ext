//
//  main.cpp
//  test-ext
//
//  Created by Paul Perekhozhikh on 10.01.15.
//  Copyright (c) 2015 Paul. All rights reserved.
//

#include <iostream>
#include <vector>
#include <assert.h>
#include <stdint.h>
#include <SDL2/SDL_surface.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_image/SDL_image.h>

#define mask32(BYTE) (*(uint32_t *)(uint8_t [4]){ [BYTE] = 0xff })

void BoxBlurHorizontal(SDL_Surface* bmp, int range)
{
   int* pixels = (int*)bmp->pixels;
   int w = bmp->w;
   int h = bmp->h;
   int halfRange = range / 2;
   int index = 0;
   static std::vector<int> buffered;
   if (buffered.size() < w)
      buffered.resize(w);
   int* newColors = &buffered[0];
   
   for (int y = 0; y < h; y++)
   {
      int hits = 0;
      int r = 0;
      int g = 0;
      int b = 0;
      int a = 0;
      for (int x = -halfRange; x < w; x++)
      {
         int oldPixel = x - halfRange - 1;
         if (oldPixel >= 0)
         {
            int col = pixels[index + oldPixel];
            if (col != 0)
            {
               a -= ((unsigned char)(col >> 24));
               r -= ((unsigned char)(col >> 16));
               g -= ((unsigned char)(col >> 8 ));
               b -= ((unsigned char)col);
            }
            hits--;
         }
         
         int newPixel = x + halfRange;
         if (newPixel < w)
         {
            int col = pixels[index + newPixel];
            if (col != 0)
            {
               a += ((unsigned char)(col >> 24));
               r += ((unsigned char)(col >> 16));
               g += ((unsigned char)(col >> 8 ));
               b += ((unsigned char)col);
            }
            hits++;
         }
         
         if (x >= 0)
         {
            int color =
            ((unsigned char)(a / hits) << 24)
            | ((unsigned char)(r / hits) << 16)
            | ((unsigned char)(g / hits) << 8 )
            | ((unsigned char)(b / hits));
            
            newColors[x] = color;
         }
      }
      
      for (int x = 0; x < w; x++)
      {
         pixels[index + x] = newColors[x];
      }
      
      index += w;
   }
}

void BoxBlurVertical(SDL_Surface* bmp, int range)
{
   int* pixels = (int*)bmp->pixels;
   int w = bmp->w;
   int h = bmp->h;
   int halfRange = range / 2;
   static std::vector<int> buffered;
   if (buffered.size() < h)
      buffered.resize(h);
   int* newColors = &buffered[0];
   int oldPixelOffset = -(halfRange + 1) * w;
   int newPixelOffset = (halfRange) * w;
   
   for (int x = 0; x < w; x++)
   {
      int hits = 0;
      int a = 0;
      int r = 0;
      int g = 0;
      int b = 0;
      int index = -halfRange * w + x;
      for (int y = -halfRange; y < h; y++)
      {
         int oldPixel = y - halfRange - 1;
         if (oldPixel >= 0)
         {
            int col = pixels[index + oldPixelOffset];
            if (col != 0)
            {
               a -= ((unsigned char)(col >> 24));
               r -= ((unsigned char)(col >> 16));
               g -= ((unsigned char)(col >> 8 ));
               b -= ((unsigned char)col);
            }
            hits--;
         }
         
         int newPixel = y + halfRange;
         if (newPixel < h)
         {
            int col = pixels[index + newPixelOffset];
            if (col != 0)
            {
               a += ((unsigned char)(col >> 24));
               r += ((unsigned char)(col >> 16));
               g += ((unsigned char)(col >> 8 ));
               b += ((unsigned char)col);
            }
            hits++;
         }
         
         if (y >= 0)
         {
            int color =
            ((unsigned char)(a / hits) << 24)
            | ((unsigned char)(r / hits) << 16)
            | ((unsigned char)(g / hits) << 8 )
            | ((unsigned char)(b / hits));
            
            newColors[y] = color;
         }
         
         index += w;
      }
      
      for (int y = 0; y < h; y++)
      {
         pixels[y * w + x] = newColors[y];
      }
   }
}

void BoxBlur(SDL_Surface* bmp, int range)
{
   if (range < 1)
   {
      return;
   }
   
   BoxBlurHorizontal(bmp, range);
   BoxBlurVertical(bmp, range);
}

SDL_Surface* ChangeColor(SDL_Surface* bmp, int color){
   SDL_Surface* result = SDL_ConvertSurface(bmp, bmp->format, SDL_SWSURFACE);
   for (int x = 0; x < result->w; x++){
      for (int y = 0; y < result->h; y++){
         int col = ((int*)result->pixels)[y*result->w + x];
         unsigned char a = ((unsigned char)(col >> 24));
         unsigned char r = ((unsigned char)(color >> 16));
         unsigned char g = ((unsigned char)(color >> 8 ));
         unsigned char b =((unsigned char)color);
         
         ((int*)result->pixels)[y*result->w + x] = (a << 24)
         | (r << 16)
         | (g << 8 )
         | (b);
      }
   }
   return result;
}

int main(int argc, const char * argv[]) {
   std::vector<char> buffer;
   buffer.resize(1024 * 1024 * 4);
   TTF_Init();
   IMG_Init(IMG_INIT_PNG);
   SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(&buffer[0], 1024, 1024, 32, 1024 * 4,
                                                   0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
   assert(surface);
   SDL_FillRect(surface, nullptr, 0);
   
   TTF_Font* font = TTF_OpenFont("/Library/Fonts/Arial Unicode.ttf", 108);
   assert(font);
   surface = TTF_RenderUTF8_Blended_WrappedFrom(surface, font, "TEST TEXT TEST TEST", {255}, 0, 2, 0);
   assert(surface);
   SDL_Surface* tmp = ChangeColor(surface, 0x00ff00);
   BoxBlur(tmp, 25);
//   BoxBlur(surface, 5);
   SDL_UpperBlit(surface, 0, tmp, 0);
   IMG_SavePNG(tmp, "/tmp/test.png");
   TTF_CloseFont(font);
   SDL_FreeSurface(surface);
   TTF_Quit();
   return 0;
}
