/*	$NetBSD: if_mecreg.h,v 1.5 2004/01/11 14:01:46 sekiya Exp $	*/

/*
 * Copyright (c) 2001 Christopher Sekiya
 * Copyright (c) 2000 Soren S. Jorvang
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *          This product includes software developed for the
 *          NetBSD Project.  See http://www.NetBSD.org/ for
 *          information about NetBSD.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * MACE MAC110 ethernet register definitions
 */

#define MEC_MAC_CONTROL			0

#define MEC_MAC_CORE_RESET		0x0000000000000001
#define MEC_MAC_FULL_DUPLEX		0x0000000000000002 /* 1 to enable */
#define MEC_MAC_INT_LOOPBACK		0x0000000000000004 /* 0 = normal oper */
#define MEC_MAC_SPEED_SELECT		0x0000000000000008 /* 0/1 10/100 */
#define MEC_MAC_MII_SELECT		0x0000000000000010
#define MEC_MAC_FILTER_STATION		0x0000000000000000
#define MEC_MAC_FILTER_MATCHMULTI	0x0000000000000020
#define MEC_MAC_FILTER_ALLMULTI		0x0000000000000040
#define MEC_MAC_FILTER_PROMISC		0x0000000000000060
#define MEC_MAC_LINK_FAILURE		0x0000000000000080
#define MEC_MAC_IPGT			0x0000000000007f00
#define MEC_MAC_IPGT_SHIFT		8
#define MEC_MAC_IPGR1			0x00000000003f8000
#define MEC_MAC_IPGR1_SHIFT		15
#define MEC_MAC_IPGR2			0x000000001fc00000
#define MEC_MAC_IPGR2_SHIFT		22
#define MEC_MAC_REVISION		0x00000000e0000000
#define MEC_MAC_REVISION_SHIFT		29

#define MEC_INT_STATUS                  0x08
#define MEC_INT_TX_EMPTY                0x00000001
#define MEC_INT_TX_PACKET_SENT          0x00000002
#define MEC_INT_TX_LINK_FAIL            0x00000004
#define MEC_INT_TX_MEM_ERROR            0x00000008
#define MEC_INT_TX_ABORT                0x00000010
#define MEC_INT_RX_THRESHOLD            0x00000020
#define MEC_INT_RX_FIFO_UNDERFLOW       0x00000040
#define MEC_INT_RX_DMA_UNDERFLOW        0x00000080
#define MEC_INT_RX_MCL_FIFO_ALIAS       0x00001f00
#define MEC_INT_TX_RING_BUFFER_ALIAS    0x01ff0000
#define MEC_INT_RX_SEQUENCE_NUMBER      0x3e000000
#define MEC_INT_MCAST_HASH_OUTPUT       0x40000000

#define MEC_DMA_CONTROL                 0x10
#define MEC_DMA_TX_INT_ENABLE           0x00000001
#define MEC_DMA_TX_DMA_ENABLE           0x00000002
#define MEC_DMA_TX_RING_SIZE_MASK       0x0000000c
#define MEC_DMA_RX_INT_THRESHOLD        0x000001f0
#define MEC_DMA_RX_INT_ENABLE           0x00000200
#define MEC_DMA_RX_RUNT                 0x00000400
#define MEC_DMA_RX_PACKET_GATHER        0x00000800
#define MEC_DMA_RX_DMA_OFFSET           0x00007000
#define MEC_DMA_RX_DMA_ENABLE           0x00008000

#define MEC_TIMER                       0x18
#define MEC_TX_ALIAS                    0x20
#define MEC_TX_ALIAS_INT_ENABLE         0x01

#define MEC_RX_ALIAS                    0x28
#define MEC_RX_ALIAS_INT_ENABLE         0x200
#define MEC_RX_ALIAS_INT_THRESHOLD      0x1f0

#define MEC_TX_RING_PTR                 0x30
#define MEC_TX_RING_WRITE_PTR           0x000001ff
#define MEC_TX_RING_READ_PTR            0x01ff0000
#define MEC_TX_RING_PTR_ALIAS           0x38

#define MEC_RX_FIFO                     0x40
#define MEC_RX_FIFO_ELEMENT_COUNT       0x0000001f
#define MEC_RX_FIFO_READ_PTR            0x00000f00
#define MEC_RX_FIFO_GEN_NUMBER          0x00001000
#define MEC_RX_FIFO_WRITE_PTR           0x000f0000
#define MEC_RX_FIFO_GEN_NUMBER_2        0x00100000

#define MEC_RX_FIFO_ALIAS1              0x48
#define MEC_RX_FIFO_ALIAS2              0x50
#define MEC_TX_VECTOR                   0x58
#define MEC_IRQ_VECTOR                  0x58

#define MEC_PHY_DATA		12 << 3
#define MEC_PHY_DATA_BUSY	0x8000
#define MEC_PHY_DATA_VALUE	0x7fff

#define MEC_PHY_ADDRESS		13 << 3
#define MEC_PHY_ADDR_REGISTER	0x000000000000001f
#define MEC_PHY_ADDR_DEVICE	0x00000000000003e0
#define MEC_PHY_ADDR_DEVSHIFT	5

#define MEC_PHY_READ_INITIATE	14 << 3

#define MEC_PHY_BACKOFF		15 << 3

#define MEC_STATION		0xa0
#define MEC_STATION_ALT		0xa8
#define MEC_STATION_MASK	0x0000ffffffffffff

#define MEC_MULTICAST                   0xb0
#define MEC_TX_RING_BASE                0xb8
#define MEC_TX_PKT1_CMD_1               0xc0
#define MEC_TX_PKT1_BUFFER_1            0xc8
#define MEC_TX_PKT1_BUFFER_2            0xd0
#define MEC_TX_PKT1_BUFFER_3            0xd8
#define MEC_TX_PKT2_CMD_1               0xe0
#define MEC_TX_PKT2_BUFFER_1            0xe8
#define MEC_TX_PKT2_BUFFER_2            0xf0
#define MEC_TX_PKT2_BUFFER_3            0xf8
#define MEC_MCL_RX_FIFO                 0x100

/* these structs should be moved to if_mecvar.h */

/* the first eight bytes of the tx vector are status.  it can be a command
   header, a transmit concatenation pointer, or a status vector. */

#define MEC_TX_COMMAND_HDR_LENGTH       0x0000ffff      /* length, bytes-1 */
#define MEC_TX_COMMAND_HDR_START_OFFSET 0x007f0000      /* minimum 8 bytes */
#define MEC_TX_COMMAND_HDR_TX_IRQ       0x00800000 /* raise TX DMA on abort */
#define MEC_TX_COMMAND_CONCAT_1         0x01000000
#define MEC_TX_COMMAND_CONCAT_2         0x02000000
#define MEC_TX_COMMAND_CONCAT_3         0x04000000

#define MEC_TX_CONCAT_PTR_ADDRESS       0x00000000fffffff8
#define MEC_TX_CONCAT_PTR_LENGTH        0x0000ffff00000000

#define MEC_TX_STATUS_VECTOR_LENGTH             0x000000000000ffff
#define MEC_TX_STATUS_VECTOR_COL_RETRY          0x00000000000f0000
#define MEC_TX_STATUS_VECTOR_LATE_COLLISION     0x0000000000100000
#define MEC_TX_STATUS_VECTOR_CRC_ERROR          0x0000000000200000
#define MEC_TX_STATUS_VECTOR_DEFERRED           0x0000000000400000
#define MEC_TX_STATUS_VECTOR_TX_SUCCESS         0x0000000000800000
#define MEC_TX_STATUS_VECTOR_TOOBIG             0x0000000001000000
#define MEC_TX_STATUS_VECTOR_UNDERRUN           0x0000000002000000
#define MEC_TX_STATUS_VECTOR_COLLISIONS         0x0000000004000000
#define MEC_TX_STATUS_VECTOR_EX_DEFERRAL        0x0000000008000000
#define MEC_TX_STATUS_VECTOR_COLLIDED           0x0000000010000000
#define MEC_TX_STATUS_VECTOR_VALID              0x8000000000000000

struct tx_fifo {
        u_int64_t control;
        u_int8_t data[120];
};

struct rx_fifo {
        u_int64_t control;
        u_int8_t data[120];
};
