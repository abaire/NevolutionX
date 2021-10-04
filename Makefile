XBE_TITLE = NevolutionX
INCDIR = $(CURDIR)/Includes
RESOURCEDIR = $(CURDIR)/Resources

SRCS += \
	$(CURDIR)/main.cpp \
	$(INCDIR)/audioMenu.cpp \
	$(INCDIR)/config.cpp \
	$(INCDIR)/font.cpp \
	$(INCDIR)/ftpConnection.cpp \
	$(INCDIR)/ftpServer.cpp \
	$(INCDIR)/infoLog.cpp \
	$(INCDIR)/langMenu.cpp \
	$(INCDIR)/logViewer.cpp \
	$(INCDIR)/logViewerMenu.cpp \
	$(INCDIR)/menu.cpp \
	$(INCDIR)/networkManager.cpp \
	$(INCDIR)/networking.cpp \
	$(INCDIR)/renderer.cpp \
	$(INCDIR)/settingsMenu.cpp \
	$(INCDIR)/sntpClient.cpp \
	$(INCDIR)/subAppRouter.cpp \
	$(INCDIR)/subsystems.cpp \
	$(INCDIR)/timeMenu.cpp \
	$(INCDIR)/timing.cpp \
	$(INCDIR)/videoMenu.cpp \
	$(INCDIR)/wipeCache.cpp \
	$(INCDIR)/xbeLauncher.cpp \
	$(INCDIR)/xbeScanner.cpp \
	$(CURDIR)/nxdk-sdl-gpu/nxdkSDLGPU.cpp \
	$(CURDIR)/3rdparty/SDL_FontCache/SDL_FontCache.c

NXDK_DIR ?= $(CURDIR)/../nxdk
NXDK_SDL = y
NXDK_CXX = y
NXDK_NET = y
NXDK_DISABLE_AUTOMOUNT_D = y

GEN_XISO = ${XBE_TITLE}.iso

CXXFLAGS += -I$(CURDIR) -I$(INCDIR) -I$(SDL_GPU_DIR)/include -I$(PBGL_DIR)/include -Wall -Wextra -std=gnu++11 -DFC_USE_SDL_GPU
CFLAGS   += -I$(SDL_GPU_DIR)/include -std=gnu11 -DFC_USE_SDL_GPU

ifneq ($(DEBUG),y)
CFLAGS += -O2
CXXFLAGS += -O2
endif

CLEANRULES = clean-resources clean-gl

include $(NXDK_DIR)/Makefile

override PBGL_DIR := 3rdparty/pbgl
include 3rdparty/pbgl/Makefile

override SDL_GPU_DIR := 3rdparty/sdl-gpu
include nxdk-sdl-gpu/Makefile.inc

RESOURCES = \
	$(OUTPUT_DIR)/config.json \
	$(OUTPUT_DIR)/480.png \
	$(OUTPUT_DIR)/720.png

TARGET += $(RESOURCES)
$(GEN_XISO): $(RESOURCES)

$(OUTPUT_DIR)/config.json: $(CURDIR)/sampleconfig.json
	@mkdir -p $(OUTPUT_DIR)
	cp $(CURDIR)/sampleconfig.json $(OUTPUT_DIR)/config.json
$(OUTPUT_DIR)/%: $(RESOURCEDIR)/%
	$(VE)cp -r '$<' '$@'

.PHONY: clean-resources
clean-resources:
	$(VE)rm -rf $(OUTPUT_DIR)/NeXThemes

.PHONY: clean-gl
clean-gl: clean-sdl-gpu clean-pbgl
