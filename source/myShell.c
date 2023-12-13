#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>


//Out put the command line prompt
void print_prompt(){
    char *user_name = getlogin(); //the user nmae 
    char host_name[256];
    gethostname(host_name, sizeof(host_name)); //get host name
    char cwd[4096];
    getcwd(cwd, sizeof(cwd));//get current working directory
    int isRootUser = getuid(); //get user id to determine whether he is the root user
    char userType;
    if(!isRootUser){
        //root user
        userType = '#';
    }
    else{
        //non-root user
        userType = '$';
    }
    printf("\033[0;35m%s@%s:%s%c\033[0m", user_name, host_name, cwd, userType);//print the command line prompt
}
//get user's command
void get_command(int argc,char**argv,char*command){

    const char delimiter = ' ';
    for (int j = 0; j < (argc);++j){
        if(j==0){
            strcpy(argv[0], strtok(command, &delimiter));
            //printf("%s\n", argv[0]);
        }
        else{
            strcpy(argv[j], strtok(NULL, &delimiter));
            //printf("%s\n", argv[j]);
        }
    }
}

//deal the command
void deal_command(){

}
int main(){

    while(1){
        char *command; // User input
        
        print_prompt();// display the command line prompt

        int argc = 0;
        char **argv;
        const char delimiter = ' ';

        command = readline("");
        char temp[4096];
        strcpy(temp, command);        
        char *t=strtok(temp,&delimiter);
        while(t!=NULL){
           //calculate argc   
           ++argc;
           t = strtok(NULL, &delimiter);
        }

        argv = (char **)malloc(argc * sizeof(char *)); //initial argv
        for (int i = 0; i < argc;++i){
            argv[i] = (char *)malloc(strlen(command) + 1);
        }

        get_command(argc, argv, command); // get stoken


        //free the memory
        for (int i = 0; i < argc;++i){
            free(argv[i]);
        }
        free(argv);
        break;
    }
    return 0;
}
