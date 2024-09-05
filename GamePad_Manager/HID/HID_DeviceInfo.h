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
	char* path; //�f�o�C�X�p�X

	unsigned short vendor_id;	//�f�o�C�X�̃x���_�[ID
	unsigned short product_id;	//�f�o�C�X�̃v���_�N�gID

	wchar_t* serial_number;		//�V���A���i���o�[

	unsigned short release_number;	//�f�o�C�X�̃����[�X�ԍ���2�i���ŕ\�������́A
	//�f�o�C�X�E�o�[�W�����ԍ��Ƃ��Ă΂��

	wchar_t* manufacturer_string;	//���[�J�[��
	wchar_t* product_string;		//�v���_�N�g��

	unsigned short usage_page;	//���̃f�o�C�X/�C���^�[�t�F�[�X�̎g�p�y�[�W
	//(Windows / Mac / hidraw�̂�)

	unsigned short usage;		//���̃f�o�C�X / �C���^�[�t�F�[�X�̎g�p�@
	//(Windows / Mac / hidraw�̂�)

	int interface_number;		//���̘_���f�o�C�X���\��USB�C���^�[�t�F�[�X��\���܂��B
	//�f�o�C�X��USB HID�f�o�C�X�̏ꍇ�̂ݗL���B
	//����ȊO�̏ꍇ�� - 1�ɐݒ肷��B

	HID_BusType bus_type;	//���o�X�^�C�v
	//�o�[�W���� 0.13.0 �ȍ~�A@ref HID_API_VERSION >= HID_API_MAKE_VERSION(0, 13, 0)
};

#endif // !_HID_DEVICEINFO
