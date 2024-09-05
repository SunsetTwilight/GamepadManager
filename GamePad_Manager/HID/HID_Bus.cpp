#include "HID_Bus.h"

#include <Windows.h>
#include <hidsdi.h>
#include <SetupAPI.h>
#include <cfgmgr32.h>

#include <devpkey.h>

#include <memory>

#include "HID_InternalFunc.h"

HID_Bus::HID_Bus()
{

}

HID_Bus::~HID_Bus()
{

}

DEVINST	HID_Bus::Get_DEVINST()
{
	return dev_node;
}

HID_BusType	HID_Bus::Get_BusType()
{
	return bus_type;
}

unsigned int HID_Bus::Get_BusFlags()
{
	return bus_flags;
}

//バスタイプ検出
//
////
void HID_Bus::DetectBusType(const wchar_t* interface_path)
{
	wchar_t* device_id = NULL,* compatible_ids = NULL;
	CONFIGRET cr;
	DEVINST _dev_node;
	HID_Bus bus;

	/* Get the device id from interface path */
	device_id = (wchar_t*)HID_GetDeviceInterfaceProperty(
		interface_path,
		&DEVPKEY_Device_InstanceId,
		DEVPROP_TYPE_STRING
	);

	if (!device_id)
		goto end;

	/* Open devnode from device id */
	cr = CM_Locate_DevNodeW(&_dev_node,(DEVINSTID_W)device_id, CM_LOCATE_DEVNODE_NORMAL);
	if (cr != CR_SUCCESS)
		goto end;

	/* Get devnode parent */
	cr = CM_Get_Parent( &_dev_node, _dev_node, 0);
	if (cr != CR_SUCCESS)
		goto end;

	/* Get the compatible ids from parent devnode */
	compatible_ids = (wchar_t*)HID_GetDevnodeProperty(
		_dev_node,
		&DEVPKEY_Device_CompatibleIds,
		DEVPROP_TYPE_STRING_LIST
	);
	if (!compatible_ids)
		goto end;

	/* Now we can parse parent's compatible IDs to find out the device bus type */
	for (wchar_t* compatible_id = compatible_ids; *compatible_id; compatible_id += wcslen(compatible_id) + 1) {
		/* Normalize to upper case */
		HID_Towupper(compatible_id);

		/* USB devices
		   https://docs.microsoft.com/windows-hardware/drivers/hid/plug-and-play-support
		   https://docs.microsoft.com/windows-hardware/drivers/install/standard-usb-identifiers */
		if (wcsstr(compatible_id, L"USB") != NULL) {
			bus_type = HID_BusType::HID_BUS_TYPE_USB;
			break;
		}

		/* Bluetooth devices
		   https://docs.microsoft.com/windows-hardware/drivers/bluetooth/installing-a-bluetooth-device */
		if (wcsstr(compatible_id, L"BTHENUM") != NULL) {
			bus_type = HID_BusType::HID_BUS_TYPE_BLUETOOTH;
			break;
		}

		/* Bluetooth LE devices */
		if (wcsstr(compatible_id, L"BTHLEDEVICE") != NULL) {
			bus_type = HID_BusType::HID_BUS_TYPE_BLUETOOTH;
			bus_flags |= HID_BUS_FLAG_BLE;
			break;
		}

		/* I2C devices
		   https://docs.microsoft.com/windows-hardware/drivers/hid/plug-and-play-support-and-power-management */
		if (wcsstr(compatible_id, L"PNP0C50") != NULL) {
			bus_type = HID_BusType::HID_BUS_TYPE_I2C;
			break;
		}

		/* SPI devices
		   https://docs.microsoft.com/windows-hardware/drivers/hid/plug-and-play-for-spi */
		if (wcsstr(compatible_id, L"PNP0C51") != NULL) {
			bus_type = HID_BusType::HID_BUS_TYPE_SPI;
			break;
		}
	}

	dev_node = _dev_node;

end:
	free(device_id);
	free(compatible_ids);
}
