#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "winfunc.h"

int main() {
    char column;
    int row;
    char your_color;

    initialize_board();
    your_color = BLACK; //you are black stone

    while(1) {
        print_board();
        if (current_stone == your_color) {
            printf("Enter coordinates (e.g., h8): ");
            scanf(" %c%d", &column, &row);
            add_stone(column, row, current_stone);
        } else {
            add_stone_computer();
        }
	current_stone = (current_stone == BLACK) ? WHITE : BLACK; //flip the color of the stone
        if (checkWin(current_stone)){
	    printf("Win");
	    break;

	}
    } 
    return 0;
}

