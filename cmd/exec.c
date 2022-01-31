#include "exec.h"
#include "base64.h"

int _cd(char path[MAX_PATH_LEN])
{
    return chdir(path);
}

int exec_cd(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count)
{
    switch(cmd_count)
    {
        case 1:
        {
            _cd(root);
            memset(pwd, 0, COMMAND_MAX_LEN);
	    strcpy(pwd, root);
	    memset(display_pwd, 0, COMMAND_MAX_LEN);
            break;
        }
        case 2:
        {
            char cd_path[MAX_PATH_LEN]; memset(cd_path, 0, MAX_PATH_LEN);
            char display_path[MAX_PATH_LEN]; memset(display_path, 0, MAX_PATH_LEN);
            char target_path[MAX_PATH_LEN]; memset(target_path, 0, MAX_PATH_LEN);
            strcpy(cd_path, splited_cmd[1]);
            
            if (strncmp(cd_path, "/", 1) == 0)//abspath
            {
                strcpy(target_path, root);
                strcat(target_path, cd_path);
                //strcpy(display_path, display_pwd);
		memset(display_path, 0, MAX_PATH_LEN);                
		strcpy(display_path, cd_path);
                
                int suc = _cd(target_path);
                if (suc == -1)
                {
                    printf("Invalid cd path.\n");
                }
                else
                {
                    memset(display_pwd, 0, MAX_PATH_LEN); // change display path
                    strcpy(display_pwd, display_path);
		    memset(pwd, 0, MAX_PATH_LEN);
		    strcpy(pwd, root);
		    strcat(pwd, display_pwd);   
                }
                break;
            }
            else//relpath
            {
                strcpy(target_path, pwd);
                strcat(target_path, "/");
                strcat(target_path, cd_path); 
                char norm[MAX_PATH_LEN]; memset(norm, 0, MAX_PATH_LEN);
                
                normpath(norm, target_path);
                
                if (strncmp(norm, root, strlen(root)) != 0) //root doesn't have parent directory.
                {
                    printf("Illegal dir.\n");
                    break;
                }
                
                strcpy(display_path, display_pwd);
		strcat(display_path, "/");
                strcat(display_path, cd_path);
                char norm_display[MAX_PATH_LEN]; memset(norm_display, 0, MAX_PATH_LEN);
                
                normpath(norm_display, display_path);
                
                int suc = _cd(norm);
                if (suc == -1)
                {
                    printf("Invalid cd path.\n");
                }
                else
                {
                    memset(display_pwd, 0, MAX_PATH_LEN); // change display path
                    strcpy(display_pwd, norm_display);
		    memset(pwd, 0, MAX_PATH_LEN);
		    strcpy(pwd, root);
		    strcat(pwd, norm_display);
                }
                break;
            }
        default:
            printf("Arguments error. Format: cd [path]");
        }
    }
    return 0;
}

int _rm(char file[MAX_PATH_LEN])
{
    return remove(file);
}

int exec_rm(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count)
{
    switch(cmd_count)
    {
        case 1:
        {
            printf("usage: rm [filename]\n");
            break;
        }
        case 2:
        {
            char rm_file[MAX_PATH_LEN]; memset(rm_file, 0, MAX_PATH_LEN);
            char target_file[MAX_PATH_LEN]; memset(target_file, 0, MAX_PATH_LEN);
	    char display_path[MAX_PATH_LEN]; memset(display_path, 0, MAX_PATH_LEN);
            strcpy(rm_file, splited_cmd[1]);
            
            if (strncmp(rm_file, "/", 1) == 0)
            {
                strcpy(target_file, root);
                strcat(target_file, rm_file);
                int suc = _rm(target_file);
                
                if (suc == -1)
                {
                    printf("Failed, please check file path.\n");
                }
            }
            else
            {
                strcpy(target_file, display_pwd);
                strcat(target_file, "/");
                strcat(target_file, rm_file); 
                char norm[MAX_PATH_LEN]; memset(norm, 0, MAX_PATH_LEN);
                    
                normpath(norm, target_file);
		
		char full_path[MAX_PATH_LEN]; memset(full_path, 0, MAX_PATH_LEN);
		strcpy(full_path, root);
		strcat(full_path, norm);
		char full_norm[MAX_PATH_LEN]; memset(full_norm, 0, MAX_PATH_LEN); 
		normpath(full_norm, full_path);		
		
                    
                if (strncmp(full_norm, root, strlen(root)) != 0) //can't remove file which isn't under user's pwd.
                {
                    printf("Illegal operation.\n");
                    break;
                }
                    
                strcpy(display_path, display_pwd);
                strcat(display_path, rm_file);
                char norm_display[MAX_PATH_LEN]; memset(norm_display, 0, MAX_PATH_LEN);
                    
                normpath(norm_display, display_path);
                    
                int suc = _rm(full_norm);
                if (suc == -1)
                {
                    printf("File %s doesn't exist.\n", norm_display);
                }
            }
            break;
        }
        default:
        {
            printf("Command arguments error.\n");
        }
    }
    return 0;
}

int _mkdir(char path[MAX_PATH_LEN])
{
    return mkdir(path,0777);
}

int exec_mkdir(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count)
{
    switch(cmd_count)
    {
        case 1:
            {
                printf("usage: mkdir [dir_name]");
                break;
            }
        case 2:
            {
                char mk_dir[MAX_PATH_LEN]; memset(mk_dir, 0, MAX_PATH_LEN);
                strcpy(mk_dir, splited_cmd[1]);
		char target_dir[MAX_PATH_LEN]; memset(target_dir, 0, MAX_PATH_LEN);                
		//strcpy(target_dir, display_pwd);
		//strcat(target_dir, "/");		
		//strcat(target_dir, mk_dir);
		//printf("%s\n",pwd);
		//printf("%s\n",target_dir);

                int suc = _mkdir(mk_dir);
                if (suc == -1)
                {
                    printf("Couldn't create directory.\n");
                }
		//printf("%d\n", suc);
                break;                
            }
        default:
            {
                printf("Argument error.\n");
            }  
    }
    return 0;
}



int _mv(char oldfile[MAX_PATH_LEN], char newfile[MAX_PATH_LEN])
{
    //if (link(oldfile, newfile) == 0)
    //{
	//if (unlink(oldfile) == 0)
	//{
	//    return 0;
	//}
	//else
	//{
	//    printf("unlink failed\n");
	//    return -1;
	//}
    //}
    //else
    //{
	//printf("link failed\n");
	//return -1;
    //}
    return rename(oldfile, newfile);
}

int in_or_out(char path[MAX_PATH_LEN]) //inside or outside the vault.
{
    if (strncmp(path, root, strlen(root)) != 0) //outside
    {
        return 1;       
    }
    return 0;
}


int exec_mv(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count)
{
    switch(cmd_count)
    {
        case 3:
            {
                char old_file[COMMAND_MAX_LEN]; memset(old_file, 0, COMMAND_MAX_LEN);
                char new_file[COMMAND_MAX_LEN]; memset(new_file, 0, COMMAND_MAX_LEN);
                strcpy(old_file, splited_cmd[1]);
                strcpy(new_file, splited_cmd[2]);
                //char temp_newfile[COMMAND_MAX_LEN]; memset(temp_newfile, 0, COMMAND_MAX_LEN);
                char target_newfile[MAX_PATH_LEN]; memset(target_newfile, 0, MAX_PATH_LEN);
                char target_oldfile[MAX_PATH_LEN]; memset(target_oldfile, 0, MAX_PATH_LEN);
                    
                if (strncmp(old_file, "/", 1) == 0)
                {
                    strcpy(target_oldfile, root);
                    strcat(target_oldfile, old_file);
                }
                else
                {
                    char temp_oldfile[COMMAND_MAX_LEN]; memset(temp_oldfile, 0, COMMAND_MAX_LEN);
                    strcpy(temp_oldfile, pwd);
                    strcat(temp_oldfile, "/");
                    strcat(temp_oldfile, old_file); 
                    
                    normpath(target_oldfile, temp_oldfile);
                }
                
                if (strncmp(new_file, "/", 1) == 0)
                {
                    strcpy(target_newfile, root);
                    strcat(target_newfile, new_file);
                }
                else
                {
                    char temp_newfile[COMMAND_MAX_LEN]; memset(temp_newfile, 0, COMMAND_MAX_LEN);
                    strcpy(temp_newfile, pwd);
                    strcat(temp_newfile, "/");
                    strcat(temp_newfile, new_file); 
                    
                    normpath(target_newfile, temp_newfile);
                }
                
                if (in_or_out(target_oldfile) && in_or_out(target_newfile))
                {
                    printf("choose at least one path inside the vault.\n");      
                }
                else
                {
		    
                    int suc = _mv(target_oldfile, target_newfile);
                    if (suc == -1)
                    {
                        printf("move file failed, check file path.\n");
                    }
                }
                break;
            }
        default:
            {
                printf("Argument error.\n");
            }
    }
    return 0;
}

int exec_mvin(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count)
{
    switch(cmd_count)
    {
	case 3:
	{
	    char old_file[MAX_PATH_LEN]; memset(old_file, 0, MAX_PATH_LEN);
            char new_file[MAX_PATH_LEN]; memset(new_file, 0, MAX_PATH_LEN);
	    char target_newfile[MAX_PATH_LEN]; memset(target_newfile, 0, MAX_PATH_LEN);
            strcpy(old_file, splited_cmd[1]);
            strcpy(new_file, splited_cmd[2]);
	    
	    if (strncmp(old_file, "/", 1) != 0)
	    {
		printf("use absolute path for the first argument.\n");
		break;
	    }
	    else
	    {
	        if (strncmp(new_file, "/", 1) == 0)
		{
		    strcpy(target_newfile, root);
                    strcat(target_newfile, new_file);
		}
		else
		{
		    char temp_newfile[MAX_PATH_LEN]; memset(temp_newfile, 0, MAX_PATH_LEN);
                    strcpy(temp_newfile, pwd);
                    strcat(temp_newfile, "/");
                    strcat(temp_newfile, new_file); 
                    
                    normpath(target_newfile, temp_newfile);
		}

		if (in_or_out(old_file) && !(in_or_out(target_newfile)))
                {
                    int suc = _mv(old_file, target_newfile);
                    if (suc == -1)
                    {
                        printf("move file failed, check file path.\n");
                    }         
                }
		else
		{
		    printf("Illegal path.\n");
		}		
	    }
	    break;
	}
	default:
	{
	    printf("argument error.\n");
	}
    }
    return 0;
}

int exec_mvout(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count)
{
    switch(cmd_count)
    {
	case 3:
	{
	    char old_file[MAX_PATH_LEN]; memset(old_file, 0, MAX_PATH_LEN);
            char new_file[MAX_PATH_LEN]; memset(new_file, 0, MAX_PATH_LEN);
	    char target_oldfile[MAX_PATH_LEN]; memset(target_oldfile, 0, MAX_PATH_LEN);
            strcpy(old_file, splited_cmd[1]);
            strcpy(new_file, splited_cmd[2]);
	    
	    if (strncmp(new_file, "/", 1) != 0)
	    {
		printf("use absolute path for the second argument.\n");
		break;
	    }
	    else
	    {
	        if (strncmp(old_file, "/", 1) == 0)
		{
		    strcpy(target_oldfile, root);
                    strcat(target_oldfile, old_file);
		}
		else
		{
		    char temp_oldfile[MAX_PATH_LEN]; memset(temp_oldfile, 0, MAX_PATH_LEN);
                    strcpy(temp_oldfile, pwd);
                    strcat(temp_oldfile, "/");
                    strcat(temp_oldfile, old_file); 
                    
                    normpath(target_oldfile, temp_oldfile);
		}

		if (!(in_or_out(target_oldfile)) && in_or_out(new_file))
                {
                    int suc = _mv(target_oldfile, new_file);
                    if (suc == -1)
                    {
                        printf("move file failed, check file path.\n");
                    }         
                }
		else
		{
		    printf("Illegal path.\n");
		}		
	    }
	    break;
	}
	default:
	{
	    printf("argument error.\n");
	}
    }
    return 0;
}




int _ls(char path[MAX_PATH_LEN])
{
    DIR* dp; struct dirent* ep;
    dp = opendir(path);
    if (dp != NULL)
    {
        while (ep=readdir(dp))
        {
            if(strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
                continue;
            printf("%s  ", ep->d_name);
        }
        printf("\n");
        closedir(dp);
        return 0;
    }
    else
        return -1;
}

int exec_ls(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count)
{
    switch(cmd_count)
    {
        // ls
        case 1:
        {
            int suc = _ls(pwd);
            if (suc == -1)
                printf("Couldn't open dir %s\n", display_pwd);
            break;
        } // case 1 

        // TODO: ls path
        // ls path
        case 2:
        {
            DIR* dp; struct dirent* ep;
            char ls_path[MAX_PATH_LEN]; memset(ls_path, 0, MAX_PATH_LEN);
            char display_path[MAX_PATH_LEN]; memset(display_path, 0, MAX_PATH_LEN);
            char target_path[MAX_PATH_LEN]; memset(target_path, 0, MAX_PATH_LEN);
            strcpy(ls_path, splited_cmd[1]);

            // ls_path is abspath
            if (strncmp(ls_path, "/", 1) == 0)
            {
                strcpy(target_path, pwd);
                strcat(target_path, ls_path);
                strcpy(display_path, display_pwd);
                strcat(display_path, ls_path);
                int suc = _ls(target_path);
                if (suc == -1)
                    printf("Couldn't open dir %s\n", display_path);
            }
            // ls_path is relpath
            else
            {
                strcpy(target_path, pwd);
                strcat(target_path, "/");
                strcat(target_path, ls_path); // /home/safebox/bowen/./ddd
                char norm[MAX_PATH_LEN]; memset(norm, 0, MAX_PATH_LEN);
                normpath(norm, target_path);
                if (strncmp(norm, root, strlen(root)) != 0)
                {
                    printf("Invalid dir.\n");
                    break;
                }
                
                strcpy(display_path, display_pwd);
                strcat(display_path, ls_path);
                char norm_display[MAX_PATH_LEN]; memset(norm_display, 0, MAX_PATH_LEN);
                normpath(norm_display, display_path);
                int suc = _ls(norm);
                if (suc == -1)
                    printf("Could't open dir %s\n", norm_display);
            }

            break;
        } // case 2

        default:
            printf("Command arguments error.\n");

    } // switch

    return 0;
}

int check_pswd(char old_pswd[MAX_PATH_LEN])
{
	FILE *fp;
	char *buf_pswd = (char*)malloc(100);
	char *buf_ID = (char*)malloc(100);
	base64_encode(old_pswd, &buf_pswd);
	char ID[100]; memset(ID, 0, 100);
        char password[100]; memset(password, 0, 100);

	fp = fopen("/home/safebox/password.dat", "r+");
        
        while(fscanf(fp, "%s %s", ID, password))
        {
	    base64_decode(ID, &buf_ID);
	    if (atoi(buf_ID) == getuid())
	    {
	        if(!(strcmp(password, buf_pswd)))
	        {
		    fclose(fp);
		    return 0;
	        }
	        else
	        {
		    printf("wrong old password.");
		    fclose(fp);
		    return -1;
	        }
	    }

	    if (feof(fp))
	    {
		fclose(fp);
		return -1;
    	    }
        }
	fclose(fp);
	return -1;
}

int exec_chpswd(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count)
{
    FILE *fp;
    FILE *new_fp;
    char *buf_ID;
    char ID[100]; memset(ID, 0, 100);
    char password[100]; memset(password, 0, 100);
    

    switch(cmd_count)
    {
	case 1:
	{
	    
	    if (!(fp = fopen("/home/safebox/password.dat", "r+")))
	    {
		printf("Error openning password.dat.\n");
		break;
	    }

	    if (!(new_fp = fopen("/home/safebox/password_temp.dat", "w")))
	    {
		printf("Error openning password_temp.dat.\n");
		break;
	    }

	    else
	    {
		
		while(fscanf(fp, "%s", ID))
		{
		    base64_decode(ID, &buf_ID);
		    if(atoi(buf_ID) == getuid())
		    {
			printf("enter new password.\n");
			char *new_pswd = (char*)malloc(100);
			char *buf_new = (char*)malloc(100);
			scanf("%s", new_pswd); getchar();
			
			fprintf(new_fp, "%s", ID);
			free(buf_ID);
			base64_encode(new_pswd, &buf_new);
			fprintf(new_fp, " %s\n", buf_new);
			free(buf_new);
			break;
		    }
		    else
		    {
			fprintf(new_fp, "%s", ID);
			fscanf(fp, " %s", password);
			fprintf(new_fp, " %s\n", password);
		    }
		    if(feof(fp))
		    {
			break;
		    }
		}
		remove("/home/safebox/password.dat");
		rename("/home/safebox/password_temp.dat", "/home/safebox/password.dat");
	    }
	    fclose(fp);
	    fclose(new_fp);
	    printf("success.\n");
	    break;
	}
	default:
	{
	    printf("argument error.\n");
	}
    }
    return 0;
}


int execute(char splited_cmd[][COMMAND_MAX_LEN], int cmd_count)
{
    char main_cmd[COMMAND_MAX_LEN]; memset(main_cmd, 0, COMMAND_MAX_LEN);
    strcpy(main_cmd, splited_cmd[0]);

    if(strcmp(main_cmd, CMD_CD) == 0)
        return exec_cd(splited_cmd, cmd_count);
    else if(strcmp(main_cmd, CMD_LS) == 0)
        return exec_ls(splited_cmd, cmd_count);
    else if(strcmp(main_cmd, CMD_RM) == 0)
        return exec_rm(splited_cmd, cmd_count);
    else if(strcmp(main_cmd, CMD_MKDIR) == 0)
        return exec_mkdir(splited_cmd, cmd_count);
    else if(strcmp(main_cmd, CMD_MV) == 0)
        return exec_mv(splited_cmd, cmd_count);
    else if(strcmp(main_cmd, CMD_MVIN) == 0)
	return exec_mvin(splited_cmd, cmd_count);
    else if(strcmp(main_cmd, CMD_MVOUT) == 0)
	return exec_mvout(splited_cmd, cmd_count);
    else if(strcmp(main_cmd, CMD_CHPSWD) == 0)
	return exec_chpswd(splited_cmd, cmd_count);
    else if(strcmp(main_cmd, CMD_EXIT) == 0)
        return -1;
    else
    {
        printf("Command not found.\n");
        return 0;
    }
}
