#include "user.h"
#include "base64.h"

struct passwd * get_user(){
  uid_t uid;
  gid_t gid;
  
  struct passwd *pw;
  uid = getuid();
  gid = getgid();
  
//   printf("User is %s\n",getlogin());//获取登陆用户名
  
//   printf("User IDs: uid=%s, gid=%s\n",uid,gid); //获取uid和gid
  
  pw = getpwuid(uid); //通过uid获取passwd结构的成员信息
//   printf("UID passwd entry:\n name=%s, uid=%d, gid=%d, dir=%s, \ 
//          gecos=%s, shell=%s\n",
//          pw->pw_name,pw->pw_uid,pw->pw_gid,pw->pw_dir,pw->pw_gecos,
//          pw->pw_shell );
//   pw = getpwnam("oprofile");  //获取oprofile用户的passwd结构的成员信息
//   printf("root passwd entry:\n");
//   printf("UID passwd entry:\n name=%s, uid=%d, gid=%d, dir=%s, \
//          gecos=%s, shell=%s\n",
//          pw->pw_name,pw->pw_uid,pw->pw_gid,pw->pw_dir,pw->pw_gecos,
//          pw->pw_shell );
  return pw;
}

char* check_user(int pid)
{
    char label[100], password[100];
    memset(label, 0, 100); memset(password, 0, 100);
    char *buf;
    FILE *fp;

    if (!(fp=fopen("/home/safebox/password.dat","r")))
    {
        printf("Error in open file!\n");
        exit(1);
    }

    while (fscanf(fp,"%s %s", label, password))
    {   
        base64_decode(label, &buf);
        // printf("label: %s\n", buf);
        if (atoi(buf) == pid)
        {
            char *a = base64_decode(password, &buf);
            // printf("%s", a);
            // int temp = atoi(buf);
            // free(buf);
            fclose(fp);
            return buf;
        }
        if (feof(fp))
        {
            break;
        }
    }
    free(buf);
    fclose(fp);
    return NULL;
}