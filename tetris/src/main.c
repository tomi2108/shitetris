#include <commons/log.h>
#include <curses.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ROWS 20
#define COLUMNS 15
#define FRAME_RATE 2

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

void player_move(move_t m) {
  switch (m) {
  case LEFT: {
    int min_col = COLUMNS;
    for (int i = 0; i < 4; i++) {
      if (player.positions[i][1] < min_col) {
        min_col = player.positions[i][1];
      }
    }
    if (min_col > 0) {
      for (int i = 0; i < 4; i++) {
        int prev_row = player.positions[i][0];
        int prev_col = player.positions[i][1];
        player.positions[i][1]--;
        board[prev_row][prev_col] = 0;
      }
    }
    break;
  }

  case RIGHT: {
    int max_col = 0;
    for (int i = 0; i < 4; i++) {
      if (player.positions[i][1] > max_col) {
        max_col = player.positions[i][1];
      }
    }
    if (max_col < COLUMNS - 1) {
      for (int i = 0; i < 4; i++) {
        int prev_row = player.positions[i][0];
        int prev_col = player.positions[i][1];
        player.positions[i][1]++;
        board[prev_row][prev_col] = 0;
      }
    }
    break;
  }

  case DOWN: {
    int max_row = 0;
    for (int i = 0; i < 4; i++) {
      if (player.positions[i][0] > max_row) {
        max_row = player.positions[i][0];
      }
    }
    if (max_row < ROWS - 1) {
      for (int i = 0; i < 4; i++) {
        int prev_row = player.positions[i][0];
        int prev_col = player.positions[i][1];
        player.positions[i][0]++;
        board[prev_row][prev_col] = 0;
      }
    }
    break;
  }
  }
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
    player_move(DOWN);

    update_board();
    print_board();
    refresh();
    usleep(1000000 / FRAME_RATE);
  }

  return EXIT_SUCCESS;
}
