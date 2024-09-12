#pragma once

#include <zephyr/net/http/server.h>
#include <zephyr/net/http/service.h>

#include "config.h"

static uint8_t recv_buffer[1024];

static uint16_t http_server_port = HTTP_SERVER_PORT;
HTTP_SERVICE_DEFINE(httpserver_service, "0.0.0.0", &http_server_port, 2, 10, nullptr);

////////////////////////////////////////////////////////////////////////////////
/// DATA
int handle_data_start(http_client_ctx *client, http_data_status status, uint8_t *buffer, size_t len, void *user_data);

static http_resource_detail_dynamic data_start_resource_detail = {
    .common = {.bitmask_of_supported_http_methods = BIT(HTTP_GET), .type = HTTP_RESOURCE_TYPE_DYNAMIC},
    .cb = handle_data_start,
    .data_buffer = recv_buffer,
    .data_buffer_len = sizeof(recv_buffer),
    .user_data = nullptr};

HTTP_RESOURCE_DEFINE(data_start_resource, httpserver_service, "/data/start", &data_start_resource_detail);
