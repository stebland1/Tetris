#ifndef TETROMINO_H
#define TETROMINO_H

#define TETROMINO_SIZE 4
#define TOTAL_TETROMINOS 7

struct tetromino {
  int id;
  char symbol;
  int template[TETROMINO_SIZE][TETROMINO_SIZE];
  int can_rotate;
  int height;
  int width;
};

extern struct tetromino tetrominoes[];

#endif
