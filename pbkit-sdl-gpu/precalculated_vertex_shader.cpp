#include "precalculated_vertex_shader.h"
#include <pbkit/pbkit.h>
#include <string>

#define MASK(mask, val) (((val) << (__builtin_ffs(mask) - 1)) & (mask))

// clang format off
static constexpr uint32_t kShader[] = {
#include "precalculated_vertex_shader.inl"
};
// clang format on

void LoadPrecalculatedVertexShader() {
  uint32_t* p;
  int i;

  p = pb_begin();

  // Set run address of shader
  p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_START, 0);

  p = pb_push1(p, NV097_SET_TRANSFORM_EXECUTION_MODE,
               MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_MODE,
                    NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM)
                   | MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE,
                          NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV));

  p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, 0);
  pb_end(p);

  // Set cursor and begin copying program
  p = pb_begin();
  p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_LOAD, 0);
  pb_end(p);

  for (i = 0; i < sizeof(kShader) / 16; i++) {
    p = pb_begin();
    pb_push(p++, NV097_SET_TRANSFORM_PROGRAM, 4);
    memcpy(p, &kShader[i * 4], 4 * 4);
    p += 4;
    pb_end(p);
  }
}
