#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#include "user.h"
#include "base64.h"
#include "exec.h"

// CONSTANTS
#define MAX_PAYLOAD 1024
#define NETLINK_SAFEBOX 29

// GLOBALS
int sock_fd;
struct msghdr msg;
struct nlmsghdr *nlh = NULL;
struct sockaddr_nl src_addr, dest_addr;
struct iovec iov;
extern char pwd[MAX_PATH_LEN];
extern char root[MAX_PATH_LEN];
extern char display_pwd[MAX_PATH_LEN];

static char delim[] = " ";

// send pid to kernel
void send_pid()
{
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();
    bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; // for linux kernel
    dest_addr.nl_groups = 0;

    // netlink message header
    nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // printf("Sending message to kernel\n");
    sendmsg(sock_fd, &msg, 0);
}

int main(int argc, char *argv[])
{
    // socket
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_SAFEBOX);
    if (sock_fd < 0)
        return -1;

    struct passwd *pw;
    int authentication = 0; // check if authentication succeed

    // 获取当前用户所有信息
    pw = get_user();

    // check_user()查看当前用户是否注册过保险箱，若有，则返回值为相应的密码，若无，则返回0
    send_pid();
    char *password = check_user(pw->pw_uid);
    char input[100]; memset(input, 0, 100);
    char *temp = (char*)malloc(100);
    
    if (password)
    {
        printf("Please enter your password \n");
        scanf("%s", input); getchar();
        if (strcmp(password, input) == 0)
        {
            printf("Welcome \n");
            authentication = 1;
        }
        else
            printf("Wrong password \n");
	
    }
    else
    {
        printf("Creating a new safebox, please set the password\n");
        scanf("%s", input); getchar();
        FILE *fp;

        send_pid();
        if (!(fp=fopen("/home/safebox/password.dat","a+")))
        {
            printf("Error in open file!\n");
            return -1;
        }
        // 将int型的账号密码转为字符串
        char uid[100];
        sprintf(uid, "%d", pw->pw_uid);

        // 这部分不知道为啥，不能两个同时加密后再同时fprintf()写入，会有问题，我猜是buf1和buf2内存冲突了
        // 所以我就换了一下思路，一个一个写入fp
        char *buf1 = (char *)malloc(50);
        base64_encode(uid, &buf1);
        fprintf(fp, "\n%s ", buf1);
        free(buf1);

        char *buf2 = (char *)malloc(50);
        base64_encode(input, &buf2);
        fprintf(fp, "%s", buf2);
        free(buf2);

        printf("Successfully create a safebox, welcome!\n");
        fclose(fp);
        authentication = 1;
	
	char username[COMMAND_MAX_LEN]; memset(username, 0, COMMAND_MAX_LEN);
	strcpy(username, getlogin());
	chdir("/home/safebox");
	mkdir(username, 0777);
    }

    if (authentication == 0)
        return -1;
    
    memset(pwd, 0, MAX_PATH_LEN);
    memset(root, 0, MAX_PATH_LEN); 
    memset(display_pwd, 0, MAX_PATH_LEN);
    strcpy(pwd, "/home/safebox/");
    strcpy(root, "/home/safebox/");
    strcat(pwd, getlogin()); // TODO: should be modified to current user safebox directory
    strcat(root, getlogin());
    strcpy(display_pwd, "");
    
    chdir(root);



    // interactive terminal
    while(1)
    {
        // prompt
        printf("%s> ", display_pwd);

        // get command
        char command[COMMAND_MAX_LEN];
        char splited_cmd[5][COMMAND_MAX_LEN];
        memset(command, 0, COMMAND_MAX_LEN);
        scanf("%[^\n]", command); getchar();
        
        // split command
        char *token; int cmd_count = 0;
        for(token = strtok(command, delim); token != NULL; token = strtok(NULL, delim))
        {
		    strcpy(splited_cmd[cmd_count++], token);
            // printf("%s\n", token);
        }
        // printf("count: %d\n", cmd_count);
        
        send_pid(); // tell kernel the pid to enter the safebox
        int continue_ = execute(splited_cmd, cmd_count);

        if (continue_ == -1)
            break;

    } // while

    return 0;
}
