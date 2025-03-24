#include "utils.h"
#include "game.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/_select.h>
#include <sys/_types/_fd_def.h>
#include <sys/termios.h>
#include <termios.h>
#include <unistd.h>

void clear_screen() { printf("\033[H\033[J"); }

void move_cursor_to_top() { printf("\033[H"); }

void set_terminal_non_blocking() {
  struct termios newtio;
  tcgetattr(STDIN_FILENO, &newtio);
  newtio.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newtio);
  fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

void restore_terminal() {
  struct termios oldtio;
  tcgetattr(STDIN_FILENO, &oldtio);
  oldtio.c_lflag |= (ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &oldtio);
}

int get_input_select() {
  struct timeval timeout;
  fd_set readfds;
  int ret;

  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  timeout.tv_sec = 0;
  timeout.tv_usec = 100000;

  ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);
  if (ret > 0) {
    char ch;
    read(STDIN_FILENO, &ch, 1);
    return ch;
  }

  return -1;
}

int is_out_of_bounds(int dx, int dy) {
  if (dx >= BOARD_WIDTH || dx < 0) {
    return 1;
  }

  if (dy < 0 || dy >= BOARD_HEIGHT) {
    return 1;
  }

  return 0;
};
