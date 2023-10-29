#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_MAX 4

/* Game logic */
typedef enum Player {
  PLAYER_EMPTY = '\0',
  PLAYER_X = 'X',
  PLAYER_O = 'O',
} Player;

#define FIELD_SIZE 9
typedef Player Field[FIELD_SIZE];
typedef uint32_t (*input_fn)(Player, const Field);

bool move_valid(const Field f, uint32_t index) {
  return f[index] == PLAYER_EMPTY;
}

bool check_win(Player player, const Field f) {
  // Check rows
  for (uint32_t i = 0; i < 3; i++) {
    if (f[i * 3] == player && f[i * 3 + 1] == player &&
        f[i * 3 + 2] == player) {
      return true;
    }
  }

  // Check columns
  for (uint32_t i = 0; i < 3; i++) {
    if (f[i + 3 * 0] == player && f[i + 3 * 1] == player &&
        f[i + 3 * 2] == player) {
      return true;
    }
  }

  // Check diagonals
  if ((f[0] == player && f[4] == player && f[8] == player) || // NOLINT
      (f[2] == player && f[4] == player && f[6] == player)) { // NOLINT
    return true;
  }

  return false;
}

void field_display(const Field f) {
  for (uint32_t i = 0; i < FIELD_SIZE; i++) {
    if (i != 0 && i % 3 == 0) {
      printf("\n");
    }
    printf("%c ", f[i] ? f[i] : '1' + (unsigned char)i);
  }
  printf("\n");
}

bool turn(Field field, Player player, input_fn input) {
  uint32_t index = input(player, field);
  field[index] = player;
  return check_win(player, field);
}

/* Input */

uint32_t player_input(Player player, const Field f) {
  uint32_t index = 0;
  bool input_valid = false;
  while (!input_valid) {
    printf("%c: ", player);
    char choice[INPUT_MAX];
    fgets(choice, INPUT_MAX, stdin);
    const int base = 10;
    index = strtoul(choice, NULL, base) - 1;
    if (index < FIELD_SIZE && move_valid(f, index)) {
      input_valid = true;
    } else {
      printf("Enter Again!\n");
    }
  }
  return index;
}

bool check_draw(const Field f) {
  for (uint32_t i = 0; i < FIELD_SIZE; i++) {
    if (move_valid(f, i)) {
      return false;
    }
  }
  return true;
}

Player switch_player(Player current) {
  return current == PLAYER_X ? PLAYER_O : PLAYER_X;
}

/* Minmax */
int minmax(bool maximize, Player player, Field f) {
  if (check_win(player, f)) {
    return maximize ? 2 : -2;
  }
  if (check_win(switch_player(player), f)) {
    return maximize ? -2 : 2;
  }
  if (check_draw(f)) {
    return 0;
  }

  if (maximize) {
    int score = INT_MIN;
    for (uint32_t i = 0; i < FIELD_SIZE; i++) {
      if (move_valid(f, i)) {
        f[i] = player;
        int temp = minmax(false, switch_player(player), f);
        f[i] = PLAYER_EMPTY;
        if (score < temp) {
          score = temp;
        }
      }
    }
    return score;

  } // else
  int score = INT_MAX;
  for (uint32_t i = 0; i < FIELD_SIZE; i++) {
    if (move_valid(f, i)) {
      f[i] = player;
      int temp = minmax(true, switch_player(player), f);
      f[i] = PLAYER_EMPTY;
      if (temp < score) {
        score = temp;
      }
    }
  }
  return score;
}

uint32_t ai_input(Player player, const Field current_field) {
  uint32_t best_move = 0;
  int score = INT_MIN;
  const uint32_t moves[FIELD_SIZE] = {4, 0, 2, 6, 8, 1, 3, 5, 7};
  Player f[FIELD_SIZE] = {0};
  memcpy(f, current_field, FIELD_SIZE * sizeof(Player));
  for (uint32_t i = 0; i < FIELD_SIZE; i++) {
    uint32_t move = moves[i];
    if (move_valid(f, move)) {
      f[move] = player;
      int temp = minmax(false, switch_player(player), f);
      f[move] = PLAYER_EMPTY;
      if (score < temp) {
        score = temp;
        best_move = move;
      }
    }
  }
  printf("%c: %u\n", player, (uint32_t)best_move + 1);
  return best_move;
}

/* !Minmax */

void game(const input_fn input[2], const Player players[2]) {
  Field field = {PLAYER_EMPTY};
  Player winning_player = PLAYER_EMPTY;
  for (uint32_t i = 0; i < FIELD_SIZE; i++) {
    field_display(field);
    const Player current_player = players[i % 2];
    if (turn(field, current_player, input[i % 2])) {
      winning_player = current_player;
      break;
    }
  }
  field_display(field);
  if (winning_player != PLAYER_EMPTY) {
    printf("Player %c won!\n", winning_player);
  } else {
    printf("Draw!\n");
  }
}

bool replay(void) {
  bool invalid_input = true;
  while (invalid_input) {
    printf("Continue Playing? [Y/N]: ");
    char buffer[INPUT_MAX];
    fgets(buffer, INPUT_MAX, stdin);
    if (buffer[0] == 'N' || buffer[0] == 'n') {
      return false;
    }
    if (buffer[0] == 'Y' || buffer[0] == 'y') {
      return true;
    }
  }
  return false;
}

/* Game loop */
int main(void) {
  Player players[] = {PLAYER_X, PLAYER_O};
  input_fn input[] = {player_input, ai_input};

  bool playing = true;
  while (playing) {
    game(input, players);
    if (!replay()) {
      playing = false;
    }
  }
}
