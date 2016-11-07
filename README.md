# GFC
Fight controller for multicopter based on stm32f103 and mpu6050. This project uses custom hardware and software build. 

#Hardware:
1. MCU stm32f103c8t6 
2. 10-DoF sensor GY-87(MPU6050, HMC5883L, BMP180 on i2c bus)
3. bluetooth HC05 (usart) & NRF24L01+PA+LNA (used spi bus, routed on PCB, algorithms not released)
4. custom PCB
5. gps-module uc530m (routed on PCB, algorithms not released)
6. ultrasonic hc-sr04 (routed on PCB, algorithms not released)
7. Frame turnigy sk450 (ARF)
