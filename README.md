# Simple-Shell-Implementation
**This Project is (Dr.Zhang's class)SEU-CSE's  Operating System Practice class 's Experiment 2**

## Environment Preparation
**GNU readline's download**
```
sudo apt-get install libreadline-dev //command on ubuntu 
```
It is used to read user's input shell command.
Because it is a dynamic library, you need to add ```-lreadline``` when compiling with ```gcc```.
```
gcc -o xxx xxx.c -lreadline
```
## Lab Environment
* ubuntu 22.04
* VMware 
* linux 6.6.4
