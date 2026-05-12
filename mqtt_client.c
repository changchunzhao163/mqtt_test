#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include "mosquitto.h"

#define MQTT_HOST "mqtt.comway.com.cn"
#define MQTT_PORT 2883
#define MQTT_USER "user3"
#define MQTT_PASS "pass3"
#define PUB_TOPIC "/test_topic/upload_from_dtu"
#define SUB_TOPIC "/test_topic/download_to_dtu"
#define PUB_INTERVAL 5
#define KEEPALIVE 60

static struct mosquitto *g_mosq = NULL;
static int g_connected = 0;
static int g_running = 1;

static void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    if (rc == 0) {
        g_connected = 1;
        mosquitto_subscribe(mosq, NULL, SUB_TOPIC, 0);
        fprintf(stderr, "Connected to broker, subscribed to %s\n", SUB_TOPIC);
    } else {
        fprintf(stderr, "Connection failed (rc=%d)\n", rc);
    }
}

static void on_message(struct mosquitto *mosq, void *obj,
                       const struct mosquitto_message *msg)
{
    fprintf(stderr, "Received [%s]: %.*s\n",
           msg->topic, msg->payloadlen, (char *)msg->payload);
}

static void on_publish(struct mosquitto *mosq, void *obj, int mid)
{
}

static void handle_sigint(int sig)
{
    (void)sig;
    fprintf(stderr, "\nShutting down...\n");
    g_running = 0;
}

int main(void)
{
    int rc;
    int seq = 0;
    char payload[64];

    signal(SIGINT, handle_sigint);

    mosquitto_lib_init();

    g_mosq = mosquitto_new(NULL, true, NULL);
    if (!g_mosq) {
        fprintf(stderr, "Failed to create mosquitto instance\n");
        mosquitto_lib_cleanup();
        return 1;
    }

    mosquitto_username_pw_set(g_mosq, MQTT_USER, MQTT_PASS);
    mosquitto_connect_callback_set(g_mosq, on_connect);
    mosquitto_message_callback_set(g_mosq, on_message);
    mosquitto_publish_callback_set(g_mosq, on_publish);

    rc = mosquitto_connect(g_mosq, MQTT_HOST, MQTT_PORT, KEEPALIVE);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Connect failed: %s\n", mosquitto_strerror(rc));
        mosquitto_destroy(g_mosq);
        mosquitto_lib_cleanup();
        return 1;
    }

    {
        time_t last_pub = 0;
        while (g_running) {
            rc = mosquitto_loop(g_mosq, 100, 1);
            if (rc != MOSQ_ERR_SUCCESS) {
                if (rc == MOSQ_ERR_CONN_LOST) {
                    fprintf(stderr, "Connection lost, reconnecting...\n");
                    g_connected = 0;
                    sleep(3);
                    mosquitto_reconnect(g_mosq);
                } else {
                    fprintf(stderr, "Loop error: %s\n", mosquitto_strerror(rc));
                }
                continue;
            }
            if (g_connected && time(NULL) - last_pub >= PUB_INTERVAL) {
                int len = snprintf(payload, sizeof(payload),
                                   "test info [%d]", seq++);
                mosquitto_publish(g_mosq, NULL, PUB_TOPIC,
                                  len, payload, 0, false);
                last_pub = time(NULL);
            }
        }
    }

    mosquitto_disconnect(g_mosq);
    mosquitto_destroy(g_mosq);
    mosquitto_lib_cleanup();
    return 0;
}
