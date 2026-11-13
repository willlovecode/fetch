#ifndef FETCH_LOGO_COLOR_H
#define FETCH_LOGO_COLOR_H

#include <limits.h>
#include <stdio.h>

#define LOGO_COLOR_RGB 0x1000000
#define LOGO_COLOR_INDEXED 0x2000000

static int logo_sgr_color(const char *text, int *color) {
  int params[32], count = 0, value = 0, valid = 1;
  const char *p = text;
  while ((*p >= '0' && *p <= '9') || *p == ';') {
    if (*p == ';') {
      if (count < 32) params[count++] = value;
      else valid = 0;
      value = 0;
    } else if (value <= (INT_MAX - (*p - '0')) / 10) {
      value = value * 10 + (*p - '0');
    } else {
      valid = 0;
    }
    p++;
  }
  if (count < 32) params[count++] = value;
  else valid = 0;
  if (*p != 'm') return 0;
  if (valid) {
    for (int i = 0; i < count; i++) {
      int code = params[i];
      if (code == 38 || code == 48) {
        if (i + 1 >= count) break;
        int mode = params[++i];
        int channels = mode == 2 ? 3 : mode == 5 ? 1 : 0;
        if (!channels || i + channels >= count) break;
        int in_range = 1;
        for (int k = 1; k <= channels; k++)
          if (params[i + k] > 255) in_range = 0;
        if (code == 38 && in_range) {
          if (mode == 2)
            *color = LOGO_COLOR_RGB | (params[i + 1] << 16) |
                     (params[i + 2] << 8) | params[i + 3];
          else
            *color = LOGO_COLOR_INDEXED | params[i + 1];
        }
        i += channels;
      } else if (code == 0) {
        *color = 0;
      } else if ((code >= 30 && code <= 37) || code == 39 ||
                 (code >= 90 && code <= 97)) {
        *color = code;
      }
    }
  }
  return (int)(p - text) + 1;
}

static int logo_color_escape(int color, char *out, size_t capacity) {
  if (color & LOGO_COLOR_INDEXED)
    return snprintf(out, capacity, "\033[38;5;%dm", color & 255);
  if (color & LOGO_COLOR_RGB)
    return snprintf(out, capacity, "\033[38;2;%d;%d;%dm",
                    (color >> 16) & 255, (color >> 8) & 255, color & 255);
  return snprintf(out, capacity, "\033[1;%dm", color);
}

#endif
