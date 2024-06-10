#include "extmem_hal.h"
#include "sst25vf_spi.h"

void SST25ExtFlash::Start()
{
	sFLASH_Init();
}
uint32_t SST25ExtFlash::pageSize()
{
	return sFLASH_PAGESIZE;
}

uint32_t SST25ExtFlash::pageCount()
{
	return sFLASH_PAGECOUNT;
}

uint32_t SST25ExtFlash::page2Address(uint32_t page)
{
	return uint32_t(page) * pageSize();
}

uint32_t SST25ExtFlash::address2Page(uint32_t address)
{
	return address/pageSize();
}

bool SST25ExtFlash::isPageAddress(uint32_t address)
{
	return (address % pageSize()) == 0;
}

bool SST25ExtFlash::isValidAddress(uint32_t address, uint32_t extent) 
{
	return address + extent <= page2Address(pageCount()) && (extent==0 || (address2Page(address)==address2Page(address+extent-1)));
}

bool SST25ExtFlash::eraseAll(void)
{
	uint32_t end = page2Address(pageCount());
	uint32_t size = pageSize();
	bool success = true;
	for (uint32_t i = 0; i<end; i+=size) {
		success = success && erasePage(i);
	}
	return success;
}

bool SST25ExtFlash::erasePage(uint32_t address)
{
	bool success = false;
	if (address < page2Address(pageCount()) && (address % pageSize()) == 0) {
	sFLASH_EraseSector(address);
	success = true;
	}
	return success;
}

bool SST25ExtFlash::write(const void* data, uint32_t address, uint32_t length_bytes)
{
	// TODO: SPI interface shouldn't need mutable data buffer to write?
	//sFLASH_WriteBuffer(data, address, length_bytes);
	sFLASH_WriteBuffer(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(data)), address, length_bytes);
	return true;
}

bool SST25ExtFlash::read(void* data, uint32_t address, uint32_t length_bytes)
{
	//sFLASH_ReadBuffer(data, address, length_bytes);
	sFLASH_ReadBuffer(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(data)), address, length_bytes);
	return true;
}

int SST25ExtFlash::selfTest(void)
{
	return sFLASH_SelfTest();
}
