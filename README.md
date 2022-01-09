Projeto SIMA.
Programa para implementar protocolo Modbus utilizando microcontrolador ATSAMV71Q21B - CORTEX M7
Testes realizado com o kit SAM V71 Xplained Ultra.

Modbus utiliza USART0: RXD = PB0 - TXD = PB1 - RTS = PB3.
Renomear PB3 para RTS_RS485 no Atmel Start.
Implementa as funcoes do modbus:
03 (0x03) Read Holding Registers.
04 (0x04) Read Input Registers.
16 (0x10) Write Multiple registers.
