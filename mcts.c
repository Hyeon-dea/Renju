#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#define SIZE 7
#define WIN_SCORE 1000000
#define DRAW_SCORE 0

typedef struct {
    int board[SIZE][SIZE];
    int current_stone;
    int last_move[2];
} RenjuState;

typedef struct RenjuNode {
    RenjuState state;
    int visits;
    int wins;
    struct RenjuNode* parent;
    struct RenjuNode** children;
    int num_children;
} RenjuNode;

RenjuNode* create_node(RenjuState state, RenjuNode* parent);
RenjuNode* select_node(RenjuNode* node);
void expand(RenjuNode* node);
RenjuNode* select_child(RenjuNode* node);
int simulate(RenjuState state);
bool is_terminal(RenjuState state);
bool is_board_full(RenjuState state);
bool is_winner(RenjuState state, int stone);
bool is_valid_position(int x, int y);
int get_result(RenjuState state);
void backpropagate(RenjuNode* node, int result);
void free_node(RenjuNode* node);
int get_best_move_coordinates(int board[SIZE][SIZE]);

RenjuNode* create_node(RenjuState state, RenjuNode* parent) {
    RenjuNode* node = (RenjuNode*)malloc(sizeof(RenjuNode));
    node->state = state;
    node->visits = 0;
    node->wins = 0;
    node->parent = parent;
    node->children = NULL;
    node->num_children = 0;
    return node;
}

RenjuNode* select_node(RenjuNode* node) {
    while (node->num_children > 0) {
        int best_child_index = 0;
        double best_uct = -1;
        for (int i = 0; i < node->num_children; i++) {
            RenjuNode* child = node->children[i];
            double uct = (double)child->wins / child->visits +
                         2.0 * sqrt(log(node->visits) / child->visits);
            if (uct > best_uct) {
                best_uct = uct;
                best_child_index = i;
            }
        }
        node = node->children[best_child_index];
    }
    return node;
}

void expand(RenjuNode* node) {
    RenjuState state = node->state;
    int num_legal_moves = 0;
    int legal_moves[SIZE * SIZE][2];
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            if (state.board[x][y] == 0) {
                legal_moves[num_legal_moves][0] = x;
                legal_moves[num_legal_moves][1] = y;
                num_legal_moves++;
            }
        }
    }
    node->num_children = num_legal_moves;
    node->children = (RenjuNode**)malloc(num_legal_moves * sizeof(RenjuNode*));
    for (int i = 0; i < num_legal_moves; i++) {
        int x = legal_moves[i][0];
        int y = legal_moves[i][1];
        RenjuState new_state = state;
        new_state.board[x][y] = state.current_stone;
        new_state.current_stone = -state.current_stone;
        new_state.last_move[0] = x;
        new_state.last_move[1] = y;
        node->children[i] = create_node(new_state, node);
    }
}

RenjuNode* select_child(RenjuNode* node) {
    int random_index = rand() % node->num_children;
    return node->children[random_index];
}

int simulate(RenjuState state) {
    while (!is_terminal(state)) {
        int legal_moves[SIZE * SIZE][2];
        int num_legal_moves = 0;
        for (int x = 0; x < SIZE; x++) {
            for (int y = 0; y < SIZE; y++) {
                if (state.board[x][y] == 0) {
                    legal_moves[num_legal_moves][0] = x;
                    legal_moves[num_legal_moves][1] = y;
                    num_legal_moves++;
                }
            }
        }
        int random_index = rand() % num_legal_moves;
        int x = legal_moves[random_index][0];
        int y = legal_moves[random_index][1];
        state.board[x][y] = state.current_stone;
        state.current_stone = -state.current_stone;
        state.last_move[0] = x;
        state.last_move[1] = y;
    }
    return get_result(state);
}

bool is_terminal(RenjuState state) {
    return is_winner(state, 1) || is_winner(state, -1) || is_board_full(state);
}

bool is_board_full(RenjuState state) {
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            if (state.board[x][y] == 0) {
                return false;
            }
        }
    }
    return true;
}

bool is_winner(RenjuState state, int stone) {
    int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
            if (state.board[x][y] != stone) {
                continue;
            }
            for (int i = 0; i < 4; i++) {
                int dx = directions[i][0];
                int dy = directions[i][1];
                int count = 1;
                int nx = x + dx;
                int ny = y + dy;
                while (is_valid_position(nx, ny) && state.board[nx][ny] == stone) {
                    count++;
                    nx += dx;
                    ny += dy;
                }
                if (count >= 5) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool is_valid_position(int x, int y) {
    return x >= 0 && x < SIZE && y >= 0 && y < SIZE;
}

int get_result(RenjuState state) {
    if (is_winner(state, 1)) {
        return WIN_SCORE;
    } else if (is_winner(state, -1)) {
        return -WIN_SCORE;
    } else {
        return DRAW_SCORE;
    }
}

void backpropagate(RenjuNode* node, int result) {
    while (node != NULL) {
        node->visits++;
        node->wins += result;
        node = node->parent;
    }
}

void free_node(RenjuNode* node) {
    if (node == NULL) {
        return;
    }
    for (int i = 0; i < node->num_children; i++) {
        free_node(node->children[i]);
    }
    free(node->children);
    free(node);
}

int get_best_move_coordinates(int board[SIZE][SIZE]) {
    
    printf("---------in---------");
    
    RenjuState state;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            state.board[i][j] = board[i][j];
        }
    }
    state.current_stone = 1;
    state.last_move[0] = -1;
    state.last_move[1] = -1;

    RenjuNode* root = create_node(state, NULL);
    srand(time(NULL));

    const int iterations = 10000;
    for (int i = 0; i < iterations; i++) {
        RenjuNode* selected_node = select_node(root);
        expand(selected_node);
        RenjuNode* new_node = select_child(selected_node);
        int result = simulate(new_node->state);
        backpropagate(new_node, result);
    }

    int best_index = 0;
    int best_score = -1;
    for (int i = 0; i < root->num_children; i++) {
        RenjuNode* child = root->children[i];
        int score = child->wins / child->visits;
        if (score > best_score) {
            best_score = score;
            best_index = i;
        }
    }

    int best_x = root->children[best_index]->state.last_move[0];
    int best_y = root->children[best_index]->state.last_move[1];

    free_node(root);

    return best_x * SIZE + best_y;
}

int main() {
    int board[SIZE][SIZE] = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, -1, 0, 0},
        {0, 0, 0, 1, -1, 0, 0},
        {0, 0, 0, 1, -1, 0, 0},
        {0, 0, 0, 1, -1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0}
    };

    int best_move = get_best_move_coordinates(board);
    printf("-------");
    int x = best_move / SIZE;
    int y = best_move % SIZE;
    printf("Best move: (%d, %d)\n", x, y);
    return 0;
}
