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
	$(CURDIR)/3rdparty/SDL_FontCache/SDL_FontCache.c

NXDK_DIR ?= $(CURDIR)/../nxdk
NXDK_SDL = y
NXDK_CXX = y
NXDK_NET = y
NXDK_DISABLE_AUTOMOUNT_D = y

GEN_XISO = ${XBE_TITLE}.iso

CXXFLAGS += -I$(CURDIR) -I$(INCDIR) -I$(SDL_GPU_DIR)/include -I$(PBGL_DIR)/include -Wall -Wextra -std=gnu++11 -DSCANNER_THREADED
CFLAGS   += -I$(SDL_GPU_DIR)/include -std=gnu11 -DSCANNER_THREADED

ifneq ($(DEBUG),y)
CFLAGS += -O2
CXXFLAGS += -O2
endif

new_all: copy_resources all

include $(NXDK_DIR)/Makefile

copy_resources: $(OUTPUT_DIR)/config.json
	@cp $(RESOURCEDIR)/480.png $(RESOURCEDIR)/720.png $(RESOURCEDIR)/vegur.ttf $(OUTPUT_DIR)

$(OUTPUT_DIR)/config.json: $(CURDIR)/sampleconfig.json
	@mkdir -p $(OUTPUT_DIR)
	cp $(CURDIR)/sampleconfig.json $(OUTPUT_DIR)/config.json
