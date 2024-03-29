#include <esp8266.h>

#include <neorv32.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <rt/tick.h>
#include <rt/task.h>



// Server Driver
//

size_t server_recv(char *buffer, size_t size)
{
    //size_t recv_size;

    return 0;
}




// ESP8266 Interface
//

// Writes AT+<command><newline> to the ESP
static void send_command(const char *command, ...)
{
    fputs("AT+", AT_COMMAND_UART);
    va_list args;
    va_start(args, command);
    fprintf(AT_COMMAND_UART, command, args);
    va_end(args);
    fputs(AT_COMMAND_NEWLINE, AT_COMMAND_UART);
}

// Waits for a response
//
// Returns RESULT_<OK|ERROR|TIMEOUT>
// 
// If format is NULL, only waits for "OK" or "ERROR"
static enum esp_result recv_response(const char *format, ...)
{
    uint32_t start_tick = rt_tick_count();
    while (rt_tick_count() - start_tick < AT_COMMAND_TIMEOUT)
    {
        // get line
        char buffer[AT_RESPONSE_BUFFER_LEN + 1];
        int len = 0;
        while (len < AT_RESPONSE_BUFFER_LEN)
        {
            char c = fgetc(AT_COMMAND_UART);

            // ignore CR
            if (c == '\r')
                continue;

            fputc(c, stdout);

            // stop on LR
            if (c == '\n')
                break;

            buffer[len++] = c;
        }
        buffer[len] = '\0';
    
        // handle response
        if (len == 0)
            continue;

        if (strcmp(buffer, "OK") == 0)
            return RESULT_OK;

        if (strcmp(buffer, "ERROR") == 0)
            return RESULT_ERROR;

        // scan reponse
        if (format != NULL && buffer[0] == '+')
        {
            va_list args;
            va_start(args, format);
            vsscanf(buffer + 1, format, args);
            va_end(args);
        }
    }

    return RESULT_TIMEOUT;
}

enum esp_result esp_test(void)
{
    fputs("AT" AT_COMMAND_NEWLINE, AT_COMMAND_UART);
    return recv_response(NULL);
}

enum esp_result esp_restart(void)
{
    send_command("RST");
    return recv_response(NULL);
}

enum esp_result esp_set_echo(int enabled)
{
    return recv_response(NULL);
}

enum esp_result esp_set_wifi_mode(int mode)
{
    send_command("CWMODE=%d", mode);
    return recv_response(NULL);
}

enum esp_result esp_get_wifi_mode(int *mode)
{
    send_command("CWMODE?");
    return recv_response("CWMODE:%d", mode);
}

enum esp_result esp_ap_connect(const char *ssid, const char *pwd)
{
    send_command("CWJAP=\"%s\",\"%s\"", ssid, pwd);
    return recv_response(NULL);
}

enum esp_result esp_ap_check(char *ssid)
{
    send_command("CWJAP=?");
    return recv_response("+CWJAP", ssid);
}

enum esp_result esp_ap_list_available(void)
{
    uint32_t start_tick = rt_tick_count();
    while (rt_tick_count() - start_tick < AT_COMMAND_TIMEOUT)
    {
        // get line
        char buffer[AT_RESPONSE_BUFFER_LEN + 1];
        int len = 0;
        while (len < AT_RESPONSE_BUFFER_LEN)
        {
            char c = fgetc(AT_COMMAND_UART);

            // ignore CR
            if (c == '\r')
                continue;

            // stop on LR
            if (c == '\n')
                break;

            buffer[len++] = c;
        }
        buffer[len] = '\0';
    
        // handle response
        if (len == 0)
            continue;

        if (strcmp(buffer, "OK") == 0)
            return RESULT_OK;

        if (strcmp(buffer, "ERROR") == 0)
            return RESULT_ERROR;

        // scan reponse
        if (buffer[0] == '+')
        {
            char ssid[MAX_SSID_LEN];
            int rssi;
            char mac[MAX_MAC_ADDR_LEN];

            sscanf(buffer + 1, "CWLAP:(%*d,\"%s\",%d,\"%s\",%*d,%*d,%*d)", ssid, &rssi, mac);
            fprintf(stdout, "AP={ssid=\"%s\", mac=\"%s\", signal=%d}\n", ssid, mac, rssi);
        }
    }

    return RESULT_TIMEOUT;
}

enum esp_result esp_ap_disconnect(void)
{
    send_command("CWQAP");
    return recv_response(NULL);
}

enum esp_result esp_set_max_connections(int count)
{
    send_command("CIPMUX=%d", count);
    return recv_response(NULL);
}

enum esp_result esp_get_addr(char *ip_addr, char *mac_addr)
{
    uint32_t start_tick = rt_tick_count();
    while (rt_tick_count() - start_tick < AT_COMMAND_TIMEOUT)
    {
        // get line
        char buffer[AT_RESPONSE_BUFFER_LEN + 1];
        int len = 0;
        while (len < AT_RESPONSE_BUFFER_LEN)
        {
            char c = fgetc(AT_COMMAND_UART);

            // ignore CR
            if (c == '\r')
                continue;

            // stop on LR
            if (c == '\n')
                break;

            buffer[len++] = c;
        }
        buffer[len] = '\0';
    
        // handle response
        if (len == 0)
            continue;

        if (strcmp(buffer, "OK") == 0)
            return RESULT_OK;

        if (strcmp(buffer, "ERROR") == 0)
            return RESULT_ERROR;

        // scan reponse
        if (buffer[0] == '+')
        {
            char type[64];
            char ip_addr[MAX_IP_ADDR_LEN];
            sscanf(buffer + 1, "CWLAP:%s:\"%s\"", type, ip_addr);
            fprintf(stdout, "%s:\t%s", type, ip_addr);
        }
    }

    return RESULT_TIMEOUT;
}

enum esp_result esp_create_server(int port)
{
    send_command("CIPSERVER=1,%d", port);
    return recv_response(NULL);
}

enum esp_result esp_delete_server(void)
{
    send_command("CIPSERVER=0");
    int result = recv_response(NULL);
    return (result != RESULT_OK) ? result : esp_restart();
}





