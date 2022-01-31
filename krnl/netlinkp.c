#include <linux/string.h>
#include <linux/mm.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/sched.h>
#include <linux/fs_struct.h>
#include <linux/limits.h>

#define NETLINK_SAFEBOX 29

u32 recv_pid=0;
struct sock *nl_sk = NULL;


//发送netlink消息message
int netlink_sendmsg(const void *buffer, unsigned int size)
{
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	int len = NLMSG_SPACE(1200);
	if((!buffer) || (!nl_sk) || (recv_pid == 0)) 	return 1;
	skb = alloc_skb(len, GFP_ATOMIC); 	//分配一个新的sk_buffer
	if (!skb)
	{
		printk(KERN_ERR "net_link: allocat_skb failed.\n");
		return 1;
	}
	nlh = nlmsg_put(skb,0,0,0,1200,0);
	NETLINK_CB(skb).creds.pid = 0;      /* from kernel */
	//下面必须手动设置字符串结束标志\0，否则用户程序可能出现接收乱码
	memcpy(NLMSG_DATA(nlh), buffer, size);
	//使用netlink单播函数发送消息
	if( netlink_unicast(nl_sk, skb, recv_pid, MSG_DONTWAIT) < 0)
	{
	//如果发送失败，则打印警告并退出函数
		printk(KERN_ERR "net_link: can not unicast skb \n");
		return 1;
	}
	return 0;
}

void nl_recv_pid(struct sk_buff *skb)
{
	struct nlmsghdr *nlh;
	printk(KERN_INFO "Entering: %s\n", __FUNCTION__);
	
	nlh = (struct nlmsghdr *)skb->data;
	recv_pid = nlh->nlmsg_pid;
	printk(KERN_INFO "Netlink received pid %d\n", recv_pid);
}

void netlink_init(void) {
    struct netlink_kernel_cfg cfg = 
	{
        .input = nl_recv_pid,
    };

    nl_sk=netlink_kernel_create(&init_net,NETLINK_SAFEBOX, &cfg);

    if (!nl_sk)
    {
		printk(KERN_ERR "net_link: Cannot create netlink socket.\n");
		if (nl_sk != NULL)
    		sock_release(nl_sk->sk_socket);
    }	
    else  printk("net_link: create socket ok.\n");
}

void netlink_release(void)
{
    if (nl_sk != NULL)
 		sock_release(nl_sk->sk_socket);
}
