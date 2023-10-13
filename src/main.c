#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_MAX 4

typedef enum Player {
  PLAYER_EMPTY = '\0',
  PLAYER_X = 'X',
  PLAYER_O = 'O',
} Player;

#define FIELD_SIZE 9
Player field[FIELD_SIZE];

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

bool turn(Player player, size_t (*input)(Player player, const Player *)) {
  size_t index = input(player, field);
  field_insert(player, index);
  return check_win(player, field);
}

size_t player_input(Player player, const Player *f) {
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

int minmax(bool maximize, Player player, Player *f);

int max(Player player, Player *sim_field) {
  int score = INT_MIN;
  for (size_t i = 0; i < FIELD_SIZE; i++) {
    if (move_valid(sim_field, i)) {
      sim_field[i] = player;
      int temp =
          minmax(false, player == PLAYER_X ? PLAYER_O : PLAYER_X, sim_field);
      if (score < temp) {
        score = temp;
      }
      sim_field[i] = PLAYER_EMPTY;
    }
  }
  return score;
}

int min(Player player, Player *sim_field) {
  int score = INT_MAX;
  for (size_t i = 0; i < FIELD_SIZE; i++) {
    if (move_valid(sim_field, i)) {
      sim_field[i] = player;
      int temp =
          minmax(true, player == PLAYER_X ? PLAYER_O : PLAYER_X, sim_field);
      if (temp < score) {
        score = temp;
      }
      sim_field[i] = PLAYER_EMPTY;
    }
  }
  return score;
}

int minmax(bool maximize, Player player, Player *f) {
  if (check_win(player, f)) {
    return maximize ? 1 : -1;
  }
  if (check_draw(f)) {
    return 0;
  }
  if (maximize) {
    return max(player, f);
  }
  return min(player, f);
}

size_t ai_input(Player player, const Player *f) {
  size_t move = 0;
  int score = INT_MIN;
  Player sim_field[FIELD_SIZE];
  memcpy(sim_field, f, FIELD_SIZE * sizeof(Player));
  for (size_t i = 0; i < FIELD_SIZE; i++) {
    if (move_valid(sim_field, i)) {
      sim_field[i] = player;
      int temp =
          minmax(false, player == PLAYER_X ? PLAYER_O : PLAYER_X, sim_field);
      if (score < temp) {
        score = temp;
        move = i;
      }
      sim_field[i] = PLAYER_EMPTY;
    }
  }
  printf("%c: %ld\n", player, move + 1);
  return move;
}

int main(void) {
  Player winning_player = PLAYER_EMPTY;
  Player players[] = {PLAYER_X, PLAYER_O};
  size_t (*input[])(Player, const Player *) = {player_input, ai_input};
  for (size_t i = 0; i < FIELD_SIZE; i++) {
    field_display(field);
    Player current_player = players[i % 2];
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
