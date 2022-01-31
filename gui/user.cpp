#include "user.h"
#include "utils.h"


User::User()
{
    uid_t uid = getuid();
    pw = getpwuid(uid);
}


char* User::check_user(int uid)
{
    char label[30], password[100];
    memset(label, 0, 30); memset(password, 0, 30);
    char *buf = (char *)malloc(40);
    FILE *fp;

    if (!(fp=fopen("/home/safebox/password.dat","r")))
    {
        printf("Error in open password.dat while checking user!\n");
        exit(1);
    }

    while (fscanf(fp,"%s %s", label, password))
    {   
        base64_decode(label, &buf);
        if (atoi(buf) == uid)
        {
            base64_decode(password, &buf);
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


int User::pid()
{
    return pw->pw_uid;
}

char* User::pname()
{
    return pw->pw_name;
}

int User::set_password(char* pswd)
{
    FILE *fp;
    if (!(fp=fopen("/home/safebox/password.dat","a+")))
    {
        printf("Error in open password.dat while setting password!\n");
        exit(1);
    }

    char uid[20]; sprintf(uid, "%d", pw->pw_uid);
    char *buf1 = (char *)malloc(40);
    base64_encode(uid, &buf1);
    fprintf(fp, "\n%s ", buf1);
    free(buf1);

    char *buf2 = (char *)malloc(40);
    base64_encode(pswd, &buf2);
    fprintf(fp, "%s", buf2);
    free(buf2);

    fclose(fp);
    return 1;
}

void User::reset_password(char* pswd)
{
    FILE *fp;
    if (!(fp=fopen("/home/safebox/password.dat","r+")))
    {
        printf("Error in open password.dat while resetting password!\n");
        exit(1);
    }

    FILE *new_fp;
    if (!(new_fp=fopen("/home/safebox/password_temp.dat","w")))
    {
        printf("Error in create password_temp.dat while resetting password!\n");
        exit(1);
    }

    // int uid; sprintf(uid, "%d", pw->pw_uid);
    char *buf1 = (char *)malloc(40);
    char label[30];
    char passwd[30];

    while (fscanf(fp,"%s", label))
    {   
        base64_decode(label, &buf1);
        if (atoi(buf1) == pw->pw_uid)
        {
            fprintf(new_fp, "%s", label);
            free(buf1);
            char *buf2 = (char *)malloc(40);
            base64_encode(pswd, &buf2);
            fprintf(new_fp, " %s\n", buf2);
            free(buf2);
            break;
        }
        else
        {
            fprintf(new_fp, "%s", label);
            fscanf(fp," %s", passwd);
            fprintf(new_fp, " %s\n", passwd);
        }
            
        if (feof(fp))
            break;
    }

    fclose(fp);
    fclose(new_fp);
    QFile::remove(QString::fromStdString("/home/safebox/password.dat"));
    QFile::rename(QString::fromStdString("/home/safebox/password_temp.dat"),QString::fromStdString("/home/safebox/password.dat"));
}
