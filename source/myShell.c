#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int saved_stdout=0;
int saved_stdin=0;


//save the descriptor of the STDOUT_FILENO and STDIN_FILENO
void saveDescriptor(){
    saved_stdout = dup(STDOUT_FILENO);
    saved_stdin = dup(STDIN_FILENO);

}


void get_command(int argc,char**argv,char*command){

    const char delimiter = ' ';
    char temp[1024];
    strcpy(temp, command);
    for (int j = 0; j < (argc);++j){
        if(j==0){
            strcpy(argv[0], strtok(temp, &delimiter));
            //printf("%s\n", argv[0]);
        }
        else{
            strcpy(argv[j], strtok(NULL, &delimiter));
            //printf("%s\n", argv[j]);
        }
    }
}


//help command
void help(int argc,char**argv){
   //show all internal command
   if(argc==1){
       printf("A simple shell implementation by whe1ve @version 1.0\n");
       printf("These shell commands are defined internally. Type 'help' to see the list.\n");
       printf("Type 'help name' to find out more about the function 'name'.\n");
       printf("cd\t[dir]\n");
       printf("exit\t[n]\n");
       printf("help\t[patten]\n");
       printf("mv \t[source]\t[destination]\n");
       printf("rm \t[file|[[-r] [dir]]]\n");
   }
   else{
      if(strcmp(argv[1],"cd")==0){
        //show 'cd' command's detail information
        printf("cd:\tcd [dir]\n");
        printf("\tChange the shell working directory.\n");
        printf("\n");
        printf("\tChange the current directory to DIR. The default DIR is the value of the HOME shell variable.\n");
      }
      else if(strcmp(argv[1],"exit")==0){
        //show 'exit' command's detail information
        printf("exit:\texit [n]\n");
        printf("\tExit the shell.\n");
        printf("\n");
        printf("\tExits the shell with a status of N.  If N is omitted, the exit status is 0.\n");
      }
      else if(strcmp(argv[1],"help")==0){
        //show 'help' command's detail information
        printf("help:\thelp [pattern]\n");
        printf("\tDisplay information about builtin commands.\n");
        printf("\n");
        printf("\tDisplays brief summaries of builtin commands. If PATTERN is specified, gives detailed help on all commands matching PATTERN, otherwise the list of help topics is printed.\n");
      }
      else if(strcmp(argv[1],"mv")==0){
        //show 'mv' command's detail information
        printf("mv:\tmv [source] [destination]\n");
        printf("\tMove the source file to the destination diretory.\n");
        printf("\n");
        printf("\tIf the source and destination are in the same directory, the rename the source file.\n");
      } 
      else if(strcmp(argv[1],"rm")==0){
        //show 'rm' command's detail information
        printf("rm:\trm [fire|[[-r] [dir]]]\n");
        printf("\tRemove file or remove the directory recursively.\n");
      }
      else{
          perror("Error:no such inner command.\n");
          return;
      }
   }
}

//cd command
void cd(int argc,char**argv){
    if(argc==1){
        //change the current working directory to home directory.
        char *homeDir = getenv("HOME");
        chdir(homeDir);
    }
    else if(argc==2){
        //change the current working directory to higher level directory
        if(strcmp(argv[1],"..")==0){
            chdir("..");
        }
        else{
            chdir(argv[1]);
        }
    }
    else if(argc>2){
        perror("Error:too many arguments.\n");
    }
}

//remove directory recursively
int removeDirectory(const char *dirPath) {
    DIR *dir;
    struct dirent *entry;
    char path[1024];

    if ((dir = opendir(dirPath)) == NULL) {
        perror("Error:can not open the directory.\n");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(path, sizeof(path), "%s/%s", dirPath, entry->d_name);

            if (entry->d_type == DT_DIR) {
                // remove sub-directory recursively
                if (removeDirectory(path) != 0) {
                    closedir(dir);
                    return 1;
                }
            } 
            else {
                // remove the file
                if (remove(path) != 0) {
                    perror("Error:can not remove the file.\n");
                    closedir(dir);
                    return 1;
                }
            }
        }
    }

    closedir(dir);

    // remove empty directory
    if (rmdir(dirPath) == 0) {
            return 0;
    } 
    else {
        perror("Error:can not remove the directory.\n");
        return 1;
    }
}

//rm command
void rm(int argc,char**argv){
    if(argc==2){
        //remove the file
        if(opendir(argv[1])!=NULL){
            perror("Error:can not remove the directory that is not empty.\n");
            return;
        }
        if(remove(argv[1])!=0){
            perror("Error: the file is not exist or cannot remove it.\n");
            return;
        }
    }
    else{
        if(argc==1){
            //missing operand
            perror("Error: missing operand!\n");
            return;
        }
        else if(argc>3){
            //too many arguments
            perror("Error: too many arguments.\n");
            return;
        }
        else if(argc==3){

            if(strcmp(argv[1],"-r")!=0){
                perror("Error: wrong option.\n");
                return;
            }
            //remove the directory recursively
            if(removeDirectory(argv[2])){
                perror("Error:remove failed.\n");
            }

        }
    }
}

//mv command
void mv(int argc,char**argv){
    if(argc<3){
        perror("Error: incomplete command.\n");
        return;
    }
    else if(argc>3){
        perror("Error: too many arguments.\n");
        return;
    }
    else{
        //rename or move the file
        if(opendir(argv[2])==NULL){
            //rename the file
            rename(argv[1], argv[2]);
            return;
        }
        else{
            //move the file to the target directory
            char destinationPath[1024];
            //integrate the destination path
            snprintf(destinationPath, sizeof(destinationPath), "%s/%s", argv[2], argv[1]);
            if(rename(argv[1],destinationPath)!=0){
                perror("Error: can not move the file.\n");
                return;
            }
        }
    }
}

// redirect(maybe) implementation
void redirect(char*command){
    char buf[1024]; //temp store the command
    strcpy(buf, command);

    int argc = 0;
    char **argv;
    const char delimiter = ' ';
    char *t=strtok(buf,&delimiter);   //calculate argc
    while(t!=NULL){
           //calculate argc   
           ++argc;
           t = strtok(NULL, &delimiter);
    }

    argv = (char **)malloc((argc+1) * sizeof(char *)); //initial argv
    for (int i = 0; i < argc;++i){
            argv[i] = (char *)malloc(strlen(command) + 1);
    }
    argv[argc] = NULL;

    get_command(argc, argv, command); // get stoken
    //printf("Argc argv over.\n");
    //printf("%s\n", argv[0]);

    int type = -1;  //initial type 
    //type 3: command < infile 
    //type 4: command > outfile
    //type 5: command > outfile < infile
    //type 6: command < outfile > infile
    int indexOfInfile = -1;
    int indexOfOutfile = -1;

    size_t len = strlen(command);


      
    
    // scan the command to determine the redirect type
    for (int i = 0; i < len;++i){
        if(command[i]=='<'){
            if(type==-1){
                type = 3;
            }
            else if(type==4){
                type = 5;
            }
            else{
                perror("Error: wrong redirect type!\n");
            }
        }
        if(command[i]=='>'){
            if(type==-1){
                type = 4;
            }
            else if(type==3){
                type = 6;
            }
            else{
                perror("Error: wrong redirect type!\n");
            }
        }
    }
    

    if(type==-1){
        //no redirect

        //printf("no redirect!\n");
        pid_t pid;
        
        if((pid=fork())==0){

            //child process
            if(execvp(argv[0], argv)==-1){
                perror("Error: execvp.\n");
            }
            exit(0);//exit child process
        }
        else{
           //father process
           wait(NULL);
           //printf("redirect father and child is Over.\n");
        }
   
    }
    else if(type == 3){
        //type 3: command < infile
        for (int i = 0; i < argc;++i){
            if(strcmp(argv[i],"<")==0){
                indexOfInfile = i + 1;
                if(indexOfInfile>=argc){
                    fprintf(stderr, "No input file.\n");
                    
                }
                break;
            }
           
        }
        if(indexOfInfile<argc){
            int fd = open(argv[indexOfInfile], O_RDWR|O_CREAT,0777);
            dup2(fd, STDIN_FILENO); // redirect stdin to input file
            close(fd);

            argv[indexOfInfile-1] = NULL; // update command

            pid_t pid;
            if((pid=fork())==0){
                //child process
                if(execvp(argv[0],argv)==-1){
                    perror("Error: execvp.\n");
                }

                
                exit(0);//exit child process
            }
            else{
                //father process
                wait(NULL);

            }
        }
    }
    else if(type == 4){
        //type 4: command > outfile
        //perror("Reach type 4.\n");
        for (int i = 0; i < argc;++i){
            if(strcmp(argv[i],">")==0){
                indexOfOutfile = i + 1;
                if(indexOfOutfile>=argc){
                    perror("Error: No output file.\n");
                }
                break;
            }
        }
        if(indexOfOutfile<argc){
            int fd = creat(argv[indexOfOutfile], 0644);
            dup2(fd, STDOUT_FILENO); // redirect stdout to output file
            close(fd);

            argv[indexOfOutfile-1] = NULL;//update command



            pid_t pid;
            if((pid=fork())==0){
                //child process
                if(execvp(argv[0],argv)==-1){
                    perror("Error: execvp.\n");
                }

                
                exit(0); //exit child process
            }
            else{
                //father process
                wait(NULL);
            }
        }
    }
    else if(type == 5){
        //type 5: command > outfile < infile
        for (int i = 0; i < argc;++i){
            if(strcmp(argv[i],">")==0){
                indexOfOutfile = i + 1;
                if(indexOfOutfile>=argc){
                    perror("Error: No output file.\n");
                }
            }
            if(strcmp(argv[i],"<")==0){
                indexOfInfile = i + 1;
                if(indexOfOutfile>=argc){
                    perror("Error: No input file.\n");
                }
            }
        }

        if((indexOfInfile>0&&indexOfInfile<argc)&&(indexOfOutfile>0&&indexOfOutfile<argc)){
            int fdin = open(argv[indexOfInfile], O_RDWR | O_CREAT, 0777); //redirect stdin
            dup2(fdin, STDIN_FILENO);
            close(fdin);

            int fdout = creat(argv[indexOfOutfile], 0644); //redirect stdout
            dup2(fdout, STDOUT_FILENO);
            close(fdout);

            argv[indexOfOutfile-1] = NULL; //update command

            pid_t pid;
            if((pid=fork())==0){
                // child process
                if(execvp(argv[0],argv)==-1){
                    perror("Error: execvp.\n");
                }

                exit(0); //exit process
            }
            else{
                //father process
                wait(NULL);
            }
        }
    }
    else if(type == 6){
    //type 6: command < infile > outfile
        for (int i = 0; i < argc;++i){
            if(strcmp(argv[i],">")==0){
                indexOfOutfile = i + 1;
                if(indexOfOutfile>=argc){
                    perror("Error: No output file.\n");
                }
            }
            if(strcmp(argv[i],"<")==0){
                indexOfInfile = i + 1;
                if(indexOfOutfile>=argc){
                    perror("Error: No input file.\n");
                }
            }
        }

        if((indexOfInfile>0&&indexOfInfile<argc)&&(indexOfOutfile>0&&indexOfOutfile<argc)){
            int fdin = open(argv[indexOfInfile], O_RDWR | O_CREAT, 0777); //redirect stdin
            dup2(fdin, STDIN_FILENO);
            close(fdin);

            int fdout = creat(argv[indexOfOutfile], 0644); //redirect stdout
            dup2(fdout, STDOUT_FILENO);
            close(fdout);

            argv[indexOfInfile-1] = NULL; //update command

            pid_t pid;
            if((pid=fork())==0){
                // child process
                if(execvp(argv[0],argv)==-1){
                    perror("Error: execvp.\n");
                }

                exit(0); //exit process
            }
            else{
                //father process
                wait(NULL);
            }
        }
    }
    else{
        perror("Error: redirect type error.\n");
    }


    //free the memory
    for (int i = 0; i < argc;++i){
            free(argv[i]);
    }
    free(argv);


    
}


void pipeLine(char*command){
    char buf[1024];
    char*firstCommand=(char*)malloc(strlen(command)+1);
    char*otherCommands=(char*)malloc(strlen(command)+1);
    strcpy(buf, command);
    const char delimiter = '|';


    strcpy(firstCommand,strtok(buf,&delimiter)); // first command;
 
    if(strlen(command)==strlen(firstCommand)){
        otherCommands = NULL;
    }
    else{
        strcpy(otherCommands,strtok(NULL,"")); // other commands
    }

    int fd[2]; // file descriptor



    if(otherCommands==NULL){
        // only one command 
        redirect(firstCommand);


    }
    else{
        pipe(fd); //create pipe
        pid_t pid;
        //printf("Reach 366.\n");
        if((pid=fork())==0){
            //child process

            close(fd[0]);//close pipe output

            close(STDOUT_FILENO); // close stdout 

            dup(fd[1]); //duplicate the fd[1] to STDOUT_FILENO

            close(fd[1]); //close fd[1]

            redirect(firstCommand);


            close(fd[1]);
            close(fd[0]);

            exit(0); //exit the child process


        }
        else{
            //father process, excutes other commands recursively
            close(fd[1]); //close pipe input
            close(STDIN_FILENO); //close stdin
            dup(fd[0]); //duplicate the fd[0] to STDIN_FILENO

            close(fd[0]); //close fd[0]

            wait(NULL);

            pipeLine(otherCommands); //executes other commands recursively
            
          }

    }

    close(fd[1]);
    close(fd[0]);
    //free memory
    free(firstCommand);
    free(otherCommands);

    //recover stdin and stdout 
    if(!isatty(STDIN_FILENO)){
        dup2(saved_stdin, STDIN_FILENO);
    }
    if(!isatty(STDOUT_FILENO)){
        dup2(saved_stdout, STDOUT_FILENO);
    }
    return;
}


//Out put the command line prompt
void print_prompt(char*prompt){
    char *user_name = getlogin(); //the user name
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

    sprintf(prompt, "\033[0;35m%s@%s:%s%c \033[0m", user_name, host_name, cwd, userType);
    
}



//deal the command
void deal_command(int argc,char**argv,char*command){


   //internal command
   //exit command
   if(strcmp(argv[0],"exit")==0){
       if(argc==1){
           exit(0);
       }
       else if(argc==2){
           exit(atoi(argv[1]));
       }
       else{
           perror("Error:invalid command\n");
           return;
       }
   }

   //help command
   if(strcmp(argv[0],"help")==0){
       help(argc,argv);
       return;
   }

   //cd command
   if(strcmp(argv[0],"cd")==0){
       cd(argc, argv);
       return;
   }

   //rm command
   if(strcmp(argv[0],"rm")==0){
       rm(argc, argv);
       return;
   }
   

   //mv command
   if(strcmp(argv[0],"mv")==0){
       mv(argc, argv);
       return;
   }


   //external command (includes pipes and redirect)
   pipeLine(command);

   return;
}


int main(){

    saveDescriptor();
    while(1){
        char *command; // User input
        char prompt[1024]; //prompt info
        
        print_prompt(prompt);// display the command line prompt
        

        int argc = 0;
        char **argv;
        const char delimiter = ' ';


        command = readline(prompt);

        if(strcmp(command,"\n")==0){
            continue;
        }
        
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

        deal_command(argc, argv,command); //deal command

        //free the memory
        for (int i = 0; i < argc;++i){
            free(argv[i]);
        }
        free(argv);
        free(command);
    }
    return 0;
}
