/*	$NetBSD: res_private.h,v 1.1.1.1 2002/06/20 10:30:43 itojun Exp $	*/

#ifndef res_private_h
#define res_private_h

struct __res_state_ext {
	union res_sockaddr_union nsaddrs[MAXNS];
	struct sort_list {
		int     af;
		union {
			struct in_addr  ina;
			struct in6_addr in6a;
		} addr, mask;
	} sort_list[MAXRESOLVSORT];
	char nsuffix[64];
	char bsuffix[64];
	char nsuffix2[64];
};

extern int
res_ourserver_p(const res_state statp, const struct sockaddr *sa);

#endif
