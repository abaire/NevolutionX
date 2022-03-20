#include "xbeLauncher.hpp"

#ifdef NXDK
#include <hal/video.h>
#include <hal/xbox.h>
#include <xboxkrnl/xboxkrnl.h>
#endif

void XBELauncher::shutdown() {
#ifdef NXDK
  HalInitiateShutdown();
#endif
}

void XBELauncher::exitToDashboard() {
  showLaunchImage();
#ifdef NXDK
  XLaunchXBE(nullptr);
#else
  exit(0);
#endif
}

void XBELauncher::launch(std::string const& xbePath) {
#ifdef NXDK
  showLaunchImage();
  XLaunchXBE(const_cast<char*>(xbePath.c_str()));
#endif
}

void XBELauncher::showLaunchImage() {
#ifdef NXDK
  VIDEO_MODE mode = XVideoGetMode();

  // TODO(XboxDev/nxdk#507): Display launch image instead when framebuffer can be persisted.
  unsigned char* fb = XVideoGetFB();

  //  unsigned char* framebufferMemory = (unsigned char*)MmAllocateContiguousMemoryEx(
  //      totalSize, 0x00000000, 0x7FFFFFFF, 0x1000, PAGE_READWRITE | PAGE_WRITECOMBINE);
  //  memcpy(framebufferMemory, &surface, sizeof(surface));
  //  unsigned char* fb = XVideoGetFB();
  //
  //  memcpy(framebufferMemory + sizeof(surface), fb, screenSize);
  //  MmPersistContiguousMemory(framebufferMemory, totalSize, TRUE);
  AvSetSavedDataAddress(nullptr);

  memset(fb, 0, mode.width * mode.height * (mode.bpp >> 3));
  XVideoFlushFB();
  XVideoSetVideoEnable(FALSE);
#endif
}
