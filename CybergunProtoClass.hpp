/* 
* CybergunProtoClass.hpp
*
* Created: 03.06.2022 13:21:22
* Author: tabr
*/


#ifndef __CYBERGUNPROTOCLASS_HPP__
#define __CYBERGUNPROTOCLASS_HPP__

#include "TRXPacketClass.hpp"

#ifdef USE_UART_DEBUG_COMMANDS
#warning("ANY DATA CANNOT BE MORE THAN 250")
#endif //USE_UART_DEBUG_COMMANDS

class CybergunProtoClass{
	public:
	const static uint8_t STUCK_THRESHOLD = 10;
	#ifdef USE_UART_DEBUG_COMMANDS
		const static uint8_t UART_COMMAND_DUMMY			= 0xFF;
		const static uint8_t UART_COMMAND_PRORO_RESET	= 0xFE;
		const static uint8_t UART_COMMAND_SHOT			= 0xFD;
	#endif //USE_UART_DEBUG_COMMANDS
	const static uint8_t PACKET_COMMAND_EMPTY	= 0x00;



	void rx_ISR(uint8_t data);
	void ms_ISR(void);
	//void clear_rx_bufer(void);
	//void clear_tx_bufer(void);
	void reset(void);
	//bool is_packet_valid1(void);
	//uint16_t get_packet_crcr16(void);
	bool is_packet_ready_to_proceed(void);
	void main(void);
	void pin_reset_up(void);
	void pin_reset_down(void);
	void pin_pio11_up(void);
	void pin_pio11_down(void);
	CybergunProtoClass(void);
	void send_answer_data(uint16_t dataMSB);
	void send_answer_data(uint32_t dataMSB);
	void send_answer_operation_successfull(void);
	void send_answer_data(uint8_t data0, uint8_t data1);
	void send_answer_data(uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3);
	//void send_mcuirq_command(MCUIRQ_COMMANDS command);
	void send_mcuirq_command(TRXPacketClass::MCUIRQ_COMMANDS command, uint8_t data);
	void send_answer_error(TRXPacketClass::CG_ERRORS error_data0);
	void send_answer_error(TRXPacketClass::CG_ERRORS error_data0, uint8_t data);
	private:
	//void refresh_tx_packet_crc(void);
	//uint16_t calculate_tx_packet_crc(void);
	void send_tx_buffer(void);
	//uint8_t rx_buffer[CG_PACKET_STUCTURE_PACKET_SIZE];
	//volatile uint8_t rx_buffer_ptr;
	//uint8_t tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_PACKET_SIZE];
	//volatile uint8_t tx_buffer_ptr;
	//STATUSES current_status;
	volatile uint8_t antistuck_counter;
	TRXPacketClass RXPacket;
	TRXPacketClass TXPacket;
};



#endif //__CYBERGUNPROTOCLASS_HPP__
