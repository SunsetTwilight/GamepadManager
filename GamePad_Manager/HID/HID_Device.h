#ifndef _HID_DEVICE
#define _HID_DEVICE

#include <hidsdi.h> 
#include <Windows.h>

#include "HID_DeviceInfo.h"

class HID_Device
{
public:
	HID_Device();
	~HID_Device();

private:
	HANDLE device_handle;

	HID_DeviceInfo* device_info;

	size_t input_report_length;
	USHORT output_report_length;
	USHORT feature_report_length;

	char* read_buf;
	unsigned char* write_buf;
	unsigned char* feature_buf;

	wchar_t* last_error_str;

	OVERLAPPED ol;
	OVERLAPPED write_ol;

	BOOL blocking;
	BOOL read_pending;
};

#endif // !_HID_DEVICE