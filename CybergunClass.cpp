/* 
* CybergunClass.cpp
*
* Created: 03.06.2022 12:52:50
* Author: tabr
*/


#include "CybergunClass.hpp"
#include "CybergunProtoClass.hpp"

// default constructor
CybergunClass::CybergunClass()
{
} //CybergunClass

// default destructor
CybergunClass::~CybergunClass()
{
} //~CybergunClass


bool CybergunClass::is_active(void){
	return (this->Storage.get_status() & (1<<CYBERGUN_STATUS_BIT_0_ACTIVE));
}
void CybergunClass::set_status(CYBERGUN_STATUS_BIT_FIELD st, bool set_unset){
	if (set_unset == true){
		sbi(this->Storage.status,st);
		} else {
		cbi(this->Storage.status,st);
	}
	
}
void CybergunClass::turn_off_red_led(void){
	cbi(PORTD,6);
}
void CybergunClass::turn_on_red_led(void){
	sbi(PORTD,6);
}
void CybergunClass::turn_off_green_led(void){
	cbi(PORTD,4);
}
void CybergunClass::turn_on_green_led(void){
	sbi(PORTD,4);
}
void CybergunClass::shot_detected(void){
	if (this->Storage.get_shots() > 0){
		this->Storage.shots_num--;
		CybergunProto.send_mcuirq_command(TRXPacketClass::MCUIRQ_COMMANDS::MCUIRQ_COMMAND_SHOT_DETECTED, this->Storage.get_shots());
		} else {
		//this should be never happened!
		//CybergunProto.send_error(CybergunProtoClass::TRXPacketClass::CG_ERROR_SHOT_DETECTED_ON_ZERO_BULLETS);
		CybergunProto.send_mcuirq_command(TRXPacketClass::MCUIRQ_COMMANDS::MCUIRQ_COMMAND_ERROR_DETECTED, TRXPacketClass::MCUIRQ_ERRORS::MCUIRQ_ERROR_SHOT_DETECTED_ON_ZERO_BULLETS);
	}
	this->update_status();
}
void CybergunClass::update_status(void){
	if (this->Storage.get_shots() == 0){
		this->set_status(CYBERGUN_STATUS_BIT_0_ACTIVE, false);
	}
	//if (this->Storage.get_status())
	if (this->is_active()){
		this->turn_off_red_led();
		this->turn_on_green_led();
		} else {
		this->turn_on_red_led();
		this->turn_off_green_led();
	}
}