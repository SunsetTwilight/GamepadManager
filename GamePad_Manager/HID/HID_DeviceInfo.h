#ifndef _HID_DEVICEINFO
#define _HID_DEVICEINFO

#include "HID_BusType.h"
#include "HID_ID.h"

#include <Windows.h>
#include <cfgmgr32.h>

#include <memory>
#include <list>

#include <initguid.h>

class HID_DeviceInfo
{
public:
	HID_DeviceInfo();
	~HID_DeviceInfo();

public:
	
	void SetDeviceInfo(const wchar_t* path, HANDLE handle);

	void hid_internal_get_usb_info(DEVINST dev_node);
	void hid_internal_get_ble_info(DEVINST dev_node);

public:
	
	static std::list<std::shared_ptr<HID_DeviceInfo>> HID_EnumDeviceInfos(
		_In_ const HID_SearchID* pSearch_id,
		_In_ const UINT size_of_searchID
	);

	static std::shared_ptr<HID_DeviceInfo> HID_GetDeviceInfo(
		const wchar_t* path, 
		HANDLE handle
	);

//private:
	char* path; //デバイスパス

	unsigned short vendor_id;	//デバイスのベンダーID
	unsigned short product_id;	//デバイスのプロダクトID

	wchar_t* serial_number;		//シリアルナンバー

	unsigned short release_number;	//デバイスのリリース番号を2進数で表したもの、
	//デバイス・バージョン番号とも呼ばれる

	wchar_t* manufacturer_string;	//メーカー名
	wchar_t* product_string;		//プロダクト名

	unsigned short usage_page;	//このデバイス/インターフェースの使用ページ
	//(Windows / Mac / hidrawのみ)

	unsigned short usage;		//このデバイス / インターフェースの使用法
	//(Windows / Mac / hidrawのみ)

	int interface_number;		//この論理デバイスが表すUSBインターフェースを表します。
	//デバイスがUSB HIDデバイスの場合のみ有効。
	//それ以外の場合は - 1に設定する。

	HID_BusType bus_type;	//基底バスタイプ
	//バージョン 0.13.0 以降、@ref HID_API_VERSION >= HID_API_MAKE_VERSION(0, 13, 0)
};

#endif // !_HID_DEVICEINFO
