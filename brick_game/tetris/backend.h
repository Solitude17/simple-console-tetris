#ifndef BACKEND_H
#define BACKEND_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define FIELD_WITDH 10
#define FIELD_LENGTH 20

#define INITIALIZATION_VALUE -1
#define BASE_SPEED 1000  // ms
#define ERR_VALUE -1

// шаблон указателя на void функцию не принимающей аргументов
typedef void (*action)();

typedef enum {
  START_STATE = 0,  // Начальное состояние.
  SPAWN_STATE = 1,  // Состояние появления фигуры или змейки и яблока
  MOVING_STATE = 2,  //  Состояние движения фигуры или змейки.
  SHIFTING_STATE = 3,   // Состояние сдвига.
  ATTACHING_STATE = 4,  // Состояние столкновения фигуры или прикрепления частей
                        // змейки после поедания яблока.
  GAMEOVER_STATE = 5,  // Конец Игры
  //++++++++++++++
  PAUSE_STATE = 6,  // Пауза
  //++++++++++++++
  EXIT_STATE = 7,  // Выход из игры
} GameState_t;

// стандартно из либы
typedef enum {
  Start,      // ENTER
  Pause,      // SPACE
  Terminate,  // завершение игры ESC
  Left,
  Right,
  Up,     // не используется в тетрисе
  Down,   // ускорение падения фигуры
  Action  //  вращение фигуры UP
} UserAction_t;

// фигуры:

//         +       +   ++   ++    +    ++
// ++++    +++   +++   ++  ++    +++    +++

typedef struct {
  int **field;  // матрица
  int **next;   // матрица
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef struct {
  int **figure;  // матрица 20x10
  int x;
  int y;
  int number;
  int position;
} Figure_t;

// обработчик инпута с FSM таблицей
void userInput(UserAction_t action, bool hold);

GameInfo_t *provideCurrentState();
GameState_t *provideGameState();
UserAction_t *provideUserAction();
Figure_t *provideFigure();
int *provideScore();

// поиск номера фигуры из матрицы 4х4
int searchNumberFigure(int **Figure);

// заполнение матрицы фигуры
void fillSmallFigure(int number, int position, int **figure);

// перенос матрицы фигуры в матрицу размеров поля
void smallFigureToBig(int **consumer, int **producers);

// сдвиг фигуры по матрице размеров поля, возвращет 1 если нельзя
int figure_to_down(int **figure);
int figure_to_right(int **Area);
int figure_to_left(int **Area);

// проверки на пустоту строки и столба и подсчёт количества заполненных
int row_is_empty(int **Area, int row);
int column_is_empty(int **Area, int column);
int row_is_full_filled(int **Area, int row);

void summ_big_matrix(int **consumer, int **producer);

// Matrix
int **createMatrix(int y, int x);
int **copyMatrix(int **Matrix, int x, int y);
void removeMatrix(int y, int x, int **Matrix);
void printMatrix(int x, int y, int **A);

// FSM

// Вытянуть из Gameinfo_t.next фигуру в основную для фигуры структуру Figure_t
// Заспаунить её на поле Gameinfo_t.field, если возможно
void spawn();
void game_pause();
void game_exit();
void game_move_down();
void game_move_left();
void game_move_right();
void game_rotate();

void game_shift_down();

// ROWS WORK
// возвращает количество удаленных строк
int delete_filled_rows();
void delete_row(int row);

// random int
int randomInt();

// для больших матриц 20х10
bool isMatrixConflict(int **M1, int **M2);
bool canMove(GameInfo_t *game_info, Figure_t *figure, int type_move);

void check_game_over();

void free_memory_before_exit();
long long current_timestamp();

// топ рекорд из файла
int extract_top_record_from_file();
void update_record_into_file(int *new_record, int *old_record);

#endif  // BACKEND_H