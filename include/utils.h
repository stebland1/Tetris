#ifndef UTILS_H
#define UTILS_H

void clear_screen(void);
void set_terminal_non_blocking(void);
void restore_terminal(void);
void move_cursor_to_top(void);
int get_input_select(void);
int is_out_of_bounds(int dx, int dy);

#endif
