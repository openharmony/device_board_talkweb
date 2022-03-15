/*
 * Copyright (c) 2022 Talkweb Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include_next "lwip/lwipopts.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#undef LWIP_CHECKSUM_ON_COPY
#define LWIP_CHECKSUM_ON_COPY    0

#undef  ETH_PAD_SIZE
#define ETH_PAD_SIZE             0

#undef  MEM_LIBC_MALLOC
#define MEM_LIBC_MALLOC          0

#undef  NO_SYS
#define NO_SYS                   0

#undef  MEM_ALIGNMENT
#define MEM_ALIGNMENT            4

#undef  MEM_SIZE
#define MEM_SIZE                 (15*1024)

#undef  MEMP_NUM_PBUF
#define MEMP_NUM_PBUF            10

#undef  MEMP_NUM_UDP_PCB
#define MEMP_NUM_UDP_PCB         6

#undef  MEMP_NUM_TCP_PCB
#define MEMP_NUM_TCP_PCB         10

#undef  MEMP_NUM_TCP_PCB_LISTEN
#define MEMP_NUM_TCP_PCB_LISTEN  5

#undef  MEMP_NUM_TCP_SEG
#define MEMP_NUM_TCP_SEG         8

#undef  MEMP_NUM_SYS_TIMEOUT
#define MEMP_NUM_SYS_TIMEOUT     10

#undef  MEMP_NUM_NETCONN
#define MEMP_NUM_NETCONN         10

#undef  PBUF_POOL_SIZE
#define PBUF_POOL_SIZE           8

#undef  PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE        512

#undef  LWIP_IPV4
#define LWIP_IPV4                1

#undef  LWIP_TCP
#define LWIP_TCP                 1

#undef  TCP_TTL
#define TCP_TTL                  255

#undef  TCP_QUEUE_OOSEQ
#define TCP_QUEUE_OOSEQ          0

#undef  TCP_MSS
#define TCP_MSS                  (1500 - 40)


#undef  TCP_SND_BUF
#define TCP_SND_BUF              (4*TCP_MSS)
 
#undef  TCP_SND_QUEUELEN
#define TCP_SND_QUEUELEN         (2* TCP_SND_BUF/TCP_MSS)

#undef  TCP_WND
#define TCP_WND                  (2*TCP_MSS)

#undef  LWIP_ICMP
#define LWIP_ICMP                1

#undef  LWIP_DHCP
#define LWIP_DHCP                1

#undef  LWIP_UDP
#define LWIP_UDP                 1

#undef  UDP_TTL
#define UDP_TTL                  255

#undef  LWIP_NETIF_LINK_CALLBACK
#define LWIP_NETIF_LINK_CALLBACK 1

#undef  LWIP_SOCKET_FCNTL_FUNC
#define LWIP_SOCKET_FCNTL_FUNC   1

#undef  CHECKSUM_BY_HARDWARE
#define CHECKSUM_BY_HARDWARE

#ifdef CHECKSUM_BY_HARDWARE
#define CHECKSUM_GEN_IP          0
#define CHECKSUM_GEN_UDP         0
#define CHECKSUM_GEN_TCP         0
#define CHECKSUM_CHECK_IP        0
#define CHECKSUM_CHECK_UDP       0
#define CHECKSUM_CHECK_TCP       0
#define CHECKSUM_GEN_ICMP        0
#else
#define CHECKSUM_GEN_IP          1
#define CHECKSUM_GEN_UDP         1
#define CHECKSUM_GEN_TCP         1
#define CHECKSUM_CHECK_IP        1
#define CHECKSUM_CHECK_UDP       1
#define CHECKSUM_CHECK_TCP       1
#define CHECKSUM_GEN_ICMP        1
#endif

#undef  LWIP_NETCONN
#define LWIP_NETCONN             1

#undef  LWIP_SOCKET
#define LWIP_SOCKET              1

#undef LWIP_DNS
#define LWIP_DNS                 1

#undef  TCPIP_THREAD_NAME
#define TCPIP_THREAD_NAME        "TCP/IP"

#undef  TCPIP_THREAD_STACKSIZE
#define TCPIP_THREAD_STACKSIZE   4096

#undef  TCPIP_MBOX_SIZE
#define TCPIP_MBOX_SIZE          6

#undef  DEFAULT_UDP_RECVMBOX_SIZE
#define DEFAULT_UDP_RECVMBOX_SIZE   6

#undef  DEFAULT_TCP_RECVMBOX_SIZE
#define DEFAULT_TCP_RECVMBOX_SIZE   6

#undef  DEFAULT_ACCEPTMBOX_SIZE
#define DEFAULT_ACCEPTMBOX_SIZE     6

#undef  DEFAULT_THREAD_STACKSIZE
#define DEFAULT_THREAD_STACKSIZE    500

#undef  TCPIP_THREAD_PRIO
#define TCPIP_THREAD_PRIO           25

#define MQTT_DEBUG                  LWIP_DBG_ON

#undef SNTP_SET_SYSTEM_TIME
#define SNTP_SET_SYSTEM_TIME(sec)   SntpSetTime(sec)

#undef  SNTP_RECV_TIMEOUT
#define SNTP_RECV_TIMEOUT           5000

#undef  SNTP_UPDATE_DELAY
#define SNTP_UPDATE_DELAY           (15*60*1000)

#undef SNTP_SERVER_DNS
#define SNTP_SERVER_DNS             1

#undef LWIP_DHCP_MAX_NTP_SERVERS
#define LWIP_DHCP_MAX_NTP_SERVERS   10

#undef MQTT_OUTPUT_RINGBUF_SIZE
#define MQTT_OUTPUT_RINGBUF_SIZE    1024

#undef MQTT_VAR_HEADER_BUFFER_LEN
#define MQTT_VAR_HEADER_BUFFER_LEN  1024

#undef MQTT_REQ_MAX_IN_FLIGHT
#define MQTT_REQ_MAX_IN_FLIGHT      15

#undef MQTT_REQ_TIMEOUT
#define MQTT_REQ_TIMEOUT            30

#undef MQTT_CONNECT_TIMOUT
#define MQTT_CONNECT_TIMOUT         100

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __LWIPOPTS_H__ */