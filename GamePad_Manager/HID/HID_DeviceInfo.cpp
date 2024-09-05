#include "HID_DeviceInfo.h"

#include <hidsdi.h>
#include <SetupAPI.h>
#include <cfgmgr32.h>

#include <string.h>

#include <devpkey.h>
#include <propkey.h>

#include "HID_BusType.h"
#include "HID_Bus.h"
#include "HID_InternalFunc.h"

#include <malloc.h>

/* MAXIMUM_USB_STRING_LENGTH from usbspec.h is 255 */
/* BLUETOOTH_DEVICE_NAME_SIZE from bluetoothapis.h is 256 */
#define MAX_STRING_WCHARS 256

/* For certain USB devices, using a buffer larger or equal to 127 wchars results
   in successful completion of HID API functions, but a broken string is stored
   in the output buffer. This behaviour persists even if HID API is bypassed and
   HID IOCTLs are passed to the HID driver directly. Therefore, for USB devices,
   the buffer MUST NOT exceed 126 WCHARs.
*/

#define MAX_STRING_WCHARS_USB 126

HID_DeviceInfo::HID_DeviceInfo()
{

}

HID_DeviceInfo::~HID_DeviceInfo()
{

}

PSP_DEVICE_INTERFACE_DETAIL_DATA_W HID_Internal_GetDevicePath(
	HANDLE h_hidDi,
	SP_DEVICE_INTERFACE_DATA device_interface_data
)
{
	//戻り値用変数初期化
	PSP_DEVICE_INTERFACE_DETAIL_DATA_W devicePath = NULL;

	//デバイスパスのサイズを問い合わせる
	DWORD requiredSize = 0;
	SetupDiGetDeviceInterfaceDetailW(
		h_hidDi,
		&device_interface_data,
		NULL,
		0,
		&requiredSize,
		NULL
	);

	//サイズチェック
	if (MAX_PATH * sizeof(wchar_t) < requiredSize)
	{
		SetupDiDestroyDeviceInfoList(h_hidDi);
		//return E_EXTERNAL_WINAPI;
		return NULL;
	}

	//デバイスパス用メモリ確保
	devicePath =
		(SP_DEVICE_INTERFACE_DETAIL_DATA_W*)_malloca(requiredSize);
	if (!devicePath) {
		SetupDiDestroyDeviceInfoList(h_hidDi);
		//return E_STACK_OVERFLOW;
		return NULL;
	}

	//デバイスパス取得
	devicePath->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
	SetupDiGetDeviceInterfaceDetailW(
		h_hidDi,
		&device_interface_data,
		devicePath,
		requiredSize,
		NULL,
		NULL
	);

	return devicePath;
}

HANDLE HID_OpenDevice(
	const wchar_t* path, 
	BOOL open_rw
)
{
	HANDLE handle;
	DWORD desired_access = (open_rw) ? (GENERIC_WRITE | GENERIC_READ) : 0;
	DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;

	handle = CreateFileW(
		path,
		desired_access,
		share_mode,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,/*FILE_ATTRIBUTE_NORMAL,*/
		0
	);

	return handle;
}

void HID_DeviceInfo::hid_internal_get_usb_info(DEVINST dev_node)
{
	wchar_t* device_id = NULL, * hardware_ids = NULL;
	
	device_id = (wchar_t*)HID_GetDevnodeProperty(
		dev_node,
		&DEVPKEY_Device_InstanceId, 
		DEVPROP_TYPE_STRING
	);
	if (!device_id)
		goto end;

	/* Normalize to upper case */
	HID_Towupper(device_id);

	/* Check for Xbox Common Controller class (XUSB) device.
	   https://docs.microsoft.com/windows/win32/xinput/directinput-and-xusb-devices
	   https://docs.microsoft.com/windows/win32/xinput/xinput-and-directinput
	*/
	if (HID_ExtractIntTokenValue(device_id, L"IG_") != -1) {
		/* Get devnode parent to reach out USB device. */
		if (CM_Get_Parent(&dev_node, dev_node, 0) != CR_SUCCESS)
			goto end;
	}

	/* Get the hardware ids from devnode */
	hardware_ids = (wchar_t*)HID_GetDevnodeProperty(
		dev_node,
		&DEVPKEY_Device_HardwareIds,
		DEVPROP_TYPE_STRING_LIST
	);
	if (!hardware_ids)
		goto end;

	/* Get additional information from USB device's Hardware ID
	   https://docs.microsoft.com/windows-hardware/drivers/install/standard-usb-identifiers
	   https://docs.microsoft.com/windows-hardware/drivers/usbcon/enumeration-of-interfaces-not-grouped-in-collections
	*/
	for (wchar_t* hardware_id = hardware_ids; *hardware_id; hardware_id += wcslen(hardware_id) + 1) 
	{
		/* Normalize to upper case */
		HID_Towupper(hardware_id);

		if (release_number == 0) 
		{
			/* USB_DEVICE_DESCRIPTOR.bcdDevice value. */
			int _release_number = HID_ExtractIntTokenValue(hardware_id, L"REV_");
			if (_release_number != -1) 
			{
				release_number = (unsigned short)_release_number;
			}
		}

		if (interface_number == -1) 
		{
			/* USB_INTERFACE_DESCRIPTOR.bInterfaceNumber value. */
			int _interface_number = HID_ExtractIntTokenValue(hardware_id, L"MI_");
			if (_interface_number != -1) 
			{
				interface_number = _interface_number;
			}
		}
	}

	/* Try to get USB device manufacturer string if not provided by HidD_GetManufacturerString. */
	if (wcslen(manufacturer_string) == 0) 
	{
		wchar_t* _manufacturer_string = (wchar_t*)HID_GetDevnodeProperty(
				dev_node, 
				&DEVPKEY_Device_Manufacturer, 
				DEVPROP_TYPE_STRING
			);
		if (_manufacturer_string) {
			free(manufacturer_string);
			manufacturer_string = _manufacturer_string;
		}
	}

	/* Try to get USB device serial number if not provided by HidD_GetSerialNumberString. */
	if (wcslen(serial_number) == 0) 
	{
		DEVINST usb_dev_node = dev_node;
		if (interface_number != -1) 
		{
			/* Get devnode parent to reach out composite parent USB device.
			   https://docs.microsoft.com/windows-hardware/drivers/usbcon/enumeration-of-the-composite-parent-device
			*/
			if (CM_Get_Parent(&usb_dev_node, dev_node, 0) != CR_SUCCESS)
				goto end;
		}

		/* Get the device id of the USB device. */
		free(device_id);
		device_id = (wchar_t*)HID_GetDevnodeProperty(
			usb_dev_node, 
			&DEVPKEY_Device_InstanceId, 
			DEVPROP_TYPE_STRING
		);
		if (!device_id)
			goto end;

		/* Extract substring after last '\\' of Instance ID.
		   For USB devices it may contain device's serial number.
		   https://docs.microsoft.com/windows-hardware/drivers/install/instance-ids
		*/
		for (wchar_t* ptr = device_id + wcslen(device_id); ptr > device_id; --ptr)
		{
			/* Instance ID is unique only within the scope of the bus.
			   For USB devices it means that serial number is not available. Skip. */
			if (*ptr == L'&') break;

			if (*ptr == L'\\')
			{
				free(serial_number);
				serial_number = _wcsdup(ptr + 1);
				break;
			}
		}
	}

	/* If we can't get the interface number, it means that there is only one interface. */
	if (interface_number == -1) interface_number = 0;

end:
	free(device_id);
	free(hardware_ids);
}

/* HidD_GetProductString/HidD_GetManufacturerString/HidD_GetSerialNumberString is not working for BLE HID devices
   Request this info via dev node properties instead.
   https://docs.microsoft.com/answers/questions/401236/hidd-getproductstring-with-ble-hid-device.html
*/
void HID_DeviceInfo::hid_internal_get_ble_info(DEVINST dev_node)
{
	if (wcslen(manufacturer_string) == 0) 
	{
		/* Manufacturer Name String (UUID: 0x2A29) */
		wchar_t* _manufacturer_string = 
			(wchar_t*)HID_GetDevnodeProperty(
				dev_node, 
				(const DEVPROPKEY*)&PKEY_DeviceInterface_Bluetooth_Manufacturer, 
				DEVPROP_TYPE_STRING
			);
		if (_manufacturer_string) {
			free(manufacturer_string);
			manufacturer_string =_manufacturer_string;
		}
	}

	if (wcslen(serial_number) == 0) 
	{
		/* Serial Number String (UUID: 0x2A25) */
		wchar_t* _serial_number = 
			(wchar_t*)HID_GetDevnodeProperty(
				dev_node, 
				(const DEVPROPKEY*)&PKEY_DeviceInterface_Bluetooth_DeviceAddress, 
				DEVPROP_TYPE_STRING
			);
		if (_serial_number) {
			free(serial_number);
			serial_number = _serial_number;
		}
	}

	if (wcslen(product_string) == 0) 
	{
		/* Model Number String (UUID: 0x2A24) */
		wchar_t* _product_string = 
			(wchar_t*)HID_GetDevnodeProperty(
				dev_node, (const DEVPROPKEY*)&PKEY_DeviceInterface_Bluetooth_ModelNumber, 
				DEVPROP_TYPE_STRING
			);
		if (!_product_string) {
			DEVINST parent_dev_node = 0;
			/* Fallback: Get devnode grandparent to reach out Bluetooth LE device node */
			if (CM_Get_Parent(&parent_dev_node, dev_node, 0) == CR_SUCCESS) {
				/* Device Name (UUID: 0x2A00) */
				_product_string = 
					(wchar_t*)HID_GetDevnodeProperty(
						parent_dev_node, 
						&DEVPKEY_NAME, 
						DEVPROP_TYPE_STRING
					);
			}
		}

		if (_product_string) {
			free(product_string);
			product_string = _product_string;
		}
	}
}

void HID_DeviceInfo::SetDeviceInfo(
	const wchar_t* _path, 
	HANDLE handle
)
{
	/* Fill out the record */
	{
		path = HID_UTF16toUTF8(_path);
		interface_number = -1;
	
		HIDD_ATTRIBUTES attrib = {};
		attrib.Size = sizeof(HIDD_ATTRIBUTES);
		if (HidD_GetAttributes(handle, &attrib))
		{
			// VID / PID
			vendor_id = attrib.VendorID;
			product_id = attrib.ProductID;

			// Release Number
			release_number = attrib.VersionNumber;
		}
	}
	// Get the Usage Page and Usage for this device. */
	{
		HIDP_CAPS caps;
		PHIDP_PREPARSED_DATA pp_data = NULL;
		if (HidD_GetPreparsedData(handle, &pp_data)) 
		{
			if (HidP_GetCaps(pp_data, &caps) == HIDP_STATUS_SUCCESS) 
			{
				usage_page = caps.UsagePage;
				usage = caps.Usage;
			}
			HidD_FreePreparsedData(pp_data);
		}
	}

	/* detect bus type before reading string descriptors */
	{
		HID_Bus bus;
		bus.DetectBusType(_path);
		bus_type = bus.bus_type;

		ULONG len;
		ULONG size;
		wchar_t string[MAX_STRING_WCHARS + 1];

		len = (bus_type == HID_BusType::HID_BUS_TYPE_USB) ? MAX_STRING_WCHARS_USB : MAX_STRING_WCHARS;
		string[len] = L'\0';
		size = len * sizeof(wchar_t);

		/* Serial Number */
		string[0] = L'\0';
		HidD_GetSerialNumberString(handle, string, size);
		serial_number = _wcsdup(string);

		/* Manufacturer String */
		string[0] = L'\0';
		HidD_GetManufacturerString(handle, string, size);
		manufacturer_string = _wcsdup(string);

		/* Product String */
		string[0] = L'\0';
		HidD_GetProductString(handle, string, size);
		product_string = _wcsdup(string);

		/* now, the portion that depends on string descriptors */
		switch (bus_type) {
		case HID_BUS_TYPE_USB:
			hid_internal_get_usb_info(bus.dev_node);
			break;

		case HID_BUS_TYPE_BLUETOOTH:
			if (bus.bus_flags & HID_BUS_FLAG_BLE)
				hid_internal_get_ble_info(bus.dev_node);
			break;

		case HID_BUS_TYPE_UNKNOWN:
		case HID_BUS_TYPE_SPI:
		case HID_BUS_TYPE_I2C:
			/* shut down -Wswitch */
			break;
		}
	}

	return;
}

std::shared_ptr<HID_DeviceInfo> HID_DeviceInfo::HID_GetDeviceInfo(
	const wchar_t* path, 
	HANDLE handle
)
{
	std::shared_ptr<HID_DeviceInfo> device_info = std::make_shared<HID_DeviceInfo>();

	if (device_info == NULL) {
		return NULL;
	}

	device_info->SetDeviceInfo(path, handle);

	return device_info;
}

std::list<std::shared_ptr<HID_DeviceInfo>> HID_DeviceInfo::HID_EnumDeviceInfos(
	_In_ const HID_SearchID* pSearch_id,
	_In_ const UINT size_of_searchID
)
{
	std::list<std::shared_ptr<HID_DeviceInfo>> device_info_list;

	GUID interface_class_guid;
	HidD_GetHidGuid(&interface_class_guid); //HIDClass デバイスの GUIDデバイス インターフェイスを返す。

	// 全ての HIDデバイス を取得 
	HANDLE h_hdi = SetupDiGetClassDevs(
		&interface_class_guid, //指定のデバイスインターフェースクラス
		NULL,
		NULL,
		DIGCF_DEVICEINTERFACE | DIGCF_PRESENT
		//
		// DIGCF_DEVICEINTERFACE
		//  指定した デバイスインターフェイスクラス の デバイスインターフェイス をサポートするデバイスを返す。
		//  第１引数で デバイスインスタンスID が指定されている場合は、第４引数でこのフラグを設定する必要があります。
		//
		// DIGCF_PRESENT 
		//  システムに現在存在するデバイスのみを返します。
	);

	if (!h_hdi ||
		(h_hdi == INVALID_HANDLE_VALUE)) //NULL or INVALID_HANDLE_VALUE なら エラー
	{
		//TODO:: return E_EXTERNAL_WINAPI;
	}

	unsigned int input_arr_index = 0;

	DWORD device_index = 0;
	SP_DEVINFO_DATA hid_info{};
	hid_info.cbSize = sizeof(SP_DEVINFO_DATA);

	UINT count_of_device = 0;

	//HIDデバイス を列挙する
	while (SetupDiEnumDeviceInfo(
		h_hdi,
		device_index,
		&hid_info))
	{
		DWORD member_index = 0;
		SP_DEVICE_INTERFACE_DATA device_interface_data{};
		device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		//すべての HIDデバイス・インターフェース を列挙する
		while (SetupDiEnumDeviceInterfaces(
			h_hdi,
			&hid_info,
			&interface_class_guid,
			member_index,
			&device_interface_data))
		{
			PSP_DEVICE_INTERFACE_DETAIL_DATA_W devicePath =
				HID_Internal_GetDevicePath(h_hdi, device_interface_data);
			if (devicePath != NULL)
			{
				HANDLE h_device = HID_OpenDevice(devicePath->DevicePath, TRUE);
				if (h_device && (h_device != INVALID_HANDLE_VALUE)) 
				{
					// Get vendor and product id
					unsigned int vendor_id = NULL;
					unsigned int product_id = NULL;

					HIDD_ATTRIBUTES deviceAttributes;
					if (HidD_GetAttributes(h_device, &deviceAttributes))
					{
						vendor_id  = deviceAttributes.VendorID;
						product_id = deviceAttributes.ProductID;
					}

					// Check if ids match
					for (size_t i = 0; i < size_of_searchID; i++) {
						if (vendor_id  != NULL && vendor_id  == (pSearch_id + i)->vendor_ID &&
							product_id != NULL && product_id == (pSearch_id + i)->product_ID)
						{
							//auto device_info = std::make_shared<HID_DeviceInfo>();
							//device_info->SetDeviceInfo(devicePath->DevicePath, h_device);
							device_info_list.push_back(HID_GetDeviceInfo(devicePath->DevicePath, h_device));

							count_of_device++; //デバイスの数追加
						}
					}

					// Close device
					CloseHandle(h_device);
				}
			}
			member_index++; //インクリメント
		}
		device_index++; //インクリメント
	}

	return device_info_list;
}