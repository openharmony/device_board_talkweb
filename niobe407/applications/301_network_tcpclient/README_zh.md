# Niobe407开发板TCP联网演示

本案例程序将演示怎么在拓维Niobe407开发板上编写一个连接tcp服务器的业务程序，实现开发板联网上报数据到服务器。

## 简述
传输控制协议（TCP，Transmission Control Protocol）是一种面向连接的、可靠的、基于字节流的传输层通信协议，由IETF的RFC 793 定义。

TCP旨在适应支持多网络应用的分层协议层次结构。 连接到不同但互连的计算机通信网络的主计算机中的成对进程之间依靠TCP提供可靠的通信服务。TCP假设它可以从较低级别的协议获得简单的，可能不可靠的数据报服务。 原则上，TCP应该能够在从硬线连接到分组交换或电路交换网络的各种通信系统之上操作。

##  TCP编程步骤
　　1、创建一个socket，用函数socket()； 
　　2、设置socket属性，用函数setsockopt();* 可选 
　　3、绑定IP地址、端口等信息到socket上，用函数bind();* 可选 
　　4、设置要连接的对方的IP地址和端口等属性； 
　　5、连接服务器，用函数connect()； 
　　6、收发数据，用函数send()和recv()，或者read()和write(); 
　　7、关闭网络连接；

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


## 函数详解

### void tcp_client_example(void)

    调用此函数初始化ETH接口，并注册eth_enable_state_callBack回调函数

    函数原型:

        void tcp_client_example(void)
        {
            EthLinkInfo info = {
                .useStaticIp = 1,
                .ipaddr.u8_addr = {192, 168, 8, 200},
                .netmask.u8_addr = {255, 255, 255, 0},
                .gw.u8_addr = {192, 168, 8, 1},

                .useStaticMac = 1,
                .macAddr = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01}
            };

            set_ethernet_link_info(&info);
            ethernet_enable(eth_enable_state_callBack); //有线网络使能
        }

    这里使用的是静态IP和静态MAC地址的模式。

    当需要使用静态IP时， 将useStaticIp设置成1，补充相应的IP参数，并使用set_ethernet_link_info()接口函数将参数注册，调用ethernet_enable()即可。

    当需要使用静态MAC时，将useStaticMac设置成1，补充MAC参数，并使用set_ethernet_link_info()接口函数将参数注册，调用ethernet_enable()即可。

    当使用DHCP和随机MAC地址时，无需设置参数，直接调用ethernet_enable()即可。

### static void eth_enable_state_callBack(EthLinkState state)   

    此函数为ETH状态回调函数，在ETH初始化的时候被注册。

    state：当前ETH连接状态

    函数原型:

        static void eth_enable_state_callBack(EthLinkState state)
        {
            /* ETH连接断开*/
            if(state == STATE_UPDATE_LINK_DOWN){
                printf("ETH LINK STATE: DisConnected!\r\n");
                osThreadTerminate(tcp_client_id);
                tcp_client_id = NULL;
            }
            /* ETH连接成功*/
            else if(state == STATE_UPDATE_LINK_UP){ 
                printf("ETH LINK STATE: Connected!\r\n");

                osThreadAttr_t attr;
                attr.name = "tcp_client";
                attr.attr_bits = 0U;
                attr.cb_mem = NULL;
                attr.cb_size = 0U;
                attr.stack_mem = NULL;
                attr.stack_size = 1024 * 4;
                attr.priority = 25;
                tcp_client_id = osThreadNew((osThreadFunc_t)tcp_client, NULL, &attr);
                if (tcp_client_id == NULL)
                {
                    printf("Falied to create tcp_client thread!\n");
                }
            }
        }

    函数实现功能: 当ETH连接状态发生改变时，回调此函数。
    
    当state == STATE_UPDATE_LINK_UP时，表示连接成功，此时创建1个TCPClient线程

    当state == STATE_UPDATE_LINK_DOWN时，表示连接断开，删除创建的TCPClient线程


### void tcp_client(void *thread_param)

    此函数为TCPClient初始化及数据收发任务函数

    thread_param： 线程传入参数，这里未使用

   实现功能: 实现TCP数据接收后，原数据返回的功能

    函数原型:

        void tcp_client(void *thread_param)
        {
            int sock = -1;
            struct sockaddr_in client_addr;
            char recv_data[512]={0};
            int recv_data_len;
            
            printf("start tcp_client test\r\n");
            while(1)
            {
                /* 申请套接字，本质是netconn_new函数的封装 */
                sock = socket(AF_INET, SOCK_STREAM, 0);
                if (sock < 0)
                {
                printf("Socket error\n");
                osDelay(100);
                continue;
                } 

                /* 清空sockaddr_in结构体内存空间 */
                memset_s(&(client_addr), sizeof(client_addr), 0, sizeof(client_addr)); 

                /* 为sockaddr_in结构体成员赋值，用于以下的connect绑定 */
                client_addr.sin_family = AF_INET;      
                client_addr.sin_port = htons(SERVER_PORT);   
                client_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
            

                printf("try connect to server "SERVER_IP":%d\n",SERVER_PORT);
                /* 将远端server的ip地址与端口进行绑定 */
                if (connect(sock, (struct sockaddr *)&client_addr, sizeof(struct sockaddr)) == -1) 
                {
                    closesocket(sock);
                    osDelay(1000);
                    continue;
                }

                printf("Connect to tcp server successful!\n");
                while(1)
                {						
                    /* 成功接收到数据，返回接收的数据长度 */
                    recv_data_len = recv(sock, recv_data, 511, 0);
                    if (recv_data_len <= 0) break;
                    else
                        recv_data[recv_data_len] = '\0';

                    /* 串口打印接收的数据内容 */
                    printf("recv:%s\n",recv_data);		
                    /* 发送数据内容 */
                    write(sock,recv_data,recv_data_len);
                }
            }
        }

        其中:
        
            SERVER_IP 定义要连接的服务器IP

            SERVER_PORT 定义要连接的服务器端口

## 编译调试
- 进入//kernel/liteos_m目录, 在menuconfig配置中进入如下选项:

     `(Top) → Platform → Board Selection → select board niobe407 → use talkweb niobe407 application → niobe407 application choose`

- 选择 `301_network_tcpclient`

- 回到sdk根目录，执行`hb build -f`脚本进行编译。

### 运行结果

示例代码编译烧录代码后，按下开发板的RESET按键，连接电脑端TCP服务器。

电脑端TCP服务器发送消息:
    "hello,this is  device tcp_client test!"

设备端TCP客户端接收消息:

    ETH Init Success!
    ETH LINK STATE: Connected!
    start tcp_client test
    try connect to server 192.168.8.119:8080
    Connect to tcp server successful!
    recv:hello,this is device tcp_client test!

    recv:hello,this is device tcp_client test!

    recv:hello,this is device tcp_client test!

    recv:hello,this is device tcp_client test!

    recv:hello,this is device tcp_client test!

    recv:hello,this is device tcp_client test!

    recv:hello,this is device tcp_client test!

    recv:hello,this is device tcp_client test!

    recv:hello,this is device tcp_client test!