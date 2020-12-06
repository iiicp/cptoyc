// How to run:
//
// $ make
// $ ./chibicc examples/nqueen.c > tmp.s
// $ gcc -static -o tmp tmp.s
// $ ./tmp


#include <stdio.h>

#define LOG(format, args...) fprintf(stdout, format, args)
#define LOG_C99(format, ...) fprintf(stdout, format, __VA_ARGS__)
#define ROW 10
int print_board(int (*board)[10]) {
#define A 10+B
#define B 10+A
    int a = A;
    int b = B;
  for (int i = 0; i < ROW; i++) {
    for (int j = 0; j < 10; j++)
      if (board[i][j])
	printf("Q ");
      else
	printf(". ");
    printf("\n");
  }
  printf("\n\n");
}

int conflict(int (*board)[10], int row, int col) {
  for (int i = 0; i < row; i++) {
    if (board[i][col])
      return 1;
    int j = row - i;
    if (0 < col - j + 1 && board[i][col - j])
      return 1;
    if (col + j < 10 && board[i][col + j])
      return 1;
  }
  return 0;
}

int solve(int (*board)[10], int row) {
  if (row > 9) {
    print_board(board);
    return 0;
  }
  for (int i = 0; i < 10; i++) {
    if (!conflict(board, row, i)) {
      board[row][i] = 1;
      solve(board, row + 1);
      board[row][i] = 0;
    }
  }
}

int main() {
  int board[100];
  for (int i = 0; i < 100; i++)
    board[i] = 0;
  solve(board, 0);
  return 0;
}
