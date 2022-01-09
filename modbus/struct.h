/*
 * struct.h
 *
 * Created: 17/12/2021 15:06:17
 *  Author: Adalbery
 */ 

//IMPORTANTE:
//ao ser compilado para o microcontrolador SAMV71 (32-bit ARM Cortex-M7)
//o valor float representado por 0x42fd051f eh dividido em dois registradores do modbus
//o primeiro ficara com o valor 0x051f e o segundo como o valor 0x42fd

#ifndef STRUCT_H_
#define STRUCT_H_


//struct para Holding Registers
//IMPORTANTE:
//sequencia de uint16_t deve ser quantidade multiplo de 4
//cuidado ao incluir ou alterar campo
//sempre vefificar a posicao dos registradores
struct StrHoldReg
{	uint16_t FuncRemota;    //000,000 - Funcao Remota
	uint16_t ID;			//001,001 - Identifica??o do medidor na rede
	uint16_t RelacaoTC;	    //002,002 - Relacao das espiras do TC
	uint16_t reservado1;    //003,003 - Reservado para uso futuro ou pelo manufaturador
	uint64_t RTC_Time;      //004,007 - Timestamp no formato Unix
	uint16_t Flag_Status0;  //008,008 - Registrador que indica status de leitura dos endere?os input registers
	uint16_t Flag_Status1;  //009,009 - Registrador que indica status de leitura dos endere?os file register
	float latitude;		    //010,011 - Latitude do Medidor
	float longitude;		//012,013 - longitude do Medidor
	uint16_t reservado2;    //014,014 - Reservado para uso futuro ou pelo manufaturador
	uint16_t reservado3;    //015,015 - Reservado para uso futuro ou pelo manufaturador
};


//struct para Input Registers
//IMPORTANTE:
//sequencia de uint16_t deve ser quantidade multiplo de 4
//cuidado ao incluir ou alterar campo
//sempre vefificar a posicao dos registradores
struct StrInputReg
{	float	 Versao;          //000,001 - Versao do Firmware
    float	 reservado;       //002,003 - Reservado para uso futuro ou pelo manufaturador
    uint64_t tStampRms;       //004,007 - timestamp das medidas rms. formato Unix
	uint64_t tStampEve;       //008,011 - timestamp do evento. formato Unix
	float    uRms        [3]; //012,017 - tensao RMS nas fases
    float	 iRms        [4]; //018,025 - corrente nas fases [0..2] e no neutro [3]
    float	 Freq        [3]; //026,031 - Frequencia fundamental nas fases
    float    DemAtiva    [4]; //032,039 - Demanda ativa das fases [0..2] e total [3]
    float    DemReati    [4]; //040,047 - Demanda reativa das fases [0..2] e total [3]
    float    DemApare    [4]; //048,055 - Demanda aparente das fases [0..2] e total [3]
    float    FatPoten    [4]; //056,063 - Fator de potencia nas fases [0..2] e no neutro [3]
    float    uDTT        [3]; //064,069 - Distorcao harmonica total de tensao (DTT) nas fases
    float    uDTTi       [3]; //070,075 - Distorcao harmonica total de tensao para as componentes impares nao multiplas de 3 (DTTi)
    float    uDTTp       [3]; //076,081 - Distorcao harmonica total de tensao para as componentes pares nao multiplas de 3 (DTTp)
    float    uDTT3       [3]; //082,087 - Distorcao harmonica total de tensao para as componentes multiplas de 3 (DTT3)
    float    iDTT        [4]; //088,095 - Distorcao harmonica total de corrente (DTT) das fases
    float    iDTTi       [4]; //096,103 - Distorcao harmonica total de corrente para as componentes impares nao multiplas de 3 (DTTi)
    float    iDTTp       [4]; //104,111 - Distorcao harmonica total de corrente para as componentes pares nao multiplas de 3 (DTTp)
    float    iDTT3       [4]; //112,119 - Distorcao harmonica total de corrente para as componentes multiplas de 3 (DTT3)
    float    uDIThA     [49]; //120,217 - Distorcao harmonica individual de tensao de ordem h na fase A. Sendo h de 2 a 50 [0..48]
    float    uDIThB     [49]; //218,315 - Distorcao harmonica individual de tensao de ordem h na fase B. Sendo h de 2 a 50 [0..48]
    float    uDIThC     [49]; //316,413 - Distorcao harmonica individual de tensao de ordem h na fase C. Sendo h de 2 a 50 [0..48]
    float    iDIThA     [49]; //414,511 - Distorcao harmonica individual de corrente de ordem h na fase A. Sendo h de 2 a 50 [0..48]
    float    iDIThB     [49]; //512,609 - Distorcao harmonica individual de corrente de ordem h na fase B. Sendo h de 2 a 50 [0..48]
    float    iDIThC     [49]; //610,707 - Distorcao harmonica individual de corrente de ordem h na fase C. Sendo h de 2 a 50 [0..48]
    float    iDIThN     [49]; //708,805 - Distorcao harmonica individual de corrente de ordem h no neutro. Sendo h de 2 a 50 [0..48]
    float    EnergiaCon  [4]; //806,813 - Energia consumida nas fases [0..2] e total [3] (integral potencia ativa desde o ultimo reset)
    float    EnergiaGer  [4]; //814,821 - Energia gerada nas fases [0..2] e total [3] (integral potencia ativa gerada desde o ultimo reset)
    float    EnergiaAti  [4]; //822,829 - Energia ativa nas fases [0..2] e total [3] (integral da potência dos ultimos 5 minutos ou período múltiplo de 5 minutos)
    float    EnergiaReat [4]; //830,837 - Energia reativa nas fases [0..2] e total [3]
    float    EnergiaApar [4]; //838,845 - Energia aparente nas fases [0..2] e total [3]
	float    uRmsInst    [3]; //846,851 - tensao RMS nas fases - instantanea
	float	 iRmsInst    [4]; //852,859 - corrente RMS nas fases [0..2] e no neutro [3] - instantanea
	float	 FreqInst    [3]; //860,867 - Frequencia fundamental nas fases - instantanea
	float    DemAtivaInst[4]; //868,875 - Demanda ativa das fases [0..2] e total [3] - instantanea
	float    DemReatiInst[4]; //876,883 - Demanda reativa das fases [0..2] e total [3] - instantanea
	float    DemApareInst[4]; //884,891 - Demanda aparente das fases [0..2] e total [3] - instantanea
	float    FatPotenInst[4]; //892,899 - Fator de potencia nas fases [0..2] e no neutro [3] - instantanea
};


#define QtdHoldReg  sizeof(struct StrHoldReg)/sizeof(uint16_t) //quantidade de Holding Registers 
#define QtdInputReg sizeof(struct StrInputReg)/sizeof(uint16_t)//quantidade de Input Registers


//union entre a struct dos registradors holding e um vetor do tipo uint16_t
//permite acessar os dados utilizando o indice dos registradores ou pelo nomes dos campos
union UniHoldReg 
{	uint16_t registrador[QtdHoldReg];
	struct StrHoldReg campo;
};


//union entre a struct dos registradors Input e um vetor do tipo uint16_t
//permite acessar os dados utilizando o indice dos registradores ou pelo nomes dos campos
union UniInputReg
{	uint16_t registrador[QtdInputReg]; //sizeof(struct SInputReg)/sizeof(int16_t)
    struct StrInputReg campo;
};


#endif /* STRUCT_H_ */

