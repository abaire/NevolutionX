#include "xbeLauncher.hpp"

#ifdef NXDK
#include <hal/video.h>
#include <hal/xbox.h>
#include <stdio.h>
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

  FILE* fp = fopen("A:\\fbdump.bin", "rb");
  if (!fp) {
    return;
  }

  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  auto framebufferMemory = (uint8_t*)MmAllocateContiguousMemoryEx(
      file_size, 0x00000000, 0x7FFFFFFF, 0x1000, PAGE_READWRITE | PAGE_WRITECOMBINE);

  long bytes_read = fread(framebufferMemory, 1, file_size, fp);
  if (bytes_read != file_size) {
    MmFreeContiguousMemory(framebufferMemory);
    fclose(fp);
    return;
  }
  fclose(fp);

  MmPersistContiguousMemory(framebufferMemory, file_size, TRUE);
  AvSetSavedDataAddress(framebufferMemory);

  XVideoFlushFB();
//  XVideoSetVideoEnable(FALSE);
#endif
}
