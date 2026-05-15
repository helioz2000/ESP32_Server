| Supported Targets | ESP32 | ESP32-P4 |
| ----------------- | ----- | -------- |

# Basic Ethernet TCP server

## Overview

This example demonstrates basic usage of internal EMAC and `Ethernet driver` together with `esp_netif` and BSD Sockets. The workflow of the example is as follows:

1. Install Ethernet driver
2. Attach the driver to `esp_netif`
3. Send DHCP requests and wait for a DHCP lease
4. If get IP address successfully, the application creates a TCP socket with the specified port number and waits for a connection request from a client.
5. After accepting a request from the client, connection between server and client is established and the application waits for some data to be received from the client. 
6. Received data is printed as ASCII text and retransmitted back to the client.


>[!TIP]
> For initialization of a wider range of Ethernet PHY chips, including SPI Ethernet modules and for advanced Ethernet configuration options, please use [Component for Ethernet Initialization](https://components.espressif.com/components/espressif/ethernet_init/).

## How to use example

In order to create TCP client that communicates with TCP server example, choose one of the following options.

There are many host-side tools which can be used to interact with the UDP/TCP server/client.
One command line tool is [netcat](http://netcat.sourceforge.net) which can send and receive many kinds of packets.
Note: please replace `192.168.0.167 3333` with desired IPV4/IPV6 address (displayed in monitor console) and port number in the following command.

### TCP client using netcat
```
nc 192.168.0.167 3333
```

## Hardware Required

To run this example, it's recommended that you have an official Espressif Ethernet capable development board - either [ESP32-Ethernet-Kit](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-ethernet-kit/index.html) or [ESP32-P4-Function-EV-Board](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/index.html). This example should also work for 3rd party ESP32 board as long as it's integrated with a IEEE 802.3 compliant Ethernet PHY chip and with the default RMII dataplane GPIO configuration.

> [!NOTE]
> `Generic 802.3 PHY` basic functionality should always work for PHY compliant with IEEE 802.3. However, some specific features might be limited. A typical example is loopback functionality, where certain PHYs may require setting a specific speed mode to operate correctly. If this is a case, use driver tailored to that specific chip.

#### Pin Assignment

This example uses the default RMII GPIO configuration as defined by `ETH_ESP32_EMAC_DEFAULT_CONFIG` for the specific ESP32 chip (ESP32, ESP32P4, etc.).

### Configure the project

```
idf.py menuconfig
```

See common configurations for Ethernet examples from [upper level](../README.md#common-configurations).

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (394) ESP32_Server: Ethernet Started
I (3934) ESP32_Server: Ethernet Link Up
I (3934) ESP32_Server: Ethernet HW Addr 30:ae:a4:c6:87:5b
I (5864) esp_netif_handlers: eth ip: 192.168.2.151, mask: 255.255.255.0, gw: 192.168.2.2
I (5864) ESP32_Server: Ethernet Got IP Address
I (5864) ESP32_Server: ~~~~~~~~~~~
I (5864) ESP32_Server: ETHIP:192.168.2.151
I (5874) ESP32_Server: ETHMASK:255.255.255.0
I (5874) ESP32_Server: ETHGW:192.168.2.2
I (5884) ESP32_Server: ~~~~~~~~~~~
I (11375) ESP32_Server: Socket created
I (11385) ESP32_Server: Socket bound, port 3333
I (11385) ESP32_Server: Socket listening
I (32845) ESP32_Server: Socket accepted ip address: 192.168.1.50
I (38305) ESP32_Server: Received 4 bytes: aaa
I (44365) ESP32_Server: Received 4 bytes: yyy
W (47275) ESP32_Server: Connection closed
I (47275) ESP32_Server: Socket listening
```

You can also ping your ESP32 in the terminal by entering `ping 192.168.X.X` (it depends on the actual IP address you get).

## Troubleshooting

See common troubleshooting for Ethernet examples from [upper level](../README.md#common-troubleshooting).

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
