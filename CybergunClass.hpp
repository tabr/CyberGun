/* 
* CybergunClass.h
*
* Created: 03.06.2022 12:52:51
* Author: tabr
*/


#ifndef __CYBERGUNCLASS_HPP__
#define __CYBERGUNCLASS_HPP__
#include "CybergunStorageClass.hpp"

enum CYBERGUN_STATUS_BIT_FIELD{
	CYBERGUN_STATUS_BIT_0_ACTIVE,//1-active, 0-inactive
	//CYBERGUN_STATUS_
};

class CybergunClass{
	public:
	
	bool is_active(void);
	void set_status(CYBERGUN_STATUS_BIT_FIELD st, bool set_unset);
	void turn_off_red_led(void);
	void turn_on_red_led(void);
	void turn_off_green_led(void);
	void turn_on_green_led(void);
	void shot_detected(void);
	void update_status(void);
	CybergunStorageClass Storage;
	private:

//functions
public:
CybergunClass();
~CybergunClass();
protected:
private:
CybergunClass( const CybergunClass &c );
CybergunClass& operator=( const CybergunClass &c );

};
/*
class CybergunClass
{
//variables
public:
protected:
private:


}; //CybergunClass
*/
#endif //__CYBERGUNCLASS_HPP__
