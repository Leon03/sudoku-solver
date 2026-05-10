#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 9

typedef int Board[SIZE][SIZE];

static int row_mask[SIZE];
static int col_mask[SIZE];
static int box_mask[SIZE];

static inline int box_index(int r, int c) {
    return (r / 3) * 3 + (c / 3);
}

static void init_masks(Board board) {
    for (int i = 0; i < SIZE; i++) {
        row_mask[i] = 0;
        col_mask[i] = 0;
        box_mask[i] = 0;
    }
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            int v = board[r][c];
            if (v != 0) {
                row_mask[r] |= 1 << v;
                col_mask[c] |= 1 << v;
                box_mask[box_index(r, c)] |= 1 << v;
            }
        }
    }
}

static int solve_recursive(Board board, int pos) {
    if (pos == SIZE * SIZE) return 1;

    int r = pos / SIZE;
    int c = pos % SIZE;

    if (board[r][c] != 0) return solve_recursive(board, pos + 1);

    int b = box_index(r, c);
    int used = row_mask[r] | col_mask[c] | box_mask[b];

    for (int n = 1; n <= SIZE; n++) {
        int bit = 1 << n;
        if (used & bit) continue;

        board[r][c] = n;
        row_mask[r] |= bit;
        col_mask[c] |= bit;
        box_mask[b] |= bit;

        if (solve_recursive(board, pos + 1)) return 1;

        board[r][c] = 0;
        row_mask[r] &= ~bit;
        col_mask[c] &= ~bit;
        box_mask[b] &= ~bit;
    }

    return 0;
}

static int solve(Board board) {
    init_masks(board);
    return solve_recursive(board, 0);
}

static void print_board(Board board) {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            printf("%c ", board[r][c] ? ('0' + board[r][c]) : '.');
            if (c == 2 || c == 5) printf("| ");
        }
        printf("\n");
        if (r == 2 || r == 5) printf("------+------+------\n");
    }
}

static int parse_81(const char *s, Board board) {
    if (strlen(s) != 81) return -1;
    for (int i = 0; i < 81; i++) {
        char ch = s[i];
        if (ch == '.' || ch == '_' || ch == '0' || ch == '*') {
            board[i / SIZE][i % SIZE] = 0;
        } else if (ch >= '1' && ch <= '9') {
            board[i / SIZE][i % SIZE] = ch - '0';
        } else {
            return -1;
        }
    }
    return 0;
}

static int parse_lines(Board board) {
    char line[256];
    int row = 0;

    while (row < SIZE && fgets(line, sizeof(line), stdin)) {
        int col = 0;
        for (int i = 0; line[i] && col < SIZE; i++) {
            char ch = line[i];
            if (ch >= '1' && ch <= '9') {
                board[row][col++] = ch - '0';
            } else if (ch == '.' || ch == '0' || ch == '*' || ch == '-' || ch == '_') {
                board[row][col++] = 0;
            }
        }
        if (col == SIZE) row++;
        else if (col > 0) {
            fprintf(stderr, "Error: row %d has %d cells (expected 9)\n", row + 1, col);
            return -1;
        }
    }

    if (row != SIZE) {
        fprintf(stderr, "Error: need exactly 9 rows, got %d\n", row);
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    Board board = {0};

    if (argc == 2) {
        if (strlen(argv[1]) == 81 && parse_81(argv[1], board) == 0) {
            /* 81-char string mode */
        } else {
            fprintf(stderr, "Usage:\n");
            fprintf(stderr, "  %s <81-char string>\n", argv[0]);
            fprintf(stderr, "  %s (then enter 9 lines)\n", argv[0]);
            fprintf(stderr, "\n81-char: digits 1-9 and . for empty, e.g.:\n");
            fprintf(stderr, "  530070000600195000098000060800060003400803001700020006060000280000419005000080079\n");
            return 1;
        }
    } else {
        if (parse_lines(board) != 0) return 1;
    }

    printf("原始数独:\n");
    print_board(board);

    if (!solve(board)) {
        printf("\n无解\n");
        return 1;
    }

    printf("\n求解结果:\n");
    print_board(board);

    return 0;
}