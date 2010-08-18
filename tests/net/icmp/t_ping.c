/*	$NetBSD: t_ping.c,v 1.3 2010/08/18 16:39:22 pooka Exp $	*/

/*-
 * Copyright (c) 2010 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND
 * CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: t_ping.c,v 1.3 2010/08/18 16:39:22 pooka Exp $");
#endif /* not lint */

#include <sys/types.h>
#include <sys/resource.h>

#include <atf-c.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rump/rump.h>

#include "../../h_macros.h"
#include "../config/netconfig.c"

ATF_TC(simpleping);
ATF_TC_HEAD(simpleping, tc)
{

	atf_tc_set_md_var(tc, "descr", "check that kernel responds to ping");
	atf_tc_set_md_var(tc, "use.fs", "true");
	atf_tc_set_md_var(tc, "timeout", "2");
}

ATF_TC_BODY(simpleping, tc)
{
	char ifname[IFNAMSIZ];
	pid_t cpid;
	bool win, win2;

	cpid = fork();
	rump_init();
	netcfg_rump_makeshmif("but-can-i-buy-your-ether-bus", ifname);

	switch (cpid) {
	case -1:
		atf_tc_fail_errno("fork failed");
	case 0:
		netcfg_rump_if(ifname, "1.1.1.10", "255.255.255.0");
		pause();
		break;
	default:
		break;
	}

	netcfg_rump_if(ifname, "1.1.1.20", "255.255.255.0");

	/*
	 * The beauty of shmif is that we don't have races here.
	 */
	win = netcfg_rump_pingtest("1.1.1.10", 500);
	win2 = netcfg_rump_pingtest("1.1.1.30", 500);

	kill(cpid, SIGKILL);

	if (!win)
		atf_tc_fail("ping failed");
	if (win2)
		atf_tc_fail("non-existent host responded");
}

ATF_TC(floodping);
ATF_TC_HEAD(floodping, tc)
{

	atf_tc_set_md_var(tc, "descr", "see how kernel responds to floodping");
	atf_tc_set_md_var(tc, "use.fs", "true");
}

/* why the hell isn't this available in userspace??? */
static uint16_t
in_cksum(void *data, size_t len)
{
	uint16_t *buf = data;
	unsigned sum;

	for (sum = 0; len > 1; len -= 2)
		sum += *buf++;
	if (len)
		sum += *(uint8_t *)buf;

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);

	return ~sum;
}

static int
doping(const char *target, int loops, size_t pktsize)
{
	char sndbuf[IP_MAXPACKET - sizeof(struct ip)];
	char recvbuf[IP_MAXPACKET];
	struct sockaddr_in dst, pingee;
	struct icmp *icmp;
	socklen_t slen;
	ssize_t n;
	int loop, i, succ;
	int x, xnon, s;

	RL(s = rump_sys_socket(PF_INET, SOCK_RAW, IPPROTO_ICMP));
	RL(x = rump_sys_fcntl(s, F_GETFL, 0));
	xnon = x | O_NONBLOCK;

	memset(&dst, 0, sizeof(dst));
	dst.sin_len = sizeof(dst);
	dst.sin_family = AF_INET;
	dst.sin_addr.s_addr = inet_addr(target);

	icmp = (struct icmp *)sndbuf;
	memset(icmp, 0, sizeof(*icmp));
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_id = htons(37);

	if (pktsize < sizeof(*icmp))
		pktsize = sizeof(*icmp);
	if (pktsize > sizeof(sndbuf))
		pktsize = sizeof(sndbuf);

	RL(rump_sys_setsockopt(s, SOL_SOCKET, SO_SNDBUF,
	    &pktsize, sizeof(pktsize)));
	RL(rump_sys_setsockopt(s, SOL_SOCKET, SO_RCVBUF,
	    &pktsize, sizeof(pktsize)));

	slen = sizeof(pingee);
	succ = 0;
	for (loop = 0; loop < loops; loop++) {
		RL(rump_sys_fcntl(s, F_SETFL, x));
		icmp->icmp_seq = htons(loop);
		icmp->icmp_cksum = 0;
		icmp->icmp_cksum = in_cksum(icmp, pktsize);
		RL(rump_sys_sendto(s, icmp, pktsize, 0,
		    (struct sockaddr *)&dst, sizeof(dst)));

		RL(rump_sys_fcntl(s, F_SETFL, xnon));
		while ((n = rump_sys_recvfrom(s, recvbuf, sizeof(recvbuf), 0,
		    (struct sockaddr *)&pingee, &slen)) > 0) {
			succ++;
		}
		if (n == -1 && errno == EAGAIN)
			continue;
		atf_tc_fail_errno("recv failed");
	}

	rump_sys_close(s);
	return succ;
}

#define LOOPS 10000

ATF_TC_BODY(floodping, tc)
{
	char ifname[IFNAMSIZ];
	pid_t cpid;
	int succ;

	cpid = fork();
	rump_init();
	netcfg_rump_makeshmif("thank-you-driver-for-getting-me-here", ifname);

	switch (cpid) {
	case -1:
		atf_tc_fail_errno("fork failed");
	case 0:
		netcfg_rump_if(ifname, "1.1.1.10", "255.255.255.0");
		pause();
		break;
	default:
		break;
	}

	netcfg_rump_if(ifname, "1.1.1.20", "255.255.255.0");

	succ = doping("1.1.1.10", LOOPS, 56);
	printf("got %d/%d\n", succ, LOOPS);

	kill(cpid, SIGKILL);
}

ATF_TC(floodping2);
ATF_TC_HEAD(floodping2, tc)
{

	atf_tc_set_md_var(tc, "descr", "two hosts floodpinging each other");
	atf_tc_set_md_var(tc, "use.fs", "true");
}

ATF_TC_BODY(floodping2, tc)
{
	char ifname[IFNAMSIZ];
	pid_t cpid;
	int succ;

	cpid = fork();
	rump_init();
	netcfg_rump_makeshmif("floodping2", ifname);

	switch (cpid) {
	case -1:
		atf_tc_fail_errno("fork failed");
	case 0:
		netcfg_rump_if(ifname, "1.1.1.10", "255.255.255.0");
		succ = doping("1.1.1.20", LOOPS, 56);
		break;
	default:
		netcfg_rump_if(ifname, "1.1.1.20", "255.255.255.0");
		succ = doping("1.1.1.10", LOOPS, 56);
		break;
	}

	printf("got %d/%d\n", succ, LOOPS);
}

ATF_TC(pingsize);
ATF_TC_HEAD(pingsize, tc)
{

	atf_tc_set_md_var(tc, "descr", "ping with packets min <= size <= max");
	atf_tc_set_md_var(tc, "use.fs", "true");
}

ATF_TC_BODY(pingsize, tc)
{
	char ifname[IFNAMSIZ];
	pid_t cpid;
	int succ, i;

	cpid = fork();
	rump_init();
	netcfg_rump_makeshmif("jippikaiee", ifname);

	switch (cpid) {
	case -1:
		atf_tc_fail_errno("fork failed");
	case 0:
		netcfg_rump_if(ifname, "1.1.1.10", "255.255.255.0");
		pause();
		break;
	default:
		break;
	}

	netcfg_rump_if(ifname, "1.1.1.20", "255.255.255.0");

	succ = 0;

	/* small sizes */
	for (i = IP_MAXPACKET - 60000; i > 0; i--)
		succ += doping("1.1.1.10", 1, i);

	/* medium sizes */
	for (i = IP_MAXPACKET - 100; i > IP_MAXPACKET - 60000; i -= 1000)
		succ += doping("1.1.1.10", 1, i);

	/* big sizes */
	for (i = IP_MAXPACKET; i > IP_MAXPACKET - 100; i -= 10)
		succ += doping("1.1.1.10", 1, i);

	printf("got %d/%d\n", succ, IP_MAXPACKET);
	kill(cpid, SIGKILL);
}

ATF_TP_ADD_TCS(tp)
{

	ATF_TP_ADD_TC(tp, simpleping);
	ATF_TP_ADD_TC(tp, floodping);
	ATF_TP_ADD_TC(tp, floodping2);
	ATF_TP_ADD_TC(tp, pingsize);

	return atf_no_error();
}
