#include "shaders.h"
#include <math.h>
#include <stdlib.h>

#define LUT_SIZE 4096
#define PI_2 1.57079632679

double freq = 0.3;
double diagonal = 0.0;
double opacity = 1.0;
static double sin_lut[LUT_SIZE];

void init_lut(void) { 
    for(int i=0; i<LUT_SIZE; i++) sin_lut[i] = sin(2.0*M_PI*i/LUT_SIZE);
}

static inline double fast_sin(double x) {
    int idx = (int)(fmod(x*LUT_SIZE/(2.0*M_PI), LUT_SIZE));
    if(idx < 0) idx += LUT_SIZE;
    return sin_lut[idx];
}

static double shader_sunset(int x, int y, double phase) {
    return sin(x * 0.15 + phase) + sin(y * 0.15 + phase * 0.5);
}

static double shader_matrix(int x, int y, double phase, double *intensity) {
    double p = phase + x * 0.1 + y * 0.1;
    double pulse = sin(phase * 3.0) * 0.15 + 0.85;
    double scanline = (fmod(y - phase * 5.0, 10.0) < 1.0) ? 0.7 : 1.0;
    *intensity = pulse * scanline * ((rand() % 100 > 98) ? 0.5 : 1.0);
    return p;
}

static double shader_pulse(int x, int y, int len, int count, double phase, double *intensity) {
    double dx = x - len / 2.0;
    double dy = y - count / 2.0;
    double dist = sqrt(dx * dx + dy * dy);
    double p = dist - phase;
    *intensity = (sin(dist * 0.5 - phase * 2.0) + 1.0) / 2.0;
    *intensity = *intensity * 0.8 + 0.2;
    return p;
}

static void apply_border_opacity(int x, int y, int len, int count, double op, int *r, int *g, int *b) {
    if (op <= 0.0) return;
    double cx = len / 2.0, cy = count / 2.0;
    double dx = x - cx, dy = y - cy;
    double factor = 1.0 - (sqrt(dx*dx + dy*dy) / sqrt(cx*cx + cy*cy));
    factor = 1.0 - (1.0 - factor) * op;
    *r = (int)(*r * factor);
    *g = (int)(*g * factor);
    *b = (int)(*b * factor);
    if(*r<0) *r=0; else if(*r>255) *r=255;
    if(*g<0) *g=0; else if(*g>255) *g=255;
    if(*b<0) *b=0; else if(*b>255) *b=255;
}

void get_color_fast(int x, int y, int mode, int len, int count, double phase, int *r, int *g, int *b) {
    double p, intensity = 1.0;
    switch(mode) {  
        case 1: p = shader_sunset(x, y, phase); break;
        case 2: p = phase; break;
        case 3: p = y*0.5+phase; break;
        case 4: p = y*0.8+fast_sin(x*0.3)*3.0+phase; break;
        case 5: { double dx=x-len/2.0, dy=y-count/2.0; p=sqrt(dx*dx+dy*dy)-phase; break; }  
        case 6: p = fast_sin(x*0.2)*fast_sin(y*0.2+PI_2)*10.0+phase; break;  
        case 7: { double center=len/2.0; p=phase+x*0.1; intensity=1.0-(fabs(x-center)/center)*0.8; if(intensity<0.2) intensity=0.2; break;}  
        case 8: { double center_y=count/2.0; p=phase+y*0.2; intensity=1.0-(fabs(y-center_y)/center_y)*0.7; if(intensity<0.3) intensity=0.3; break;}  
        case 9: p = phase+x*0.1; intensity=(x%2==0)?1.0:0.2; break;
        case 10: p = shader_matrix(x, y, phase, &intensity); break;  
        case 11: p = shader_pulse(x, y, len, count, phase, &intensity); break;
        default: p = phase+x*0.2+y*0.1+(diagonal*(x+y)); break;  
    }
    
    *r = (int)((fast_sin(freq*p+0)*127+128)*intensity);
    *g = (int)((fast_sin(freq*p+2.094)*127+128)*intensity);
    *b = (int)((fast_sin(freq*p+4.188)*127+128)*intensity);
    
    apply_border_opacity(x, y, len, count, opacity, r, g, b);
}
