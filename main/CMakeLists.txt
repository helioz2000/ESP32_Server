idf_component_register(SRCS "main.c" "tcpserver.c" "usbhost.c" "httpserver.c" 
                       PRIV_REQUIRES esp_netif esp_eth esp_http_server nvs_flash
                       INCLUDE_DIRS ".")
