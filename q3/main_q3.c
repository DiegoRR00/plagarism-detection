/*
    INF-4202B - Applied Algorithms: Strings and Images
    TDm 2: Algorithms for sequence processing: Optimal alignment and plagiarism detection software
    Question 3
    @authors Diego REYNA REYES
    @authors Ryan VAN GREUNEN
    @date 14/01/2023
    Made in Ile-de-France for the INF-4202B course given in ESIEE Paris.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include "TD2_q3.c"

#define INS_X_COST 1
#define INS_Y_COST 1
#define TOP 0
#define LEFT 1
#define DIAGONAL 2

#define INF 2147483647
#define STR_SIZE 4096
#define SPECIAL_CHAR " "

#define NUM_PARA 2
#define CHAR_PER_LINE 65

/*
    Reverses a string, substitutes strrev present on windows gcc
    @param x: A string
    @returns A copy of string x reversed
*/
char* reverse (char* x) {
    int n = strlen(x);
    char *x2 = (char*)malloc(sizeof(char) * (n+1));
    if (x2 == NULL){   
        fprintf(stderr,"reverse() : malloc failed for x2\n");
        exit(0);
    }
    for (int i = 0; i < n; i++){
        x2[i] = x[n - i -1];
    }
    //Add end char
    x2[n] = '\0';
    return x2;
}
/*
    Returns the cost for the function Ins(x[i],y[i])
    @param xi: A char value
    @param yi: A char value
    @returns (int) xi != yi
*/
int insXY(char xi, char yi){
    return (int)xi != yi;
}
/*
    Prints the allignment score table
    @param T: int[][] Table (N+1xM+1) containing the scores
    @param x: A string of size N
    @param y: A string of size M
*/
void print_table(int** T, char* x, char* y){
    char* end_text = "\t|\t";
    //Print corner
    printf(" %s@%s",end_text,end_text);
    int n = strlen(x);
    int m = strlen(y);
    //Print every char of a at the beggining
    for (int i = 0; i < m; i++){
        printf("%c%s",y[i],end_text);
    }
    printf("\n");
    //Print line between header and table
    for (int i = 0; i < m + 2; i++){
        printf("-\t-\t");
    }
    printf("\n");
    //Print the value with the char of a at the first column
    for (int i = 0; i < n + 1; i++){
        if (i==0){
            printf("@%s",end_text);
        }
        else{
            printf("%c%s",x[i-1],end_text);
        }
        for (int j = 0; j < m + 1; j++){
            printf("%i%s",T[i][j],end_text);
        }
        printf("\n");
    }
}
/*
    Obtains the optimal score of allignment for every substring x_i, y_j
    @param x: A string of size N
    @param y: A string of size M
    @returns Table with optimal scores of size (N+1xM+1)
*/
int** optimal_table(char* x, char* y){
    int ** T = NULL;
    int *p;
    int n = strlen(x);
    int m = strlen(y);
    //Assign memory space
    T = (int **)malloc((n + 1) * sizeof(int *));
    if (T == NULL){   
        fprintf(stderr,"optimal_table() : malloc failed for T\n");
        exit(0);
    }
    for (int i = 0; i < n + 1; i++){
        p = (int *)malloc((m+1) * sizeof(int));
        if (p == NULL){   
            fprintf(stderr,"optimal_table() : malloc failed for p\n");
            exit(0);
        }
        T[i] = p;
    }
    //Start algorithm
    T[0][0] = 0;
    //Initialize first column
    for (int i = 1; i < n + 1; i++){
        T[i][0] = T[i-1][0] + INS_X_COST;   
    }
    //Initialiaze first row
    for (int j = 1; j < m + 1; j++){
        T[0][j] = T[0][j - 1] + INS_Y_COST;
    }
    //Build the table
    for (int i = 1; i < n + 1; i++){
        for (int j = 1; j < m + 1; j++){
            T[i][j] = Imin3(T[i-1][j] + INS_X_COST,
                            T[i][j-1] + INS_Y_COST,
                            T[i-1][j-1] + insXY(x[i-1],y[j-1]));
        }
    }
    return T;
}
/*
    Reconstructs the optimal allignment stretch for x and y and displays it
    @param T: int[][] Table (N+1xM+1) containing the scores
    @param x: A string of size N
    @param y: A string of size M
    @param i: Length of the prefix of x to be used for the reconstruction
    @param j: Length of the prefix of y to be used for the reconstruction
*/
void reconstruct(int** T, char* x, char* y, int i, int j){    
    char x_prime[STR_SIZE] = "";
    char y_prime[STR_SIZE] = "";
    if (i == 0){
        i = 1;
    }
    if (j == 0){
        j = 1;
    }
    int k = i;
    int h = j;
    int temp[3] = {0,0,0};
    int top,left,diag;
    //Create until we reach the (0,0) value
    while (k >= 0 && h >= 0){
        //If there's a neighbour at the left
        if (h > 0){
            temp[LEFT] = T[k][h-1];
            left = T[k][h-1];
        }
        else{
            temp[LEFT] = INF;
            left = INF;
        }
        //If there's a neighbour at the top
        if (k > 0){
            temp[TOP] = T[k-1][h];
            top = T[k-1][h];
        }
        else{
            temp[TOP] = INF;
            top = INF;
        }
        //If there's a diagonal neighbour
        if (k > 0 && h > 0){
            temp[DIAGONAL] = T[k-1][h-1];
            diag = T[k-1][h-1];
        }
        else{
            temp[DIAGONAL] = INF;
            diag = INF;
        }
        //Find the smallest score of the neighbours
        int min_val = Imin3(temp[0],temp[1],temp[2]);
        //Decide where to jump
        if (k == 0 && k==h){
            k--;
            h--;
        }
        else if (min_val == diag){
            //Jump diagonal
            if (min_val == left && min_val == top){
                strncat(x_prime,&x[k-1],1);
                
                strncat(y_prime,&y[h-1],1);
                h--;
                k--;
            }
            else if (min_val == top){
                //Jump diagonal
                if (x[k-1] == y[h-1]){
                    
                    strncat(x_prime,&x[k-1],1);
                    
                    strncat(y_prime,&y[h-1],1);
                    h--;
                    k--;
                }
                //Jump top
                else{
                    
                    strncat(x_prime,&x[k-1],1);
                    strcat(y_prime,SPECIAL_CHAR);
                    k--;
                }
            }
            else if (min_val == left){
                //Jump diagonal
                if (x[k-1] == y[h-1]){
                    
                    strncat(x_prime,&x[k-1],1);
                    
                    strncat(y_prime,&y[h-1],1);
                    h--;
                    k--;
                }
                //Jump left
                else{
                    strcat(x_prime,SPECIAL_CHAR);
                    
                    strncat(y_prime,&y[h-1],1);
                    h--;
                }
            }
            //Jump diagonal
            else{
                
                strncat(x_prime,&x[k-1],1);
                
                strncat(y_prime,&y[h-1],1);
                h--;
                k--;
            }
        }
        else if (min_val == top){
            if (min_val == left){
                //Jump top
                if (k > h){
                    
                    strncat(x_prime,&x[k-1],1);
                    strcat(y_prime,SPECIAL_CHAR);
                    k--;
                }
                //Jump left
                else{
                    strcat(x_prime,SPECIAL_CHAR);
                    
                    strncat(y_prime,&y[h-1],1);
                    h--;
                }
            }
            else{
                //Jump top
                
                strncat(x_prime,&x[k-1],1);
                strcat(y_prime,SPECIAL_CHAR);
                k--;
            }
        }
        //Jump left
        else{
            strcat(x_prime,SPECIAL_CHAR);
            
            strncat(y_prime,&y[h-1],1);
            h--;
        }
    }
    char* x_final = reverse(x_prime);
    char* y_final = reverse(y_prime);
    affiche(x_final,y_final,CHAR_PER_LINE);
}
/*
    Obtains the similarity score
    @param all_score: Score of the allignment
    @param x_len: Length of string x
    @param y_len: Length of string y
    @returns Percentage of similarity
*/
float similarity_score(int all_score,int x_len, int y_len){
    return (1.0 - (float)all_score/((float)x_len + (float)y_len))*100.0;
}
/*
    Computes and displays the strecth between strings x and y
    @param x: A string
    @param y: A string
*/
void get_stretch(char* x, char* y, int print_score){
    int** T;
    T = optimal_table(x,y);
    if (print_score == 1){
        int len_x = strlen(x);
        int len_y = strlen(y);
        printf("Score of allignment: %i.\nSize of text 1: %i\nSize of text 2: %i\nSimilarity score: %.2f %%\n",T[len_x][len_y],len_x,len_y,similarity_score(T[len_x][len_y],len_x,len_y));
    }
    reconstruct(T,x,y,strlen(x),strlen(y));
}
/*
    Finds and displays the match between the text stored in argv[1] and argv[1]
*/
int main(int argc, char **argv){
    char *x, *y; 
    //If not neough parameters where given
    if(argc != 3){
        printf("usage: %s text1 text2\n", argv[0]);
        exit(0);
    }  
    //Read our input files
    x = readtextfile(argv[1]);
    y = readtextfile(argv[2]);
    //Match them
    get_stretch(x,y,1);
    free(x);
    free(y);
}
