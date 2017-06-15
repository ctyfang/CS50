/**
 * fifteen.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */
 
#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9
#define TRUE 1
#define FALSE 0

// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

// indices
int curr_col;
int curr_row;

// prototypes
int check_right(void);
int check_left(void);
int check_below(void);
int check_above(void);
int move_right(void);
int move_left(void);
int move_below(void);
int move_above(void);
void clear(void);
void greet(void);
void init(void);
bool move(int);
void draw(void);
int won(void);

int main(int argc, string argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE* file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won())
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = GetInt();
        
        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(250000);
        }

        // sleep thread for animation's sake
        usleep(500000);
    }
    
    // close log
    fclose(file);

    // success
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(3000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).  
 */
void init(void) // Pass in the dimension of the matrix
{
    int num_tiles = d*d-1; // Calculate the highest number tile
    
    for( int i = 0; i < d; i++){ // For every row...
        
        for( int j = 0; j < d; j++){ // For every column of that row...
            
            if( num_tiles > 2 ){ // If not on the last row and
                                           // not in the last, second last
                                           // or third last position of
                                           // the last row...
                
                board[i][j] = num_tiles; // Initialize current position
                                          // with value of num_tiles
                num_tiles--; // decrement num_tiles
                
            }
            else{ // If on the last row
                
                if(d%2 == 0){ // Check if board dimensions are even
                    
                    if(j == (d-3))
                        board[i][j] = 1;
                    else if(j == (d-2))
                        board[i][j] = 2;
                    else
                        board[i][j] = 0;
                  
                }
                else{
                    
                    if(j == (d-3))
                        board[i][j] = 2;
                    else if(j == (d-2))
                        board[i][j] = 1;
                    else
                        board[i][j] = 0;
                        
                }
            }
        }
    }
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    for(int i = 0; i < d; i++){
        for(int j = 0; j < d; j++){
            
         printf("%i\t", board[i][j]);   
            
        }
        printf("\n");
    }
}

int check_right(void) {
	
	if (board[curr_row][curr_col+1] == 0)
		return 1;
	else
		return 0;

}

int check_left(void) {
	
	if(board[curr_row][curr_col-1] == 0)
		return 1;
	else
		return 0;

}

int check_above(void) {
    clear();
    draw();
    
	if (board[curr_row-1][curr_col] == 0){
	    //printf("Curr row is %i, curr col is %i\n", curr_row, curr_col);
	    //printf("Above val was %i\n", board[curr_row+1][curr_col]);
	    //usleep(5000000);
		return 1;
	}
	else
		return 0;

}

int check_below(void) {

	if (board[curr_row+1][curr_col] == 0)
		return 1;
	else
		return 0;

}

int check_corners(void) {

	if ((curr_row == 0) && (curr_col == 0))
		return 1;
	else if ((curr_row == 0) && (curr_col == d-1))
		return 2;
	else if ((curr_row == d-1) && (curr_col == d-1))
		return 4;
	else if ((curr_row == d-1) && (curr_col == 0))
		return 3;
	else
		return 0;
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int tile) // 
{
    // Finds row and col indices of tile value passed in
    // Checks if it borders empty space
    // Responds accordingly
    
    bool found = FALSE;
    curr_row = -1;
    curr_col = 0;
    
    while(!found){
        curr_row++;
        
        for(curr_col = 0; ((curr_col < d) && (!found)); curr_col++){
            
            if(board[curr_row][curr_col] == tile){
                found = TRUE;
                break;
            }
            
        }
    }
    
    printf("Tile selected is on row %i, column %i\n", curr_row, curr_col);
    
    // Middle section
    if(((curr_col > 0) && (curr_col < d-1)) && ((curr_row > 0) && (curr_row < d-1))){
        if(check_left()){ // Check left
            board[curr_row][curr_col-1] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else if(check_right()){  // Check right
            board[curr_row][curr_col+1] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else if(check_below()){  // Check below
            board[curr_row+1][curr_col] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else if(check_above()){  // Check above
            board[curr_row-1][curr_col] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else
            return false;
    }
    else if(((curr_col > 0) && (curr_col < d-1)) && (curr_row == 0)){ // Top middle
        if(check_left()){ // Check left
            board[curr_row][curr_col-1] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else if(check_right()){  // Check right
            board[curr_row][curr_col+1] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else if(check_below()){  // Check below
            board[curr_row+1][curr_col] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else
            return false;
    }
    else if(((curr_col > 0) && (curr_col < d-1)) && (curr_row == d-1)){ // Bottom middle
        
        printf("Bottom middle\n");
        
        if(check_left()){ // Check left
            printf("Left\n");
            board[curr_row][curr_col-1] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else if(check_right()){  // Check right
            printf("Right\n");
            board[curr_row][curr_col+1] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else if(check_above()){  // Check above
            //printf("Above\n");
            board[curr_row-1][curr_col] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else
            return false;
    }
    else if(curr_col == 0 && ((curr_row > 0) && (curr_row < d-1))){ // Left middle
        if(check_right()){ // Check right
            board[curr_row][curr_col+1] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else if(check_below()){  // Check below
            board[curr_row+1][curr_col] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else if(check_above()){  // Check above
            board[curr_row-1][curr_col] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else
            return false;
    }
    else if((curr_col == d-1) && ((curr_row > 0) && (curr_row < d-1))){ // Right middle
        if(check_left()){ // Check left
            board[curr_row][curr_col-1] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else if(check_below()){  // Check below
            board[curr_row+1][curr_col] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else if(check_above()){  // Check above
            board[curr_row-1][curr_col] = tile;
            board[curr_row][curr_col] = 0;
            return true;
        }
        else
            return false;
    }
    else{
        int corner_flag = check_corners();
        //printf("You entered a corner\n");

        if (corner_flag == 1) {
            //printf("Top left corner\n");
        	if(check_right()){
        	    //printf("White space is to the right\n");
        	    //printf("Tile selected is on row %i, column %i\n", curr_row, curr_col);
        	    //usleep(5000000);
        	    board[curr_row][curr_col+1] = tile;
                board[curr_row][curr_col] = 0;
                return true;
        	}
        	else if(check_below()){
        	    //printf("White space is to the bottom\n");
        	    //printf("Tile selected is on row %i, column %i\n", curr_row, curr_col);
        	    //usleep(5000000);
        	    board[curr_row+1][curr_col] = tile;
                board[curr_row][curr_col] = 0;
                return true;
        	}
        	else
        	    return false;
        }
        
        else if (corner_flag == 2) {
        
        	if(check_left()){
        	    board[curr_row][curr_col-1] = tile;
                board[curr_row][curr_col] = 0;
                return true;
        	}
        	else if(check_below()){
        	    board[curr_row+1][curr_col] = tile;
                board[curr_row][curr_col] = 0;
                return true;
        	}
        	else
        	    return false;
        }
        
        else if (corner_flag == 3) {
        
        	if(check_above()){
        	    board[curr_row-1][curr_col] = tile;
                board[curr_row][curr_col] = 0;
                return true;
        	}
        	else if(check_right()){
        	    board[curr_row][curr_col+1] = tile;
                board[curr_row][curr_col] = 0;
                return true;
        	}
        	else
        	    return false;
        }
        
        else {
        
        	if(check_left()){
        	    board[curr_row][curr_col-1] = tile;
                board[curr_row][curr_col] = 0;
                return true;
        	}
        	else if(check_above()){
        	    board[curr_row-1][curr_col] = tile;
                board[curr_row][curr_col] = 0;
                return true;
        	}
        	else
        	    return false;
        }
    }
    
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
int won(void){
    int win = TRUE;
    int curr_tile = 1;
    
    for(int i = 0; i < d; i++){
        for(int j=0; j < d; j++){
            
            if((i == d-1) && (j == d-1)){
                //printf("Currently at zero..\n");
                return TRUE;
                
            }
            printf("Countdown is at %i, current tile is %i\n", curr_tile, board[i][j]);
            if(board[i][j] != curr_tile){
                win = FALSE;
                printf("Didn't win yet..\n");
                break;
            }
            
            curr_tile++;
            
        }
        
        if(win == FALSE)
            break;
        
    }
    return FALSE;
}