#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MAX_SSID_LEN 32
#define MAX_IP_ADDR_LEN 15
#define MAX_MAC_ADDR_LEN 17

#ifndef AT_COMMAND_UART
#define AT_COMMAND_UART NEORV32_UART1
#endif

#ifndef AT_COMMAND_TIMEOUT
#define AT_COMMAND_TIMEOUT 600
#endif

#ifndef AT_COMMAND_NEWLINE
#define AT_COMMAND_NEWLINE "\r\n"
#endif

#ifndef AT_RESPONSE_BUFFER_LEN
#define AT_RESPONSE_BUFFER_LEN 256
#endif

enum esp_result
{
    RESULT_OK = 0,
    RESULT_ERROR = 1,
    RESULT_TIMEOUT = 2
};

// Checks if AT are working.
enum esp_result esp_test(void);

// Resets esp
enum esp_result esp_restart(void);

// Turns on of off AT command echoing
enum esp_result esp_set_echo(int enabled);

#define WIFI_MODE_STATION 1
#define WIFI_MODE_AP 2

// Sets the WIFI mode
// 1 - station mode (client)
// 2 - ap mode (host)
// 3 - ap + station mode
enum esp_result esp_set_wifi_mode(int mode);

// Gets the current WIFI mode
enum esp_result esp_get_wifi_mode(int *mode);

// Prints available access points to stdout
enum esp_result esp_ap_list_available(void);

// Checks if the esp is connected to an AP
enum esp_result esp_ap_check(char *ssid);

// Connects to an AP
enum esp_result esp_ap_connect(const char *ssid, const char *pwd);

// Disconnects from the AP
enum esp_result esp_ap_disconnect(void);

// Sets the maximum number of connections
enum esp_result esp_set_max_connections(int count);

// Gets the station ip and mac addresses.
enum esp_result esp_get_addr(char *ip_addr, char *mac_addr);

// Creates a server with a given port.
// Max connections must be set to 1
enum esp_result esp_create_server(int port);

// Deletes the server and restarts the esp.
enum esp_result esp_delete_server(void);
