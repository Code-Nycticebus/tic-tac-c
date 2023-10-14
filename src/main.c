#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Game logic */
typedef enum Player {
  PLAYER_EMPTY = '\0',
  PLAYER_X = 'X',
  PLAYER_O = 'O',
} Player;

#define FIELD_SIZE 9
Player field[FIELD_SIZE];
typedef size_t (*input_fn)(Player, const Player[FIELD_SIZE]);

bool move_valid(const Player *f, size_t index) {
  return f[index] == PLAYER_EMPTY;
}

void field_insert(Player player, size_t field_index) {
  field[field_index] = player;
}

bool check_win(Player player, const Player *f) {
  // Check rows
  for (int i = 0; i < 3; i++) {
    if (f[i * 3] == player && f[i * 3 + 1] == player &&
        f[i * 3 + 2] == player) {
      return true; // Found a winning row
    }
  }

  // Check columns
  for (int i = 0; i < 3; i++) {
    if (f[i] == player && f[i + 3] == player && f[i + 6] == player) { // NOLINT
      return true; // Found a winning column
    }
  }

  // Check diagonals
  if ((f[0] == player && f[4] == player && f[8] == player) || // NOLINT
      (f[2] == player && f[4] == player && f[6] == player)) { // NOLINT
    return true; // Found a winning diagonal
  }

  return false; // No winning combination found
}

void field_display(const Player *f) {
  for (size_t i = 0; i < FIELD_SIZE; i++) {
    if (i % 3 == 0) {
      printf("\n");
    }
    printf("%c ", f[i] ? f[i] : '1' + (char)i);
  }
  printf("\n");
}

bool turn(Player player, input_fn input) {
  size_t index = input(player, field);
  field_insert(player, index);
  return check_win(player, field);
}

/* Input */

size_t player_input(Player player, const Player *f) {
#define INPUT_MAX 4
  size_t index = 0;
  bool input_valid = false;
  while (!input_valid) {
    printf("%c: ", player);
    char choice[INPUT_MAX];
    fgets(choice, INPUT_MAX, stdin);
    const int base = 10;
    index = strtol(choice, NULL, base) - 1;
    if ((0 <= index && index < FIELD_SIZE) && move_valid(f, index)) {
      input_valid = true;
    } else {
      printf("Enter Again!\n");
    }
  }
  return index;
}

bool check_draw(const Player *f) {
  for (size_t i = 0; i < FIELD_SIZE; i++) {
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

int minmax(bool maximize, Player player, Player *f) {
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
    for (size_t i = 0; i < FIELD_SIZE; i++) {
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
  for (size_t i = 0; i < FIELD_SIZE; i++) {
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

size_t ai_input(Player player, const Player *current_field) {
  size_t best_move = 0;
  int score = INT_MIN;
  const size_t moves[FIELD_SIZE] = {4, 0, 2, 6, 8, 1, 3, 5, 7};
  Player f[FIELD_SIZE] = {0};
  memcpy(f, current_field, FIELD_SIZE * sizeof(Player));
  for (size_t i = 0; i < FIELD_SIZE; i++) {
    size_t move = moves[i];
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
  printf("%c: %ld\n", player, best_move + 1);
  return best_move;
}

/* Game loop */
int main(void) {
  Player winning_player = PLAYER_EMPTY;
  Player players[] = {PLAYER_X, PLAYER_O};
  input_fn input[] = {player_input, ai_input};

  for (size_t i = 0; i < FIELD_SIZE; i++) {
    field_display(field);
    const Player current_player = players[i % 2];
    if (turn(current_player, input[i % 2])) {
      winning_player = current_player;
      break;
    }
  }
  field_display(field);
  if (winning_player) {
    printf("Player %c won!\n", winning_player);
  } else {
    printf("Draw!\n");
  }
}
