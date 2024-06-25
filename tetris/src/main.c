#include <commons/collections/list.h>
#include <commons/log.h>
#include <curses.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ROWS 20
#define COLUMNS 15
#define FRAME_RATE 10

typedef struct {
  int positions[4][2];
} player_t;

typedef enum { LEFT, RIGHT, DOWN } move_t;

t_log *logger;
int board[ROWS][COLUMNS];
player_t player;

void print_board() {
  for (int j = 0; j < COLUMNS + 1; j++) {
    printw("__");
  }
  printw("\n");
  for (int i = 0; i < ROWS; i++) {
    printw("|");
    for (int j = 0; j < COLUMNS; j++) {
      if (board[i][j] == 0) {
        printw("  ");
      } else {
        printw("[]");
      }
    }
    printw("|");
    printw("\n");
  }
  for (int j = 0; j < COLUMNS + 1; j++) {
    printw("__");
  }
  printw("\n");
}
void clear_screen() {
  clear();
  refresh();
}

void reset_board() {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLUMNS; j++) {
      board[i][j] = 0;
    }
  }
}

void update_board() {
  for (int i = 0; i < 4; i++) {
    int row = player.positions[i][0];
    int col = player.positions[i][1];
    board[row][col] = 1;
  }
}

void get_piece(int buffer[4][2]) {
  buffer[0][0] = 0;
  buffer[0][1] = 10;

  buffer[1][0] = 0;
  buffer[1][1] = 11;

  buffer[2][0] = 1;
  buffer[2][1] = 10;

  buffer[3][0] = 1;
  buffer[3][1] = 11;
}

void player_intialize() { get_piece(player.positions); }

int player_move(move_t m) {
  switch (m) {
  case LEFT: {
    t_list *min_cols = list_create();
    t_list *min_cols_rows = list_create();
    int min_col = COLUMNS;
    for (int i = 0; i < 4; i++) {
      if (player.positions[i][1] < min_col) {
        if (list_size(min_cols) > 0) {
          list_clean(min_cols);
          list_clean(min_cols_rows);
        }
        min_col = player.positions[i][1];
      }
      if (player.positions[i][1] > min_col)
        continue;
      list_add(min_cols, &(player.positions[i][1]));
      list_add(min_cols_rows, &(player.positions[i][0]));
    }
    if (min_col > 0) {
      for (int i = 0; i < list_size(min_cols); i++) {
        int *col = list_get(min_cols, i);
        int *row = list_get(min_cols_rows, i);
        if (board[*row][*col - 1] == 1) {
          list_destroy(min_cols);
          list_destroy(min_cols_rows);
          return 1;
        }
      }
      list_destroy(min_cols);
      list_destroy(min_cols_rows);
      for (int i = 0; i < 4; i++) {
        int prev_row = player.positions[i][0];
        int prev_col = player.positions[i][1];
        player.positions[i][1]--;
        board[prev_row][prev_col] = 0;
      }
      return 0;
    }
    break;
  }

  case RIGHT: {
    t_list *max_cols = list_create();
    t_list *max_cols_rows = list_create();
    int max_col = 0;
    for (int i = 0; i < 4; i++) {
      if (player.positions[i][1] > max_col) {
        if (list_size(max_cols) > 0) {
          list_clean(max_cols);
          list_clean(max_cols_rows);
        }
        max_col = player.positions[i][1];
      }
      if (player.positions[i][1] < max_col)
        continue;
      list_add(max_cols, &(player.positions[i][1]));
      list_add(max_cols_rows, &(player.positions[i][0]));
    }
    if (max_col < COLUMNS - 1) {
      for (int i = 0; i < list_size(max_cols); i++) {
        int *col = list_get(max_cols, i);
        int *row = list_get(max_cols_rows, i);
        if (board[*row][*col + 1] == 1) {
          list_destroy(max_cols);
          list_destroy(max_cols_rows);
          return 1;
        }
      }
      list_destroy(max_cols);
      list_destroy(max_cols_rows);
      for (int i = 0; i < 4; i++) {
        int prev_row = player.positions[i][0];
        int prev_col = player.positions[i][1];
        player.positions[i][1]++;
        board[prev_row][prev_col] = 0;
      }
      return 0;
    }
    break;
  }

  case DOWN: {
    t_list *max_rows = list_create();
    t_list *max_rows_cols = list_create();
    int max_row = 0;
    for (int i = 0; i < 4; i++) {
      if (player.positions[i][0] > max_row) {
        if (list_size(max_rows) > 0) {
          list_clean(max_rows);
          list_clean(max_rows_cols);
        }
        max_row = player.positions[i][0];
      }
      if (player.positions[i][0] < max_row)
        continue;
      list_add(max_rows, &(player.positions[i][0]));
      list_add(max_rows_cols, &(player.positions[i][1]));
    }
    if (max_row < ROWS - 1) {
      for (int i = 0; i < list_size(max_rows); i++) {
        int *row = list_get(max_rows, i);
        int *col = list_get(max_rows_cols, i);
        if (board[*row + 1][*col] == 1) {
          list_destroy(max_rows);
          list_destroy(max_rows_cols);
          return 1;
        }
      }
      list_destroy(max_rows);
      list_destroy(max_rows_cols);
      for (int i = 0; i < 4; i++) {
        int prev_row = player.positions[i][0];
        int prev_col = player.positions[i][1];
        player.positions[i][0]++;
        board[prev_row][prev_col] = 0;
      }
      return 0;
    }
    break;
  }
  }
  return 1;
}

void *user_input() {
  while (1) {
    char c = '\0';
    scanf("%c", &c);
    if (c == 'j' || c == 'J')
      player_move(LEFT);
    if (c == 'k' || c == 'K')
      player_move(RIGHT);
  }
}

int main(int argc, char *argv[]) {
  logger = log_create("tetris.log", "TETRIS", 1, LOG_LEVEL_DEBUG);
  reset_board();
  initscr();
  clear();
  noecho();
  cbreak();

  player_intialize();
  pthread_t input_thread;
  pthread_create(&input_thread, NULL, &user_input, NULL);
  pthread_detach(input_thread);

  while (1) {
    clear_screen();
    int colided = player_move(DOWN);
    if (colided)
      player_intialize();

    update_board();
    print_board();
    refresh();
    usleep(1000000 / FRAME_RATE);
  }

  return EXIT_SUCCESS;
}
