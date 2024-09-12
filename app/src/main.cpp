#include <errno.h>
#include <exception>
#include <stdlib.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>

#include "config.h"
#include "httpserver.h"
#include "main.h"
#include "wifi.h"

////////////////////////////////////////////////////////////////////////////////
/// MAIN
int run_main()
{
    try
    {
        // wifi_init();
        // wifi_connect(SSID, PSK);
        // http_server_start();

        sockaddr_in addr4;
        addr4.sin_family = AF_INET;
        addr4.sin_port = htons(80);
        zsock_inet_pton(AF_INET, "0.0.0.0", &addr4.sin_addr);
        size_t len = sizeof(sockaddr_in);

        int sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock < 0)
        {
            printk("Socket creation failed\n");
            return 1;
        }

        int flag = 1;
        if (zsock_setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0)
        {
            printk("setsockopt: %d\n", errno);
            zsock_close(sock);
            return 1;
        }

        if (zsock_bind(sock, (sockaddr *)&addr4, sizeof(addr4)) < 0)
        {
            printk("bind: %d\n", errno);
            zsock_close(sock);
            return 1;
        }

        if (listen(sock, 1) < 0)
        {
            printk("listen: %d\n", errno);
            zsock_close(sock);
            return 1;
        }
    }
    catch (std::exception &e)
    {
        printk("Exception: %s\n", e.what());
        return 1;
    }

    printk("Main thread done, sleeping forever...");
    k_sleep(K_FOREVER);
    return 0;
}
