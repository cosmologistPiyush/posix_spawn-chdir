/* $NetBSD: rump_syscalls.h,v 1.8 2008/05/29 12:02:31 pooka Exp $ */

/*
 * System call protos in rump namespace.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.201 2008/05/29 12:01:37 pooka Exp
 */

ssize_t rump_sys_read(int, void *, size_t, int *);
ssize_t rump_sys_write(int, const void *, size_t, int *);
int rump_sys_open(const char *, int, mode_t, int *);
int rump_sys_close(int, int *);
int rump_sys_link(const char *, const char *, int *);
int rump_sys_unlink(const char *, int *);
int rump_sys_chdir(const char *, int *);
int rump_sys_fchdir(int, int *);
int rump_sys_mknod(const char *, mode_t, dev_t, int *);
int rump_sys_chmod(const char *, mode_t, int *);
int rump_sys_chown(const char *, uid_t, gid_t, int *);
int rump_sys_unmount(const char *, int, int *);
int rump_sys_access(const char *, int, int *);
int rump_sys_chflags(const char *, u_long, int *);
int rump_sys_fchflags(int, u_long, int *);
void rump_sys_sync(int *);
int rump_sys_symlink(const char *, const char *, int *);
ssize_t rump_sys_readlink(const char *, char *, size_t, int *);
int rump_sys_rename(const char *, const char *, int *);
int rump_sys_mkdir(const char *, mode_t, int *);
int rump_sys_rmdir(const char *, int *);
ssize_t rump_sys_pread(int, void *, size_t, int, off_t, int *);
ssize_t rump_sys_pwrite(int, const void *, size_t, int, off_t, int *);
int rump_sys_truncate(const char *, int, off_t, int *);
int rump_sys_lchmod(const char *, mode_t, int *);
int rump_sys_lchown(const char *, uid_t, gid_t, int *);
