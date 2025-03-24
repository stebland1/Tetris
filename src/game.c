#include "game.h"
#include "tetromino.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

void spawn_new_tetromino(struct game *g) {
  struct tetromino next_active_tetromino =
      tetrominoes[(g->active_tetromino.id) % TOTAL_TETROMINOS];
  next_active_tetromino.id = g->active_tetromino.id + 1;

  g->active_tetromino = next_active_tetromino;

  g->active_x = STARTING_X;
  g->active_y = 0 - g->active_tetromino.height;

  place_active_tetromino(g);
}

int update_score(struct game *g, int lines_cleared) {
  int score_increase = 0;

  switch (lines_cleared) {
  case 1:
    score_increase = 100;
    break;
  case 2:
    score_increase = 300;
    break;
  case 3:
    score_increase = 500;
    break;
  case 4:
    score_increase = 800;
    break;
  default:
    score_increase = 10;
    break;
  }

  g->score += score_increase;
  return score_increase;
}

void increase_speed(struct game *g, int score_increase) {
  if (g->speed == 2) {
    return;
  }

  g->score_at_last_increase += score_increase;

  if (g->score_at_last_increase >= 200) {
    g->score_at_last_increase = 0;
    g->speed -= 2;
  }
}

int check_row(struct game *g) {
  int lines_cleared = 0;

  for (int r = g->active_y + TETROMINO_SIZE; r >= g->active_y; r--) {
    if (r >= BOARD_HEIGHT) {
      continue;
    }

    int del_line = 1;

    for (int c = 0; c < BOARD_WIDTH; c++) {
      if (g->board[r][c] == 0) {
        del_line = 0;
      }
    }

    if (del_line) {
      lines_cleared++;

      for (int rr = r; rr > 0; rr--) {
        int is_empty_line = 1;

        for (int c = 0; c < BOARD_WIDTH; c++) {
          g->board[rr][c] = g->board[rr - 1][c];

          if (g->board[rr][c] != 0) {
            is_empty_line = 0;
          }
        }

        if (is_empty_line) {
          break;
        }

        r++;
      }
    }
  }

  return lines_cleared;
}

enum collision_result is_colliding(struct game *g, struct tetromino *tet,
                                   int dx, int dy) {

  int out_of_bounds_flag = 0;
  for (int i = 0; i < TETROMINO_SIZE; i++) {
    for (int j = 0; j < TETROMINO_SIZE; j++) {
      if (tet->template[i][j] != 0 && is_out_of_bounds(dx + j, dy + i)) {
        out_of_bounds_flag = 1;
      }

      if (tet->template[i][j] == 0 ||
          (dy + i < 0 && dx + j >= 0 && dx + j < BOARD_WIDTH)) {
        continue;
      }

      if (dy + i == BOARD_HEIGHT) {
        return COLLISION_BOTTOM;
      }

      if (dx + j >= BOARD_WIDTH) {
        return COLLISION_RIGHT;
      }

      if (dx + j < 0) {
        return COLLISION_LEFT;
      }

      if (g->board[dy + i][dx + j] != 0 &&
          g->board[dy + i][dx + j] != g->active_tetromino.id) {
        return out_of_bounds_flag ? COLLISION_GAME_OVER : COLLISION_NEIGHBOR;
      }
    }
  }

  return COLLISION_NONE;
}

void move_tetromino(struct game *g, int dx, int dy) {
  int new_x = g->active_x + dx;
  int new_y = g->active_y + dy;

  int collision_type = is_colliding(g, &g->active_tetromino, new_x, new_y);
  switch (collision_type) {
  case COLLISION_BOTTOM:
  case COLLISION_NEIGHBOR: {
    int lines_cleared = check_row(g);
    int score_increase = update_score(g, lines_cleared);
    increase_speed(g, score_increase);
    spawn_new_tetromino(g);
    return;
  }
  case COLLISION_LEFT:
  case COLLISION_RIGHT:
    return;
  case COLLISION_GAME_OVER:
    g->state = OVER;
    return;
  default:
    break;
  }

  clear_active_tetromino(g);

  g->active_x = new_x;
  g->active_y = new_y;

  place_active_tetromino(g);
}

void rotate_tetromino(struct game *g) {
  struct tetromino rotated = g->active_tetromino;

  for (int i = 0; i < TETROMINO_SIZE; i++) {
    for (int j = 0; j < TETROMINO_SIZE; j++) {
      rotated.template[i][j] =
          g->active_tetromino.template[TETROMINO_SIZE - 1 - j][i];
    }
  }

  if (is_colliding(g, &rotated, g->active_x, g->active_y)) {
    return;
  }

  clear_active_tetromino(g);
  g->active_tetromino = rotated;
  place_active_tetromino(g);
}

void clear_active_tetromino(struct game *g) {
  for (int i = 0; i < TETROMINO_SIZE; i++) {
    for (int j = 0; j < TETROMINO_SIZE; j++) {
      if (!is_out_of_bounds(g->active_x + j, g->active_y + i) &&
          g->active_tetromino.template[i][j] != 0) {
        g->board[g->active_y + i][g->active_x + j] = 0;
      }
    }
  }
}

void place_active_tetromino(struct game *g) {
  for (int i = 0; i < TETROMINO_SIZE; i++) {
    for (int j = 0; j < TETROMINO_SIZE; j++) {
      if (!is_out_of_bounds(g->active_x + j, g->active_y + i) &&
          g->active_tetromino.template[i][j] != 0) {
        g->board[g->active_y + i][g->active_x + j] = g->active_tetromino.id;
      }
    }
  }
}

void print_next_tetromino(struct game *g, int cur_row) {
  if (cur_row > 5 || cur_row == 1) {
    return;
  }

  printf("  ");

  if (cur_row == 0) {
    printf("Next piece:");
    return;
  }

  struct tetromino next_tetromino = tetrominoes[g->next_tetromino_index];

  for (int c = 0; c < TETROMINO_SIZE; c++) {
    if (next_tetromino.template[cur_row - 2][c] == 0) {
      printf(" ");
    } else {
      printf("%c", next_tetromino.symbol);
    }
  }
}

void print_board(struct game *g) {
  clear_screen();
  move_cursor_to_top();
  int has_printed_next_tetro = 0;

  for (int i = 0; i < BOARD_PADDING; i++) {
    printf("\n");
  }

  for (int r = 0; r < BOARD_HEIGHT; r++) {
    printf("	║");
    for (int c = 0; c < BOARD_WIDTH; c++) {
      if (g->board[r][c] == EMPTY_CELL) {
        printf(" ");
      } else {
        printf("%c",
               tetrominoes[(g->board[r][c] - 1) % TOTAL_TETROMINOS].symbol);
      }
    }

    printf("║");
    print_next_tetromino(g, r);
    printf("\n");
  }

  printf("	");
  for (int i = 0; i < BOARD_WIDTH + BOARD_BORDER_SIZE * 2; i++) {
    printf("~");
  }

  printf("\n");
  printf("	[Score: %d]\n", g->score);
  printf("	[Speed: %d]\n", g->speed);

  for (int i = 0; i < BOARD_PADDING; i++) {
    printf("\n");
  }
}

void handle_user_input(struct game *g) {
  int cmd = get_input_select();
  if (cmd != -1) {
    switch (cmd) {
    case LEFT:
      move_tetromino(g, -1, 0);
      break;
    case RIGHT:
      move_tetromino(g, 1, 0);
      break;
    case DOWN:
      if (!g->is_accelerating_down) {
        g->is_accelerating_down = 1;
      }
      move_tetromino(g, 0, 1);
      break;
    case ROTATE:
      if (g->active_tetromino.can_rotate) {
        rotate_tetromino(g);
      }
      break;
    case QUIT:
      exit(1);
    }
  } else {
    g->is_accelerating_down = 0;
  }
}
