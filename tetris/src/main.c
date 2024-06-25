#include <commons/collections/list.h>
#include <commons/log.h>
#include <curses.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ROWS 20
#define COLUMNS 15
#define FRAME_RATE 5

typedef struct {
  int positions[4][2];
} player_t;

typedef enum { LEFT, RIGHT, DOWN } move_t;

t_log *logger;

int board[ROWS][COLUMNS];
player_t player;

pthread_mutex_t mutex_positions;

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
    board[row][col] = 2;
  }
}

void set_player_piece() {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLUMNS; j++) {
      if (board[i][j] == 2) {
        board[i][j] = 1;
      }
    }
  }
}

void get_piece(int buffer[4][2]) {
  int i_row = 0;
  int i_col = COLUMNS / 2;
  int r = rand() % 8;
  switch (r) {
  case 0: {
    buffer[0][0] = i_row;
    buffer[0][1] = i_col;

    buffer[1][0] = i_row + 1;
    buffer[1][1] = i_col;

    buffer[2][0] = i_row;
    buffer[2][1] = i_col + 1;

    buffer[3][0] = i_row + 1;
    buffer[3][1] = i_col + 1;
    break;
  }
  case 1: {
    buffer[0][0] = i_row;
    buffer[0][1] = i_col;

    buffer[1][0] = i_row + 1;
    buffer[1][1] = i_col;

    buffer[2][0] = i_row + 2;
    buffer[2][1] = i_col;

    buffer[3][0] = i_row + 3;
    buffer[3][1] = i_col;
    break;
  }
  case 2: {
    buffer[0][0] = i_row;
    buffer[0][1] = i_col;

    buffer[1][0] = i_row;
    buffer[1][1] = i_col + 1;

    buffer[2][0] = i_row + 1;
    buffer[2][1] = i_col;

    buffer[3][0] = i_row + 2;
    buffer[3][1] = i_col;
    break;
  }
  case 3: {
    buffer[0][0] = i_row;
    buffer[0][1] = i_col;

    buffer[1][0] = i_row;
    buffer[1][1] = i_col - 1;

    buffer[2][0] = i_row + 1;
    buffer[2][1] = i_col;

    buffer[3][0] = i_row + 2;
    buffer[3][1] = i_col;
    break;
  }
  case 4: {
    buffer[0][0] = i_row;
    buffer[0][1] = i_col;

    buffer[1][0] = i_row + 1;
    buffer[1][1] = i_col;

    buffer[2][0] = i_row + 1;
    buffer[2][1] = i_col + 1;

    buffer[3][0] = i_row + 2;
    buffer[3][1] = i_col + 1;
    break;
  }
  case 5: {
    buffer[0][0] = i_row;
    buffer[0][1] = i_col;

    buffer[1][0] = i_row + 1;
    buffer[1][1] = i_col;

    buffer[2][0] = i_row + 1;
    buffer[2][1] = i_col - 1;

    buffer[3][0] = i_row + 2;
    buffer[3][1] = i_col - 1;
    break;
  }
  case 6: {
    buffer[0][0] = i_row;
    buffer[0][1] = i_col;

    buffer[1][0] = i_row + 1;
    buffer[1][1] = i_col;

    buffer[2][0] = i_row + 1;
    buffer[2][1] = i_col - 1;

    buffer[3][0] = i_row + 2;
    buffer[3][1] = i_col;
    break;
  }
  }
}

void player_intialize() { get_piece(player.positions); }

void player_rotate(move_t m) {
  double PI = 3.14159265358979323846;
  pthread_mutex_lock(&mutex_positions);
  switch (m) {
  case RIGHT: {
    for (int i = 0; i < 4; i++) {
      int row = player.positions[i][0];
      int col = player.positions[i][1];
      int row_origin = player.positions[2][0];
      int col_origin = player.positions[2][1];
      int row_0 = row - row_origin;
      int col_0 = col - col_origin;
      double row_rot1 = col_0 * cos(PI);
      double row_rot2 = row_0 * sin(PI);
      double col_rot1 = col_0 * sin(PI);
      double col_rot2 = row_0 * cos(PI);
      int new_row = floor(row_rot1 - row_rot2) + row_origin;
      int new_col = ceil(col_rot1 + col_rot2) + col_origin;
      if (new_row != row || new_col != col) {
        board[row][col] = 0;
      }
      player.positions[i][0] = new_row;
      player.positions[i][1] = new_col;
    }
    break;
  }
  case LEFT: {
    break;
  }
  default:
    break;
  }
  pthread_mutex_unlock(&mutex_positions);
}

int player_move(move_t m) {
  pthread_mutex_lock(&mutex_positions);
  switch (m) {
  case LEFT: {
    int min_col = COLUMNS;
    for (int i = 0; i < 4; i++) {
      if (player.positions[i][1] < min_col)
        min_col = player.positions[i][1];
    }
    if (min_col > 0) {
      for (int i = 0; i < 4; i++) {
        int row = player.positions[i][0];
        int col = player.positions[i][1];
        if (board[row][col - 1] == 1) {
          pthread_mutex_unlock(&mutex_positions);
          return 1;
        }
      }
      for (int i = 0; i < 4; i++) {
        int prev_row = player.positions[i][0];
        int prev_col = player.positions[i][1];
        player.positions[i][1]--;
        board[prev_row][prev_col] = 0;
      }
      pthread_mutex_unlock(&mutex_positions);
      return 0;
    }
    break;
  }

  case RIGHT: {
    int max_col = 0;
    for (int i = 0; i < 4; i++) {
      if (player.positions[i][1] > max_col)
        max_col = player.positions[i][1];
    }
    if (max_col < COLUMNS - 1) {
      for (int i = 0; i < 4; i++) {
        int row = player.positions[i][0];
        int col = player.positions[i][1];
        if (board[row][col + 1] == 1) {
          pthread_mutex_unlock(&mutex_positions);
          return 1;
        }
      }
      for (int i = 0; i < 4; i++) {
        int prev_row = player.positions[i][0];
        int prev_col = player.positions[i][1];
        player.positions[i][1]++;
        board[prev_row][prev_col] = 0;
      }
      pthread_mutex_unlock(&mutex_positions);
      return 0;
    }
    break;
  }

  case DOWN: {
    int max_row = 0;
    for (int i = 0; i < 4; i++) {
      if (player.positions[i][0] > max_row)
        max_row = player.positions[i][0];
    }
    if (max_row < ROWS - 1) {
      for (int i = 0; i < 4; i++) {
        int row = player.positions[i][0];
        int col = player.positions[i][1];
        if (board[row + 1][col] == 1) {
          pthread_mutex_unlock(&mutex_positions);
          return 1;
        }
      }
      for (int i = 0; i < 4; i++) {
        int prev_row = player.positions[i][0];
        int prev_col = player.positions[i][1];
        player.positions[i][0]++;
        board[prev_row][prev_col] = 0;
      }
      pthread_mutex_unlock(&mutex_positions);
      return 0;
    }
    break;
  }
  }
  pthread_mutex_unlock(&mutex_positions);
  return 1;
}

void *user_input() {
  while (1) {
    char c = '\0';
    scanf("%c", &c);
    if (c == 'h' || c == 'H')
      player_move(LEFT);
    if (c == 'j' || c == 'J')
      player_move(DOWN);
    if (c == 'k' || c == 'K')
      player_move(RIGHT);
    if (c == 'x' || c == 'X')
      player_rotate(LEFT);
    if (c == 'c' || c == 'C')
      player_rotate(RIGHT);
  }
}

int main(int argc, char *argv[]) {
  logger = log_create("tetris.log", "TETRIS", 1, LOG_LEVEL_DEBUG);
  pthread_mutex_init(&mutex_positions, NULL);
  srand(time(NULL));
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
    if (colided) {
      set_player_piece();
      player_intialize();
    }

    update_board();
    print_board();
    refresh();
    usleep(1000000 / FRAME_RATE);
  }

  pthread_mutex_destroy(&mutex_positions);

  return EXIT_SUCCESS;
}
