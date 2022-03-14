# OpenHarmony驱动子系统开发—ETH驱动

## 配置说明

    1. 在lwip_adapter/include/app_ethernet.h中：

        #define USE_DHCP： 当此宏被定义时，表示开启DHCP功能，若需使用静态IP，则注释此宏定义即可   

        #define DHCP_TIMEOUT_S：当DHCP功能开启时，此宏定义值表示DHCP超时时间设置

        #define ETH_THREAD_PRIORITY：此宏定义值表示ETH初始化线程的优先级

        以下定义使用静态IP时的IP地址设置:
        /* STATIC IP */
        #define IP_ADDR0   (uint8_t) 192
        #define IP_ADDR1   (uint8_t) 168
        #define IP_ADDR2   (uint8_t) 8
        #define IP_ADDR3   (uint8_t) 200
        
        /*NETMASK*/
        #define NETMASK_ADDR0   (uint8_t) 255
        #define NETMASK_ADDR1   (uint8_t) 255
        #define NETMASK_ADDR2   (uint8_t) 255
        #define NETMASK_ADDR3   (uint8_t) 0

        /*GATEWAY*/
        #define GW_ADDR0   (uint8_t) 192
        #define GW_ADDR1   (uint8_t) 168
        #define GW_ADDR2   (uint8_t) 8
        #define GW_ADDR3   (uint8_t) 1   

     2. 在lwip_adapter/include/lwipopts.h中：  

        此头文件中定义了LWIP的一些基本选项设置，有几个比较重要的参数设置，如下:

        #define MEM_SIZE: 此宏定义了LWIP协议栈使用的内存堆大小

        #define PBUF_POOL_SIZE： 定义了数据收发时的pbuf数量
        #define PBUF_POOL_BUFSIZE：定义了每个pbuf的大小

        LWIP总内存占用基本 = MEM_SIZE + PBUF_POOL_SIZE*PBUF_POOL_BUFSIZE

        #define TCPIP_THREAD_NAME  "TCP/IP"
        #define TCPIP_THREAD_STACKSIZE  4096 ： 这两个宏定义了TCPIP线程运行时的任务名以及栈大小

## 初始化接口说明

    ETH驱动的初始化接口: void ethernet_enable(eth_state_callBack callback);

    此函数实现于：lwip_adapter/src/app_ethernet.c中

    参数callback: 此参数为ETH初始化状态回调函数。

    当调用 ethernet_enable函数，并且注册了状态回调函数后，ETH connect/disConnect状态都会通过此回调返回！

     
## 具体使用步骤

    1. 在main函数中调用 ethernet_enable(eth_callBack)函数，初始化ETH驱动接口，并注册状态回调函数eth_callBack；

    2. 实现回调函数，在回调函数中处理应用程序，例:

        void eth_callBack(EthLinkState state)
        {
            static osThreadId_t tcp_client_id = NULL;

            /* ETH连接断开*/
            if (state == STATE_UPDATE_LINK_DOWN){
                printf("ETH LINK STATE: DisConnected!\r\n");

                // 断开ETH连接时删除TCP线程
                osThreadTerminate(tcp_client_id);
                tcp_client_id = NULL;
            }
            /* ETH连接成功*/
            else if (state == STATE_UPDATE_LINK_UP){ 
                printf("ETH LINK STATE: Connected!\r\n");

                // ETH连接成功后 创建TCP线程
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

        回调函数原型位于: lwip_adapter/include/app_ethernet.h中

        注意: 回调函数实现时，当ETH连接时创建线程，ETH断开后务必记得删除线程。防止多次连接/断开时，创建多个线程而没有释放，导致程序崩溃！

        

