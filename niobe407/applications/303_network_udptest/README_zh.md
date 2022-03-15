# Niobe407开发板UDP联网演示

本案例程序将演示怎么在拓维Niobe407开发板上编写一个连接udp服务器的业务程序，实现开发板联网上报数据到服务器。

## 简述
UDP是用户数据包协议（UDP，User Datagram Protocol）, 为应用程序提供了一种无需建立连接就可以发送封装的 IP 数据包的方法。RFC 768 描述了 UDP。

Internet 的传输层有两个主要协议，互为补充。无连接的是 UDP，它除了给应用程序发送数据包功能并允许它们在所需的层次上架构自己的协议之外，几乎没有做什么特别的事情。面向连接的是 TCP，该协议几乎做了所有的事情

##  UDP客户端编程步骤
　　1、创建一个socket，用函数socket()； 
　　2、设置socket属性，用函数setsockopt();* 可选 
　　3、绑定IP地址、端口等信息到socket上，用函数bind();* 可选 
　　4、设置对方的IP地址和端口等属性; 
　　5、循环接收/发送数据，用函数recvfrom()/sendto(); 
　　6、关闭网络连接；

## 特点
1、UDP是无连接的，即发送数据之前不需要建立连接；
2、UDP使用尽最大努力交付，即不保证可靠交付；
3、UDP是面向报文的；
4、UDP支持一对一、一对多、多对一和多对多的交互通信等。

## 结构体详解

```
struct sockaddr_in {
	sa_family_t sin_family;
	in_port_t sin_port;
	struct in_addr sin_addr;
	uint8_t sin_zero[8];
};
```

**描述：**

地址和端口信息

**参数：**

| 名字   | 描述      |
| ------ | -------- |
|sin_family|	指代协议族，在socket编程中只能是AF_INET|
|sin_port|	存储端口号（使用网络字节顺序）|
|sin_addr|	存储IP地址，使用in_addr这个数据结构|
|sin_zero|	为了让sockaddr与sockaddr_in两个数据结构保持大小相同而保留的空字节|

**示例代码如下：**

```
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr)); // 初始化服务器地址
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(_PROT_);
	//server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, _SERVER_IP_, &server_addr.sin_addr);
	int sin_size = sizeof(struct sockaddr_in);
```
--------------------------------------------
```
struct sockaddr {
	sa_family_t sa_family;
	char sa_data[14];
};
```
**描述：**

通用的套接字地址，与sockaddr_in类似

**参数**

| 名字   | 描述      |
| ------ | -------- |
|sa_family|	地址族|
|sa_data|14字节，包含套接字中的目标地址和端口信息 |

------------------------------------

## 函数详解

### void udp_test_example(void)

    调用此函数初始化ETH接口，并注册eth_enable_state_callBack回调函数

    函数原型:

        void tcp_client_example(void)
        {
            ethernet_enable(eth_enable_state_callBack); //有线网络使能
        }


### static void eth_enable_state_callBack(EthLinkState state)   

    此函数为ETH状态回调函数，在ETH初始化的时候被注册。

    state：当前ETH连接状态

    函数原型:

        static void eth_enable_state_callBack(EthLinkState state)
        {
            static int net_init_finish = 0;
            /* ETH连接断开*/
            if(state == STATE_UPDATE_LINK_DOWN){
                printf("ETH LINK STATE: DisConnected!\r\n");
            }
            /* ETH连接成功*/
            else if(state == STATE_UPDATE_LINK_UP){ 
                printf("ETH LINK STATE: Connected!\r\n");
                if(net_init_finish == 0)
                {
                    osThreadAttr_t attr;
                    attr.name = "udp_test";
                    attr.attr_bits = 0U;
                    attr.cb_mem = NULL;
                    attr.cb_size = 0U;
                    attr.stack_mem = NULL;
                    attr.stack_size = 1024 * 4;
                    attr.priority = 25;
                    udp_test_id = osThreadNew((osThreadFunc_t)udp_test, NULL, &attr);
                    if (udp_test_id == NULL)
                    {
                        printf("Falied to create udp_test thread!\n");
                    }
                    net_init_finish = 1;
                }
            }
        }

    函数实现功能: 当ETH连接状态发生改变时，回调此函数。
    
    当state == STATE_UPDATE_LINK_UP时，表示连接成功，此时创建1个UDP线程, 且将net_init_finish = 1。 如此， 当ETH状态断开再连接时，将不会再次创建udp线程

    当state == STATE_UPDATE_LINK_DOWN时，表示连接断开


### void udp_test(void *arg)

   此函数为UDP初始化函数

   arg：线程传入参数，这里未使用

   实现功能: 实现UDP数据接收后，原数据返回的功能

   函数原型:

        void udp_test(void *arg)
        {
            (void)arg;
            ip_addr_t serverIP;
            err_t err;
            ipaddr_aton(DESTINATION_IP, &serverIP);//目标IP地址
            upcb = udp_new();
            if (upcb != NULL)
            {
                /* 配置本地端口 */
                upcb->local_port = LOCAL_PORT;  //开发板端口
                
                /* 配置目标IP和端口 */
                err = udp_connect(upcb, &serverIP, DESTINATION_PORT);  //目标端口号
                if (err == ERR_OK)
                {
                    /* 注册接收回调函数 */
                    udp_recv(upcb, udp_receive_callback, NULL);
                    printf("udp client connected\r\n");
                }
                else
                {
                    udp_remove(upcb);
                    printf("can not connect udp pcb.\r\n");
                }
            }
            while(!udp_rcv_flag)
            {
                printf("wait recv udp data...\n");
                osDelay(1000);
            }
        } 

        其中：
             LOCAL_PORT 表示本机端口号

             DESTINATION_PORT 表示远端端口号

             DESTINATION_IP: 表示远端IP地址

### void udp_client_send(char *pData)

    此函数为UDP数据发送函数

    pData: 发送的数据缓冲区BUF地址


### void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)

    此函数为UDP数据接收回调函数，当UDP接收到数据时会回调此函数

    arg: 函数入参，这里未使用

    upcb: UDP句柄

    p： 收到的数据

    addr: 收到的IP地址

    port: 收到的端口号

    功能: 接收到数据，将数据信息打印出来。并且返回原样数据...

    函数原型:

        void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
        {
            char *recdata = 0;
            if(p != NULL)
            {
                recdata=(char *)malloc(p->len*sizeof(char)+1);
                if(recdata != NULL)
                {
                    if(udp_rcv_flag != 1)
                        udp_rcv_flag = 1;
                    memcpy_s(recdata, p->len, p->payload, p->len);
                    recdata[p->len] = '\0';
                    printf("udp recv from[%d.%d.%d.%d:%d]: %s\n", *((uint8_t *)&addr->addr), *((uint8_t *)&addr->addr + 1),
                                    *((uint8_t *)&addr->addr + 2), *((uint8_t *)&addr->addr + 3), port, recdata);
                }
                free(recdata);
                udp_send(upcb, p);
                /* Free receive pbuf */
                pbuf_free(p);
            }
        }

## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `303_network_udptest`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，使用电脑udp连接设备。

电脑端udp发送消息:
    "hello,this is device udp test!"

设备端udp接收消息:

    ETH Init Success!
    ETH LINK STATE: Connected!
    udp client connected
    wait recv udp data...
    wait recv udp data...
    wait recv udp data...
    wait recv udp data...
    wait recv udp data...
    wait recv udp data...
    wait recv udp data...
    wait recv udp data...
    udp recv from[192.168.8.119:5678]: hello,this is device udp test!

    udp recv from[192.168.8.119:5678]: hello,this is device udp test!

    udp recv from[192.168.8.119:5678]: hello,this is device udp test!

    udp recv from[192.168.8.119:5678]: hello,this is device udp test!

    udp recv from[192.168.8.119:5678]: hello,this is device udp test!

    udp recv from[192.168.8.119:5678]: hello,this is device udp test!

    udp recv from[192.168.8.119:5678]: hello,this is device udp test!


