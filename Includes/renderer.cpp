#include "renderer.h"

#include "outputLine.h"
#include <algorithm>
#include <cmath>

#ifdef NXDK
#include <hal/video.h>
#endif

// One line of text with the default font is 31 pixels high.
// FIXME: Should probably be dynamic and dependent of font settings.
const unsigned int FONT_TEX_SIZE = 31;

Renderer::Renderer() {
#ifdef NXDK
  VIDEO_MODE xmode = XVideoGetMode();
  height = xmode.height;
  width = xmode.width;
#else
  height = 480;
  width = 640;
#endif

  overscanCompX = width * 0.075;
  overscanCompY = height * 0.075;
  menuItemCount = (height - (overscanCompY * 2)) / FONT_TEX_SIZE;
  lowerHalf = menuItemCount/2;
  upperHalf = ceil(menuItemCount/2.0);
}

Renderer::~Renderer() {
  if (background != nullptr) {
    SDL_FreeSurface(background);
  }
}

int Renderer::init() {
  pb_init();
  pb_show_front_screen();
  
}

int Renderer::init(const char* bgpath) {
  int ret = init();
  if (ret != 0) {
    return ret;
  }
  char* bgname = (char*)malloc(strlen(bgpath)+10);
  sprintf(bgname, "%s%d.bmp", bgpath, height);
  SDL_Surface *bgsurf = SDL_LoadBMP(bgname);
  free(bgname);
  if (bgsurf == nullptr) {
    outputLine("Creating background surface failed.\n");
    return 3;
  }
  background = SDL_CreateTextureFromSurface(renderer, bgsurf);
  SDL_FreeSurface(bgsurf);
  if (background == nullptr) {
    outputLine("Creating background texture failed.\n");
    return 4;
  }
  return ret;
}

void Renderer::clear() {
  pb_wait_for_vbl();
  pb_reset();
  pb_target_back_buffer();  
  while(pb_busy());
}

void Renderer::flip() {
  setDrawColor(0, 0, 0, 0xFF);
  SDL_RenderDrawRect(renderer, nullptr);
  setDrawColor();
  SDL_RenderPresent(renderer);
#ifdef NXDK
  XVideoWaitForVBlank();
#endif
}

int Renderer::setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void Renderer::drawTexture(SDL_Texture* tex, SDL_Rect &src, SDL_Rect &dst) {
  SDL_RenderCopy(renderer, tex, &src, &dst);
}

void Renderer::drawTexture(SDL_Texture* tex, SDL_Rect &dst) {
  SDL_RenderCopy(renderer, tex, nullptr, &dst);
}

void Renderer::drawTexture(SDL_Texture* tex, int x, int y) {
  SDL_Rect dst = {x, y, 0, 0};
  SDL_QueryTexture(tex, nullptr, nullptr, &dst.w, &dst.h);
  drawTexture(tex, dst);
}

void Renderer::blitSurface(SDL_Surface* bg, SDL_Surface* fg, int offset) {
  SDL_Rect dst = {offset, offset, fg->w, fg->h};
  SDL_SetSurfaceBlendMode(fg, SDL_BLENDMODE_BLEND);
  SDL_BlitSurface(fg, NULL, bg, &dst);
}

void Renderer::drawBackground() {
  if (background != nullptr) {
    drawTexture(background, 0, 0);
  }
}
