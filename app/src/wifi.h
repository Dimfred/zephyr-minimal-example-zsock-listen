#pragma once

void wifi_init();
int wifi_connect(const char *ssid, const char *psk);
int wifi_disconnect();
