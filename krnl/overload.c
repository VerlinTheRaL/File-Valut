#include <linux/string.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/fs_struct.h>
#include <linux/limits.h>

#define SAFEBOXPATH "/home/safebox"

#define TASK_COMM_LEN 16
#define MAX_PATH_LEN 256
#define MAX_LENGTH 256
#define COMP_MAX 50

#define ispathsep(ch)   ((ch) == '/' || (ch) == '\\')
#define iseos(ch)       ((ch) == '\0')
#define ispathend(ch)   (ispathsep(ch) || iseos(ch))

extern int recv_pid;


// path normalization
char *normpath(char *out, const char *in) 
{
    char *pos[COMP_MAX], **top = pos, *head = out;
    int isabs = ispathsep(*in);

    if (isabs) *out++ = '/';
    *top++ = out;

    while (!iseos(*in)) 
    {
        while (ispathsep(*in)) ++in;

        if (iseos(*in))
            break;

        if (memcmp(in, ".", 1) == 0 && ispathend(in[1])) 
        {
            ++in;
            continue;
        }

        if (memcmp(in, "..", 2) == 0 && ispathend(in[2]))
        {
            in += 2;
            if (top != pos + 1)
                out = *--top;
            else if (isabs)
                out = top[-1];
            else {
                strcpy(out, "../");
                out += 3;
            }
            continue;
        }

        if (top - pos >= COMP_MAX)
            return NULL; /* path to complicate */

        *top++ = out;
        while (!ispathend(*in))
            *out++ = *in++;
        if (ispathsep(*in))
            *out++ = '/';
    }

    *out = '\0';
    if (*head == '\0')
        strcpy(head, "./");
    return head;
}

// get fullpath of openat file
void get_fullpath(const char *pathname, char *fullpath)
{
    char buf[MAX_LENGTH]; memset(buf, 0, MAX_LENGTH);
    char raw_fullpath[MAX_PATH_LEN]; memset(raw_fullpath, 0, MAX_PATH_LEN);
    struct dentry *parent_dentry = current->fs->pwd.dentry;

    if (strncmp(pathname, "/", 1) == 0)
    {
        // pathname is abspath
        strcpy(fullpath, pathname);
        return;
    }

    // pathname is relpath
	if (*(parent_dentry->d_name.name)=='/')
    {
	    strcpy(raw_fullpath, pathname);
    }
    else
    {
        while(1)
        {
            if (strcmp(parent_dentry->d_name.name,"/")==0)
                buf[0]='\0';//reach the root dentry.
            else
                strcpy(buf,parent_dentry->d_name.name);
            strcat(buf,"/");
            strcat(buf, raw_fullpath);
            strcpy(raw_fullpath, buf);

            if ((parent_dentry == NULL) || (*(parent_dentry->d_name.name)=='/'))
                break;

            parent_dentry = parent_dentry->d_parent;
        }

        strcat(raw_fullpath, pathname);
    }

    normpath(fullpath, raw_fullpath);
}


int SafeboxCheckPath(struct pt_regs *regs, char *pathname)
{
    char fullpath[MAX_PATH_LEN]; memset(fullpath, 0, MAX_PATH_LEN);
    char safebox_path[20] = SAFEBOXPATH;

    get_fullpath(pathname, fullpath);

    // pass if not open SAFEBOXPATH
    if (strncmp(fullpath, safebox_path, strlen(safebox_path)) != 0)
        return 0;

    // in SAFEBOXPATH
    // printk("Info: fullpath: %s; recv_pid is %d; current->pid is %d;\n", fullpath, recv_pid, current->pid);
    if (recv_pid == 0 || recv_pid != current->pid)
        return -1;

    printk("Info: recv_pid is %d, current->pid is %d;\n", recv_pid, current->pid);
    return 0;
}

