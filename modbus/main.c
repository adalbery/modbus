#include <atmel_start.h>
#include "struct.h"
#include "task\modbus.h"

volatile union UniHoldReg HoldReg; //Declara Registradores Hold
volatile union UniInputReg InputReg;//Declara Registradores Input

//inicializa valores dos resgistradores Hold
//serve apenas para testes
void ValHoldReg  (volatile union UniHoldReg  *y)
{ int conta;
	for (conta = 0; conta < QtdHoldReg; conta++)
	(*y).registrador[conta] = 5*conta;
	(*y).campo.ID = 1;//ID do equipamento modbus
}

//inicializa valores dos registradores Input
//serve apenas para testes
void ValInputReg (volatile union UniInputReg *y) 
{ int conta;
	for (conta = 0; conta < QtdInputReg; conta++)
	(*y).registrador[conta] = 2*conta;
	(*y).campo.uRms[0] = 126.51;  //registradores 012,013. Tensao na fase A     - 0x42fd051f
	(*y).campo.uRms[1] = 127.36;  //registradores 014,015. Tensao na fase B     - 0x42feb852
	(*y).campo.uRms[2] = 128.99;  //registradores 016,017. Tensao na fase C     - 0x4300fd71
	(*y).campo.iRms[0] = 1.58;    //registradores 018,019. Corrente na fase A   - 0x3fca3d71
	(*y).campo.iRms[1] = 2.19;    //registradores 020,021. Corrente na fase B   - 0x400c28f6
	(*y).campo.iRms[2] = 3.42;    //registradores 022,023. Corrente na fase C   - 0x405ae148
	(*y).campo.iRms[3] = 1.29;    //registradores 024,025. Corrente no neutro   - 0x3fa51eb8
	(*y).campo.uDIThA[0] = 1.5;   //registradores 120,121. uDIThA[0] na fase A  - 0x3fc00000
	(*y).campo.uDIThA[48] = 10.75;//registradores 216,217. uDIThA[48] na fase A - 0x412c0000
	(*y).campo.uDIThB[0] = 20.25; //registradores 218,219. uDIThB[0] na fase B  - 0x41a20000
	(*y).campo.iDIThN[0] = 30.5;  //registradores 708,709. iDIThN[0] no neutro  - 0x41f40000
	(*y).campo.EnergiaCon[0] = 48.75;  //registradores 806,807. EnergiaCon[0]    - 0x42430000
	(*y).campo.iRmsInst[0] = 42.5;     //registradores 852,853. iRmsInst[0]      - 0x422a0000
	(*y).campo.FreqInst[0] = 46.5;     //registradores 860,861. FreqInst[0]      - 0x423a0000
	(*y).campo.DemAtivaInst[0] = 48.25;//registradores 866,867. DemAtivaInst[0]  - 0x42410000
	(*y).campo.DemReatiInst[0] = 48.75;//registradores 874,875. DemReatiIns[0]   - 0x42430000
	(*y).campo.FatPotenInst[0] = 50.5; //registradores 890,891. FatPotenInst[0]  - 0x424a0000
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

