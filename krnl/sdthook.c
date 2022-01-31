#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <asm/pgtable.h>
#include <asm/uaccess.h>
#include <asm/ptrace.h>


// module macros
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sjtu-wbl/xyh/nmj");
MODULE_DESCRIPTION("hook sys_call_table");

// typedef
typedef void (* demo_sys_call_ptr_t)(void);
typedef asmlinkage long (*orig_openat_t)(struct pt_regs *regs);
typedef asmlinkage long (*orig_chdir_t)(struct pt_regs *regs);
typedef asmlinkage long (*orig_mkdir_t)(struct pt_regs *regs);
typedef asmlinkage long (*orig_symlinkat_t)(struct pt_regs *regs);
typedef asmlinkage long (*orig_linkat_t)(struct pt_regs *regs);
typedef asmlinkage long (*orig_rename_t)(struct pt_regs *regs);

// functions
void netlink_release(void);
void netlink_init(void);
int SafeboxCheckPath(struct pt_regs*, char*);
demo_sys_call_ptr_t* get_sys_call_table(void);

// vars
demo_sys_call_ptr_t* sys_call_table = NULL;
orig_openat_t orig_openat = NULL;
orig_chdir_t orig_chdir = NULL;
orig_mkdir_t orig_mkdir = NULL;
orig_symlinkat_t orig_symlinkat = NULL;
orig_linkat_t orig_linkat = NULL;
orig_rename_t orig_rename = NULL;
unsigned int level;
pte_t *pte;


asmlinkage long hooked_openat(struct pt_regs *regs)
{
	long sys_ret, safe_ret;
	char buffer[PATH_MAX];
	long nbytes;

	// di, si: filepath, dx: flags
  	nbytes = strncpy_from_user(buffer, (char*)regs->si, PATH_MAX);

	// check if it's visiting SAFEBOX
	safe_ret = SafeboxCheckPath(regs, buffer);
	if (safe_ret == -1)
		return safe_ret;

	// original sys_openat
	sys_ret = orig_openat(regs);
  	return sys_ret;
}

asmlinkage long hooked_chdir(struct pt_regs *regs)
{
	// chdir("/home/safebox/dir_test")
	long sys_ret, safe_ret;
	char buffer[PATH_MAX];
	long nbytes;

	nbytes = strncpy_from_user(buffer, (char*)regs->di, PATH_MAX);
	safe_ret = SafeboxCheckPath(regs, buffer);
	if (safe_ret == -1)
		return safe_ret;

	sys_ret = orig_chdir(regs);
	return sys_ret;
}

asmlinkage long hooked_mkdir(struct pt_regs *regs)
{
	// mkdir("test", 0777) 
	long sys_ret, safe_ret;
	char buffer[PATH_MAX];
	long nbytes;

	nbytes = strncpy_from_user(buffer, (char*)regs->di, PATH_MAX);
	safe_ret = SafeboxCheckPath(regs, buffer);
	if (safe_ret == -1)
		return safe_ret;

	sys_ret = orig_mkdir(regs);
	return sys_ret;
}

asmlinkage long hooked_symlinkat(struct pt_regs *regs)
{
	// symlinkat("/home/safebox/a.txt", AT_FDCWD, "/home/bowen/work/a")
	long sys_ret, safe_ret;
	char buffer[PATH_MAX];
	long nbytes;

	nbytes = strncpy_from_user(buffer, (char*)regs->di, PATH_MAX);
	safe_ret = SafeboxCheckPath(regs, buffer);
	if (safe_ret == -1)
		return safe_ret;

	sys_ret = orig_symlinkat(regs);
	return sys_ret;
}

asmlinkage long hooked_linkat(struct pt_regs *regs)
{
	// linkat(AT_FDCWD, "txt_test", AT_FDCWD, "/home/bowen/work/txt_test", 0)
	long sys_ret, safe_ret;
	char buffer[PATH_MAX];
	long nbytes;

	nbytes = strncpy_from_user(buffer, (char*)regs->si, PATH_MAX);
	safe_ret = SafeboxCheckPath(regs, buffer);
	if (safe_ret == -1)
		return safe_ret;

	sys_ret = orig_linkat(regs);
	return sys_ret;
}

asmlinkage long hooked_rename(struct pt_regs *regs)
{
	long sys_ret, safe_ret;
	char buffer[PATH_MAX];
	long nbytes;

	// di, si: filepath, dx: flags
  	nbytes = strncpy_from_user(buffer, (char*)regs->di, PATH_MAX);

	// check if it's visiting SAFEBOX
	safe_ret = SafeboxCheckPath(regs, buffer);
	if (safe_ret == -1)
		return safe_ret;

	// original sys_openat
	sys_ret = orig_rename(regs);
  	return sys_ret;
}

static int __init safebox_init(void)
{
	// get sys_call_table
	sys_call_table = get_sys_call_table();
	printk("Info: sys_call_table found at %lx\n",(unsigned long)sys_call_table) ;

	// find sys_openat, sys_chdir, sys_mkdir, sys_symlinkat, sys_linkat
	orig_openat = (orig_openat_t) sys_call_table[__NR_openat];
	orig_chdir = (orig_chdir_t) sys_call_table[__NR_chdir];
	orig_mkdir = (orig_mkdir_t) sys_call_table[__NR_mkdir];
	orig_symlinkat = (orig_symlinkat_t) sys_call_table[__NR_symlinkat];
	orig_linkat = (orig_linkat_t) sys_call_table[__NR_linkat];
	orig_rename = (orig_rename_t) sys_call_table[__NR_rename];

	printk("Info: sys_openat: %lx; sys_chdir: %lx; sys_mkdir: %lx; sys_symlinkat: %lx; sys_linkat: %lx;; sys_rename: %lx;\n",\
				(long)orig_openat, (long)orig_chdir, (long)orig_mkdir, (long)orig_symlinkat, (long)orig_linkat, (long)orig_rename);

	// change PTE to allow writing
	pte = lookup_address((unsigned long)sys_call_table, &level);
	set_pte_atomic(pte, pte_mkwrite(*pte));
	printk("Info: Disable write-protection of page with sys_call_table\n");
	// hook sys_openat, sys_chdir, sys_mkdir, sys_symlinkat, sys_linkat
	sys_call_table[__NR_openat] = (demo_sys_call_ptr_t) hooked_openat;
	sys_call_table[__NR_chdir] = (demo_sys_call_ptr_t) hooked_chdir;
	sys_call_table[__NR_mkdir] = (demo_sys_call_ptr_t) hooked_mkdir;
	sys_call_table[__NR_symlinkat] = (demo_sys_call_ptr_t) hooked_symlinkat;
	sys_call_table[__NR_linkat] = (demo_sys_call_ptr_t) hooked_linkat;
	sys_call_table[__NR_rename] = (demo_sys_call_ptr_t) hooked_rename;
	set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));

	printk("Info: sys_call_table hooked!\n");

	netlink_init();
	return 0;
}


static void __exit safebox_exit(void)
{
    pte = lookup_address((unsigned long) sys_call_table, &level);
    set_pte_atomic(pte, pte_mkwrite(*pte));
	// remove hook
	sys_call_table[__NR_openat] = (demo_sys_call_ptr_t) orig_openat;
	sys_call_table[__NR_chdir] = (demo_sys_call_ptr_t) orig_chdir;
	sys_call_table[__NR_mkdir] = (demo_sys_call_ptr_t) orig_mkdir;
	sys_call_table[__NR_symlinkat] = (demo_sys_call_ptr_t) orig_symlinkat;
	sys_call_table[__NR_linkat] = (demo_sys_call_ptr_t) orig_linkat;
	sys_call_table[__NR_rename] = (demo_sys_call_ptr_t) orig_rename;
	set_pte_atomic(pte, pte_clear_flags(*pte, _PAGE_RW));

    netlink_release();
}

module_init(safebox_init);
module_exit(safebox_exit);
