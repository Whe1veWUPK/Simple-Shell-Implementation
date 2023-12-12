#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>


//Out put the command line prompt
void print_prompt(){

}
int main(){
    char *input; // User input
    while(1){
        input = readline("Please Enter a command:");
        printf("You entered: %s\n", input);
        break;
    }
    return 0;
}
