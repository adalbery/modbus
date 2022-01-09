/*
 * modbus.c
 *
 * Created: 17/12/2021 15:03:33
 *  Author: Adalbery
 * Projeto SIMA.
 * Programa para implementar protocolo Modbus utilizando microcontrolador ATSAMV71Q21B - CORTEX M7
 * Testes realizado com o kit SAM V71 Xplained Ultra.
 * Modbus utiliza USART0: RXD = PB0 - TXD = PB1 - RTS = PB3.
 * Renomear PB3 para RTS_RS485 no Atmel Start.
 * Implementa as funcoes do modbus:
 * 03 (0x03) Read Holding Registers.
 * 04 (0x04) Read Input Registers.
 * 16 (0x10) Write Multiple registers.
 */ 

#include <atmel_start.h>
#include <FreeRTOS.h>
#include "modbus.h"
#include "struct.h"
#include "hpl_usart_config.h"

//static uint8_t example_USART_0[12] = "Hello World!";
struct io_descriptor *io;
static TaskHandle_t handler_modbus;
static struct timer_task TIMER0_100us;
static uint8_t  frame_in[max_frame]; //armazena frame (pacote) modbus recebido pela porta serial
static uint8_t  frame_out[max_frame]; //armazena frame (pacote) modbus enviado pela porta serial
static uint8_t  buffer[8];//bufer para receber dados pela porta serial
static uint16_t interval_t15, interval_t35; //armazenar numero de ticks para t15 e t35
static uint16_t num_bytes;//numero de bytes recebidos pela porta serial
static uint16_t conta_timer;//contador de tick do timer - 100us
static uint8_t modbus_id;//armazena a ID do equipamento modbus
volatile union UniHoldReg *PHoldReg; //ponteiro para os registradores Holding
volatile union UniInputReg *PInputReg;//ponteiro para os registradores Input
static uint16_t  frame_pos;//armazena a proxima posicao do frame
static enum states state = ocioso;//estado do modbus
static bool frame_timer_ok;//flag para indicar frame nao ok devido estouro do tempo t1.5

void task_modbus(void *p)
{  	uint16_t frame_crc;//armazena o CRC do quadro recebido
    //printf("Inicio. t15 = %d. t35 = %d. QtdHoldReg = %d. QtdInputReg = %d.\n ", interval_t15, interval_t35, QtdHoldReg, QtdInputReg);
    while (1)
    {   if (state == resposta)//verifica se estado = reposta
		{   if (frame_timer_ok && (frame_pos >= 2))//verifica frame_timer_ok e se recebeu mais de 2 bytes
            {	frame_crc = (frame_in[frame_pos-1]<<8) + frame_in[frame_pos-2];//calcula o CRC do frame recebido
				if ((frame_crc == crc16(frame_in,(frame_pos-2))) && (frame_in[0] == modbus_id))//verifica o CRC e o ID
				{	switch (frame_in[1])//verifica a Function Code recebida no frame
					{	case 3://(0x03) Read Holding Registers
						    modbus_ReadHoldReg();//Read Holding Registers - monta PDU para responder funcao 03 (0x03) 
                        break;
						
						case 4://(0x04) Read Input Registers
							modbus_ReadInputReg();//Read Input Registers - monta PDU para responder funcao 04 (0x04) 
						break;
						
						case 16://(0x10) Write Multiple registers
							modbus_WriteMultReg();//Write Multiple Registers - monta PDU para responder funcao 16 (0x10)
						break;
								
						default:
							modbus_exception(1);//envia frame de excecao
						break;
					}
				}
			}			
			modbus_reset();//inicializa as variaveis para aguardar inicio de recepcao de dados
        }				
    }   
}


//base de tempo. Funcao chamada no estouro do timer - 100us
static void TIMER0_100us_cb(const struct timer_task *const timer_task)
{   conta_timer++;
    if (conta_timer >= interval_t35)//verifica se estourou o tempo t3.5
    {   timer_stop(&TIMER_0);//para contador
        state = resposta;//trata o frame recebido
    }
}


//funcao chamada quando enviado dados pela USART0
static void tx_cb_USART_0(const struct usart_async_descriptor *const io_descr)
{
	/* Transfer completed */
	gpio_set_pin_level(RTS_RS485, false); //RTS = 0
}


//funcao chamada quando receber dados pela USART0
static void rx_cb_USART_0(const struct usart_async_descriptor *const io_descr)
{	uint16_t conta;//variavel generica para contador
	conta_timer = 0;
    if (state == ocioso)//verifica se esta no estado ocioso
    {   state = datain;//recebendo dados pela serial
        timer_start(&TIMER_0);//inicia contador
    }
    if (state == datain)
    {   num_bytes = io_read(io,buffer,8);//tenta ler ate 8 bytes recebidos da porta serial
        if (num_bytes > 0)//verifica se num_bytes > 0
        {   frame_timer_ok = (conta_timer < interval_t15);//verifica se estourou o tempo t1.5
			if ((frame_pos + num_bytes) <= max_frame)//verifica se bytes recebido cabem no frame
            {   for (conta=0; conta<num_bytes; conta++)
                {   frame_in[frame_pos+conta] = buffer[conta];
                }
                frame_pos += num_bytes;
            }
        }
    }        
}


//inicialiaza o uso da tarefa do modbus
void modbus_init(volatile union UniHoldReg *hold, volatile union UniInputReg *input)
{   PHoldReg  = hold;  //armazena ponteiro para registradores Holding
	PInputReg = input; //armazena ponteiro para registradores Input
    modbus_id = (uint8_t)(*PHoldReg).registrador[1];//armazena a ID do equipamento modbus
	modbus_reset();
   
	//pino RTS (PB3) indica quando dados devera ser
	//enviado ou recebido pela porta RS485 utilizada pelo ModBus
	//0 = recebe dados - normal, pois fica aguardando receber dados
	//1 = envia dados  - somente no momento do envio
	gpio_set_pin_direction(RTS_RS485, GPIO_DIRECTION_OUT);
    gpio_set_pin_function(RTS_RS485, GPIO_PIN_FUNCTION_OFF);
    gpio_set_pin_level(RTS_RS485, false);
	
    //Configura USART
    usart_async_register_callback(&USART_0, USART_ASYNC_TXC_CB, tx_cb_USART_0);
    usart_async_register_callback(&USART_0, USART_ASYNC_RXC_CB, rx_cb_USART_0);
    //usart_async_register_callback(&USART_0, USART_ASYNC_ERROR_CB, err_cb);
    usart_async_get_io_descriptor(&USART_0, &io);
    usart_async_enable(&USART_0);
    
    //Configura timer0
    //tempos baseados em MODBUS over Serial Line Specification and Implementation Guide V1.02
    //Foi acrescido o tempo de  char devido nao ter como saber o momento do início do recebimento
    //de um novo byte, apenas quando he recebido que existe o tratamento
    if (CONF_USART_0_BAUD <= 19200)
    {   interval_t15 = ((1.5 + 1)*8*10000)/CONF_USART_0_BAUD + 1; //considera o 100us o tempo do tick do timer0
        interval_t35 = ((3.5 + 1)*8*10000)/CONF_USART_0_BAUD + 1; //considera o 100us o tempo do tick do timer0
    }
    else
    {   interval_t15 = 8;  //MODBUS over Serial Line Specification and Implementation Guide V1.02 - 750us
        interval_t35 = 18; //MODBUS over Serial Line Specification and Implementation Guide V1.02 - 1750us
    }
    TIMER0_100us.interval = 1; //estouro do TIMER0
    TIMER0_100us.cb       = TIMER0_100us_cb;
    TIMER0_100us.mode     = TIMER_TASK_REPEAT;
    timer_add_task(&TIMER_0, &TIMER0_100us);
    xTaskCreate(task_modbus,"dbg_modbus",1000,NULL,tskIDLE_PRIORITY,&handler_modbus);
}


//inicializa as variaveis para aguardar inicio de recepcao de dados
void modbus_reset(void)
{   conta_timer = 0;//contador de tick do timer - 100us
    state = ocioso;//estado do modbus
    frame_pos = 0;//armazena a proxima posicao do frame
    frame_timer_ok = true;//flag para indicar frame nao ok devido estouro do tempo t1.5
}


//calcula o CRC de um vetor
//baseado no codigo do site https://mundoprojetado.com.br/implementando-modbus-rtu-no-arduino/
uint16_t crc16(uint8_t msg[], uint8_t len)
{	uint16_t crc = 0xFFFF;
	for (uint8_t pos = 0; pos < len; pos++)
	{	crc ^= msg[pos];				// Faz uma XOR entre o LSByte do CRC com o byte de dados atual
		for (uint8_t i = 8; i != 0; i--)// Itera sobre cada bit
		{	if ((crc & 0b1) != 0)		// Se o LSB for 1:
			{	crc >>= 1;              // Desloca para a direita
				crc ^= 0xA001;          // E faz XOR com o polinômio 0xA001 (1010 0000 0000 0001 ): x16 + x15 + x2 + 1
			}else						// Senão:
			{	crc >>= 1;              // Desloca para a direita
			}
		}
	}	
	return crc;// O formato retornado já sai invertido (LSByte primeiro que o MSByte)
}


//Envia quadro modbus - ADU
//len deve ser o comprimento apenas do PDU
//o endereco do modbus e o CRC do frame
//serao informados nesta funcao
void modbus_send(uint8_t len)
{	uint16_t frame_crc;//armazena o CRC do quadro a ser enviado
	frame_out[0] = modbus_id;//ID do modmus
	frame_crc = crc16(frame_out, (len + 1));//calcula o CRC do frame que sera enviado
	frame_out[len + 1] = (uint8_t)(frame_crc & 0xFF);//byte menos significativo do CRC
	frame_out[len + 2] = (uint8_t)((frame_crc >> 8) & 0xFF);//byte mais significativo do CRC
	gpio_set_pin_level(RTS_RS485, true);//RTS = 1. Devera retornar para zero na funcao tx_cb_USART_0
	io_write(io, frame_out, (len + 3));//envia frame
	//gpio_set_pin_level(RTS_RS485, false);//RTS = 0
}


//monta PDU para envio de excecao
//nao e necessario informar endereco do modbus nem o CRC
void modbus_exception(uint8_t exception)
{   frame_out[1] = frame_in[1] + 0x80;//codigo da funcao com erro
	frame_out[2] = exception;//exception code
	modbus_send(2);//len deve ser o comprimento apenas do PDU
}


//Read Holding Registers - monta PDU para responder funcao 03 (0x03)
//03 (0x03) Read Holding Registers
void modbus_ReadHoldReg(void)
{	uint16_t conta;//variável generica para contagem
    uint16_t QtdReg;//armazena a quantidade de registros solicitados
    uint16_t EndIni;//armazena o endereco inicial dos registradores
    QtdReg = (frame_in[4]<<8) + frame_in[5];//quantidade de registros solicitados
    if ((QtdReg < 1) || (QtdReg > 0x007D))//QtdReg deve ser > 1 e < 0x007D (125)
    {	modbus_exception(3);//excecao codigo 3
    }
    else
    {	EndIni = (frame_in[2]<<8) + frame_in[3];//endereco inicial dos registradores
        if ((EndIni >= QtdHoldReg) || ((EndIni + QtdReg) > QtdHoldReg))
        {	modbus_exception(2);//excecao codigo 2
        }
        else
        {	//ok para formacao da PDU
            frame_out[1] = 0x03;//(0x03) Read Holding Registers
            frame_out[2] = (uint8_t)(2*QtdReg);//quantidade de bytes de dados enviados
            for (conta=0; conta<QtdReg ;conta++)
            {	//byte mais significativo do registrador
                frame_out[(2*conta) + 3] = (uint8_t)(((*PHoldReg).registrador[conta+EndIni] >> 8) & 0xFF);
                //byte menos significativo do registrador
                frame_out[(2*conta) + 4] = (uint8_t)((*PHoldReg).registrador[conta+EndIni] & 0xFF);
            }
            modbus_send((uint8_t)(2*QtdReg + 2));//len deve ser o comprimento apenas do PDU - 2*QtdReg + 2         
        }
    }
}


//Read Input Registers - monta PDU para responder funcao 04 (0x04) 
//04 (0x04) Read Input Registers
void modbus_ReadInputReg(void)
{	uint16_t conta;//variável generica para contagem
	uint16_t QtdReg;//armazena a quantidade de registros solicitados
	uint16_t EndIni;//armazena o endereco inicial dos registradores
	QtdReg = (frame_in[4]<<8) + frame_in[5];//quantidade de registros solicitados
	if ((QtdReg < 1) || (QtdReg > 0x007D))//QtdReg deve ser > 1 e < 0x007D (125)
	{	modbus_exception(3);//excecao codigo 3 
	}
	else
	{	EndIni = (frame_in[2]<<8) + frame_in[3];//endereco inicial dos registradores
		if ((EndIni >= QtdInputReg) || ((EndIni + QtdReg) > QtdInputReg))
		{	modbus_exception(2);//excecao codigo 2
		}
		else
		{	//ok para formacao da PDU
			frame_out[1] = 0x04;//(0x04) Read Input Registers
			frame_out[2] = (uint8_t)(2*QtdReg);//quantidade de bytes de dados enviados
			for (conta=0; conta<QtdReg ;conta++)
			{	//byte mais significativo do registrador
				frame_out[(2*conta) + 3] = (uint8_t)(((*PInputReg).registrador[conta+EndIni] >> 8) & 0xFF);
				//byte menos significativo do registrador
				frame_out[(2*conta) + 4] = (uint8_t)((*PInputReg).registrador[conta+EndIni] & 0xFF);
			}
			modbus_send((uint8_t)(2*QtdReg + 2));//len deve ser o comprimento apenas do PDU - 2*QtdReg + 2		
		}
	}
}


//Write Multiple Registers - monta PDU para responder funcao 16 (0x10)
//16 (0x10) Write Multiple registers
//escreve nos registradores Hold os dados recebidos
void modbus_WriteMultReg(void)
{	uint16_t conta;//variável generica para contagem
	uint16_t QtdReg;//armazena a quantidade de registros solicitados
	uint16_t EndIni;//armazena o endereco inicial dos registradores
	uint16_t ByteCount;//armazena a quantidade de byte de dados recebidos
	QtdReg = (frame_in[4]<<8) + frame_in[5];//quantidade de registros solicitados
	ByteCount = (uint16_t)frame_in[6];//armazena a quantidade de byte de dados recebidos
	if ((QtdReg < 1) || (QtdReg > 0x007B) || (ByteCount != (2*QtdReg)))//QtdReg deve ser > 1 e < 0x007B (123). ByteCount deve ser igual a 2*QtdReg
	{	modbus_exception(3);//excecao codigo 3
	}
	else
	{	EndIni = (frame_in[2]<<8) + frame_in[3];//endereco inicial dos registradores
		if ((EndIni >= QtdHoldReg) || ((EndIni + QtdReg) > QtdHoldReg))
		{	modbus_exception(2);//excecao codigo 2
		}
		else
		{	//ok para escrever os dados e formacao da PDU
			//escreve nos registradores Hold os dados recebidos
			for (conta=0; conta<QtdReg; conta++)
			{	(*PHoldReg).registrador[conta+EndIni] = (((uint16_t)frame_in[(2*conta) + 7]) << 8 ) + ((uint16_t)frame_in[(2*conta) + 8]);				
			}
			//monta PDU para responder funcao 16 (0x10)
			frame_out[1] = 0x10;//(0x10) Write Multiple registers
			frame_out[2] = frame_in[2];//byte mais significativo do endereco inicial
			frame_out[3] = frame_in[3];//byte menos significativo do endereco inicial
			frame_out[4] = frame_in[4];//byte mais significativo da quantidade de registradores escritos
			frame_out[5] = frame_in[5];//quantidade de registradores escritos
			modbus_send(5);//len deve ser o comprimento apenas do PDU
		}
	}
}

