# AGENTS.md

## Repo purpose
MQTT client test program for an OpenWrt (MIPS little-endian) board. Publishes "test info" every 5s to
`/test_topic/upload_from_dtu` and subscribes to `/test_topic/download_to_dtu`.

Original requirements (Chinese): `项目需求.md`

## Build & deployment flow (must follow this order)
1. **Write C code** linking against `libmosquitto.a` (static, v1.6.7) with `mosquitto.h`.
2. **SCP sources to compile machine**:
   - `scp -i C:\zcc\zcc_private_keys\zcc_id_rsa mqtt_client.c Makefile libmosquitto.a mosquitto.h ubuntu@192.168.0.250:/home/ubuntu/mqtt_client_ai_test/`
   - Create dir first if needed: `ssh ... "mkdir -p /home/ubuntu/mqtt_client_ai_test"`
3. **SSH in & cross-compile**: `ssh -i C:\zcc\zcc_private_keys\zcc_id_rsa ubuntu@192.168.0.250`
   - `cd /home/ubuntu/mqtt_client_ai_test && make` (uses `mipsel-openwrt-linux-gcc`).
   - Output: `mqtt_client_ai_test`
   - Note: `libmosquitto.a` was built with TLS support — linking needs `-lssl -lcrypto` from the OpenWrt staging dir. The `Makefile` has a hardcoded `STAGING_DIR` path that may need updating for different toolchain versions.
4. **Deploy to HTTP server**: `sudo cp /home/ubuntu/mqtt_client_ai_test/mqtt_client_ai_test /var/www/html/`
5. **Download on target board** (IP 192.168.0.251): `wget http://192.168.0.250/mqtt_client_ai_test`
   - Target SSH: `ssh -i C:\zcc\zcc_private_keys\zcc_id_rsa -o KexAlgorithms=+diffie-hellman-group1-sha1 -o HostKeyAlgorithms=+ssh-rsa -o MACs=+hmac-sha1 -o PubkeyAcceptedKeyTypes=+ssh-rsa root@192.168.0.251`

## MQTT server
- `mqtt.comway.com.cn:2883`, user `user3` / pass `pass3`

## Local testing (Windows)
- Publisher: `C:\tools\mosquitto-2.0.9-install-windows-x86\mosquitto_pub.exe`
- Subscriber: `C:\tools\mosquitto-2.0.9-install-windows-x86\mosquitto_sub.exe`

## Key constraints
- Build via `make` on compile machine. Cross-compiler: `mipsel-openwrt-linux-gcc`.
- `scp` does NOT work to target board (251); use `wget` from compile machine's HTTP server.
- `libmosquitto.a` is already pre-cross-compiled for the target — link it as-is.
