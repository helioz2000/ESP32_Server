idf_component_register(SRCS "main.c" "tcpserver.c" "usbhost.c"
                       PRIV_REQUIRES esp_netif esp_eth
                       INCLUDE_DIRS ".")
