#ifndef _HID_BUS
#define _HID_BUS

#include "HID_BusType.h"

#include <Windows.h>
#include <cfgmgr32.h>

#include <initguid.h>

#define HID_BUS_FLAG_BLE 0x01

class HID_Bus
{
public:
	HID_Bus();
	~HID_Bus();

public:
	void DetectBusType(const wchar_t* interface_path);

	DEVINST			Get_DEVINST();
	HID_BusType		Get_BusType();
	unsigned int	Get_BusFlags();

private:

private:
	DEVINST dev_node;
	HID_BusType bus_type;
	unsigned int bus_flags;

	friend class HID_DeviceInfo;
};

#endif // !_HID_BUS
