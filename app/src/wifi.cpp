#include <zephyr/net/net_event.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/wifi_mgmt.h>

#include "wifi.h"

static struct net_mgmt_event_callback wifi_cb;

static K_SEM_DEFINE(wifi_connected, 0, 1);
static K_SEM_DEFINE(ipv4_address_obtained, 0, 1);

////////////////////////////////////////////////////////////////////////////////
/// IMPL
static void handle_wifi_connect_result(net_mgmt_event_callback *cb)
{
    const wifi_status *status = (const wifi_status *)cb->info;
    if (status->status)
    {
        printk("Connection request failed status (%i)\n", status->status);
    }
    else
    {
        printk("Connected\n");
        k_sem_give(&wifi_connected);
    }
}

static void handle_wifi_disconnect_result(net_mgmt_event_callback *cb)
{
    const wifi_status *status = (const wifi_status *)cb->info;

    if (status->status)
    {
        printk("Disconnection request (%d)\n", status->status);
    }
    else
    {
        printk("Disconnected\n");
        k_sem_take(&wifi_connected, K_NO_WAIT);
    }
}

static void handle_ipv4_result(struct net_if *iface)
{
    k_sem_give(&ipv4_address_obtained);
    printk("IP Address obtained...\n");
}

static void wifi_mgmt_event_handler(net_mgmt_event_callback *cb, uint32_t mgmt_event, struct net_if *iface)
{
    switch (mgmt_event)
    {

    case NET_EVENT_WIFI_CONNECT_RESULT:
        handle_wifi_connect_result(cb);
        break;

    case NET_EVENT_WIFI_DISCONNECT_RESULT:
        handle_wifi_disconnect_result(cb);
        break;

    case NET_EVENT_IPV4_ADDR_ADD:
        handle_ipv4_result(iface);
        break;

    default:
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// API
int wifi_connect(const char *ssid, const char *psk)
{
    struct net_if *iface = net_if_get_default();

    struct wifi_connect_req_params wifi_params = {0};
    wifi_params.ssid = (uint8_t *)ssid;
    wifi_params.psk = (uint8_t *)psk;
    wifi_params.ssid_length = strlen(ssid);
    wifi_params.psk_length = strlen(psk);
    wifi_params.channel = 0;
    wifi_params.security = WIFI_SECURITY_TYPE_PSK;
    wifi_params.band = WIFI_FREQ_BAND_2_4_GHZ;
    wifi_params.mfp = WIFI_MFP_OPTIONAL;
    wifi_params.timeout = 20;

    printk("Connecting to SSID: %s\n", wifi_params.ssid);
    if (net_mgmt(NET_REQUEST_WIFI_CONNECT, iface, &wifi_params, sizeof(struct wifi_connect_req_params)))
    {
        printk("WiFi Connection Request Failed\n");
        return 1;
    }

    k_sem_take(&wifi_connected, K_FOREVER);

    // printk("Waiting for IP Address...\n");
    // k_sem_take(&ipv4_address_obtained, K_FOREVER);

    return 0;
}

int wifi_disconnect()
{
    struct net_if *iface = net_if_get_default();

    if (net_mgmt(NET_REQUEST_WIFI_DISCONNECT, iface, NULL, 0))
    {
        printk("WiFi Disconnection Request Failed\n");
        return 1;
    }

    return 0;
}

void wifi_init()
{
    printk("WiFi initializing...\n");
    net_mgmt_init_event_callback(&wifi_cb, wifi_mgmt_event_handler,
                                 NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT);
    net_mgmt_add_event_callback(&wifi_cb);
    printk("WiFi initialized...\n");
}
