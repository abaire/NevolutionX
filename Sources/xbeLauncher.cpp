#include "xbeLauncher.hpp"
#include "infoLog.hpp"

#ifdef NXDK
#include <hal/video.h>
#include <hal/xbox.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>
#include "nxdk/path.h"
#endif

#ifdef NXDK
#define USE_TRAMPOLINE
#ifdef USE_TRAMPOLINE
struct TrampolineLaunchData {
  union
  {
    char _reserve[3072];
    char target_path[MAX_PATH];
  };
};

struct TrampolineLaunchData g_launch_data;

#endif // USE_TRAMPOLINE
#endif // NXDK

void XBELauncher::shutdown() {
#ifdef NXDK
  HalInitiateShutdown();
#endif
}

void XBELauncher::exitToDashboard() {
  showLaunchImage();
  // TODO: Switch to XLaunchXBE(nullptr) if XboxDev/nxdk#501 is merged.
  exit(0);
}

void XBELauncher::launch(std::string const& xbePath) {
#ifdef NXDK
#ifdef USE_TRAMPOLINE
  // XConvertDOSFilenameToXBOX inside XLaunchXBEEx cannot handle arbitrary drive letters.
  char trampoline_path[MAX_PATH];
  char* finalSeparator;
  nxGetCurrentXbeNtPath(trampoline_path);

  finalSeparator = strrchr(trampoline_path, '\\');
  strcpy(finalSeparator + 1, "NeXData\\Trampoline.xbe");

  InfoLog::outputLine(InfoLog::DEBUG, "Trampoline xbe %s\n", xbePath.c_str());
  strcpy(g_launch_data.target_path, xbePath.c_str());
  XLaunchXBEEx(trampoline_path, &g_launch_data);
#else
  showLaunchImage();
  XLaunchXBE(const_cast<char*>(xbePath.c_str()));
#endif // USE_TRAMPOLINE
#endif
}

void XBELauncher::showLaunchImage() {
#ifdef NXDK
  VIDEO_MODE mode = XVideoGetMode();

  // TODO(XboxDev/nxdk#507): Display launch image instead when framebuffer can be persisted.
  unsigned char* fb = XVideoGetFB();
  memset(fb, 0, mode.width * mode.height * (mode.bpp >> 3));
  XVideoFlushFB();
  XVideoSetVideoEnable(FALSE);
#endif
}
