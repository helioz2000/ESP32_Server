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

// Example variable in your C code
int my_counter_variable = 42;

// Raw HTML Webpage
/*const char* html_page = 
"<!DOCTYPE html><html>"
"<head><meta name='viewport' content='width=device-width, initial-scale=1'>"
"<style>html { font-family: sans-serif; display: inline-block; margin: 0px auto; text-align: center;}"
".btn { border: none; color: white; padding: 16px 40px;"
"text-decoration: none; font-size: 30px; margin: 10px; cursor: pointer; border-radius: 5px;}"
".btn-on { background-color: #4CAF50; }"
".btn-off { background-color: #f44336; }"
"</style>"
"<script>"
"function toggleGPIO(action) {"
"  fetch('/api/' + action)"
"    .then(response => console.log('Action sent: ' + action))"
"    .catch(err => console.error('Error:', err));"
"}"
"</script>"
"<title>ESP32-P4 Control</title></head>"
"<body><h1>ESP32-P4 GPIO Control</h1>"
"<p><button class='btn btn-on' onclick='toggleGPIO(\"on\")'>ON</button></p>"
"<p><button class='btn btn-off' onclick='toggleGPIO(\"off\")'>OFF</button></p>"
"</body></html>";
*/
/*
const char* html_page = 
"<!DOCTYPE html><html>"
"<head><meta name='viewport' content='width=device-width, initial-scale=1'>"
"<style>html { font-family: sans-serif; display: inline-block; margin: 0px auto; text-align: center;}"
".btn { border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 10px; cursor: pointer; border-radius: 5px;}"
".btn-on { background-color: #4CAF50; }"
".btn-off { background-color: #f44336; }"
".data-box { font-size: 24px; color: #333; margin-top: 20px; font-weight: bold; }"
"</style>"
"<script>"
"function toggleGPIO(action) {"
"  fetch('/api/' + action)"
"    .then(response => console.log('Action sent: ' + action))"
"    .catch(err => console.error('Error:', err));"
"}"
""
"function updateVariable() {"
"  fetch('/api/status')"
"    .then(response => response.text())"
"    .then(data => {"
"      document.getElementById('live-variable').innerText = data;"
"    })"
"    .catch(err => console.error('Error fetching data:', err));"
"}"
""
"// Fetch the variable automatically when the page loads"
"window.onload = function() {"
"  updateVariable();"
"  setInterval(updateVariable, 2000); // Refresh data every 2000ms (2 seconds)"
"}"
"</script>"
"<title>ESP32-P4 Control</title></head>"
"<body>"
"  <h1>ESP32-P4 GPIO Control</h1>"
"  <p><button class='btn btn-on' onclick='toggleGPIO(\"on\")'>ON</button></p>"
"  <p><button class='btn btn-off' onclick='toggleGPIO(\"off\")'>OFF</button></p>"
"  "
"  <!-- The variable value will display here -->"
"  <div class='data-box'>Current Variable Value: <span id='live-variable'>...</span></div>"
"</body>"
"</html>";
*/

const char* html_page = 
"<!DOCTYPE html><html>"
"<head><meta name='viewport' content='width=device-width, initial-scale=1'>"
"<style>html { font-family: sans-serif; display: inline-block; margin: 0px auto; text-align: center;}"
".btn { border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 10px; cursor: pointer; border-radius: 5px;}"
".btn-on { background-color: #4CAF50; }"
".btn-off { background-color: #f44336; }"
".data-box { font-size: 24px; color: #333; margin-top: 20px; font-weight: bold; }"
"</style>"
"<script>"
"function toggleGPIO(action) {"
"  fetch('/api/' + action)"
"    .then(response => console.log('Action sent: ' + action))"
"    .catch(err => console.error('Error:', err));"
"}"
""
"function updateVariable() {"
"  fetch('/api/status')"
"    .then(response => response.text())"
"    .then(data => {"
"      document.getElementById('live-variable').innerText = data;"
"    })"
"    .catch(err => console.error('Error fetching data:', err));"
"}"
""
"// Fetch the variable automatically when the page loads"
"window.onload = function() {"
"  updateVariable();"
"  setInterval(updateVariable, 2000);"
"}"
"</script>"
"<title>ESP32-P4 Control</title></head>"
"<body>"
"  <h1>ESP32-P4 GPIO Control</h1>"
"  <p><button class='btn btn-on' onclick=\"toggleGPIO('on')\">ON</button></p>"
"  <p><button class='btn btn-off' onclick=\"toggleGPIO('off')\">OFF</button></p>"
"  "
"  <!-- The variable value will display here -->"
"  <div class='data-box'>Current Variable Value: <span id='live-variable'>...</span></div>"
"</body>"
"</html>";



#define HTTP_PORT 80

// HTTP GET Handler function
static esp_err_t root_get_handler(httpd_req_t *req)
{
    const char *response_html = "<html><body><h1>Hello from ESP32-P4 under ESP-IDF v6!</h1></body></html>";
    
    // Set HTTP status code and content type metadata
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_status(req, "200 OK");

    // disable page caching
    httpd_resp_set_hdr(req, "Cache-Control", "no-cache, no-store, must-revalidate");
    httpd_resp_set_hdr(req, "Pragma", "no-cache");
    httpd_resp_set_hdr(req, "Expires", "0");
    
    // Stream the raw response buffer back down the TCP connection socket
    httpd_resp_send(req, html_page, HTTPD_RESP_USE_STRLEN);
    
    ESP_LOGI(TAG, "Webpage request handled successfully");
    return ESP_OK;

    // Failure
    return ESP_FAIL;
}

// Handler to silence favicon.ico requests with an empty 204 response
static esp_err_t favicon_get_handler(httpd_req_t *req) {
    httpd_resp_set_status(req, "204 No Content");
    httpd_resp_send(req, NULL, 0); // Send no body data
    return ESP_OK;
}

// Handler for ON button (GET /on)
static esp_err_t on_get_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "ON button activated");
    // Redirect or re-serve the root page to keep the buttons visible
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, "OFF_OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Handler for OFF button (GET /off)
static esp_err_t off_get_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Off button activated");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, "ON_OK", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static const httpd_uri_t favicon_uri = {
    .uri       = "/favicon.ico",
    .method    = HTTP_GET,
    .handler   = favicon_get_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t root_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t on_uri = {
    .uri       = "/api/on",
    .method    = HTTP_GET,
    .handler   = on_get_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t off_uri = {
    .uri       = "/api/off",
    .method    = HTTP_GET,
    .handler   = off_get_handler,
    .user_ctx  = NULL
};

/* Handler to send the variable value (GET /api/status) */
static esp_err_t status_get_handler(httpd_req_t *req) {
    char response_buffer[32];
    
    // Convert the variable into a plain text string
    snprintf(response_buffer, sizeof(response_buffer), "%d", my_counter_variable);
    
    ESP_LOGI(TAG,"Status: <%s>", response_buffer);

    // Simulate updating the variable every time it's read (optional)
    my_counter_variable++; 

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, response_buffer, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Define the structure
static const httpd_uri_t status_uri = {
    .uri       = "/api/status",
    .method    = HTTP_GET,
    .handler   = status_get_handler,
    .user_ctx  = NULL
};

// Server Startup
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
        httpd_register_uri_handler(server, &root_uri);
        httpd_register_uri_handler(server, &on_uri);
        httpd_register_uri_handler(server, &off_uri);
        httpd_register_uri_handler(server, &favicon_uri);
        httpd_register_uri_handler(server, &status_uri);
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