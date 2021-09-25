#include "xbeLauncher.h"
#include <hal/video.h>
#include <hal/xbox.h>
#include <fstream>

void XBELauncher::exitToDashboard() {
  showLaunchImage();
  // TODO: Switch to XLaunchXBE(nullptr) if XboxDev/nxdk#501 is merged.
  //  exit(0);
  XLaunchXBE(nullptr);
}

void XBELauncher::launch(std::string const& xbePath) {
  showLaunchImage();
  XLaunchXBE(const_cast<char*>(xbePath.c_str()));
}


typedef struct _X_D3DSURFACE_DESC {
  DWORD Format;
  DWORD Type;
  DWORD Usage;
  UINT Size;
  DWORD MultiSampleType;
  UINT Width;
  UINT Height;
} surface_desc;

struct X_D3DResource {
  DWORD Common{
    1 | 0x00050000 | 0x01000000 | 0x80000000
  }; // 1 refcount? | X_D3DCOMMON_TYPE_SURFACE | X_D3DCOMMON_D3DCREATED | ?
  DWORD Data{ 0x0004D040 }; // ?
  DWORD Lock{ 0x0000D141 }; // ? This one changes with a new launch. X_D3DLOCK_TILED?
};

struct X_D3DPixelContainer : public X_D3DResource {
  DWORD Format{
    0x8 | 0x01 | (2 << 4) | (0x1e << 8) | (1 << 16)
  }; // X_D3DFORMAT_BORDERSOURCE_COLOR | X_D3DFORMAT_DMACHANNEL_A | 2 <<
     // X_D3DFORMAT_DIMENSION_SHIFT | X_D3DFMT_LIN_A8R8G8B8 << X_D3DFORMAT_FORMAT_SHIFT | 1
     // << X_D3DFORMAT_MIPMAP_SHIFT (always 1 for surfaces)
  DWORD Size{ 0 }; // Size of a non power-of-2 texture, must be zero otherwise
};

struct X_D3DSurface : public X_D3DPixelContainer {
  /*X_D3DBaseTexture*/ void* Parent{ nullptr };
  unsigned char Padding[40];
};

void XBELauncher::showLaunchImage() {
  VIDEO_MODE mode = XVideoGetMode();

  int bytesPerPixel = (mode.bpp + 7) / 8;
  int pitch = mode.width * bytesPerPixel;
  int screenSize = pitch * mode.height;

  //  surface_desc s;
  //  s.Format = 0x06; // X_D3DFMT_A8R8G8B8
  //  s.Type = 1; // X_D3DRTYPE_SURFACE
  //  s.Usage = 0;
  //  s.Size = 0; // TODO: Is this size, or is this memory pool?
  //  s.MultiSampleType = 0x0011; // NONE
  //  s.Width = mode.width;
  //  s.Height = mode.height;

  X_D3DSurface surface;
  // The Size field of a container will be zero if the texture is swizzled or compressed.
// It is guarenteed to be non-zero otherwise because either the height/width will be
// greater than one or the pitch adjust will be nonzero because the minimum texture
// pitch is 8 bytes.
#define X_D3DSIZE_WIDTH_MASK 0x00000FFF // Width of the texture - 1, in texels
//#define X_D3DSIZE_WIDTH_SHIFT           0
#define X_D3DSIZE_HEIGHT_MASK 0x00FFF000 // Height of the texture - 1, in texels
#define X_D3DSIZE_HEIGHT_SHIFT 12
#define X_D3DSIZE_PITCH_MASK 0xFF000000 // Pitch / 64 - 1
#define X_D3DSIZE_PITCH_SHIFT 24

  char* oldSavedFramebuffer = (char*)AvGetSavedDataAddress();
  if (oldSavedFramebuffer) {
    memcpy(&surface, oldSavedFramebuffer, sizeof(surface));
  }

  surface.Format = 0x8 | 0x01 | (2 << 4) | (0x1e << 8) | (1 << 16);
  surface.Size = (((DWORD)mode.width - 1) & 0xFFF)
                 | ((((DWORD)mode.height - 1) & 0xFFF) << 12)
                 | ((((DWORD)pitch / 64u - 1u) & 0xFF) << 24);
  //  surface.Size = 0x2C1DF2CF;
  surface.Size = 0x271DF27F;
  //  surface.Size = (mode.width & X_D3DSIZE_WIDTH_MASK)
  //                 | ((mode.height << X_D3DSIZE_HEIGHT_SHIFT) & X_D3DSIZE_HEIGHT_MASK)
  //                 | (((pitch / 64 - 1) << X_D3DSIZE_PITCH_SHIFT) & X_D3DSIZE_PITCH_MASK);

  //  memset(surface.Padding, 0xFF, sizeof(surface.Padding));

  DWORD totalSize = sizeof(surface) + screenSize;

  unsigned char* framebufferMemory = (unsigned char*)MmAllocateContiguousMemoryEx(
      totalSize, 0x00000000, 0x7FFFFFFF, 0x1000, PAGE_READWRITE | PAGE_WRITECOMBINE);
  memcpy(framebufferMemory, &surface, sizeof(surface));
  unsigned char* fb = XVideoGetFB();

  memcpy(framebufferMemory + sizeof(surface), fb, screenSize);
  MmPersistContiguousMemory(framebufferMemory, totalSize, TRUE);
  AvSetSavedDataAddress(framebufferMemory);

  {
    SIZE_T sz = MmQueryAllocationSize(framebufferMemory);
    std::ofstream outputFile("A:\\prelaunch.bin");
    outputFile.write((char*)framebufferMemory, sz);
  }

  // TODO(XboxDev/nxdk#507): Display launch image instead when framebuffer can be persisted.
  memset(fb, 0, mode.width * mode.height * (mode.bpp >> 3));
  //  XVideoSetFB((unsigned char*)((DWORD)(framebufferMemory + sizeof(surface)) &
  //  0x7FFFFFFF));
  XVideoFlushFB();
}
