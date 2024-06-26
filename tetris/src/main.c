#include <commons/log.h>
#include <curses.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ROWS 20
#define COLUMNS 16
#define FRAME_RATE 60
#define BOUNDING_BOX_CENTER 10

typedef enum { LEFT, RIGHT, DOWN } move_t;
typedef enum { NORTH, EAST, SOUTH, WEST } rotation_t;

typedef struct {
  int positions[2];
  int bounding_box[16];
  rotation_t rotation;
  int piece;
} player_t;

t_log *logger;

int board[ROWS][COLUMNS];
player_t player;

pthread_mutex_t mutex_positions;

int get_block_row_col(int block_index, int *row, int *col) {
  if (player.bounding_box[block_index] == 0)
    return 0;

  int bb_col = block_index % 4;
  int bb_row = block_index / 4;
  int bb_center_col = BOUNDING_BOX_CENTER % 4;
  int bb_center_row = BOUNDING_BOX_CENTER / 4;
  int offset_col = bb_center_col - bb_col;
  int offset_row = bb_center_row - bb_row;
  int block_col = player.positions[1] - offset_col;
  int block_row = player.positions[0] - offset_row;
  *row = block_row;
  *col = block_col;
  return 1;
}

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
      } else if (board[i][j] == 1) {
        printw("[]");
      } else if (board[i][j] == 2) {
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
  for (int i = 0; i < 16; i++) {
    int block_col = 0;
    int block_row = 0;
    int res = get_block_row_col(i, &block_row, &block_col);
    if (res == 1) {
      board[block_row][block_col] = 2;
    }
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

void get_piece(int *r) {
  int random = rand() % 7;
  if (r == NULL) {
    r = &random;
    player.piece = random;
  }
  switch (*r) {
  case 0: {
    player.bounding_box[1] = 1;
    player.bounding_box[2] = 1;
    player.bounding_box[5] = 1;
    player.bounding_box[6] = 1;
    break;
  }
  case 1: {
    switch (player.rotation) {
    case NORTH: {
      player.bounding_box[4] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[6] = 1;
      player.bounding_box[7] = 1;
      break;
    }
    case EAST: {
      player.bounding_box[2] = 1;
      player.bounding_box[6] = 1;
      player.bounding_box[10] = 1;
      player.bounding_box[14] = 1;
      break;
    }
    case SOUTH: {
      player.bounding_box[8] = 1;
      player.bounding_box[9] = 1;
      player.bounding_box[10] = 1;
      player.bounding_box[11] = 1;
      break;
    }
    case WEST: {
      player.bounding_box[1] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[9] = 1;
      player.bounding_box[13] = 1;
      break;
    }
    }
    break;
  }
  case 2: {
    switch (player.rotation) {
    case NORTH: {
      player.bounding_box[0] = 1;
      player.bounding_box[4] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[6] = 1;
      break;
    }
    case EAST: {
      player.bounding_box[1] = 1;
      player.bounding_box[2] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[9] = 1;
      break;
    }
    case SOUTH: {
      player.bounding_box[10] = 1;
      player.bounding_box[4] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[6] = 1;
      break;
    }
    case WEST: {
      player.bounding_box[1] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[9] = 1;
      player.bounding_box[8] = 1;
      break;
    }
    }
    break;
  }
  case 3: {
    switch (player.rotation) {
    case NORTH: {
      player.bounding_box[2] = 1;
      player.bounding_box[4] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[6] = 1;
      break;
    }
    case EAST: {
      player.bounding_box[10] = 1;
      player.bounding_box[1] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[9] = 1;
      break;
    }
    case SOUTH: {
      player.bounding_box[8] = 1;
      player.bounding_box[4] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[6] = 1;
      break;
    }
    case WEST: {
      player.bounding_box[0] = 1;
      player.bounding_box[1] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[9] = 1;
      break;
    }
    }
    break;
  }
  case 4: {
    switch (player.rotation) {
    case NORTH: {
      player.bounding_box[1] = 1;
      player.bounding_box[2] = 1;
      player.bounding_box[4] = 1;
      player.bounding_box[5] = 1;
      break;
    }
    case EAST: {
      player.bounding_box[1] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[6] = 1;
      player.bounding_box[10] = 1;
      break;
    }
    case SOUTH: {
      player.bounding_box[1] = 1;
      player.bounding_box[2] = 1;
      player.bounding_box[4] = 1;
      player.bounding_box[5] = 1;
      break;
    }
    case WEST: {
      player.bounding_box[1] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[6] = 1;
      player.bounding_box[10] = 1;
      break;
    }
    }
    break;
  }
  case 5: {
    switch (player.rotation) {
    case NORTH: {
      player.bounding_box[0] = 1;
      player.bounding_box[1] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[6] = 1;
      break;
    }
    case EAST: {
      player.bounding_box[1] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[4] = 1;
      player.bounding_box[8] = 1;
      break;
    }
    case SOUTH: {
      player.bounding_box[0] = 1;
      player.bounding_box[1] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[6] = 1;
      break;
    }
    case WEST: {
      player.bounding_box[1] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[4] = 1;
      player.bounding_box[8] = 1;
      break;
    }
    }
    break;
  }
  case 6: {
    switch (player.rotation) {
    case NORTH: {
      player.bounding_box[1] = 1;
      player.bounding_box[4] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[6] = 1;
      break;
    }
    case EAST: {
      player.bounding_box[1] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[6] = 1;
      player.bounding_box[9] = 1;
      break;
    }
    case SOUTH: {
      player.bounding_box[9] = 1;
      player.bounding_box[4] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[6] = 1;
      break;
    }
    case WEST: {
      player.bounding_box[1] = 1;
      player.bounding_box[5] = 1;
      player.bounding_box[4] = 1;
      player.bounding_box[9] = 1;
      break;
    }
    }
    break;
  }
  }
}

void reset_bounding_box() {
  for (int i = 0; i < 16; i++) {
    int block_col = 0;
    int block_row = 0;
    int res = get_block_row_col(i, &block_row, &block_col);
    if (res == 1)
      board[block_row][block_col] = 0;
    player.bounding_box[i] = 0;
  }
}

void player_intialize() {
  player.rotation = NORTH;
  player.positions[0] = 2;
  player.positions[1] = COLUMNS / 2;
  reset_bounding_box();
  get_piece(NULL);
}

void player_rotate(move_t m) {
  reset_bounding_box();
  switch (m) {
  case RIGHT: {
    switch (player.rotation) {
    case NORTH: {
      player.rotation = EAST;
      break;
    }
    case EAST: {
      player.rotation = SOUTH;
      break;
    }
    case SOUTH: {
      player.rotation = WEST;
      break;
    }
    case WEST: {
      player.rotation = NORTH;
      break;
    }
    }
    break;
  }
  case LEFT: {
    switch (player.rotation) {
    case NORTH: {
      player.rotation = WEST;
      break;
    }
    case EAST: {
      player.rotation = NORTH;
      break;
    }
    case SOUTH: {
      player.rotation = EAST;
      break;
    }
    case WEST: {
      player.rotation = SOUTH;
      break;
    }
    }
    break;
  }
  default:
    break;
  }
  get_piece(&player.piece);
  pthread_mutex_unlock(&mutex_positions);
}

int player_move(move_t m) {
  pthread_mutex_lock(&mutex_positions);
  switch (m) {
  case LEFT: {
    int min_col = COLUMNS;
    for (int i = 0; i < 16; i++) {
      int block_col = 0;
      int block_row = 0;
      int res = get_block_row_col(i, &block_row, &block_col);
      if (res == 0)
        continue;

      if (block_col < min_col)
        min_col = block_col;
    }
    if (min_col > 0) {
      for (int i = 0; i < 16; i++) {
        int block_col = 0;
        int block_row = 0;
        int res = get_block_row_col(i, &block_row, &block_col);
        if (res == 0)
          continue;
        if (board[block_row][block_col - 1] == 1) {
          pthread_mutex_unlock(&mutex_positions);
          return 1;
        }
      }
      for (int i = 0; i < 16; i++) {
        int prev_row = 0;
        int prev_col = 0;
        int res = get_block_row_col(i, &prev_row, &prev_col);
        if (res == 0)
          continue;
        board[prev_row][prev_col] = 0;
      }
      player.positions[1]--;
      pthread_mutex_unlock(&mutex_positions);
      return 0;
    }
    break;
  }

  case RIGHT: {
    int max_col = 0;
    for (int i = 0; i < 16; i++) {
      int block_col = 0;
      int block_row = 0;
      int res = get_block_row_col(i, &block_row, &block_col);
      if (res == 0)
        continue;
      if (block_col > max_col)
        max_col = block_col;
    }
    if (max_col < COLUMNS - 1) {
      for (int i = 0; i < 16; i++) {
        int block_col = 0;
        int block_row = 0;
        int res = get_block_row_col(i, &block_row, &block_col);
        if (res == 0)
          continue;

        if (board[block_row][block_col + 1] == 1) {
          pthread_mutex_unlock(&mutex_positions);
          return 1;
        }
      }
      for (int i = 0; i < 16; i++) {
        int block_col = 0;
        int block_row = 0;
        int res = get_block_row_col(i, &block_row, &block_col);
        if (res == 0)
          continue;
        board[block_row][block_col] = 0;
      }
      player.positions[1]++;
      pthread_mutex_unlock(&mutex_positions);
      return 0;
    }
    break;
  }

  case DOWN: {
    int max_row = 0;
    for (int i = 0; i < 16; i++) {
      int block_col = 0;
      int block_row = 0;
      int res = get_block_row_col(i, &block_row, &block_col);
      if (res == 0)
        continue;

      if (block_row > max_row)
        max_row = block_row;
    }
    if (max_row < ROWS - 1) {
      for (int i = 0; i < 16; i++) {
        int block_col = 0;
        int block_row = 0;
        int res = get_block_row_col(i, &block_row, &block_col);
        if (res == 0)
          continue;
        if (board[block_row + 1][block_col] == 1) {
          pthread_mutex_unlock(&mutex_positions);
          return 1;
        }
      }
      for (int i = 0; i < 16; i++) {
        int prev_col = 0;
        int prev_row = 0;
        int res = get_block_row_col(i, &prev_row, &prev_col);
        if (res == 0)
          continue;
        board[prev_row][prev_col] = 0;
      }
      player.positions[0]++;
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

int check_line(int *index) {
  int cleared = 0;
  *index = 0;
  for (int i = 0; i < ROWS; i++) {
    int is_full = 1;
    for (int j = 0; j < COLUMNS; j++) {
      if (board[i][j] != 1) {
        is_full = 0;
      }
    }
    if (is_full == 0)
      continue;
    for (int j = 0; j < COLUMNS; j++) {
      board[i][j] = 0;
    }
    *index = i;
    cleared++;
  }
  return cleared;
};

void move_board_down(int row_index, int rows) {
  if (rows == 0)
    return;
  for (int i = row_index; i >= 0; i--) {
    for (int j = 0; j < COLUMNS; j++) {
      board[i][j] = (i - rows) > 0 ? board[i - rows][j] : 0;
    }
  }
};

int main(int argc, char *argv[]) {
  logger = log_create("tetris.log", "TETRIS", 1, LOG_LEVEL_DEBUG);
  pthread_mutex_init(&mutex_positions, NULL);

  srand(time(NULL));
  initscr();
  clear();
  noecho();
  cbreak();

  player_intialize();
  pthread_t input_thread;
  pthread_create(&input_thread, NULL, &user_input, NULL);
  pthread_detach(input_thread);

  reset_board();
  while (1) {
    clear_screen();
    int colided = player_move(DOWN);
    update_board();
    if (colided) {
      set_player_piece();
      player_intialize();
      update_board();
      int last_line_index = 0;
      int cleared_lines = check_line(&last_line_index);
      move_board_down(last_line_index, cleared_lines);
    }
    print_board();
    refresh();
    usleep(10000000 / FRAME_RATE);
  }

  pthread_mutex_destroy(&mutex_positions);

  return EXIT_SUCCESS;
}
