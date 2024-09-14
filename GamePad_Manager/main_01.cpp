#include <Windows.h>

#include <stdio.h>

#include "HID/HID_DeviceInfo.h"

#pragma comment(lib, "Hid.lib")
#pragma comment(lib, "Cfgmgr32.lib")
#pragma comment(lib, "SetupAPI.lib")

#define NOMINMAX

#define DualSense_VendorID		0x054c
#define DualSense_ProductID		0x0ce6

#define DualShock4_VendorID		0x054c
#define DualShock4_ProductID	0x09cc

#include "Console.h"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance, 
	_In_opt_ HINSTANCE hPrevInstance, 
	_In_ LPWSTR lpCmdLine, 
	_In_ int nCmdShow) 
{
	InitConsole();

	printf("HelloWorld\n");

	HID_SearchID search_id[] = { 
		{ DualSense_VendorID , DualSense_ProductID  },
		{ DualShock4_VendorID, DualShock4_ProductID },
	};

	auto hid_list = HID_DeviceInfo::HID_EnumDeviceInfos(
		search_id, 
		sizeof(search_id) / sizeof(search_id[0])
	);
	if (!hid_list.size()) {
		printf("HID No find!\n");
		
	}
	else
	{
		printf("HID Find count of %d\n\n", (unsigned int)hid_list.size());

		for (auto hid = hid_list.begin(); hid != hid_list.end(); hid++)
		{
			printf("[%02d]\n\n", std::distance(hid_list.begin(), hid));

			printf("DevicePath         :%s\n", (*hid)->path);

			printf("VendorID           :0x%04x\n", (*hid)->vendor_id);
			printf("productID          :0x%04x\n", (*hid)->product_id);

			printf("Serial Number      :%ls\n", (*hid)->serial_number);

			printf("Release Number     :0x%04x\n", (*hid)->release_number);

			printf("Manufacture String :%ls\n", (*hid)->manufacturer_string);
			printf("Product String     :%ls\n", (*hid)->product_string);

			printf("Usage Page         :0x%04x\n", (*hid)->usage_page)
				;
			printf("Usage              :0x%04x\n", (*hid)->usage);

			printf("Interface Numuber  :%d\n", (*hid)->interface_number);

			{
				printf("BUS                :");
				switch ((*hid)->bus_type)
				{
				case HID_BUS_TYPE_UNKNOWN:		printf("Unknown");		break;
				case HID_BUS_TYPE_USB:			printf("USB");			break;
				case HID_BUS_TYPE_BLUETOOTH:	printf("Bluetooth");	break;
				case HID_BUS_TYPE_I2C:			printf("I2C");			break;
				case HID_BUS_TYPE_SPI:			printf("SPI");			break;
				default:
					break;
				}
				printf("\n");
			}

			printf("\n");
		}
	}

	printf("Escape:èIóπ\n");
	while (1)
	{
		if (GetAsyncKeyState(VK_ESCAPE)) break;
	}

	CleanUpConsole();
	return 0;
}