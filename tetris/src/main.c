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
#define POINTS_PER_LINE 100
#define DEFAULT_ROTATION NORTH

typedef enum { LEFT, RIGHT, DOWN } move_t;
typedef enum { NORTH, EAST, SOUTH, WEST } rotation_t;
typedef enum { LOST, RUNNING } game_state_t;

typedef struct {
  int position[2];
  int bounding_box[16];
  rotation_t rotation;
  int piece;
  int next_piece;
  int score;
} player_t;

game_state_t game_state;
int board[ROWS][COLUMNS];
player_t player;

pthread_mutex_t mutex_positions;

void get_block_row_col(int block_index, int *row, int *col) {
  int bb_col = block_index % 4;
  int bb_row = block_index / 4;
  int bb_center_col = BOUNDING_BOX_CENTER % 4;
  int bb_center_row = BOUNDING_BOX_CENTER / 4;
  int offset_col = bb_center_col - bb_col;
  int offset_row = bb_center_row - bb_row;
  int block_col = player.position[1] - offset_col;
  int block_row = player.position[0] - offset_row;
  *row = block_row;
  *col = block_col;
}

void get_piece(int *r, rotation_t rotation, int buffer[16]) {
  int random = rand() % 7;
  if (r == NULL) {
    r = &player.piece;
    player.next_piece = random;
  }
  switch (*r) {
  case 0: {
    buffer[1] = 1;
    buffer[2] = 1;
    buffer[5] = 1;
    buffer[6] = 1;
    break;
  }
  case 1: {
    switch (rotation) {
    case NORTH: {
      buffer[4] = 1;
      buffer[5] = 1;
      buffer[6] = 1;
      buffer[7] = 1;
      break;
    }
    case EAST: {
      buffer[2] = 1;
      buffer[6] = 1;
      buffer[10] = 1;
      buffer[14] = 1;
      break;
    }
    case SOUTH: {
      buffer[8] = 1;
      buffer[9] = 1;
      buffer[10] = 1;
      buffer[11] = 1;
      break;
    }
    case WEST: {
      buffer[1] = 1;
      buffer[5] = 1;
      buffer[9] = 1;
      buffer[13] = 1;
      break;
    }
    }
    break;
  }
  case 2: {
    switch (rotation) {
    case NORTH: {
      buffer[0] = 1;
      buffer[4] = 1;
      buffer[5] = 1;
      buffer[6] = 1;
      break;
    }
    case EAST: {
      buffer[1] = 1;
      buffer[2] = 1;
      buffer[5] = 1;
      buffer[9] = 1;
      break;
    }
    case SOUTH: {
      buffer[10] = 1;
      buffer[4] = 1;
      buffer[5] = 1;
      buffer[6] = 1;
      break;
    }
    case WEST: {
      buffer[1] = 1;
      buffer[5] = 1;
      buffer[9] = 1;
      buffer[8] = 1;
      break;
    }
    }
    break;
  }
  case 3: {
    switch (rotation) {
    case NORTH: {
      buffer[2] = 1;
      buffer[4] = 1;
      buffer[5] = 1;
      buffer[6] = 1;
      break;
    }
    case EAST: {
      buffer[10] = 1;
      buffer[1] = 1;
      buffer[5] = 1;
      buffer[9] = 1;
      break;
    }
    case SOUTH: {
      buffer[8] = 1;
      buffer[4] = 1;
      buffer[5] = 1;
      buffer[6] = 1;
      break;
    }
    case WEST: {
      buffer[0] = 1;
      buffer[1] = 1;
      buffer[5] = 1;
      buffer[9] = 1;
      break;
    }
    }
    break;
  }
  case 4: {
    switch (rotation) {
    case NORTH: {
      buffer[1] = 1;
      buffer[2] = 1;
      buffer[4] = 1;
      buffer[5] = 1;
      break;
    }
    case EAST: {
      buffer[1] = 1;
      buffer[5] = 1;
      buffer[6] = 1;
      buffer[10] = 1;
      break;
    }
    case SOUTH: {
      buffer[1] = 1;
      buffer[2] = 1;
      buffer[4] = 1;
      buffer[5] = 1;
      break;
    }
    case WEST: {
      buffer[1] = 1;
      buffer[5] = 1;
      buffer[6] = 1;
      buffer[10] = 1;
      break;
    }
    }
    break;
  }
  case 5: {
    switch (rotation) {
    case NORTH: {
      buffer[0] = 1;
      buffer[1] = 1;
      buffer[5] = 1;
      buffer[6] = 1;
      break;
    }
    case EAST: {
      buffer[1] = 1;
      buffer[5] = 1;
      buffer[4] = 1;
      buffer[8] = 1;
      break;
    }
    case SOUTH: {
      buffer[0] = 1;
      buffer[1] = 1;
      buffer[5] = 1;
      buffer[6] = 1;
      break;
    }
    case WEST: {
      buffer[1] = 1;
      buffer[5] = 1;
      buffer[4] = 1;
      buffer[8] = 1;
      break;
    }
    }
    break;
  }
  case 6: {
    switch (rotation) {
    case NORTH: {
      buffer[1] = 1;
      buffer[4] = 1;
      buffer[5] = 1;
      buffer[6] = 1;
      break;
    }
    case EAST: {
      buffer[1] = 1;
      buffer[5] = 1;
      buffer[6] = 1;
      buffer[9] = 1;
      break;
    }
    case SOUTH: {
      buffer[9] = 1;
      buffer[4] = 1;
      buffer[5] = 1;
      buffer[6] = 1;
      break;
    }
    case WEST: {
      buffer[1] = 1;
      buffer[5] = 1;
      buffer[4] = 1;
      buffer[9] = 1;
      break;
    }
    }
    break;
  }
  }
}

void print_screen() {
  printw("\n");
  printw("SCORE: %04d", player.score);

  printw("\n");
  printw("NEXT PIECE:");
  printw("\n");
  int piece_preview[16];
  for (int i = 0; i < 16; i++) {
    piece_preview[i] = 0;
  }
  get_piece(&player.next_piece, DEFAULT_ROTATION, piece_preview);

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      int index = i * 4 + j;
      if (piece_preview[index] == 1) {
        printw("[]");
      } else {
        printw("  ");
      }
    }
    printw("\n");
  }

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
}
void clear_screen() {
  clear();
  refresh();
}

void reset_game() {
  player.piece = rand() % 7;
  game_state = RUNNING;
  player.score = 0;
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLUMNS; j++) {
      board[i][j] = 0;
    }
  }
}

void bb_to_board() {
  for (int i = 0; i < 16; i++) {
    if (player.bounding_box[i] == 0)
      continue;
    int block_col = 0;
    int block_row = 0;
    get_block_row_col(i, &block_row, &block_col);
    board[block_row][block_col] = 2;
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

void reset_bounding_box() {
  for (int i = 0; i < 16; i++) {
    if (player.bounding_box[i] == 0)
      continue;
    int block_col = 0;
    int block_row = 0;
    get_block_row_col(i, &block_row, &block_col);
    board[block_row][block_col] = 0;
    player.bounding_box[i] = 0;
  }
}

void player_new_piece() {
  player.rotation = DEFAULT_ROTATION;
  player.position[0] = 2;
  player.position[1] = COLUMNS / 2;
  reset_bounding_box();
  get_piece(NULL, player.rotation, player.bounding_box);
}

void player_rotate(move_t m) {
  rotation_t next_rotation;
  switch (m) {
  case RIGHT: {
    switch (player.rotation) {
    case NORTH: {
      next_rotation = EAST;
      break;
    }
    case EAST: {
      next_rotation = SOUTH;
      break;
    }
    case SOUTH: {
      next_rotation = WEST;
      break;
    }
    case WEST: {
      next_rotation = NORTH;
      break;
    }
    }
    break;
  }
  case LEFT: {
    switch (player.rotation) {
    case NORTH: {
      next_rotation = WEST;
      break;
    }
    case EAST: {
      next_rotation = NORTH;
      break;
    }
    case SOUTH: {
      next_rotation = EAST;
      break;
    }
    case WEST: {
      next_rotation = SOUTH;
      break;
    }
    }
    break;
  }
  default:
    return;
  }
  int buffer[16];
  get_piece(&player.piece, next_rotation, buffer);

  for (int i = 0; i < 16; i++) {
    if (buffer[i] == 0)
      continue;
    int row = 0;
    int col = 0;
    get_block_row_col(i, &row, &col);
    if (board[row][col] == 1 || col >= COLUMNS || col < 0 || row>=ROWS || row<0)
      return;
  }

  player.rotation = next_rotation;
  reset_bounding_box();
  get_piece(&player.piece, player.rotation, player.bounding_box);
  pthread_mutex_unlock(&mutex_positions);
}

int player_move(move_t m) {
  pthread_mutex_lock(&mutex_positions);
  switch (m) {
  case LEFT: {
    int min_col = COLUMNS;
    for (int i = 0; i < 16; i++) {
      if (player.bounding_box[i] == 0)
        continue;
      int block_col = 0;
      int block_row = 0;
      get_block_row_col(i, &block_row, &block_col);
      if (block_col < min_col)
        min_col = block_col;
    }
    if (min_col > 0) {
      for (int i = 0; i < 16; i++) {
        if (player.bounding_box[i] == 0)
          continue;
        int block_col = 0;
        int block_row = 0;
        get_block_row_col(i, &block_row, &block_col);
        if (board[block_row][block_col - 1] == 1) {
          pthread_mutex_unlock(&mutex_positions);
          return 1;
        }
      }
      for (int i = 0; i < 16; i++) {
        if (player.bounding_box[i] == 0)
          continue;
        int prev_row = 0;
        int prev_col = 0;
         get_block_row_col(i, &prev_row, &prev_col);
        board[prev_row][prev_col] = 0;
      }
      player.position[1]--;
      pthread_mutex_unlock(&mutex_positions);
      return 0;
    }
    break;
  }

  case RIGHT: {
    int max_col = 0;
    for (int i = 0; i < 16; i++) {
      if (player.bounding_box[i] == 0)
        continue;
      int block_col = 0;
      int block_row = 0;
      get_block_row_col(i, &block_row, &block_col);
      if (block_col > max_col)
        max_col = block_col;
    }
    if (max_col < COLUMNS - 1) {
      for (int i = 0; i < 16; i++) {
        if (player.bounding_box[i] == 0)
          continue;
        int block_col = 0;
        int block_row = 0;
        get_block_row_col(i, &block_row, &block_col);

        if (board[block_row][block_col + 1] == 1) {
          pthread_mutex_unlock(&mutex_positions);
          return 1;
        }
      }
      for (int i = 0; i < 16; i++) {
        if (player.bounding_box[i] == 0)
          continue;
        int block_col = 0;
        int block_row = 0;
        get_block_row_col(i, &block_row, &block_col);
        board[block_row][block_col] = 0;
      }
      player.position[1]++;
      pthread_mutex_unlock(&mutex_positions);
      return 0;
    }
    break;
  }

  case DOWN: {
    int max_row = 0;
    for (int i = 0; i < 16; i++) {
      if (player.bounding_box[i] == 0)
        continue;
      int block_col = 0;
      int block_row = 0;
      get_block_row_col(i, &block_row, &block_col);
      if (block_row > max_row)
        max_row = block_row;
    }
    if (max_row < ROWS - 1) {
      for (int i = 0; i < 16; i++) {
        if (player.bounding_box[i] == 0)
          continue;
        int block_col = 0;
        int block_row = 0;
        get_block_row_col(i, &block_row, &block_col);
        if (board[block_row + 1][block_col] == 1) {
          pthread_mutex_unlock(&mutex_positions);
          return 1;
        }
      }
      for (int i = 0; i < 16; i++) {
        if (player.bounding_box[i] == 0)
          continue;
        int prev_col = 0;
        int prev_row = 0;
        get_block_row_col(i, &prev_row, &prev_col);
        board[prev_row][prev_col] = 0;
      }
      player.position[0]++;
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
    if (game_state == RUNNING) {
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

    if (game_state == LOST) {
      if (c == 'r' || c == 'R')
        reset_game();
    }
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
    *index = i;
    for (int j = 0; j < COLUMNS; j++) {
      board[i][j] = 0;
    }
    player.score += POINTS_PER_LINE;
    cleared++;
  }
  return cleared;
};

void check_lost() {
  for (int j = 0; j < COLUMNS; j++) {
    if (board[0][j] == 1) {
      game_state = LOST;
      return;
    }
  }
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
  pthread_mutex_init(&mutex_positions, NULL);

  srand(time(NULL));
  initscr();
  clear();
  noecho();
  cbreak();

  player_new_piece();
  pthread_t input_thread;
  pthread_create(&input_thread, NULL, &user_input, NULL);
  pthread_detach(input_thread);

  reset_game();

  while (1) {
    while (game_state == RUNNING) {
      clear_screen();
      int colided = player_move(DOWN);
      bb_to_board();
      if (colided) {
        set_player_piece();
        int last_line_index = 0;
        int cleared_lines = check_line(&last_line_index);
        move_board_down(last_line_index, cleared_lines);
        player.piece = player.next_piece;
        player_new_piece();
        check_lost();
        bb_to_board();
      }
      print_screen();
      refresh();
      usleep(10000000 / FRAME_RATE);
    }

    while (game_state == LOST) {
      clear_screen();
      print_screen();
      printw("\nGAME OVER\n");
      printw("Press 'r' to restart");
      refresh();
      usleep(10000000 / FRAME_RATE);
    }
  }

  pthread_mutex_destroy(&mutex_positions);

  return EXIT_SUCCESS;
}
