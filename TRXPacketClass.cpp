/*
 * TRXPacketClass.cpp
 *
 * Created: 07.07.2022 11:35:26
 *  Author: MateshkoAV
 */ 
#include "TRXPacketClass.hpp"
#include "crc16ccitt.c"

TRXPacketClass::TRXPacketClass(){
	this->clear();
}
void TRXPacketClass::newpacket_ping(){
	this->newpacket_raw(CG_ACTION_REQUEST, CG_RA_COMMANDS::CG_RA_COMMAND_PING, 0, 0, 0, 0);
}
void TRXPacketClass::newpacket_pong(){
	this->newpacket_raw(CG_ACTION_ANSWER, CG_RA_COMMANDS::CG_RA_COMMAND_PING, 0, 0, 0, 0);
}
bool TRXPacketClass::newpacket_raw(CG_ACTIONS act, uint8_t cmd, uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3){
	this->clear();
	this->set_raw_packet_data(CG_PACKET_STUCTURE_ACTION, act);
	this->set_raw_packet_data(CG_PACKET_STUCTURE_COMMAND, cmd);
	this->set_raw_packet_data(CG_PACKET_STUCTURE_DATA0, data0);
	this->set_raw_packet_data(CG_PACKET_STUCTURE_DATA1, data1);
	this->set_raw_packet_data(CG_PACKET_STUCTURE_DATA2, data2);
	this->set_raw_packet_data(CG_PACKET_STUCTURE_DATA3, data3);
	this->refresh_packet_crc16();
	if (this->validate() == CG_ERROR_NO_ERROR){
		return true;
	}
	return false;
}
TRXPacketClass::CG_ERRORS TRXPacketClass::validate(void){
	//Do i need check WHOLE integrity?
	CG_ACTIONS act = this->get_field_action();
	if (act == CG_ACTION_NONE){
		this->last_error = TRXP_ERROR_ACTION_FIELD_ERROR;
		return CG_ERROR_PACKET_STRUCTURE_ERROR;
	}
	//CG_RA_COMMANDS cmd = this->get_field_command();
	//if (cmd != CG_COMMAND_NONE){
	//	if (act == CG_ACTION_REQUEST || act == CG_ACTION_ANSWER){
	//		if ()
	//		this->last_error = TRXP_ERROR_COMMAND_ALLOWED_ONLY_TO_REQUEST_ANSWER;
	//		return CG_ERROR_PACKET_STRUCTURE_ERROR;
	//	} else if (act == CG_ACTION_MCUIRQ){
	//
	//	}
	//}
	if (this->is_not_full()){
		this->last_error = TRXP_ERROR_INCOMPLETE_PACKET;
		return CG_ERROR_PACKET_STRUCTURE_ERROR;
	}
	#ifndef CRC_CALC_BYPASS
	uint16_t packet_crc = this->get_field_crc16();
	if (packet_crc != this->calculate_crc16()){
		this->last_error = TRXP_ERROR_CRC;
		return CG_ERROR_PACKET_STRUCTURE_ERROR;
	}
	#endif
	this->last_error = TRXP_ERROR_NO_ERROR;
	return CG_ERROR_NO_ERROR;
}
void TRXPacketClass::recalculate_crc16(void){
	uint16_t crc16 = this->calculate_crc16();
	this->set_raw_packet_data(CG_PACKET_STUCTURE_CRC16H, crc16 >>8);
	this->set_raw_packet_data(CG_PACKET_STUCTURE_CRC16L, crc16 & 0xFF);
}
trxbuffer_data_t TRXPacketClass::get_raw_packet_data(CG_PACKET_STUCTURE data_ptr){
	if(data_ptr == CG_PACKET_STUCTURE_PACKET_SIZE){
		this->last_error = TRXP_ERROR_REQUESTED_DATA_OVF;
		return 0;
	}
	this->last_error = TRXP_ERROR_NO_ERROR;
	return this->data[data_ptr];
}
uint16_t TRXPacketClass::get_field_crc16(){
	return ((this->get_raw_packet_data(CG_PACKET_STUCTURE_CRC16H) << 8) | this->get_raw_packet_data(CG_PACKET_STUCTURE_CRC16L));
}
void TRXPacketClass::refresh_packet_crc16(void){
	uint16_t crc16 = this->calculate_crc16();
	this->set_raw_packet_data(CG_PACKET_STUCTURE_CRC16H, (crc16 >> 8));
	this->set_raw_packet_data(CG_PACKET_STUCTURE_CRC16L, (crc16 & 0xFF));
}
uint16_t TRXPacketClass::calculate_crc16(void){
	return crc16ccitt(this->data, TRXPacketClass::CG_PACKET_STUCTURE_PACKET_SIZE - this->CRC_LENGTH_BYTES);
}
TRXPacketClass::CG_ACTIONS TRXPacketClass::get_field_action(void){
	trxbuffer_data_t act = this->get_raw_packet_data(CG_PACKET_STUCTURE::CG_PACKET_STUCTURE_ACTION);
	if (act < CG_ACTIONS_SIZE){
		this->last_error = TRXP_ERROR_NO_ERROR;
		return static_cast<CG_ACTIONS>(act);
	}
	//ERROR!!!
	this->last_error = TRXP_ERROR_ACTION_FIELD_ERROR;
	return CG_ACTION_NONE;
}
TRXPacketClass::MCUIRQ_COMMANDS TRXPacketClass::get_MCUIRQ_field_command(void){
	trxbuffer_data_t cmd = this->get_raw_field_command();
	if (cmd < MCUIRQ_COMMANDS::MCUIRQ_COMMANDS_SIZE){
		this->last_error = TRXP_ERROR_NO_ERROR;
		return static_cast<MCUIRQ_COMMANDS>(cmd);
	}
	//ERROR!!!
	this->last_error = TRXP_ERROR_COMMAND_FIELD_ERROR;
	return MCUIRQ_COMMANDS::MCUIRQ_COMMAND_NONE;
}
TRXPacketClass::CG_RA_COMMANDS TRXPacketClass::get_RA_field_command(void){
	trxbuffer_data_t cmd = this->get_raw_field_command();
	if (cmd < CG_ACTIONS_SIZE){
		this->last_error = TRXP_ERROR_NO_ERROR;
		return static_cast<CG_RA_COMMANDS>(cmd);
	}
	//ERROR!!!
	this->last_error = TRXP_ERROR_COMMAND_FIELD_ERROR;
	return CG_RA_COMMAND_NONE;
}
trxbuffer_data_t TRXPacketClass::get_raw_field_command(void){
	return this->get_raw_packet_data(CG_PACKET_STUCTURE::CG_PACKET_STUCTURE_COMMAND);
}

void TRXPacketClass::set_raw_packet_data(CG_PACKET_STUCTURE position, uint8_t byte){
	if (position != CG_PACKET_STUCTURE_PACKET_SIZE){
		this->data[position] = byte;
	}
}
bool TRXPacketClass::append_rawdata(trxbuffer_data_t data_to_append){
	if (this->is_full()){
		this->data[this->ptr] = data_to_append;
		this->ptr++;
		return true;
	}
	return false;
}
bool TRXPacketClass::is_full(void){
	return this->ptr < CG_PACKET_STUCTURE_PACKET_SIZE;
}
bool TRXPacketClass::is_not_full(void){
	return !(this->is_full());
}
bool TRXPacketClass::is_empty(void){
	return this->ptr == 0;
}
void TRXPacketClass::clear(void){
	memset(this->data, 0, CG_PACKET_STUCTURE_PACKET_SIZE);
	this->ptr = 0;
	this->last_error = TRXP_ERROR_NO_ERROR;
}
bool TRXPacketClass::is_packet_valid(void){
	return false;
}
//uint16_t get_packet_crc16(void){
//	return (this->data[CG_PACKET_STUCTURE_CRC16H] << 8) | this->data[CG_PACKET_STUCTURE_CRC16L];
//}
TRXPacketClass::TRXP_ERRORS TRXPacketClass::get_last_error(void){
	return this->last_error;
}
