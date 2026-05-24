#include "shaders.h"
#include <math.h>
#include <stdlib.h>

float freq = 0.3f;
float gradient_angle = -1.0f;
float opacity = 0.0f;
bool use_quantization = false;
int32_t sin_lut_fixed[LUT_SIZE];

#define RAD_TO_INDEX_FIXED 42722831 

void init_lut(void) { 
    for(int i=0; i<LUT_SIZE; i++) {
        sin_lut_fixed[i] = FLOAT_TO_FIXED(sin(2.0 * M_PI * i / LUT_SIZE));
    }
}

int32_t fast_sin_fixed(int32_t x_fixed) {
    int32_t scaled = FIXED_MUL(x_fixed, RAD_TO_INDEX_FIXED);
    int idx = ((uint32_t)scaled >> FIXED_SHIFT) & (LUT_SIZE - 1);
    int next = (idx + 1) & (LUT_SIZE - 1);
    int32_t frac = scaled & (FIXED_ONE - 1);
    int32_t val1 = sin_lut_fixed[idx];
    int32_t val2 = sin_lut_fixed[next];
    return val1 + FIXED_MUL(frac, (val2 - val1));
}

float fast_sin(float x) {
    int32_t x_fixed = FLOAT_TO_FIXED(x);
    return FIXED_TO_FLOAT(fast_sin_fixed(x_fixed));
}

float shader_sunset(int x, int y, float phase) {
    return sinf(x * 0.15f + phase) + sinf(y * 0.15f + phase * 0.5f);
}

float shader_matrix(int x, int y, float phase, float *intensity) {
    float p = phase + x * 0.1f + y * 0.1f;
    float pulse = sinf(phase * 3.0f) * 0.15f + 0.85f;
    float scanline = (fmodf(y - phase * 5.0f, 10.0f) < 1.0f) ? 0.7f : 1.0f;
    float sparkle = (rand() % 100 > 98) ? 0.5f : 1.0f;
    *intensity = pulse * scanline * sparkle;
    if (*intensity < 0.15f) *intensity = 0.15f;
    return p;
}

float shader_pulse(int x, int y, int len, int count, float phase, float *intensity) {
    float dx = x - len / 2.0f;
    float dy = y - count / 2.0f;
    float dist = sqrtf(dx * dx + dy * dy);    
    float p = dist - phase;
    *intensity = (sinf(dist * 0.5f - phase * 2.0f) + 1.0f) / 2.0f;
    *intensity = *intensity * 0.8f + 0.2f;
    return p;
}

void apply_border_opacity(int x, int y, int len, int count, float op, int *r, int *g, int *b) {
    if (op <= 0.0f) return;
    float cx = len / 2.0f, cy = count / 2.0f;
    float dx = x - cx, dy = y - cy;
    float factor = 1.0f - (sqrtf(dx*dx + dy*dy) / sqrtf(cx*cx + cy*cy));
    factor = 1.0f - (1.0f - factor) * op;
    *r = (int)(*r * factor);
    *g = (int)(*g * factor);
    *b = (int)(*b * factor);
    if(*r<0) *r=0; else if(*r>255) *r=255;
    if(*g<0) *g=0; else if(*g>255) *g=255;
    if(*b<0) *b=0; else if(*b>255) *b=255;
}

void get_color_fast(int x, int y, int mode, int len, int count, float phase, int *r, int *g, int *b) {
    float p, intensity = 1.0f;
    
    switch(mode) {  
        case 1: p = shader_sunset(x, y, phase); break;
        case 2: p = phase; break;
        case 3: p = y*0.5f+phase; break;
        case 4: p = y*0.8f+fast_sin(x*0.3f)*3.0f+phase; break;
        case 5: { float dx=x-len/2.0f, dy=y-count/2.0f; p=sqrtf(dx*dx+dy*dy)-phase; break; }  
        case 6: p = fast_sin(x*0.2f)*fast_sin(y*0.2f+PI_2)*10.0f+phase; break;  
        case 7: { float center=len/2.0f; p=phase+x*0.1f; intensity=1.0f-(fabsf(x-center)/center)*0.8f; if(intensity<0.2f) intensity=0.2f; break;}  
        case 8: { float center_y=count/2.0f; p=phase+y*0.2f; intensity=1.0f-(fabsf(y-center_y)/center_y)*0.7f; if(intensity<0.3f) intensity=0.3f; break;}  
        case 9: p = phase+x*0.1f; intensity=(x%2==0)?1.0f:0.2f; break;
        case 10: p = shader_matrix(x, y, phase, &intensity); break;  
        case 11: p = shader_pulse(x, y, len, count, phase, &intensity); break;
        default: p = phase + x*0.2f + y*0.1f; if (gradient_angle >= 0.0f) {
        float rad = gradient_angle * M_PI / 180.0f; p += cosf(rad) * x + sinf(rad) * y; } break;
    }

    int32_t p_fixed = FLOAT_TO_FIXED(p);
    int32_t freq_fixed = FLOAT_TO_FIXED(freq);
    int32_t base_phase = FIXED_MUL(freq_fixed, p_fixed);

    int32_t sin_r = fast_sin_fixed(base_phase);
    int32_t sin_g = fast_sin_fixed(base_phase + FLOAT_TO_FIXED(2.094f));
    int32_t sin_b = fast_sin_fixed(base_phase + FLOAT_TO_FIXED(4.188f));

    int raw_r = ((sin_r * 127) / FIXED_ONE) + 128;
    int raw_g = ((sin_g * 127) / FIXED_ONE) + 128;
    int raw_b = ((sin_b * 127) / FIXED_ONE) + 128;

    *r = (int)(raw_r * intensity);
    *g = (int)(raw_g * intensity);
    *b = (int)(raw_b * intensity);

    if (use_quantization) {
        *r &= 0xF8; *g &= 0xF8; *b &= 0xF8;
    }

    if (*r < 0) *r = 0; if (*r > 255) *r = 255;
    if (*g < 0) *g = 0; if (*g > 255) *g = 255;
    if (*b < 0) *b = 0; if (*b > 255) *b = 255;

    apply_border_opacity(x, y, len, count, opacity, r, g, b);
}
