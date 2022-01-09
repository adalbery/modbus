/*
 * modbus.h
 *
 * Created: 01/12/2021 10:05:00
 *  Author: Adalbery
 */ 

#include "struct.h"



#ifndef MODBUS_H_
#define MODBUS_H_

#define max_frame 256//buffer para pacote modbus


void task_modbus(void *p);
void modbus_init(volatile union UniHoldReg *hold, volatile union UniInputReg *input);
void modbus_reset(void);//inicializa as variaveis para aguardar inicio de recepcao de dados
void modbus_exception(uint8_t exception);//monta PDU para envio de excecao
void modbus_send(uint8_t len);//envia quadro modbus - ADU
void modbus_ReadHoldReg(void);//Read Holding Registers - monta PDU para responder funcao 03 (0x03) 
void modbus_ReadInputReg(void);//Read Input Registers - monta PDU para responder funcao 04 (0x04) 
void modbus_WriteMultReg(void);//Write Multiple Registers - monta PDU para responder funcao 16 (0x10)
uint16_t crc16(uint8_t msg[], uint8_t len);//calcula o CRC de um vetor
//void modbus_reposta(void);//responde ao frame recebido

enum states {ocioso=0, datain, resposta}; //identifica, codigo, ReadHoldTest, ReadHoldSend, ReadInputTest, ReadInputSend, WriteHoldTest, WriteHoldSend, ExceptionSend};
//Estados do Modbus - state
//reset         - inicializa as variaveis para aguardar inicio de recepcao de dados
//ocioso        - ocioso
//dadosin       - recebendo dados pela serial
//nok           - indica quadro não ok devido tempo entre dois bytes ter estourado o tempo t1.5
//resposta      - trata o frame recebido
//identifica    - analisa a identificao (ID) e o CRC do quadro recebido
//codigo        - verifica se o codigo da funcao eh suportada
//ReadHoldTest  - verifica condicoes para envio, codigo 3 - Read Rolding Registers
//ReadHoldSend  - enviar dados, codigo 3 - Read Holding Registers
//ReadInputTest - verifica condicoes para envio, codigo 4 - Read Input Registers
//ReadInputSend - envia dados do codigo 4:Read Input Registers
//WriteHoldTest - verifica condicoes para escrita de registros, codigo 16 - Write Multiple Registers
//WriteHoldSend - responde codigo 16 - Write Multiple Registers
//ExceptionSend - //envia dados da excecao

#endif /* MODBUS_H_ */