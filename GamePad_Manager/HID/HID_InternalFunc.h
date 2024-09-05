#ifndef _HID_INTERNAL_FUNC
#define _HID_INTERNAL_FUNC

#include <Windows.h>
#include <cfgmgr32.h>

 void* HID_GetDevnodeProperty(
	DEVINST dev_node,
	const DEVPROPKEY* property_key,
	DEVPROPTYPE expected_property_type
);

void* HID_GetDeviceInterfaceProperty(
	const wchar_t* interface_path,
	const DEVPROPKEY* property_key,
	DEVPROPTYPE expected_property_type
);

 void HID_Towupper(wchar_t* string);

 int HID_ExtractIntTokenValue(
	wchar_t* string, 
	const wchar_t* token
);

 char* HID_UTF16toUTF8(const wchar_t* src);
 wchar_t* HID_UTF8toUTF16(const char* src);

#endif // !_HID_INTERNAL_FUNC

