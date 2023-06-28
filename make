import numpy as np
from random import choice

# Constants
MCTS_SIMULATIONS = 1000  # Number of Monte Carlo simulations
WIN_SCORE = 1  # Score assigned to a winning move
DRAW_SCORE = 0  # Score assigned to a draw move

class RenjuNode:
    def __init__(self, state, parent=None):
        self.state = state
        self.parent = parent
        self.children = []
        self.wins = 0
        self.visits = 0

    def is_fully_expanded(self):
        return len(self.children) == len(self.state.get_legal_moves())

    def select_child(self):
        exploration_factor = 1.4  # Exploration factor to balance exploration and exploitation

        scores = [child.wins / child.visits + exploration_factor * np.sqrt(2 * np.log(self.visits) / child.visits)
                  for child in self.children]
        return self.children[np.argmax(scores)]

    def expand(self):
        untried_moves = [move for move in self.state.get_legal_moves() if move not in self.get_child_moves()]
        move = choice(untried_moves)
        new_state = self.state.play(move)
        child = RenjuNode(new_state, self)
        self.children.append(child)
        return child

    def update(self, result):
        self.visits += 1
        self.wins += result

    def get_child_moves(self):
        return [child.state.last_move for child in self.children]

    def get_best_move(self):
        best_child = max(self.children, key=lambda c: c.visits)
        return best_child.state.last_move

class RenjuMCTS:
    def __init__(self, size, current_stone):
        self.size = size
        self.current_stone = current_stone
        self.root = None

    def search(self, board):
        self.root = RenjuNode(RenjuState(board, self.current_stone))

        for _ in range(MCTS_SIMULATIONS):
            node = self.select_node()
            result = self.simulate(node.state)
            self.backpropagate(node, result)

        best_move = self.root.get_best_move()
        return best_move

    def select_node(self):
        node = self.root
        while node.is_fully_expanded():
            if not node.children:
                break
            node = node.select_child()
        if not node.is_fully_expanded() and node.visits > 0:
            node = node.expand()
        return node

    def simulate(self, state):
        current_state = state
        while not current_state.is_terminal():
            move = choice(current_state.get_legal_moves())
            current_state = current_state.play(move)
        return current_state.get_result()

    def backpropagate(self, node, result):
        while node is not None:
            node.update(result)
            result = -result  # Switch the result sign for each player
            node = node.parent

# RenjuState class represents the state of the Renju game
class RenjuState:
    def __init__(self, board, current_stone):
        self.board = np.copy(board)
        self.current_stone = current_stone
        self.last_move = None
        self.size = SIZE

    def get_legal_moves(self):
        moves = []
        for x in range(self.size):
            for y in range(self.size):
                if self.board[x][y] == 0:
                    moves.append((x, y))
        return moves

    def play(self, move):
        new_state = RenjuState(np.copy(self.board), -self.current_stone)
        new_state.board[move[0]][move[1]] = self.current_stone
        new_state.last_move = move
        return new_state

    def is_terminal(self):
        # Check for winning conditions
        if self.check_row_win() or self.check_column_win() or self.check_diagonal_win():
            return True
        # Check for a draw
        return len(self.get_legal_moves()) == 0

    def check_row_win(self):
        for x in range(self.size):
            for y in range(self.size - 4):
                if self.board[x][y] == self.board[x][y+1] == self.board[x][y+2] == self.board[x][y+3] == self.board[x][y+4] != 0:
                    return True
        return False

    def check_column_win(self):
        for y in range(self.size):
            for x in range(self.size - 4):
                if self.board[x][y] == self.board[x+1][y] == self.board[x+2][y] == self.board[x+3][y] == self.board[x+4][y] != 0:
                    return True
        return False

    def check_diagonal_win(self):
        for x in range(self.size - 4):
            for y in range(self.size - 4):
                if self.board[x][y] == self.board[x+1][y+1] == self.board[x+2][y+2] == self.board[x+3][y+3] == self.board[x+4][y+4] != 0:
                    return True
        for x in range(self.size - 4):
            for y in range(4, self.size):
                if self.board[x][y] == self.board[x+1][y-1] == self.board[x+2][y-2] == self.board[x+3][y-3] == self.board[x+4][y-4] != 0:
                    return True
        return False

    def get_result(self):
        if self.check_row_win() or self.check_column_win() or self.check_diagonal_win():
            return WIN_SCORE * self.current_stone
        return DRAW_SCORE

# Example usage
SIZE = 9
current_stone = -1  # 1 for black, -1 for white
#board = np.zeros((SIZE, SIZE), dtype=int)
board = np.array([
    [0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 1, -1, 0, 0, 0],
    [0, 0, 0, 0, 1, -1, 0, 0, 0],
    [0, 0, 0, 0, 1, -1, 0, 0, 0],
    [0, 0, 0, 0, 1, -1, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0]
])
mcts = RenjuMCTS(SIZE, current_stone)
best_move = mcts.search(board)
print("Best Move:", best_move)
