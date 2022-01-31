#ifndef COMM_H
#define COMM_H
#include <sys/socket.h>
#include <linux/netlink.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
using namespace std;

#define MAX_PAYLOAD 1024
#define NETLINK_SAFEBOX 29



class KernelComm {
private:
    int sock_fd;
    bool succ;

public:
    KernelComm();
    int send_pid();
    bool success() {return succ;}
    ~KernelComm(){};

};



#endif