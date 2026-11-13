#include <assert.h>
#include <string.h>
#include "../fetch_logo_color.h"

static void check_color(const char *sgr, int initial, int expected) {
  int color = initial;
  int consumed = logo_sgr_color(sgr, &color);
  assert(consumed == (int)strlen(sgr));
  assert(color == expected);
}

int main(void) {
  check_color("31m", 0, 31);
  check_color("1;93m", 0, 93);
  check_color("0;37m", 31, 37);
  check_color("22m", 31, 31);
  check_color("39m", 31, 39);
  check_color("m", 31, 0);
  check_color("0m", LOGO_COLOR_RGB | 0x9e6540, 0);
  check_color("38;2;158;101;64m", 0, LOGO_COLOR_RGB | 0x9e6540);
  check_color("38;2;0;0;0m", 0, LOGO_COLOR_RGB);
  check_color("1;38;2;255;255;255m", 0, LOGO_COLOR_RGB | 0xffffff);
  check_color("38;5;130m", 0, LOGO_COLOR_INDEXED | 130);
  check_color("38;5;0m", 31, LOGO_COLOR_INDEXED);
  check_color("38;2;31m", 37, 37);
  check_color("38;2;256;101;64m", 37, 37);
  check_color("38;5;256m", 37, 37);
  check_color("38;2;999999999999999999999;0;0m", 37, 37);
  check_color("48;2;31;32;33m", 37, 37);
  check_color("48;5;31;38;2;158;101;64;22m", 37, LOGO_COLOR_RGB | 0x9e6540);
  check_color("38;2;158;101;64;0m", 37, 0);
  check_color("38;2;158;101;64;36m", 37, 36);

  int color = 37;
  assert(logo_sgr_color("38;2;158;101;64", &color) == 0 && color == 37);
  assert(logo_sgr_color("2J", &color) == 0 && color == 37);
  char sequence[32];
  int length = logo_color_escape(LOGO_COLOR_RGB | 0x9e6540, sequence, sizeof(sequence));
  assert(length == (int)strlen(sequence));
  assert(strcmp(sequence, "\033[38;2;158;101;64m") == 0);
  logo_color_escape(LOGO_COLOR_INDEXED | 130, sequence, sizeof(sequence));
  assert(strcmp(sequence, "\033[38;5;130m") == 0);
  logo_color_escape(93, sequence, sizeof(sequence));
  assert(strcmp(sequence, "\033[1;93m") == 0);
  for (int value = 0; value <= 255; value++) {
    int rgb = LOGO_COLOR_RGB | (value << 16) | (value << 8) | value;
    logo_color_escape(rgb, sequence, sizeof(sequence));
    check_color(sequence + 2, 0, rgb);
    logo_color_escape(LOGO_COLOR_INDEXED | value, sequence, sizeof(sequence));
    check_color(sequence + 2, 0, LOGO_COLOR_INDEXED | value);
  }
  puts("Logo colour tests: passed");
  return 0;
}
