#include "stdafx.h"

class nt_api
{
public:
	__checkReturn static BOOL device_io_control(
										__in         HANDLE hDevice,
										__in         DWORD dwIoControlCode,
										__in_opt     LPVOID lpInBuffer,
										__in         DWORD nInBufferSize,
										__out_opt    LPVOID lpOutBuffer,
										__in         DWORD nOutBufferSize,
										__out_opt    LPDWORD lpBytesReturned,
										__inout_opt  LPOVERLAPPED lpOverlapped)
	{
		return device_io_control_pimpl(hDevice, 
									 dwIoControlCode, 
									 lpInBuffer, 
									 nInBufferSize, 
									 lpOutBuffer, 
									 nOutBufferSize, 
									 lpBytesReturned, 
									 lpOverlapped);
	}

private:
	static BOOL device_io_control_pimpl(
										__in         HANDLE hDevice,
										__in         DWORD dwIoControlCode,
										__in_opt     LPVOID lpInBuffer,
										__in         DWORD nInBufferSize,
										__out_opt    LPVOID lpOutBuffer,
										__in         DWORD nOutBufferSize,
										__out_opt    LPDWORD lpBytesReturned,
										__inout_opt  LPOVERLAPPED lpOverlapped)
	{
		std::cout << "Actual Windows API called" << std::endl;
		return TRUE;
	}
};

#define IOCTL_DEVICE_SET_POWER_LIMIT	CTL_CODE(FILE_DEVICE_PMI, 0xA200, METHOD_BUFFERED, FILE_READ_ACCESS)

class our_device
{
public:
	bool getPowerLimit() const { return _power_limit; }
	
	void setPowerLimit(__deref_in_bcount(powerLimitSize) const TCHAR *powerLimit, __in size_t powerLimitSize)
	{
		DWORD bytesReturned = 0;
		BOOL ret_code = nt_api::device_io_control(_device_handle, 
												  IOCTL_DEVICE_SET_POWER_LIMIT,
												  (LPVOID)powerLimit, 
												  powerLimitSize,
												  NULL,
												  0,
												  &bytesReturned,
												  NULL);
		
		_power_limit = (ret_code == TRUE);
		
		std::cout << "Return value was: " 
				  << (_power_limit ? "Success, so do nothing" : "Failure, either log or throw an exception")
				  << std::endl;
	}

	our_device() : _power_limit(false), _device_handle(NULL) {}

private:
	HANDLE		_device_handle;
	bool		_power_limit;
};


TEST(GivenOurDevice, WhenPowerLimitSet_ThenShouldCallDriver)
{
	our_device device;
	START_STUBBING;
		REGISTER_STUB(nt_api::device_io_control, TRUE);
	END_STUBBING;

	tstring powerLimit = _T("adaptive");

	device.setPowerLimit(powerLimit.c_str(), powerLimit.size());

	EXPECT_TRUE(device.getPowerLimit());
}