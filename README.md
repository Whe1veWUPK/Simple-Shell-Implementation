# Simple-Shell-Implementation
This Project is SEU-CSE's  Operating System Practice class 's Experiment 2

## 环境准备
**GNU readline 库的 下载**
```
sudo apt-get install libreadline-dev //这条是ubuntu 的命令 Fedora 以及 CentOS可能并不一致
```
用于读取用户输入的shell命令
因为它是动态库 所以用gcc编译时要加上 -lreadline
```
gcc -o xxx xxx.c -lreadline
```
## 实验环境
* ubuntu 22.04
* VMware 
* linux 6.6.4
