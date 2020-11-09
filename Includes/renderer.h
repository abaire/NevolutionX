#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <SDL.h>

#include <xgu/xgu.h>
#include <xgu/xgux.h>

int min(int lhs, int rhs);
int max(int lhs, int rhs);

typedef struct Vertex {
    XguVec3 pos;
    XguVec3 normal;
    float texcoord[2];
} Vertex;

class Renderer {
public:
  Renderer();
  ~Renderer();

  int init();
  int init(const char* bg);
  int clear();
  void flip();

  int getHeight() const {return height;}

  int setDrawColor(uint8_t r = 0x40, uint8_t g = 0x40,
                   uint8_t b = 0xE0, uint8_t a = 0x00);

  void drawTexture(SDL_Texture* tex, SDL_Rect &src, SDL_Rect &dst);
  void drawTexture(SDL_Texture* tex, SDL_Rect &dst);
  void drawTexture(SDL_Texture* tex, int x, int y);

  void blitSurface(SDL_Surface* bg, SDL_Surface* fg, int offset);

  void drawBackground();

private:
  SDL_Surface *background = nullptr;

  XguVec4 v_obj_rot = {0,0,0,1};
  XguVec4 v_obj_scale = {1,1,1,1};
  XguVec4 v_obj_pos = {0,0,0,1};
  XguVec4 v_cam_pos = {0,0,360,1};
  XguVec4 v_cam_rot = {0,0,0,1};

  Vertex bgVert[4] = {
    { {{-1, -1,  0}}, {{-0,  0, -1}}, {    0,   0} },
    { {{ 1, -1,  0}}, {{-0,  0, -1}}, { 1280,   0} },
    { {{-1,  1,  0}}, {{-0,  0, -1}}, {    0, 720} },
    { {{ 1,  1,  0}}, {{-0,  0, -1}}, { 1280, 720} }
  };

  int height = 0;
  int width = 0;
  int overscanCompX = 0;
  int overscanCompY = 0;
  size_t menuItemCount = 0;
  size_t lowerHalf = 0;
  size_t upperHalf = 0;
};

#endif
