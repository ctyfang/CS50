#include <stdio.h>
#include <cs50.h>

#define DIM_MIN 3
#define DIM_MAX 9
#define TRUE 1
#define FALSE 0

int board[DIM_MAX][DIM_MAX];

void draw(int d)
{
    for(int i = 0; i < d; i++){
        for(int j = 0; j < d; j++){
            
         printf("%i\t", board[i][j]);   
            
        }
        printf("\n");
    }
}

void init(int d) // Pass in the dimension of the matrix
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

int main(int argc, char* argv[]){
    
    int d = atoi(argv[1]);
    
    init(d);
    draw(d);
    
    printf("Enter test tile to find:");
    int tile = GetInt();
    
    bool found = FALSE;
    int curr_row = -1;
    int curr_col = 0;
    
    while(!found){
        curr_row++;
        
        for(curr_col = 0; (curr_col < d) && !found; curr_col++){
            
            if(board[curr_row][curr_col] == tile){
                found = TRUE;
                break;
            }
            
        }
        
        
    }

    printf("Tile selected is on row %i, column %i\n", curr_row, curr_col);
    
    return 0;
}