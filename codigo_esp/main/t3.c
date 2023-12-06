
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
#include "lwip/sockets.h" // Para sockets
#include "time.h" // Para obtener el tiempo
#include "esp_sntp.h"
#include "math.h"

//Credenciales de WiFi
#define WIFI_SSID "grupo2"
#define WIFI_PASSWORD "g2passg2"
#define SERVER_IP     "192.168.4.1" // IP del servidor
#define SERVER_PORT   1234

// Variables de WiFi
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
static const char* TAG = "WIFI";
static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;

// funtion to get the battery level, it returns a random int between 1 and 100
int getBatteryLevel(){
    time_t t;
    srand((unsigned) time(&t));
    int n = rand() % 100 + 1;
    return n;
}
char* header(char protocol, char transportLayer){
	// 12 bytes for the header
    char* head = malloc(12);

	// generate a random short of 2 bytes to use in the id
    char* id = "D1";

    // copy the id into the head first 2 bytes of the header
    memcpy(head, id, 2);
    
	// 6 bytes para MAC Adress
    uint8_t mac[6];
    esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);

    // copy the mac into the head 2 to 8 bytes of the header
    memcpy(head+2, mac, 6);

	// 1 byte for the Transport Layer
    head[8] = transportLayer;

	// 1 byte for the ID Protocol
    head[9] = protocol;

	// 2 bytes para Length Message
    uint16_t *length = malloc(sizeof(uint16_t));

    if (protocol == '0'){*length = 13;}

    if (protocol == '1'){*length = 17;}

    if (protocol == '2'){*length = 27;}

    if (protocol == '3'){*length = 55;}

    if (protocol == '4'){*length = 48027;}
    

    // copy the length into the head 10 to 12 bytes of the header
    memcpy(head+10, length, 2);
    
	return head;
}
// function to get the temperature, humidity, pressure and CO level
char* get_thpc(){
    // create a list of 10 bytes to store the data
    char* data = malloc(10);
    // create the data for the temperature using random, its a int between 5 and 30
    time_t t;
    srand((unsigned) time(&t));
    int n = rand() % 25 + 5;
    // copy the temperature into the data
    memcpy(data, &n, 1);
    // create the data for the pressure using random, its a int between 1000 and 1200
    n = rand() % 200 + 1000;
    // copy the pressure into the data
    memcpy(data+1, &n, 4);
    // create the data for the humidity using random, its a int between 30 and 80
    n = rand() % 50 + 30;
    // copy the humidity into the data
    memcpy(data+5, &n, 1);
    // create the data for the CO level using random, its a float between 30 and 200
    float f = (rand() % 170 + 30);
    //add random decimal
    f = f + (rand() % 100) / 100.0;
    // copy the CO level into the data
    memcpy(data+6, &f, 4);
    // return the data
    return data;
}

char* get_kpi(){
    //create a list of 28 bytes to store the data
    char* data = malloc(28);

    // create the data for the ampx using random, its a float between 0.0059 y 0.12
    float x1 = (rand() % 114 + 59);
    x1 = x1 / 10000.0; 
    // copy the ampx into the data
    memcpy(data + 4, &x1, 4);
    // create the data for the frecx using random, its a float between 29.0 y 31.0
    float x2 = (rand() % 20 + 290);
    x2 = x2 / 10.0;
    // copy the frecx into the data
    memcpy(data+8, &x2, 4);


    // create the data for the ampy using random, its a float between 0.0041 y 0.11
    float y1 = (rand() % 69 + 41);
    y1 = y1 / 10000.0;
    // copy the ampy into the data
    memcpy(data+12, &y1, 4);
    // create the data for the frecy using random, its a float between 59.0 y 61.0
    float y2 = (rand() % 20 + 590);
    y2 = y2 / 10.0;
    // copy the frecy into the data
    memcpy(data+16, &y2, 4);


    // create the data for the ampz using random, its a float between 0.008 y 0.15
    float z1 = (rand() % 142 + 8);
    z1 = z1 / 1000.0;
    // copy the ampz into the data
    memcpy(data+20, &z1, 4);
    // create the data for the frecz using random, its a float between 89.0 y 91.0
    float z2 = (rand() % 20 + 890);
    z2 = z2 / 10.0;
    // copy the frecz into the data
    memcpy(data+24, &z2, 4);

    //create dat for RMS, its the sqrt{(Ampx^2 + Ampy^2 + Ampz^2)}
    float rms = sqrt(pow(x1,2) + pow(y1,2) + pow(z1,2));
    // copy the RMS into the data
    memcpy(data, &rms, 4);

    // return the data
    return data;
}

char* get_acc(){
    //create a list of 48000 bytes to store the data
    char* data = malloc(48000);
    // create a list called acc_x of 2000 floats between -16.0 y 16.0
    float* acc_x = malloc(sizeof(float)*2000);
    for (int i = 0; i < 2000; i++){
        float x = (rand() % 3200 + 1600);
        x = x / 100.0;
        x = x - 16.0;
        acc_x[i] = x;
    }
    //copy acc_x into data
    memcpy(data, acc_x, 8000);

    //create a list called rgyr_x of 2000 floats between -1000.0 y 1000.0
    float* rgyr_x = malloc(sizeof(float)*2000);
    for (int i = 0; i < 2000; i++){
        float x = (rand() % 2000 + 1000);
        x = x / 10.0;
        x = x - 1000.0;
        rgyr_x[i] = x;
    }
    //copy rgyr_x into data
    memcpy(data+8000, rgyr_x, 8000);

    //create a list called acc_y of 2000 floats between -16.0 y 16.0
    float* acc_y = malloc(sizeof(float)*2000);
    for (int i = 0; i < 2000; i++){
        float x = (rand() % 3200 + 1600);
        x = x / 100.0;
        x = x - 16.0;
        acc_y[i] = x;
    }
    //copy acc_y into data
    memcpy(data+16000, acc_y, 8000);

    //create a list called rgyr_y of 2000 floats between -1000.0 y 1000.0
    float* rgyr_y = malloc(sizeof(float)*2000);
    for (int i = 0; i < 2000; i++){
        float x = (rand() % 2000 + 1000);
        x = x / 10.0;
        x = x - 1000.0;
        rgyr_y[i] = x;
    }
    //copy rgyr_y into data
    memcpy(data+24000, rgyr_y, 8000);

    //create a list called acc_z of 2000 floats between -16.0 y 16.0
    float* acc_z = malloc(sizeof(float)*2000);
    for (int i = 0; i < 2000; i++){
        float x = (rand() % 3200 + 1600);
        x = x / 100.0;
        x = x - 16.0;
        acc_z[i] = x;
    }
    //copy acc_z into data
    memcpy(data+32000, acc_z, 8000);

    //create a list called rgyr_z of 2000 floats between -1000.0 y 1000.0
    float* rgyr_z = malloc(sizeof(float)*2000);
    for (int i = 0; i < 2000; i++){
        float x = (rand() % 2000 + 1000);
        x = x / 10.0;
        x = x - 1000.0;
        rgyr_z[i] = x;
    }
    //copy rgyr_z into data
    memcpy(data+40000, rgyr_z, 8000);

    //return data
    return data;
}
void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 10) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
void wifi_init_sta(char* ssid, char* password) {
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config_t));

    // Set the specific fields
    strcpy((char*)wifi_config.sta.ssid, WIFI_SSID);
    strcpy((char*)wifi_config.sta.password, WIFI_PASSWORD);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", ssid,
                 password);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", ssid,
                 password);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
        IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
        WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    // Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        printf("Done\n");

        // Read
        printf("Reading config counter from NVS ... ");
        int32_t config = 0; // value will default to 0, if not set yet in NVS
        err = nvs_get_i32(my_handle, "config", &config);
        printf("Committing updates in NVS ... ");
        switch (err) {
            case ESP_OK:
                printf("config = %" PRIu32 "\n", config);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        // case config = 0 esp ready to recibe config
        if (config == 0)
        {
            wifi_init_sta(WIFI_SSID, WIFI_PASSWORD);
            ESP_LOGI(TAG,"Conectado a WiFi!\n");

            struct sockaddr_in server_addr;
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(SERVER_PORT);
            inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr.s_addr);

            // Crear un socket
            int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sock < 0) {
                ESP_LOGE(TAG, "Error al crear el socket");
            }

            // Conectar al servidor
            if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
                ESP_LOGE(TAG, "Error al conectar");
                close(sock);
            }
            char rx_buffer[128];
            int rx_len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
            if (rx_len < 0) {
                ESP_LOGE(TAG, "Error al recibir datos");
            }
            rx_buffer[rx_len] = '\0'; // Para que no haya basura al final
            ESP_LOGI(TAG, "Datos recibidos: %s", rx_buffer);
            close(sock);
            if (rx_buffer[0]=='1' && rx_buffer[1]=='0' && rx_buffer[2]=='0'){
                ESP_LOGI(TAG , "Configuración correcta\n");
                err = nvs_set_i32(my_handle, "config", 1);
                printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
                printf("Committing updates in NVS ... ");
                err = nvs_commit(my_handle);
                printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
                nvs_close(my_handle);
                esp_restart();
            }
        }

        if (config == 1){
            printf("Do protocol 0 ");
            
            wifi_init_sta(WIFI_SSID, WIFI_PASSWORD);
            ESP_LOGI(TAG,"Conectado a WiFi!\n");

            struct sockaddr_in server_addr;
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(SERVER_PORT);
            inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr.s_addr);

            // Crear un socket
            int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (sock < 0) {
                ESP_LOGE(TAG, "Error al crear el socket");
            }

            // Conectar al servidor
            if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
                ESP_LOGE(TAG, "Error al conectar");
                close(sock);
            }
            // create a list of 13 bytes to store the header and the data
            char* msg = malloc(13);
            ESP_LOGI(TAG, "Executing protocol 0 via tcp");
            //create header
            char* head = header('0', '0');
            // copy the header into the msg
            memcpy(msg, head, 12);
            // create the data for the battery level using random
            uint8_t batteryLevel = getBatteryLevel();
            // copy the battery level into the msg
            msg[12] = batteryLevel;
            // send the msg to the server
            send(sock, msg, 13, 0);
            

            err = nvs_set_i32(my_handle, "config", 0);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
            printf("Committing updates in NVS ... ");
            err = nvs_commit(my_handle);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
            nvs_close(my_handle);
        }
        

        // // Write
        // printf("Updating restart counter in NVS ... ");
        // restart_counter++;
        // err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
        // printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // // Commit written value.
        // // After setting any values, nvs_commit() must be called to ensure changes are written
        // // to flash storage. Implementations may write to storage at other times,
        // // but this is not guaranteed.
        // printf("Committing updates in NVS ... ");
        // err = nvs_commit(my_handle);
        // printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // // Close
        // nvs_close(my_handle);
    }
    // fflush(stdout);
    // esp_restart();
}