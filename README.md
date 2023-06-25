# sensor_data
## 项目背景

此项目是来源于2023年毕设作品。与之配套的还有一个[宠物论坛](https://github.com/dd-smile/community01)。使用keil5开发。下面介绍一下项目使用的芯片与传感器。

- 本项目芯片使用的是STM32F103C8T6。
- 感知节点有：SGP30二氧化碳传感器，DHT11温湿度传感器。
- 传输节点有：DL-22（zigbee）模块，esp8266模块。
- 控制节点有：SG90舵机模块，L9110电机驱动的风扇模块。
- 显示节点有：OLED液晶显示屏模块（七针）。

本项目的技术栈为：

- 编程语言：C语言，STM32标准库。
- 串口通信：UART，IIC，SPI，单总线。
- 通信协议：MQTT。

环境搭建：

- MQTT服务器：基于EMQX的MQTT服务器。
- 腾讯云服务器：轻量应用服务器（2核4G-60G    Centos7系统）

## 如何使用

本项目为三层架构中的感知层部分，需要配套应用层部分[宠物论坛](https://github.com/dd-smile/community01)方可使用。

修改配置：

- 修改esp8266.c中的wifi配置和MQTT服务器配置。
- 修改onenet.c中的MQTT客户端配置。
- 修改main函数中的MQTT主题与QOS等级。
  - 注意修改QOS等级后，需要同步修改MqttKit.c中的QOS等级。

