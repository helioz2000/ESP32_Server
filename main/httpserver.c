#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_http_server.h"

// Note: If you are using a specific hardware board like the Waveshare ESP32-P4-ETH, 
// make sure to initialize your specific Ethernet PHY pins (like RTL8211 or LAN8720) 
// using the `esp_eth` driver before starting the network interface.
#include "esp_eth.h" 

#include "httpserver.h"

static const char *TAG = "httpserver";

#define HTTP_PORT 80

// 1. Define the HTTP GET Handler function
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    const char *response_html = "<html><body><h1>Hello from ESP32-P4 under ESP-IDF v6!</h1></body></html>";
    
    // Set HTTP status code and content type metadata
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_status(req, "200 OK");
    
    // Stream the raw response buffer back down the TCP connection socket
    httpd_resp_send(req, response_html, HTTPD_RESP_USE_STRLEN);
    
    ESP_LOGI(TAG, "Webpage request handled successfully");
    return ESP_OK;

    // Failure
    return ESP_FAIL;
}

// 2. Associate the handler to a URI path string
static const httpd_uri_t hello_uri_route = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    .user_ctx  = NULL
};

// 3. Server Startup Sequence
httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    
    // Set the TCP listening port (Standard is port 80)
    config.server_port = HTTP_PORT;

    // Start the httpd server daemon loop
    ESP_LOGI(TAG, "Starting web server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Register the active route endpoints
        httpd_register_uri_handler(server, &hello_uri_route);
        return server;
    }

    ESP_LOGE(TAG, "Error starting server!");
    return NULL;
}

// Event handler loop tracking network statuses
static void network_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == IP_EVENT && event_id == IP_EVENT_ETH_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "ESP32-P4 Network Online! IP Address: " IPSTR, IP2STR(&event->ip_info.ip));
        
        // Trigger server boot once IP allocation is complete
        start_webserver();
    }
}

void httpd_init(void) 
{
    // Initialize Non-Volatile Flash memory storage allocation
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

/*
void app_main(void)
{
    // Initialize Non-Volatile Flash memory storage allocation
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize the baseline TCP/IP Stack systems
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Register handlers to track successful DHCP IP addresses assignments
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &network_event_handler, NULL));

    // * NOTE FOR ETH INTERFACE INSTANTIATION:
    // * To run this code on real hardware, you must add your board's physical Ethernet 
    // * initialisation routine here (e.g., using `esp_eth_driver_install`). 
    // * Once your PHY registers a physical link layer carrier signal, the DHCP engine 
    // * will trigger IP_EVENT_ETH_GOT_IP automatically.
     
   ESP_LOGI(TAG, "Network sub-system initialized. Awaiting physical carrier link interface connection...");
}
*/