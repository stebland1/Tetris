#include "game.h"
#include "tetromino.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

int main(void) {
  struct game g = {
      .state = ON,
      .board = {0},
      .score = 0,
      .score_at_last_increase = g.score,
      .speed = STARTING_SPEED,
      .active_tetromino = tetrominoes[0],
      .active_x = STARTING_X,
      .active_y = 0 - g.active_tetromino.height,
      .is_accelerating_down = 0,
      .next_tetromino_index = 1,
  };
  char cmd;

  struct timespec tm;
  tm.tv_sec = 0;
  tm.tv_nsec = 1000000;

  int speed_counter = 0;

  set_terminal_non_blocking();

  while (g.state == ON) {
    print_board(&g);

    nanosleep(&tm, NULL);
    speed_counter++;
    if (speed_counter >= g.speed) {
      move_tetromino(&g, 0, 1);
      speed_counter = 0;
    }

    handle_user_input(&g);
  }

  restore_terminal();
}
