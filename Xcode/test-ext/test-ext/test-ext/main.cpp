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
   surface = TTF_RenderUTF8_BlendedFrom(surface, font, "TEST TEXT TEST TEST", {1});
   assert(surface);
   IMG_SavePNG(surface, "/tmp/test.png");
   TTF_CloseFont(font);
   SDL_FreeSurface(surface);
   TTF_Quit();
   return 0;
}
