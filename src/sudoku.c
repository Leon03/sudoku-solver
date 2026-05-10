#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define SIZE 9
#define EMPTY 0
#define ALL_BITS 0x3FE  /* bits 1-9 set: 0b1111111110 */

typedef int Board[SIZE][SIZE];

static int row_mask[SIZE];
static int col_mask[SIZE];
static int box_mask[SIZE];

/* board 参数用于 find_mrv 判断空格，mask 只用于约束传播 */
static inline int box_of(int r, int c) {
    return (r / 3) * 3 + (c / 3);
}

static void init_masks(Board board) {
    for (int i = 0; i < SIZE; i++) {
        row_mask[i] = col_mask[i] = box_mask[i] = 0;
    }
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            int v = board[r][c];
            if (v != EMPTY) {
                row_mask[r] |= 1 << v;
                col_mask[c] |= 1 << v;
                box_mask[box_of(r, c)] |= 1 << v;
            }
        }
    }
}

/* 返回 (r,c) 处可选候选数的位掩码 */
static inline int candidates(int r, int c) {
    return ~(row_mask[r] | col_mask[c] | box_mask[box_of(r, c)]) & ALL_BITS;
}

/* MRV: 找候选数最少的空格 */
static int find_mrv(Board board, int *br, int *bc, int *bbits) {
    int min_cnt = INT_MAX;
    int found = 0;

    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (board[r][c] != EMPTY) continue;  /* 跳过已填格子 */

            int bits = candidates(r, c);
            int cnt = __builtin_popcount(bits);
            if (cnt == 0) return -1;   /* 死路 */
            if (cnt < min_cnt) {
                min_cnt = cnt;
                *br = r; *bc = c; *bbits = bits;
                found = 1;
            }
        }
    }
    return found ? 0 : 1;  /* 0=继续搜索, 1=全部填完 */
}

static int solve_recursive(Board board) {
    int r, c, bits;

    int st = find_mrv(board, &r, &c, &bits);
    if (st == 1) return 1;   /* 所有格子已填满 */
    if (st == -1) return 0;  /* 死路 */

    int b = box_of(r, c);

    while (bits) {
        int n = __builtin_ctz(bits);   /* 最低位的 1 对应候选数字 n */
        bits &= bits - 1;              /* 清除该位 */
        int bit = 1 << n;

        board[r][c] = n;
        row_mask[r] |= bit;
        col_mask[c] |= bit;
        box_mask[b] |= bit;

        if (solve_recursive(board)) return 1;

        board[r][c] = EMPTY;
        row_mask[r] &= ~bit;
        col_mask[c] &= ~bit;
        box_mask[b] &= ~bit;
    }

    return 0;
}

static int solve(Board board) {
    init_masks(board);
    return solve_recursive(board);
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
            board[i / SIZE][i % SIZE] = EMPTY;
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
                board[row][col++] = EMPTY;
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