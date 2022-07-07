/*
 * TRXPacketClass.hpp
 *
 * Created: 06.07.2022 9:28:31
 *  Author: MateshkoAV
 */ 


#ifndef TRXPACKETCLASS_HPP_
#define TRXPACKETCLASS_HPP_
//#include "CybergunProtoClass.hpp"
#include <avr/io.h>
#include <string.h>

//#include "crc16ccitt.c"

typedef uint8_t trxbuffer_data_t;
typedef uint8_t trxbuffer_dataptr_t;





class TRXPacketClass{
	public:
	const static uint8_t CRC_LENGTH_BYTES		= 2;
	enum CG_PACKET_STUCTURE{
		CG_PACKET_STUCTURE_ACTION,
		CG_PACKET_STUCTURE_COMMAND,
		CG_PACKET_STUCTURE_DATA0,
		CG_PACKET_STUCTURE_DATA1,
		CG_PACKET_STUCTURE_DATA2,
		CG_PACKET_STUCTURE_DATA3,
		CG_PACKET_STUCTURE_CRC16H,
		CG_PACKET_STUCTURE_CRC16L,
		CG_PACKET_STUCTURE_PACKET_SIZE
	};
	enum CG_ACTIONS{
		CG_ACTION_NONE,
		CG_ACTION_REQUEST,
		CG_ACTION_ANSWER,
		CG_ACTION_MCUIRQ,
		//CG_ACTION_ERROR,
		CG_ACTIONS_SIZE
	};
	enum CG_RA_COMMANDS{
		CG_RA_COMMAND_NONE,
		CG_RA_COMMAND_ERROR,
		CG_RA_COMMAND_SET_STORAGE,
		CG_RA_COMMAND_GET_STORAGE,
		CG_RA_COMMAND_SET_PIN,//unimplemented
		CG_RA_COMMAND_GET_PIN,//unimplemented
		CG_RA_COMMAND_RESTART_MCU,//unimplemented
		CG_RA_COMMAND_RESTART_BTDEVICE,//unimplemented
		CG_RA_COMMAND_RESTART_FULL,//unimplemented
		CG_RA_COMMAND_PING,//unimplemented
		CG_RA_COMMAND_UPTIME,
		CG_RA_COMMANDS_SIZE
	};
	enum CG_ERRORS{
		CG_ERROR_NO_ERROR,
		CG_ERROR_UNKNOWN_ERROR,
		CG_ERROR_INTERNAL_ERROR_BUFFER_PTR,
		CG_ERROR_UNKNOWN_ACTION,
		CG_ERROR_UNKNOWN_COMMAND,
		//CG_ERROR_SHOT_DETECTED_ON_ZERO_BULLETS,
		//CG_ERROR_STUCK_DETECTED,
		CG_ERROR_UNIMPLEMENTED_ACTION,
		CG_ERROR_UNIMPLEMENTED_COMMAND,
		CG_ERROR_PACKET_STRUCTURE_ERROR,
		CG_ERRORS_SIZE
	};
	enum TRXP_ERRORS{
		TRXP_ERROR_NO_ERROR,
		TRXP_ERROR_UNKNOWN,
		TRXP_ERROR_CRC,
		TRXP_ERROR_ACTION_FIELD_ERROR,
		TRXP_ERROR_COMMAND_FIELD_ERROR,
		TRXP_ERROR_COMMAND_ALLOWED_ONLY_TO_REQUEST_ANSWER,
		TRXP_ERROR_REQUESTED_DATA_OVF,
		TRXP_ERROR_INCOMPLETE_PACKET,
		TRXP_ERROR_,
		TRXP_ERRORS_SIZE
	};
	enum MCUIRQ_COMMANDS{
		MCUIRQ_COMMAND_NONE,
		MCUIRQ_COMMAND_ERROR_DETECTED,
		MCUIRQ_COMMAND_SHOT_DETECTED,
		MCUIRQ_COMMANDS_SIZE
	};
	enum MCUIRQ_ERRORS{
		MCUIRQ_ERROR_NONE,
		MCUIRQ_ERROR_SHOT_DETECTED_ON_ZERO_BULLETS,
		MCUIRQ_ERROR_STUCK_DETECTED,
		MCUIRQ_ERRORS_SIZE
	};
	TRXPacketClass();
	void newpacket_ping(void);
	void newpacket_pong(void);
	bool newpacket_raw(CG_ACTIONS act, uint8_t cmd, uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3);
	CG_ERRORS validate(void);
	void recalculate_crc16(void);
	trxbuffer_data_t get_raw_packet_data(CG_PACKET_STUCTURE data_ptr);
	uint16_t get_field_crc16();
	void refresh_packet_crc16(void);
	uint16_t calculate_crc16(void);
	CG_ACTIONS get_field_action(void);
	MCUIRQ_COMMANDS get_MCUIRQ_field_command(void);
	CG_RA_COMMANDS get_RA_field_command(void);
	trxbuffer_data_t get_raw_field_command(void);
	
	void set_raw_packet_data(CG_PACKET_STUCTURE position, uint8_t byte);
	bool append_rawdata(trxbuffer_data_t data_to_append);
	bool is_full(void);
	bool is_not_full(void);
	bool is_empty(void);
	void clear(void);
	bool is_packet_valid(void);
	TRXP_ERRORS get_last_error(void);
	//Public data
	trxbuffer_data_t data[CG_PACKET_STUCTURE_PACKET_SIZE];
	private:
	trxbuffer_dataptr_t ptr;
	TRXP_ERRORS last_error;
};



#endif /* TRXPACKETCLASS_HPP_ */