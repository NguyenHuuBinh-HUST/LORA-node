#ifndef EXTMEM_HAL_H
#define EXTMEM_HAL_H

#include <stdint.h>

class SST25ExtFlash
{
public:
	/**
	* @return Init Flash.
	*/
	void Start(void);
	/**
	* @return Return size of each page.
	*/
	uint32_t pageSize(void);
	/**
	* @return Return number of page on device.
	*/
	uint32_t pageCount(void);
	/**
	* @return Convert page to address.
	*/
	uint32_t page2Address(uint32_t page);
	/**
	* @return Convert address to page.
	*/
	uint32_t address2Page(uint32_t address);
	/**
	* @return Determines if the given address represents the start of a page.
	*/
	bool isPageAddress(uint32_t address);
	
	bool isValidAddress(uint32_t address, uint32_t extent);

	bool erasePage(uint32_t address);
	
	bool eraseAll(void);
	
	bool write(const void* data, uint32_t address, uint32_t length_bytes);
	
	bool read(void* data, uint32_t address, uint32_t length_bytes);
	
	int selfTest(void);
};

#endif
