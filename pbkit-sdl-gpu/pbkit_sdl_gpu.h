#ifndef NEVOLUTIONX_PBKIT_SDL_GPU_H
#define NEVOLUTIONX_PBKIT_SDL_GPU_H

#include <stdio.h>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

// Dummy implementation of fopen_s to allow SDL_gpu to compile.
int fopen_s(FILE** pFile, const char* filename, const char* mode);


void PBKitSDLGPUInit();

#ifdef __cplusplus
}; // extern "C"
#endif


#endif // NEVOLUTIONX_PBKIT_SDL_GPU_H
