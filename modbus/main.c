#include <atmel_start.h>
#include "struct.h"
#include "task\modbus.h"

volatile union UniHoldReg HoldReg; //Declara Registradores Hold
volatile union UniInputReg InputReg;//Declara Registradores Input



void ValInputReg (volatile union UniInputReg *y) //inicializa valores dos registradores Input
{ int conta;
	for (conta = 0; conta < QtdInputReg; conta++)
	(*y).registrador[conta] = 1*conta;
	(*y).campo.uRms[0] = 126.51; //registradores XXX,XXX.   Tensao na fase A   - 0x42fd051f
	(*y).campo.uRms[1] = 127.36; //registradores XXX,XXX.   Tensao na fase B   - 0x42feb852
	(*y).campo.uRms[2] = 128.99; //registradores XXX,XXX.   Tensao na fase C   - 0x4300fd71
	(*y).campo.iRms[0] = 1.58;   //registradores XXX,XXX. Corrente na fase A - 0x3fca3d71
	(*y).campo.iRms[1] = 2.19;   //registradores XXX,XXX. Corrente na fase B - 0x400c28f6
	(*y).campo.iRms[2] = 3.42;   //registradores XXX,XXX. Corrente na fase C - 0x405ae148
	(*y).campo.iRms[3] = 1.29;   //registradores XXX,XXX. Corrente no neutro  - 0x3fa51eb8
}

void ValHoldReg  (volatile union UniHoldReg  *y) //inicializa valores dos resgistradores Hold
{ int conta;
	for (conta = 0; conta < QtdHoldReg; conta++)
	(*y).registrador[conta] = 1*conta;
	(*y).campo.ID = 1;//ID do equipamento modbus
}

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	ValInputReg (&InputReg); //inicializa valores dos resgistradores Input para testes
	ValHoldReg  (&HoldReg); //inicializa valores dos resgistradores Hold para testes
	modbus_init(&HoldReg, &InputReg);//inicialiaza o uso da tarefa do modbus
	vTaskStartScheduler();
	/* Replace with your application code */
	while (1) {
	}
}

