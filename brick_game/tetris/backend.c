#include "backend.h"

const int bit_figure[][4] = {
    // 7 фигур 0-6
    {0b0000111100000000, 0b0010001000100010, 0b0000000011110000,
     0b0100010001000100},  // I WHITE BLUE
    {0b1000111000000000, 0b0110010001000000, 0b0000111000100000,
     0b0100010011000000},  // J BLUE
    {0b0010111000000000, 0b0100010001100000, 0b0000111010000000,
     0b1100010001000000},  // L ORANGE
    {0b0110011000000000, 0b0110011000000000, 0b0110011000000000,
     0b0110011000000000},  // O YELLOW
    {0b0110110000000000, 0b0100011000100000, 0b0000011011000000,
     0b1000110001000000},  // S GREEN
    {0b0100111000000000, 0b0100011001000000, 0b0000111001000000,
     0b0100110001000000},  // T PURPURE
    {0b1100011000000000, 0b0010011001000000, 0b0000110001100000,
     0b0100110010000000}  // Z RED
};

const int record[] = {0, 100, 300, 700, 1500};

void userInput(UserAction_t action_input, bool hold) {
  action fsm_table[8][8] = {
      {spawn, NULL, game_exit, NULL, NULL, NULL, NULL, NULL},
      {spawn, NULL, game_exit, spawn, spawn, NULL, spawn, spawn},
      {NULL, game_pause, game_exit, game_move_left, game_move_right, NULL,
       game_move_down, game_rotate},
      {NULL, game_pause, game_exit, game_move_left, game_move_right, NULL, NULL,
       game_rotate},
      {spawn, game_pause, game_exit, spawn, spawn, spawn, spawn, spawn},
      {spawn, NULL, game_exit, NULL, NULL, NULL, NULL, NULL},
      {NULL, spawn, game_exit, NULL, NULL, NULL, NULL, NULL},
      {game_exit, game_exit, game_exit, game_exit, game_exit, game_exit,
       game_exit, game_exit},
  };

  GameState_t *g_state;
  g_state = provideGameState();
  GameInfo_t *g_info;
  g_info = provideCurrentState();

  if ((int)action_input != ERR_VALUE) {
    action act;
    act = fsm_table[*g_state][action_input];
    if (act) act();
  }

  if (*g_state == ATTACHING_STATE && !hold) {
    int mult_record;
    // Функция подсчёта очков, проверки заполненных строк и удаления их!
    mult_record = delete_filled_rows();
    g_info->score += record[mult_record];
    if (g_info->level < 10) {
      g_info->level = (g_info->score / 600) + 1;
    }
    g_info->speed = BASE_SPEED - (g_info->level * 75);
    spawn();
  }
}

GameInfo_t *provideCurrentState() {
  // инициализируется нулями
  static GameInfo_t GameInfo;

  if (!GameInfo.field) {
    GameInfo.field = createMatrix(FIELD_LENGTH, FIELD_WITDH);
    GameInfo.level = 1;
    GameInfo.speed = BASE_SPEED;
  }
  if (!GameInfo.next) {
    GameInfo.next = createMatrix(4, 4);
    fillSmallFigure(randomInt(), 0, GameInfo.next);
  }
  if (!GameInfo.high_score) {
    GameInfo.high_score = extract_top_record_from_file();
  }
  return &GameInfo;
}

GameState_t *provideGameState() {
  // т.к enum инициализирую числом
  // инит нулём по умолчанию, т.к начальное состояние
  static GameState_t GameState = START_STATE;
  return &GameState;
}

UserAction_t *provideUserAction() {
  // инит -1 чтобы отслеживать, ноль - занят
  static UserAction_t UserAction = INITIALIZATION_VALUE;
  // static UserAction_t UserAction;
  // UserAction++;
  return &UserAction;
}

Figure_t *provideFigure() {
  static Figure_t figure;
  if (!figure.figure) {
    figure.figure = createMatrix(FIELD_LENGTH, FIELD_WITDH);
    GameInfo_t *game_info;
    game_info = provideCurrentState();
    smallFigureToBig(figure.figure, game_info->next);

    figure.number = searchNumberFigure(game_info->next);
    removeMatrix(4, 4, game_info->next);
    game_info->next = NULL;
    figure.position = 0;
    figure.y = 0;
    figure.x = 3;  // базовая привязка к полю при респе
  }
  return &figure;
}

int *provideScore() {
  static int score;
  return &score;
}

int searchNumberFigure(int **Figure) {
  int answer = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (Figure[i][j]) {
        answer = Figure[i][j];
        i = 4;
        j = 4;
      }
    }
  }
  // т.к номер фигуры при fill увеличен на 1
  answer--;
  return answer;
}

// y - количество строк (длина), x- количество столбов (ширина)
int **createMatrix(int y, int x) {
  int **Matrix = calloc(y, x * sizeof(int));
  for (int i = 0; i < y; i++) {
    Matrix[i] = calloc(x, sizeof(int));
  }
  return Matrix;
}

int **copyMatrix(int **Matrix, int x, int y) {
  int **newMatrix;
  newMatrix = createMatrix(x, y);
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y; j++) {
      newMatrix[i][j] = Matrix[i][j];
    }
  }
  return newMatrix;
}

void removeMatrix(int y, int x, int **Matrix) {
  if (Matrix && x) {
    for (int i = 0; i < y; i++) {
      if (Matrix[i]) {
        free(Matrix[i]);
      }
    }
  }
  free(Matrix);
}

void fillSmallFigure(int number, int position, int **figure) {
  int mask = 1;
  // 0b0000111100000000
  // 0b0000000000000001 Mask

  // 0000
  // 1111
  // 0000
  // 0000
  int figure_bit = bit_figure[number][position];
  int iteration = 1;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if ((figure_bit >> (16 - iteration)) & mask) {
        figure[i][j] = number + 1;
      }
      iteration++;
    }
  }
};

void summ_big_matrix(int **consumer, int **producer) {
  for (int i = 0; i < FIELD_LENGTH; i++) {
    for (int j = 0; j < FIELD_WITDH; j++) {
      consumer[i][j] = consumer[i][j] + producer[i][j];
    }
  }
}

// small is 4x4, BigIs20x10
void smallFigureToBig(int **Big, int **Small) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      Big[i][j + 3] = Small[i][j];
    }
  }
}

int figure_to_down(int **figure) {
  if (!row_is_empty(figure, 19)) {
    return 1;
  }

  // SEGFAULT DANGER
  for (int i = FIELD_LENGTH - 2; i >= 0; i--) {
    for (int j = FIELD_WITDH - 1; j >= 0; j--) {
      if (figure[i][j]) {
        figure[i + 1][j] = figure[i][j];
        figure[i][j] = 0;
      }
    }
  }
  return 0;
}

int figure_to_right(int **Area) {
  if (!column_is_empty(Area, 9)) {
    return 1;
  }

  for (int j = FIELD_WITDH - 1; j >= 0; j--) {
    for (int i = FIELD_LENGTH - 1; i >= 0; i--) {
      if (Area[i][j]) {
        Area[i][j + 1] = Area[i][j];
        Area[i][j] = 0;
      }
    }
  }
  return 0;
}

int figure_to_left(int **Area) {
  if (!column_is_empty(Area, 0)) {
    return 1;
  }

  for (int j = 0; j < FIELD_WITDH; j++) {
    for (int i = FIELD_LENGTH - 1; i >= 0; i--) {
      if (Area[i][j]) {
        Area[i][j - 1] = Area[i][j];
        Area[i][j] = 0;
      }
    }
  }
  return 0;
}

int row_is_empty(int **Area, int row) {
  int answer = 1;
  for (int i = 0; i < FIELD_WITDH; i++) {
    if (Area[row][i]) {
      answer = 0;
      i = FIELD_WITDH;
    }
  }
  return answer;
}

int column_is_empty(int **Area, int column) {
  int answer = 1;
  for (int i = 0; i < FIELD_LENGTH; i++) {
    if (Area[i][column]) {
      answer = 0;
      i = FIELD_LENGTH;
    }
  }
  return answer;
}

int row_is_full_filled(int **Area, int row) {
  int answer = 1;
  for (int i = 0; i < FIELD_WITDH; i++) {
    if (Area[row][i] == 0) {
      answer = 0;
      i = FIELD_WITDH;
    }
  }
  return answer;
}

void spawn() {
  GameInfo_t *game_info;
  game_info = provideCurrentState();
  Figure_t *figure;
  figure = provideFigure();
  GameState_t *game_state;
  game_state = provideGameState();
  *game_state = MOVING_STATE;

  // если есть конфликт и фигуре некуда спауниться то конец игры
  if (isMatrixConflict(game_info->field, figure->figure)) {
    *game_state = GAMEOVER_STATE;

    removeMatrix(FIELD_LENGTH, FIELD_WITDH, figure->figure);
    figure->figure = NULL;
    removeMatrix(FIELD_LENGTH, FIELD_WITDH, game_info->field);
    game_info->field = NULL;
    removeMatrix(4, 4, game_info->next);
    game_info->next = NULL;
    game_info->level = 0;
    game_info->speed = 0;
    int *score_for_print = provideScore();
    *score_for_print = game_info->score;
  }
};

void game_pause() {
  GameState_t *game_state;
  game_state = provideGameState();
  *game_state = PAUSE_STATE;
};
void game_exit() {
  GameState_t *game_state;
  game_state = provideGameState();
  GameInfo_t *game_info;
  game_info = provideCurrentState();
  update_record_into_file(&game_info->score, &game_info->high_score);
  *game_state = EXIT_STATE;
};

void game_move_down() {
  GameInfo_t *game_info;
  game_info = provideCurrentState();
  Figure_t *figure;
  figure = provideFigure();
  GameState_t *game_state;
  game_state = provideGameState();

  while ((*game_state != ATTACHING_STATE)) {
    if (canMove(game_info, figure, Down)) {
      figure_to_down(figure->figure);
      figure->y++;
    } else {
      *game_state = ATTACHING_STATE;  // ТОЛЬКО ДЛЯ ДВИЖЕНИЯ ВНИЗ!
      summ_big_matrix(game_info->field, figure->figure);
      removeMatrix(FIELD_LENGTH, FIELD_WITDH, figure->figure);
      figure->figure = NULL;
    }
  }
};

void game_move_left() {
  GameInfo_t *game_info;
  game_info = provideCurrentState();
  Figure_t *figure;
  figure = provideFigure();

  if (canMove(game_info, figure, Left)) {
    figure_to_left(figure->figure);
    figure->x--;
  }
};
void game_move_right() {
  GameInfo_t *game_info;
  game_info = provideCurrentState();
  Figure_t *figure;
  figure = provideFigure();

  if (canMove(game_info, figure, Right)) {
    figure_to_right(figure->figure);
    figure->x++;
  }
};
void game_rotate() {
  GameInfo_t *game_info;
  game_info = provideCurrentState();
  Figure_t *figure;
  figure = provideFigure();

  int cant_move_flag = 0;  // если не 0 значит нельзя
  int **new_figure_big = createMatrix(FIELD_LENGTH, FIELD_WITDH);
  int **new_figure_small = createMatrix(4, 4);

  fillSmallFigure(figure->number, (figure->position + 1) % 4, new_figure_small);
  smallFigureToBig(new_figure_big, new_figure_small);
  removeMatrix(4, 4, new_figure_small);
  new_figure_small = NULL;

  for (int i = figure->y; i > 0; i--) {
    cant_move_flag = figure_to_down(new_figure_big);
  }
  if (!cant_move_flag) {
    if (figure->x < 3) {
      for (int j = 3; j > figure->x; j--) {
        cant_move_flag = figure_to_left(new_figure_big);
      }
    }
    if (figure->x > 3) {
      for (int j = 3; j < figure->x; j++) {
        cant_move_flag = figure_to_right(new_figure_big);
      }
    }
  }
  if (!cant_move_flag && !isMatrixConflict(game_info->field, new_figure_big)) {
    removeMatrix(FIELD_LENGTH, FIELD_WITDH, figure->figure);
    figure->figure = NULL;
    figure->figure = new_figure_big;
    figure->position = (figure->position + 1) % 4;
  } else {
    removeMatrix(FIELD_LENGTH, FIELD_WITDH, new_figure_big);
    new_figure_big = NULL;
  }
};

void game_shift_down() {
  GameInfo_t *game_info;
  game_info = provideCurrentState();
  Figure_t *figure;
  figure = provideFigure();
  GameState_t *game_state;
  game_state = provideGameState();
  if (canMove(game_info, figure, Down)) {
    figure_to_down(figure->figure);
    figure->y++;
  } else {
    *game_state = ATTACHING_STATE;  // ТОЛЬКО ДЛЯ ДВИЖЕНИЯ ВНИЗ!
    summ_big_matrix(game_info->field, figure->figure);
    removeMatrix(FIELD_LENGTH, FIELD_WITDH, figure->figure);
    figure->figure = NULL;
  }
};

int delete_filled_rows() {
  GameInfo_t *game_info;
  game_info = provideCurrentState();
  int count_deleted_rows = 0;

  for (int i = FIELD_LENGTH - 1; i >= 0; i--) {
    if (row_is_full_filled(game_info->field, i)) {
      count_deleted_rows++;
      delete_row(i);
      i++;
    }
  }
  return count_deleted_rows;
}

void delete_row(int row) {
  GameInfo_t *game_info;
  game_info = provideCurrentState();

  for (int i = row; i > 0; i--) {
    for (int j = 0; j < FIELD_WITDH; j++) {
      game_info->field[i][j] = game_info->field[i - 1][j];
    }
  }
}

int randomInt() {
  int rand_int = rand() % 7;
  return rand_int;
}

bool isMatrixConflict(int **M1, int **M2) {
  bool answer = false;
  for (int i = 0; i < FIELD_LENGTH; i++) {
    for (int j = 0; j < FIELD_WITDH; j++) {
      if (M1[i][j] != 0 && M2[i][j] != 0) {
        answer = true;
        i = FIELD_LENGTH;
        j = FIELD_WITDH;
      }
    }
  }
  return answer;
};

bool canMove(GameInfo_t *game_info, Figure_t *figure, int type_move) {
  int **temp_figure = copyMatrix(figure->figure, FIELD_LENGTH, FIELD_WITDH);
  bool can_move = 1;

  // функция вниз вернёт 1 если низя
  switch (type_move) {
    case Left:
      can_move = !figure_to_left(temp_figure);
      break;
    case Right:
      can_move = !figure_to_right(temp_figure);
      break;
    case Down:
      can_move = !figure_to_down(temp_figure);
      break;
  }

  // если сдвиг произошёл, то проверяем конфликт
  if (can_move) {
    can_move = !isMatrixConflict(temp_figure, game_info->field);
  }
  removeMatrix(FIELD_LENGTH, FIELD_WITDH, temp_figure);
  temp_figure = NULL;
  return can_move;
};

void free_memory_before_exit() {
  GameInfo_t *game_info;
  game_info = provideCurrentState();
  Figure_t *figure;
  figure = provideFigure();

  if (game_info->field) {
    removeMatrix(FIELD_LENGTH, FIELD_WITDH, game_info->field);
    game_info->field = NULL;
  }

  if (game_info->next) {
    removeMatrix(4, 4, game_info->next);
    game_info->next = NULL;
  }
  if (figure->figure) {
    removeMatrix(FIELD_LENGTH, FIELD_WITDH, figure->figure);
    figure->figure = NULL;
  }
}

long long current_timestamp() {
  struct timeval te;
  gettimeofday(&te, NULL);
  long long miliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
  return miliseconds;
}

int extract_top_record_from_file() {
  int ret = 0;
  FILE *file = fopen("build/top.txt", "r");
  if (file) {
    fscanf(file, "%d", &ret);
    fclose(file);
  }
  return ret;
}

void update_record_into_file(int *new_record, int *old_record) {
  if (*new_record > *old_record) {
    FILE *file = fopen("build/top.txt", "w");
    if (file) {
      fprintf(file, "%d", *new_record);
      *old_record = *new_record;
      fclose(file);
    }
  }
  *new_record = 0;
}
