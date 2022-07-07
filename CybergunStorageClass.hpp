/* 
* CybergunStorageClass.h
*
* Created: 03.06.2022 12:59:00
* Author: tabr
*/


#ifndef __CYBERGUNSTORAGECLASS_HPP__
#define __CYBERGUNSTORAGECLASS_HPP__
#include <avr/io.h>
/*
class CybergunStorageClass
{
//variables
public:
protected:
private:

//functions
public:
	CybergunStorageClass();
	~CybergunStorageClass();
protected:
private:
	CybergunStorageClass( const CybergunStorageClass &c );
	CybergunStorageClass& operator=( const CybergunStorageClass &c );

}; //CybergunStorageClass
*/
class CybergunStorageClass{
	public:
	uint8_t get_shots(void){
		return this->shots_num;
	}
	uint8_t get_status(void){
		return this->status;
	}
	uint16_t get_raw_MSB(void){
		return (this->get_shots() << 8) | this->get_status();
	}
	void set_raw_MSB(uint8_t data0, uint8_t data1){
		this->set_shots(data0);
		this->set_status(data1);
	}
	void set_raw_MSB(uint16_t data){
		this->set_shots(data>>8);
		this->set_status(data & 0xFF);
		//		this->update_status();
	}
	void set_shots(uint8_t num){
		this->shots_num = num;
	}
	void set_status(uint8_t st){
		this->status = st;
	}
	CybergunStorageClass(void){
		this->set_shots(0);
		this->set_status(0);
	}
	uint8_t shots_num;
	uint8_t status;
	private:
};
#endif //__CYBERGUNSTORAGECLASS_HPP__
