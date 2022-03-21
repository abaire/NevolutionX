XBE_TITLE = NevolutionX
INCDIR = $(CURDIR)/Includes
RESOURCEDIR = $(CURDIR)/Resources
SRCDIR = $(CURDIR)/Sources

SRCS += \
	$(CURDIR)/main.cpp \
	$(SRCDIR)/audioMenu.cpp \
	$(SRCDIR)/config.cpp \
	$(SRCDIR)/font.cpp \
	$(SRCDIR)/ftpConnection.cpp \
	$(SRCDIR)/ftpServer.cpp \
	$(SRCDIR)/infoLog.cpp \
	$(SRCDIR)/langMenu.cpp \
	$(SRCDIR)/logViewer.cpp \
	$(SRCDIR)/logViewerMenu.cpp \
	$(SRCDIR)/menu.cpp \
	$(SRCDIR)/networkManager.cpp \
	$(SRCDIR)/networking.cpp \
	$(SRCDIR)/renderer.cpp \
	$(SRCDIR)/screensaver.cpp \
	$(SRCDIR)/settingsMenu.cpp \
	$(SRCDIR)/sntpClient.cpp \
	$(SRCDIR)/subAppRouter.cpp \
	$(SRCDIR)/subsystems.cpp \
	$(SRCDIR)/theme.cpp \
	$(SRCDIR)/timeMenu.cpp \
	$(SRCDIR)/timing.cpp \
	$(SRCDIR)/videoMenu.cpp \
	$(SRCDIR)/wipeCache.cpp \
	$(SRCDIR)/xbeLauncher.cpp \
	$(SRCDIR)/xbeScanner.cpp \
	$(CURDIR)/pbkit-sdl-gpu/color_combiner.cpp \
	$(CURDIR)/pbkit-sdl-gpu/debug_output.cpp \
	$(CURDIR)/pbkit-sdl-gpu/math3d.c \
	$(CURDIR)/pbkit-sdl-gpu/pbkit_sdl_gpu.cpp \
	$(CURDIR)/pbkit-sdl-gpu/precalculated_vertex_shader.cpp \
	$(CURDIR)/3rdparty/swizzle.c \
	$(CURDIR)/3rdparty/SDL_FontCache/SDL_FontCache.c

SHADER_OBJS = \
	$(CURDIR)/pbkit-sdl-gpu/precalculated_vertex_shader.inl

NXDK_DIR ?= $(CURDIR)/../nxdk
NXDK_SDL = y
NXDK_CXX = y
NXDK_NET = y
NXDK_DISABLE_AUTOMOUNT_D = y

GEN_XISO = ${XBE_TITLE}.iso

CXXFLAGS += -I$(CURDIR) -I$(INCDIR) -Wall -Wextra -std=gnu++11 -I$(SDL_GPU_DIR)/include -I$(CURDIR)/pbkit-sdl-gpu -DFC_USE_SDL_GPU
CFLAGS   += -std=gnu11 -I$(SDL_GPU_DIR)/include  -I$(CURDIR)/pbkit-sdl-gpu -DFC_USE_SDL_GPU

ifneq ($(DEBUG),y)
CFLAGS += -O2
CXXFLAGS += -O2
endif

CLEANRULES = clean-resources clean-gl
include $(NXDK_DIR)/Makefile

override SDL_GPU_DIR := 3rdparty/sdl-gpu
include pbkit-sdl-gpu/Makefile.inc

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

.PHONY: clean-resources
clean-resources:
	$(VE)rm -rf $(OUTPUT_DIR)/NeXThemes

.PHONY: clean-gl
clean-gl: clean-sdl-gpu
