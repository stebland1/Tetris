#ifndef GAME_H
#define GAME_H

#include "tetromino.h"

#define BOARD_HEIGHT 16
#define BOARD_WIDTH 10
#define BOARD_BORDER_SIZE 1
#define BOARD_PADDING 2
#define EMPTY_CELL 0
#define STARTING_X BOARD_WIDTH - TETROMINO_SIZE - BOARD_BORDER_SIZE
#define STARTING_SPEED 20

#define GAME_TICK_INTERVAL 400000

#define LEFT 'h'
#define RIGHT 'l'
#define DOWN 'j'
#define QUIT 'q'
#define ROTATE ' '

enum state {
  ON,
  OVER,
};

struct game {
  enum state state;
  int board[BOARD_HEIGHT][BOARD_WIDTH];
  struct tetromino active_tetromino;
  int speed;
  int score_at_last_increase;
  int score;
  int active_x;
  int active_y;
  int is_accelerating_down;
  int next_tetromino_index;
};

enum collision_result {
  COLLISION_NONE,
  COLLISION_BOTTOM,
  COLLISION_NEIGHBOR,
  COLLISION_LEFT,
  COLLISION_RIGHT,
  COLLISION_GAME_OVER,
};

void print_board(struct game *g);
void print_next_tetromino(struct game *g, int cur_row);
void place_active_tetromino(struct game *g);
void clear_active_tetromino(struct game *g);
void move_tetromino(struct game *g, int dx, int dy);
enum collision_result is_colliding(struct game *g, struct tetromino *tet,
                                   int dx, int dy);
void spawn_new_tetromino(struct game *g);
int check_row(struct game *g);
void rotate_tetromino(struct game *g);
void handle_user_input(struct game *g);
int update_score(struct game *g, int lines_cleared);
void increase_speed(struct game *g, int score_increase);
int is_game_over(struct game *g);

#endif
