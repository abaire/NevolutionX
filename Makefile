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
	$(INCDIR)/theme.cpp \
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

CXXFLAGS += -I$(CURDIR) -I$(INCDIR) -Wall -Wextra -std=gnu++11
CFLAGS   += -std=gnu11

ifneq ($(DEBUG),y)
CFLAGS += -O2
CXXFLAGS += -O2
endif

include $(NXDK_DIR)/Makefile

RESOURCES = \
	$(OUTPUT_DIR)/config.json \
	$(patsubst $(CURDIR)/Resources/%,$(OUTPUT_DIR)/%,$(wildcard $(CURDIR)/Resources/NeXThemes/*))
TARGET += $(RESOURCES)
$(GEN_XISO): $(RESOURCES)

$(OUTPUT_DIR)/NeXThemes/%: $(CURDIR)/Resources/NeXThemes/%
	$(VE)mkdir -p '$(dir $@)'
	$(VE)cp -r '$<' '$@'

$(OUTPUT_DIR)/config.json: $(CURDIR)/sampleconfig.json
	@mkdir -p $(OUTPUT_DIR)
	cp $(CURDIR)/sampleconfig.json $(OUTPUT_DIR)/config.json
