# Niobe407开发板TCP联网演示

本案例程序将演示怎么在拓维Niobe407开发板上编写一个创建tcp服务器的业务程序，实现开发板联网与tcp客户端数据通信。

## 简述
传输控制协议（TCP，Transmission Control Protocol）是一种面向连接的、可靠的、基于字节流的传输层通信协议，由IETF的RFC 793 定义。

TCP旨在适应支持多网络应用的分层协议层次结构。 连接到不同但互连的计算机通信网络的主计算机中的成对进程之间依靠TCP提供可靠的通信服务。TCP假设它可以从较低级别的协议获得简单的，可能不可靠的数据报服务。 原则上，TCP应该能够在从硬线连接到分组交换或电路交换网络的各种通信系统之上操作。

##  TCP编程步骤
　　1、创建一个socket，用函数socket()； 
　　2、设置socket属性，用函数setsockopt(); * 可选 
　　3、绑定IP地址、端口等信息到socket上，用函数bind(); 
　　4、开启监听，用函数listen()； 
　　5、接收客户端上来的连接，用函数accept()； 
　　6、收发数据，用函数send()和recv()，或者read()和write(); 
　　7、关闭网络连接； 
　　8、关闭监听

## 特点
（1）基于流的方式；
（2）面向连接；
（3）可靠通信方式；
（4）在网络状况不佳的时候尽量降低系统由于重传带来的带宽开销；
（5）通信连接维护是面向通信的两个端点的，而不考虑中间网段和节点

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
	//服务端地址信息
	struct sockaddr_in server_sock;
	bzero(&server_sock, sizeof(server_sock));
	server_sock.sin_family = AF_INET;
	//server_sock.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, "192.168.1.121", &server_sock.sin_addr);
	server_sock.sin_port = htons(_PROT_);
```

------------------------------------

```
typedef struct {
	unsigned long fds_bits[FD_SETSIZE / 8 / sizeof(long)];
} fd_set;
```

**描述：**

文件描述符集合

**操作函数：**

| 名字   | 描述      |
| ------ | -------- |
|void FD_CLR(int fd, fd_set *set)|	清除某一个被监视的文件描述符|
|int  FD_ISSET(int fd, fd_set *set)|测试一个文件描述符是否是集合中的一员|
|void FD_SET(int fd, fd_set *set)|	添加一个文件描述符，将set中的某一位设置成1|
|void FD_ZERO(fd_set *set)|	清空集合中的文件描述符,将每一位都设置为0|

**示例代码如下：**

```
	fd_set fds;
	FD_ZERO(&fds);
    FD_SET(sock_fd,&fds);//将sock_fd添加至fds

    if(FD_ISSET(sock_fd,&fds))//判断sock_fd是否在fds中
        ;//

```

## 函数详解

### void tcp_server_example(void)

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
                    attr.name = "tcp_server";
                    attr.attr_bits = 0U;
                    attr.cb_mem = NULL;
                    attr.cb_size = 0U;
                    attr.stack_mem = NULL;
                    attr.stack_size = 1024 * 4;
                    attr.priority = 25;
                    tcp_server_id = osThreadNew((osThreadFunc_t)tcp_server, NULL, &attr);
                    if (tcp_server_id == NULL)
                    {
                        printf("Failed to create tcp_server thread!\n");
                    }
                    net_init_finish = 1;
                }
            }
        }

    函数实现功能: 当ETH连接状态发生改变时，回调此函数。
    
    当state == STATE_UPDATE_LINK_UP时，表示连接成功，此时创建1个TCPClient线程, 且将net_init_finish = 1。 如此， 当ETH状态断开再连接时，将不会再次创建TCPServer线程

    当state == STATE_UPDATE_LINK_DOWN时，表示连接断开

### void tcp_server(void *argument)

   此函数为TCPServer初始化及数据收发任务函数 

   argument：线程传入参数，这里未使用

   实现功能: 实现TCP数据接收后，原数据返回的功能

   函数原型:

        void tcp_server(void *argument)
        {
            (void)argument;
            int sock = -1,connected;
            char recv_data[512]={0};
            struct sockaddr_in server_addr,client_addr;
            socklen_t sin_size;
            int recv_data_len;

            printf("start tcp_server test\r\n");
            /* 申请套接字，本质是netconn_new函数的封装 */
            sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0){
                printf("Socket error\n");
                goto __exit;
            }
            /* 为sockaddr_in结构体成员赋值，用于以下的bind绑定 */
            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = INADDR_ANY;
            server_addr.sin_port = htons(SERVER_LISTEN_PORT);
            /* 清空sockaddr_in结构体内存空间 */
            memset_s(&(server_addr.sin_zero), sizeof(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

            /* 服务器绑定ip地址与端口 */
            if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
            {
                printf("Unable to bind\n");
                goto __exit;
            }

            /* 服务器进入监听状态 */
            if (listen(sock, 5) == -1)
            {
                printf("Listen error\n");
                goto __exit;
            }

            while(1)
            {
                sin_size = sizeof(struct sockaddr_in);
                /* 等待远端client的链接 */
                connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);

                printf("new client connected from (%s, %d)\n",
                inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                {
                    int flag = 1;
                    /* 设置套接字的选项 */
                    setsockopt(connected,
                    IPPROTO_TCP,     /* set option at TCP level */
                    TCP_NODELAY,     /* name of option */
                    (void *) &flag,  /* the cast is historical cruft */
                    sizeof(int));    /* length of option value */
                }

                while(1)
                {
                    /* 成功接收到数据，返回接收的数据长度 */
                    recv_data_len = recv(connected, recv_data, 511, 0);

                    if (recv_data_len <= 0) 
                        break;
                    else
                        recv_data[recv_data_len] = '\0';

                    printf("recv %s\n",recv_data); 

                    /* 发送数据内容 */			
                    write(connected,recv_data,recv_data_len);

                    osDelay(200);	
                }
                if (connected >= 0) 
                    closesocket(connected);

                connected = -1;
            }
            __exit:
            if (sock >= 0) closesocket(sock);
            if (recv_data) free(recv_data);   
        }  

         其中：
         
            server_addr.sin_addr.s_addr = INADDR_ANY 表示连接所有IP的TCPClient

            SERVER_LISTEN_PORT 表示服务器监听的端口号

        
## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `302_network_tcpserver`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，使用电脑客户端连接设备服务器。

电脑端TCP客户端发送消息:
    "hello,this is device tcp_service test!"

设备端TCP服务器接收消息:

    ETH Init Success!
    ETH LINK STATE: Connected!
    start tcp_server test
    new client connected from (192.168.8.119, 64333)
    recv hello,this is device tcp_service test!

    recv hello,this is device tcp_service test!

    recv hello,this is device tcp_service test!

    recv hello,this is device tcp_service test!

    recv hello,this is device tcp_service test!

    recv hello,this is device tcp_service test!

    recv hello,this is device tcp_service test!

    recv hello,this is device tcp_service test!
