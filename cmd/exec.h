#ifndef __EXEC_H__
#define __EXEC_H__

#include <linux/string.h>
#include <stdio.h>
#include <dirent.h>
#include "utils.h"

#define COMMAND_MAX_LEN 40
#define MAX_PATH_LEN 256
#define COMMAND_MAX_LEN 40

#define CMD_CD "cd"
#define CMD_LS "ls"
#define CMD_EXIT "exit"
#define CMD_RM "rm"
#define CMD_MKDIR "mkdir"
#define CMD_MV "mv"
#define CMD_MVIN "mvin"
#define CMD_MVOUT "mvout"
#define CMD_CHPSWD "chpswd"


char pwd[MAX_PATH_LEN];
char root[MAX_PATH_LEN];
char display_pwd[MAX_PATH_LEN];

// help functions
int _ls(char path[MAX_PATH_LEN]);
int _cd(char path[MAX_PATH_LEN]);
int _mkdir(char path[MAX_PATH_LEN]);
int _rm(char path[MAX_PATH_LEN]);
int _mv(char oldfile[MAX_PATH_LEN], char newfile[MAX_PATH_LEN]);


// special execute
int exec_cd(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_ls(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_rm(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_mkdir(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_mv(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_mvin(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_mvout(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);
int exec_chpswd(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);


// main execute
int execute(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count);

#endif
