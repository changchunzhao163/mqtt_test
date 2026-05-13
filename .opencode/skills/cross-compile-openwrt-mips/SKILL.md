---
name: cross-compile-openwrt-mips
description: Cross-compile environment, deploy flow, SSH credentials, and MQTT test server for OpenWrt MIPS (mt76xx) target boards
---

## Environment

| Item | Value |
|------|-------|
| Compile machine IP | `192.168.0.250` |
| Compile machine user | `ubuntu` |
| SSH key | `C:\zcc\zcc_private_keys\zcc_id_rsa` |
| Login | `ssh -i <key> ubuntu@192.168.0.250` |
| Cross-compiler prefix | `mipsel-openwrt-linux-` |
| Toolchain staging dir | `/home/ubuntu/openwrt_CC_mt76xx_zhuotk_source/staging_dir/target-mipsel_24kec+dsp_uClibc-0.9.33.2` |
| Target board IP | `192.168.0.251` |
| Target board user | `root` |

## SSH notes

**Target board** requires legacy algorithms:

```
ssh -i C:\zcc\zcc_private_keys\zcc_id_rsa \
  -o KexAlgorithms=+diffie-hellman-group1-sha1 \
  -o HostKeyAlgorithms=+ssh-rsa \
  -o MACs=+hmac-sha1 \
  -o PubkeyAcceptedKeyTypes=+ssh-rsa \
  root@192.168.0.251
```

SCP does NOT work to target board — deploy via HTTP.

## Build

Compile machine runs Ubuntu. Cross-compiler `mipsel-openwrt-linux-gcc` (gcc 4.8.3, uClibc) is available via the toolchain at the staging dir above.

Mandatory link flags for TLS-dependent static libs (libmosquitto, libcurl, etc.):
```
-L$(STAGING_DIR)/usr/lib -lssl -lcrypto
```

## Deploy

```
sudo cp <binary> /var/www/html/
```

Target downloads via:
```
wget http://192.168.0.250/<binary> -O /tmp/<binary>
```

## MQTT test server

| Field | Value |
|-------|-------|
| Host | `mqtt.comway.com.cn` |
| Port | `2883` |
| User | `user3` |
| Pass | `pass3` |

## Local Windows test tools

- Publisher: `C:\tools\mosquitto-2.0.9-install-windows-x86\mosquitto_pub.exe`
- Subscriber: `C:\tools\mosquitto-2.0.9-install-windows-x86\mosquitto_sub.exe`
