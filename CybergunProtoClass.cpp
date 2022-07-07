/* 
* CybergunProtoClass.cpp
*
* Created: 03.06.2022 13:21:22
* Author: tabr
*/


#include "CybergunProtoClass.hpp"

void CybergunProtoClass::rx_ISR(uint8_t data){
	#ifdef USE_UART_DEBUG_COMMANDS
		/*if (this->rx_buffer_ptr < CG_PACKET_STUCTURE_CRC16H){
			if (data == UART_COMMAND_PRORO_RESET){
				this->reset();
				return;
			} else*/ if (data == UART_COMMAND_DUMMY){
				UDR = UART_COMMAND_DUMMY;
				return;
			} else if (data == UART_COMMAND_SHOT){
				Cybergun.shot_detected();
				return;
			}
			//if (this->rx_buffer_ptr >= CG_PACKET_STUCTURE_PACKET_SIZE){
			if (this->RXPacket.is_full()){
				return;
			}
		//}
	#endif //USE_UART_DEBUG_COMMANDS
	//ibi(PORTD,4);
	//this->rx_buffer[this->rx_buffer_ptr] = data;
	//this->rx_buffer_ptr++;//can't be more than CG_PACKET_STUCTURE_PACKET_SIZE-1
	this->RXPacket.append_rawdata(data);
	#warning "can receive data but if buffer would be full data would be lost"
	//UDR = (this->buffer_ptr+48);
	if (this->is_packet_ready_to_proceed()){
		if (this->RXPacket.validate() == TRXPacketClass::CG_ERROR_NO_ERROR){
			//ok
		} else {
			//error
			//reset if packet invalid
			this->reset();
		}
		//if (!(this->is_packet_valid())){
		//	this->reset();
		//	return;
		//}
	}
}
void CybergunProtoClass::ms_ISR(void){
	//if (this->rx_buffer_ptr >= CG_PACKET_STUCTURE_PACKET_SIZE){
		//sbi(PORTD,6);
	//}
	//if (this->antistuck_counter < STUCK_THRESHOLD && this->rx_buffer_ptr != 0){
	if (this->antistuck_counter < STUCK_THRESHOLD && !(this->RXPacket.is_empty())){
		this->antistuck_counter++;
	} else {
		this->antistuck_counter = 0;
	}
}
//void CybergunProtoClass::clear_rx_bufer(void){
//	//memset(this->rx_buffer, 0, CG_PACKET_STUCTURE_PACKET_SIZE);
//	this->RXPacket.clear();
//}
//void CybergunProtoClass::clear_tx_bufer(void){
//	memset(this->tx_buffer, 0, TRXPacketClass::CG_PACKET_STUCTURE_PACKET_SIZE);
//}
void CybergunProtoClass::reset(void){
	//this->clear_rx_bufer();
	//this->clear_tx_bufer();
	this->RXPacket.clear();
	this->TXPacket.clear();
	//this->rx_buffer_ptr = 0;
	//this->tx_buffer_ptr = 0;
	//this->current_status = STATUS_AWAINING_REQUEST;
	this->antistuck_counter = 0;
	//sbi(PORTD, 4);
	//sbi(PORTD, 6);
	//_delay_ms(100);
	//cbi(PORTD, 4);
	//cbi(PORTD, 6);
	//UDR = 90;
	
}
//bool CybergunProtoClass::is_packet_valid1(void){
//	if (this->RXPacket.validate() != TRXPacketClass::CG_ERROR_NO_ERROR){
//		this->send_error(TRXPacketClass::CG_ERRORS::CG_ERROR_PACKET_STRUCTURE_ERROR, this->RXPacket.get_last_error());
//		return false;
//	}
//	return true;
//	/*
//	if (this->rx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_ACTION] >= TRXPacketClass::CG_ACTIONS_SIZE){
//		//UDR = 'A';
//		this->send_error(TRXPacketClass::CG_ERROR_UNKNOWN_PACKET_HEAD);
//		return false;
//	}
//	//if (this->rx_buffer_ptr != TRXPacketClass::CG_PACKET_STUCTURE_PACKET_SIZE){
//	if (this->RXPacket.is_not_full()){
//		//UDR = 'B';
//		this->send_error(TRXPacketClass::CG_ERROR_INTERNAL_ERROR_BUFFER_PTR);
//		return false;
//	}
//	switch (this->rx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_ACTION])
//	{
//		case TRXPacketClass::CG_ACTION_REQUEST:
//		case TRXPacketClass::CG_ACTION_ANSWER:
//		if (this->rx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_ACTION] >= TRXPacketClass::CG_COMMANDS_SIZE){
//			//UDR = 'C';
//			this->send_error(TRXPacketClass::CG_ERROR_UNKNOWN_REQUEST);
//			return false;
//		}
//		break;
//	}
//	#ifndef CRC_CALC_BYPASS
//	uint16_t crc16 = crc16ccitt(this->rx_buffer, CG_PACKET_STUCTURE_PACKET_SIZE - 2 );
//	if (crc16 != this->get_packet_crcr16()){
//		//UDR = 'D';
//		this->send_error(CG_PACKET_STUCTURE_ERROR_CRC_ERROR);
//		return false;
//	}
//	#endif // CRC_CALC_BYPASS
//	return true;
//	*/
//}
//uint16_t CybergunProtoClass::get_packet_crcr16(void){
//	return (this->rx_buffer[CG_PACKET_STUCTURE_CRC16H] << 8) | this->rx_buffer[CG_PACKET_STUCTURE_CRC16L];
//}
bool CybergunProtoClass::is_packet_ready_to_proceed(void){
	//return this->rx_buffer_ptr == TRXPacketClass::CG_PACKET_STUCTURE_PACKET_SIZE;
	return this->RXPacket.is_full();
}
void CybergunProtoClass::main(void){
	TRXPacketClass::CG_ERRORS err = TRXPacketClass::CG_ERROR_NO_ERROR;
	if (this->is_packet_ready_to_proceed()){
		//if (this->is_packet_valid()){
		if (this->RXPacket.validate() == TRXPacketClass::CG_ERROR_NO_ERROR){
			//switch (this->rx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_ACTION]){
			switch (this->RXPacket.get_field_action()){
				case TRXPacketClass::CG_ACTIONS::CG_ACTION_REQUEST:
				/*
				//if (this->rx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_COMMAND] == TRXPacketClass::CG_COMMAND_SET_STORAGE){
				if (this->RXPacket.get_RA_field_command() == TRXPacketClass::CG_COMMAND_SET_STORAGE){
					//Cybergun.Storage.set_raw_MSB(this->rx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_DATA0], this->rx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_DATA1]);
					Cybergun.Storage.set_raw_MSB(this->RXPacket.data[TRXPacketClass::CG_PACKET_STUCTURE_DATA0], this->RXPacket.data[TRXPacketClass::CG_PACKET_STUCTURE_DATA1]);
					Cybergun.update_status();
					this->send_answer_operation_successfull();
					this->reset();
					return;
				//} else if (this->rx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_COMMAND] == TRXPacketClass::CG_COMMAND_GET_STORAGE){
				} else if (this->RXPacket.get_RA_field_command() == TRXPacketClass::CG_COMMAND_GET_STORAGE){
					this->send_answer(Cybergun.Storage.get_raw_MSB());
					this->reset();
					return;
				//} else if (this->rx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_COMMAND] == TRXPacketClass::CG_PACKET_STUCTURE_REQUEST_UPTIME){
				//} else if (this->rx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_COMMAND] == TRXPacketClass::CG_COMMAND_UPTIME){
				} else if (this->RXPacket.get_RA_field_command() == TRXPacketClass::CG_COMMAND_UPTIME){
					this->send_answer(uptime);
					this->reset();
					return;
				}
				*/
				switch (this->RXPacket.get_RA_field_command()){
					case TRXPacketClass::CG_RA_COMMANDS::CG_RA_COMMAND_SET_STORAGE:
						Cybergun.Storage.set_raw_MSB(this->RXPacket.data[TRXPacketClass::CG_PACKET_STUCTURE_DATA0], this->RXPacket.data[TRXPacketClass::CG_PACKET_STUCTURE_DATA1]);
						Cybergun.update_status();
						this->send_answer_operation_successfull();
						this->reset();
					break;

					case TRXPacketClass::CG_RA_COMMANDS::CG_RA_COMMAND_GET_STORAGE:
						this->send_answer_data(Cybergun.Storage.get_raw_MSB());
						this->reset();
					break;

					case TRXPacketClass::CG_RA_COMMANDS::CG_RA_COMMAND_UPTIME:
						this->send_answer_data(uptime);
						this->reset();
					break;
					case TRXPacketClass::CG_RA_COMMANDS::CG_RA_COMMAND_PING:
						this->TXPacket.newpacket_pong();
						this->send_tx_buffer();
						this->reset();
					break;
					case TRXPacketClass::CG_RA_COMMANDS::CG_RA_COMMAND_SET_PIN:
					case TRXPacketClass::CG_RA_COMMANDS::CG_RA_COMMAND_GET_PIN:
					case TRXPacketClass::CG_RA_COMMANDS::CG_RA_COMMAND_ERROR: //wat should i do?
						err = TRXPacketClass::CG_ERROR_UNIMPLEMENTED_COMMAND;
					case TRXPacketClass::CG_RA_COMMANDS::CG_RA_COMMAND_NONE:
					case TRXPacketClass::CG_RA_COMMANDS::CG_RA_COMMANDS_SIZE:
					//default:
						
						err = TRXPacketClass::CG_ERROR_UNKNOWN_COMMAND;
						this->send_answer_error(err, this->RXPacket.get_raw_field_command());
						this->reset();
					break;
				}
				break;
				case TRXPacketClass::CG_ACTIONS::CG_ACTION_ANSWER: //do i need to do something?
				case TRXPacketClass::CG_ACTIONS::CG_ACTION_MCUIRQ:
					err = TRXPacketClass::CG_ERROR_UNIMPLEMENTED_ACTION;
					#warning "TODO: CybergunProtoClass::main get_field_action switch"
				case TRXPacketClass::CG_ACTIONS::CG_ACTION_NONE:
				case TRXPacketClass::CG_ACTIONS::CG_ACTIONS_SIZE:
				//default:
					err = TRXPacketClass::CG_ERROR_UNKNOWN_ACTION;
					this->send_answer_error(err, this->RXPacket.get_field_action());
					this->reset();
				break;
			}
		}
	}
	if (this->antistuck_counter >= this->STUCK_THRESHOLD){
		//this->send_error(this->CG_PACKET_STUCTURE_ERROR_CRC_ERROR);
		this->send_mcuirq_command(TRXPacketClass::MCUIRQ_COMMANDS::MCUIRQ_COMMAND_ERROR_DETECTED,  TRXPacketClass::MCUIRQ_ERRORS::MCUIRQ_ERROR_STUCK_DETECTED);
		this->reset();
		return;
	}
	
}
CybergunProtoClass::CybergunProtoClass(void){
	this->reset();
};
void CybergunProtoClass::send_answer_operation_successfull(void){
	this->send_answer_data(0,0);
}
void CybergunProtoClass::send_answer_data(uint16_t dataMSB){
	this->send_answer_data((dataMSB >> 8), (dataMSB & 0xFF));
}
void CybergunProtoClass::send_answer_data(uint32_t dataMSB){
	this->send_answer_data((dataMSB >> 24), (dataMSB >> 16), (dataMSB >> 8), (dataMSB & 0xFF));
}
void CybergunProtoClass::send_answer_data(uint8_t data0, uint8_t data1){
	this->send_answer_data(data0, data1, 0, 0);
	//this->clear_tx_bufer();
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_ACTION]		= TRXPacketClass::CG_ACTION_ANSWER;
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_COMMAND]		= 0;
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_DATA0]		= data0;
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_DATA1]		= data1;
	//this->refresh_tx_packet_crc();
	this->send_tx_buffer();
}
void CybergunProtoClass::send_answer_data(uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3){
	this->TXPacket.newpacket_raw(TRXPacketClass::CG_ACTION_ANSWER, TRXPacketClass::CG_RA_COMMAND_NONE, data0, data1, data2, data3);
	//this->clear_tx_bufer();
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_ACTION]		= TRXPacketClass::CG_ACTION_ANSWER;
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_COMMAND]	= 0;
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_DATA0]		= data0;
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_DATA1]		= data1;
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_DATA2]		= data2;
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_DATA3]		= data3;
	//this->refresh_tx_packet_crc();
	this->send_tx_buffer();
}

//void CybergunProtoClass::send_mcuirq_command(MCUIRQ_COMMANDS command){
//	this->send_mcuirq_command(command, 0);
//}
void CybergunProtoClass::send_mcuirq_command(TRXPacketClass::MCUIRQ_COMMANDS command, uint8_t data){
	this->TXPacket.newpacket_raw(TRXPacketClass::CG_ACTION_MCUIRQ, command, data, 0 , 0, 0);
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_ACTION]		= TRXPacketClass::CG_ACTION_MCUIRQ;
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_COMMAND]		= command;
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_DATA0]		= data;
	//this->refresh_tx_packet_crc();
	this->send_tx_buffer();
}

void CybergunProtoClass::send_answer_error(TRXPacketClass::CG_ERRORS error_data0){
	this->send_answer_error(error_data0, 0);
}
void CybergunProtoClass::send_answer_error(TRXPacketClass::CG_ERRORS error_data0, uint8_t error_data1){
	this->TXPacket.newpacket_raw(TRXPacketClass::CG_ACTION_ANSWER, TRXPacketClass::CG_RA_COMMANDS::CG_RA_COMMAND_ERROR, error_data0 , error_data1, 0, 0);
	//this->clear_tx_bufer();
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_ACTION]		= TRXPacketClass::CG_ACTION_ANSWER;
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_COMMAND]		= TRXPacketClass::CG_COMMAND_ERROR;
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_DATA0]		= error_data0;
	//this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_DATA1]		= error_data1;
	//this->refresh_tx_packet_crc();
	this->send_tx_buffer();
}
//void CybergunProtoClass::refresh_tx_packet_crc(void){
//	uint16_t crc = this->calculate_tx_packet_crc();
//	this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_CRC16H] = crc >> 8;
//	this->tx_buffer[TRXPacketClass::CG_PACKET_STUCTURE_CRC16L] = crc & 0xFF;
//}
//uint16_t CybergunProtoClass::calculate_tx_packet_crc(void){
//	return crc16ccitt(this->tx_buffer, TRXPacketClass::CG_PACKET_STUCTURE_PACKET_SIZE - this->CRC_LENGTH_BYTES);
//}
void CybergunProtoClass::send_tx_buffer(void){
	//TRX_environment::send_data(this->tx_buffer, TRXPacketClass::CG_PACKET_STUCTURE_PACKET_SIZE);
	TRX_environment::send_data(this->TXPacket.data, TRXPacketClass::CG_PACKET_STUCTURE_PACKET_SIZE);
	
	/*
	this->tx_buffer_ptr = 0;
	while (this->tx_buffer_ptr < this->CG_PACKET_STUCTURE_PACKET_SIZE){
		UDR=this->tx_buffer[this->tx_buffer_ptr];
		while ((UCSRA & DATA_REGISTER_EMPTY)==0);//wait for TX complete
		this->tx_buffer_ptr++;
	}
	*/
}