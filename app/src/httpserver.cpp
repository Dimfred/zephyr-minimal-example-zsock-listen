#include "httpserver.h"

int handle_data_start(http_client_ctx *client, http_data_status status, uint8_t *buffer, size_t len, void *user_data)
{
    printk("Start data acquisition task\n");
    return 0;
}
