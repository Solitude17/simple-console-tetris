#include <check.h>

#include "../brick_game/tetris/backend.h"

// Provide Score
START_TEST(test1) {
  int* score = provideScore();
  ck_assert_int_eq(*score, 0);
  (*score)++;
  ck_assert_int_eq(*score, 1);
}
END_TEST

// Длинная фигура в дефолтной позиции
START_TEST(test2) {
  int** i_figure_small = createMatrix(4, 4);
  fillSmallFigure(0, 0, i_figure_small);
  int** i_figure_big = createMatrix(FIELD_LENGTH, FIELD_WITDH);
  smallFigureToBig(i_figure_big, i_figure_small);
  ck_assert_int_eq(i_figure_big[1][3], 1);
  ck_assert_int_eq(i_figure_big[1][4], 1);
  ck_assert_int_eq(i_figure_big[1][5], 1);
  ck_assert_int_eq(i_figure_big[1][6], 1);
  ck_assert_int_eq(i_figure_big[1][7], 0);
  ck_assert_int_eq(i_figure_big[1][8], 0);
  removeMatrix(4, 4, i_figure_small);
  removeMatrix(FIELD_LENGTH, FIELD_WITDH, i_figure_big);
}
END_TEST

// Длинная фигура в дефолтной позиции вниз
START_TEST(test3) {
  int** i_figure_small = createMatrix(4, 4);
  fillSmallFigure(0, 0, i_figure_small);
  int** i_figure_big = createMatrix(FIELD_LENGTH, FIELD_WITDH);
  smallFigureToBig(i_figure_big, i_figure_small);
  figure_to_down(i_figure_big);
  ck_assert_int_eq(i_figure_big[2][3], 1);
  ck_assert_int_eq(i_figure_big[2][4], 1);
  ck_assert_int_eq(i_figure_big[2][5], 1);
  removeMatrix(4, 4, i_figure_small);
  removeMatrix(FIELD_LENGTH, FIELD_WITDH, i_figure_big);
}
END_TEST

// Длинная фигура в дефолтной позиции вниз вправо
START_TEST(test4) {
  int** i_figure_small = createMatrix(4, 4);
  fillSmallFigure(0, 0, i_figure_small);
  int** i_figure_big = createMatrix(FIELD_LENGTH, FIELD_WITDH);
  smallFigureToBig(i_figure_big, i_figure_small);
  figure_to_right(i_figure_big);
  ck_assert_int_eq(i_figure_big[1][3], 0);
  ck_assert_int_eq(i_figure_big[1][4], 1);
  ck_assert_int_eq(i_figure_big[1][5], 1);
  ck_assert_int_eq(i_figure_big[1][6], 1);
  removeMatrix(4, 4, i_figure_small);
  removeMatrix(FIELD_LENGTH, FIELD_WITDH, i_figure_big);
}
END_TEST

// Длинная фигура в дефолтной позиции влево
START_TEST(test5) {
  // Длинная фигура в дефолтной позиции
  int** i_figure_small = createMatrix(4, 4);
  fillSmallFigure(0, 0, i_figure_small);
  int** i_figure_big = createMatrix(FIELD_LENGTH, FIELD_WITDH);
  smallFigureToBig(i_figure_big, i_figure_small);
  figure_to_left(i_figure_big);
  ck_assert_int_eq(i_figure_big[1][3], 1);
  ck_assert_int_eq(i_figure_big[1][4], 1);
  ck_assert_int_eq(i_figure_big[1][5], 1);
  ck_assert_int_eq(i_figure_big[1][6], 0);
  removeMatrix(4, 4, i_figure_small);
  removeMatrix(FIELD_LENGTH, FIELD_WITDH, i_figure_big);
}
END_TEST

// Длинная фигура в дефолтной позиции поворот
START_TEST(test6) {
  // Длинная фигура в дефолтной позиции
  int** i_figure_small = createMatrix(4, 4);
  fillSmallFigure(0, 0, i_figure_small);
  int** i_figure_big = createMatrix(FIELD_LENGTH, FIELD_WITDH);
  smallFigureToBig(i_figure_big, i_figure_small);

  Figure_t* figure = provideFigure();
  removeMatrix(FIELD_LENGTH, FIELD_WITDH, figure->figure);
  //    изменю фигуру вручную (т.к внутри рандом)
  figure->figure = i_figure_big;
  figure->x = 3;
  figure->y = 0;
  figure->number = 0;
  figure->position = 0;

  game_rotate();

  ck_assert_int_eq(figure->figure[0][5], 1);
  ck_assert_int_eq(figure->figure[1][5], 1);
  ck_assert_int_eq(figure->figure[2][5], 1);
  ck_assert_int_eq(figure->figure[3][5], 1);
  ck_assert_int_eq(figure->figure[4][5], 0);
  removeMatrix(4, 4, i_figure_small);
  GameInfo_t* game_info;
  game_info = provideCurrentState();

  free_memory_before_exit();
  game_info->field = NULL;
}
END_TEST

START_TEST(test7) {
  int old = 1;
  int new = 100;
  update_record_into_file(&new, &old);
  ck_assert_int_eq(old, 100);
}
END_TEST

// Заполненная строка
START_TEST(test8) {
  int** i_figure_small = createMatrix(4, 4);
  fillSmallFigure(0, 0, i_figure_small);
  int** i_figure_big = createMatrix(FIELD_LENGTH, FIELD_WITDH);
  smallFigureToBig(i_figure_big, i_figure_small);
  for (int i = 0; i < FIELD_WITDH; i++) {
    i_figure_big[19][i] = 1;
  }
  ck_assert_int_eq(row_is_full_filled(i_figure_big, 19), 1);
  removeMatrix(4, 4, i_figure_small);
  removeMatrix(FIELD_LENGTH, FIELD_WITDH, i_figure_big);
}
END_TEST

// две заполненных строки
// проверка полной заполненности
// удаление и сдвиг при полной заполненности
// искусственно меняю гейм инфо и удаляю
START_TEST(test9) {
  int** i_figure_small = createMatrix(4, 4);
  fillSmallFigure(0, 0, i_figure_small);
  int** i_figure_big = createMatrix(FIELD_LENGTH, FIELD_WITDH);
  smallFigureToBig(i_figure_big, i_figure_small);
  for (int i = 0; i < FIELD_WITDH; i++) {
    i_figure_big[19][i] = 1;
  }
  for (int i = 0; i < FIELD_WITDH; i++) {
    i_figure_big[18][i] = 1;
  }
  ck_assert_int_eq(row_is_full_filled(i_figure_big, 19), 1);
  ck_assert_int_eq(row_is_empty(i_figure_big, 19), 0);
  ck_assert_int_eq(row_is_full_filled(i_figure_big, 18), 1);

  GameInfo_t* game_info = provideCurrentState();
  removeMatrix(FIELD_LENGTH, FIELD_WITDH, game_info->field);
  game_info->field = i_figure_big;

  ck_assert_int_eq(delete_filled_rows(), 2);
  ck_assert_int_eq(column_is_empty(game_info->field, 4), 0);

  removeMatrix(4, 4, i_figure_small);
  free_memory_before_exit();
}
END_TEST

// эмуляция сдвига на 1 вниз
START_TEST(test10) {
  // создаём палку
  int** i_figure_small = createMatrix(4, 4);
  fillSmallFigure(0, 0, i_figure_small);
  int** i_figure_big = createMatrix(FIELD_LENGTH, FIELD_WITDH);
  smallFigureToBig(i_figure_big, i_figure_small);
  // искусственно подменяю матрицу
  Figure_t* figure = provideFigure();
  removeMatrix(FIELD_LENGTH, FIELD_WITDH, figure->figure);
  figure->figure = i_figure_big;
  // проверяю создание
  ck_assert_int_eq(figure->figure[1][3], 1);
  ck_assert_int_eq(figure->figure[1][4], 1);
  ck_assert_int_eq(figure->figure[1][5], 1);
  ck_assert_int_eq(figure->figure[1][6], 1);
  ck_assert_int_eq(figure->figure[2][3], 0);
  ck_assert_int_eq(figure->figure[2][4], 0);
  ck_assert_int_eq(figure->figure[2][5], 0);
  ck_assert_int_eq(figure->figure[2][6], 0);
  // делаю сдвиг
  game_shift_down();
  // проверяю
  ck_assert_int_eq(figure->figure[2][3], 1);
  ck_assert_int_eq(figure->figure[2][4], 1);
  ck_assert_int_eq(figure->figure[2][5], 1);
  ck_assert_int_eq(figure->figure[2][6], 1);

  removeMatrix(4, 4, i_figure_small);
  free_memory_before_exit();
}
END_TEST

// эмуляция сдвига на 1 влево
START_TEST(test11) {
  // создаём палку
  int** i_figure_small = createMatrix(4, 4);
  fillSmallFigure(0, 0, i_figure_small);
  int** i_figure_big = createMatrix(FIELD_LENGTH, FIELD_WITDH);
  smallFigureToBig(i_figure_big, i_figure_small);
  // искусственно подменяю матрицу
  Figure_t* figure = provideFigure();
  removeMatrix(FIELD_LENGTH, FIELD_WITDH, figure->figure);
  figure->figure = i_figure_big;
  // проверяю создание
  ck_assert_int_eq(figure->figure[1][2], 0);
  ck_assert_int_eq(figure->figure[1][3], 1);
  ck_assert_int_eq(figure->figure[1][4], 1);
  ck_assert_int_eq(figure->figure[1][5], 1);
  ck_assert_int_eq(figure->figure[1][6], 1);

  // делаю сдвиг
  game_move_left();
  // проверяю
  ck_assert_int_eq(figure->figure[1][2], 1);
  ck_assert_int_eq(figure->figure[1][3], 1);
  ck_assert_int_eq(figure->figure[1][4], 1);
  ck_assert_int_eq(figure->figure[1][5], 1);
  ck_assert_int_eq(figure->figure[1][6], 0);

  removeMatrix(4, 4, i_figure_small);
  free_memory_before_exit();
}
END_TEST

// эмуляция сдвига на 1 вправо
START_TEST(test12) {
  // создаём палку
  int** i_figure_small = createMatrix(4, 4);
  fillSmallFigure(0, 0, i_figure_small);
  int** i_figure_big = createMatrix(FIELD_LENGTH, FIELD_WITDH);
  smallFigureToBig(i_figure_big, i_figure_small);
  // искусственно подменяю матрицу
  Figure_t* figure = provideFigure();
  removeMatrix(FIELD_LENGTH, FIELD_WITDH, figure->figure);
  figure->figure = i_figure_big;
  // проверяю создание
  ck_assert_int_eq(figure->figure[1][3], 1);
  ck_assert_int_eq(figure->figure[1][4], 1);
  ck_assert_int_eq(figure->figure[1][5], 1);
  ck_assert_int_eq(figure->figure[1][6], 1);
  ck_assert_int_eq(figure->figure[1][7], 0);

  // делаю сдвиг
  game_move_right();
  // проверяю
  ck_assert_int_eq(figure->figure[1][3], 0);
  ck_assert_int_eq(figure->figure[1][4], 1);
  ck_assert_int_eq(figure->figure[1][5], 1);
  ck_assert_int_eq(figure->figure[1][6], 1);
  ck_assert_int_eq(figure->figure[1][7], 1);

  removeMatrix(4, 4, i_figure_small);
  free_memory_before_exit();
}
END_TEST

// эмуляция падения фигуры
START_TEST(test13) {
  // создаём палку
  int** i_figure_small = createMatrix(4, 4);
  fillSmallFigure(0, 0, i_figure_small);
  int** i_figure_big = createMatrix(FIELD_LENGTH, FIELD_WITDH);
  smallFigureToBig(i_figure_big, i_figure_small);
  // создаём курент стейт
  GameInfo_t* game_info = provideCurrentState();
  // искусственно подменяю матрицу
  Figure_t* figure = provideFigure();
  removeMatrix(FIELD_LENGTH, FIELD_WITDH, figure->figure);
  figure->figure = i_figure_big;
  // проверяю создание
  ck_assert_int_eq(figure->figure[1][3], 1);
  ck_assert_int_eq(figure->figure[1][4], 1);
  ck_assert_int_eq(figure->figure[1][5], 1);
  ck_assert_int_eq(figure->figure[1][6], 1);

  // делаю сдвиг
  game_move_down();
  // проверяю
  // т.к после падения фигура зафиксировалась в гейминфо нам нужен гейминфо
  ck_assert_int_eq(game_info->field[19][3], 1);
  ck_assert_int_eq(game_info->field[19][4], 1);
  ck_assert_int_eq(game_info->field[19][5], 1);
  ck_assert_int_eq(game_info->field[19][6], 1);
  removeMatrix(4, 4, i_figure_small);
  free_memory_before_exit();
}
END_TEST

// тест ситуации когда некуда спауниться
START_TEST(test14) {
  // создаём палку
  int** i_figure_small = createMatrix(4, 4);
  fillSmallFigure(0, 0, i_figure_small);
  // создаём курент стейт и фигуру
  GameInfo_t* game_info = provideCurrentState();
  Figure_t* figure = provideFigure();
  // искусственно изменяем гейм инфо и фигуру
  smallFigureToBig(figure->figure, i_figure_small);
  smallFigureToBig(game_info->field, i_figure_small);

  // проверяю создание
  ck_assert_int_eq(figure->figure[1][3], 1);
  ck_assert_int_eq(figure->figure[1][4], 1);
  ck_assert_int_eq(game_info->field[1][3], 1);
  ck_assert_int_eq(game_info->field[1][4], 1);
  ck_assert_int_eq(*(provideGameState()), 0);

  // делаю спаун
  //  при попытке спауна из-за наличия заполненных матриц они не пересоздадуться
  //  должен возникнуть конфликт и игра перетечь в гейм овер стейт
  // а так же матрицы figure->figure и game_state->field должны очиститься
  spawn();
  ck_assert_int_eq(*(provideGameState()), 5);
  ck_assert_ptr_eq(game_info->field, NULL);
  ck_assert_ptr_eq(game_info->next, NULL);
  ck_assert_ptr_eq(figure->figure, NULL);

  removeMatrix(4, 4, i_figure_small);
  free_memory_before_exit();
}
END_TEST

// Создание тестового набора
Suite* brick_game_suite(void) {
  Suite* s;
  TCase* tc_core;

  s = suite_create("BrickGame_v01");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test1);
  tcase_add_test(tc_core, test2);
  tcase_add_test(tc_core, test3);
  tcase_add_test(tc_core, test4);
  tcase_add_test(tc_core, test5);
  tcase_add_test(tc_core, test6);
  tcase_add_test(tc_core, test7);
  tcase_add_test(tc_core, test8);
  tcase_add_test(tc_core, test9);
  tcase_add_test(tc_core, test10);
  tcase_add_test(tc_core, test11);
  tcase_add_test(tc_core, test12);
  tcase_add_test(tc_core, test13);
  tcase_add_test(tc_core, test14);

  suite_add_tcase(s, tc_core);

  return s;
}

// Запуск тестов
int main(void) {
  int number_failed;
  Suite* s;
  SRunner* sr;

  s = brick_game_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
