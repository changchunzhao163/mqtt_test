CROSS = mipsel-openwrt-linux-
CC = $(CROSS)gcc
LD = $(CROSS)ld

CFLAGS = -I.
LDFLAGS = -L$(STAGING_DIR)/usr/lib libmosquitto.a -lpthread -lssl -lcrypto
STAGING_DIR = /home/ubuntu/openwrt_CC_mt76xx_zhuotk_source/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2

TARGET = mqtt_client_ai_test

all: $(TARGET)

$(TARGET): mqtt_client.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
