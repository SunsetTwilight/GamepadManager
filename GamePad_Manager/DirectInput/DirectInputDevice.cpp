#include "DirectInputDevice.h"

//------------------------------------------------------------------------------------------------
//Constructor
//------------------------------------------------------------------------------------------------
DirectInputDevice::DirectInputDevice(
	const GUID guid,
	LPCDIDATAFORMAT lpDataFormat
)
{
	//DirectInputDevice8オブジェクトの生成とインタフェースの取得
	if (!m_directInput.CreateDirectInputDevice(
		m_device.GetAddressOf(),
		guid))
	{
		::OutputDebugString(L"Error - DirectInputDeviceが初期化出来ませんでした(DirectInputDevice::DirectInputDevice)¥n");
		return;
	}

	//データフォーマットの設定
	HRESULT hr = m_device->SetDataFormat(lpDataFormat);
	if (hr != DI_OK) //失敗
	{
		::OutputDebugString(L"Error - Error - データフォーマット設定失敗(CDInput8_CreateKeyboard)¥n");
	}
	return;
}

//------------------------------------------------------------------------------------------------
//Destructor
//------------------------------------------------------------------------------------------------
DirectInputDevice::~DirectInputDevice()
{
	Unacquire(); //アクセス権解放
}

//------------------------------------------------------------------------------------------------
//状態の更新
//------------------------------------------------------------------------------------------------
void DirectInputDevice::UpdateState()
{
	GetState(); //
}


//------------------------------------------------------------------------------------------------
//協調レベル設定
// 第１引数：デバイス入力の制限をするステータス
// 第２引数：排他占有設定をするステータス
// 戻り値　：BOOL 成功：TRUE、失敗：FALSE
//------------------------------------------------------------------------------------------------
BOOL DirectInputDevice::SetCooperativeLevel(
	CL_GROUND ground, 
	CL_EXCLUSIVE exclusive
)
{
	HRESULT hr = m_device->SetCooperativeLevel(
		m_directInput.GetHWND(), //ウィンドウハンドル
		ground | exclusive		 //協調レベルのステータス（ビット演算）
	);
	if (hr != DI_OK) //失敗
	{
		::OutputDebugString(L"Error - 協調レベル設定に失敗しました(CDInput8_CreateKeyboard)¥n");
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------------------------------
//アクセス権の取得
//------------------------------------------------------------------------------------------------
void DirectInputDevice::Acquire()
{
	//デバイスが存在するか
	if (!m_device.operator Microsoft::WRL::Details::BoolType())
	{
		::OutputDebugString(L"Error - デバイスが存在しません(DirectInputDevice::Acquire)¥n");
		return;
	}

	HRESULT hr = m_device->Acquire();
	if (hr != DI_OK) //失敗
	{
		::OutputDebugString(L"Error - アクセス権取得に失敗しました(DirectInputDevice::Acquire)¥n");
	}
	return;
}

//------------------------------------------------------------------------------------------------
//アクセス権の解放
//------------------------------------------------------------------------------------------------
void DirectInputDevice::Unacquire()
{
	//デバイスが存在するか
	if (!m_device.operator Microsoft::WRL::Details::BoolType())
	{
		::OutputDebugString(L"Error - デバイスが存在しません(DirectInputDevice::Unacquire)¥n");
		return;
	}

	HRESULT hr = m_device->Acquire();
	if(hr != DI_OK) //失敗
	{
		::OutputDebugString(L"Error - アクセス権解放に失敗しました(DirectInputDevice::Unacquire)¥n");
	}
	return;
}

//------------------------------------------------------------------------------------------------
//協調レベル設定
// 第１引数：DWORD 取得するデータのサイズ
// 第２引数：LPVOID 取得したを入れる変数のポインタ 
// 戻り値　：BOOL 成功：TRUE、失敗：FALSE
//------------------------------------------------------------------------------------------------
BOOL DirectInputDevice::GetDeviceState(
	DWORD sizeOfByte, 
	LPVOID lpData
)
{
	//デバイスが存在するか
	if (!m_device.operator Microsoft::WRL::Details::BoolType())
	{
		::OutputDebugString(L"Error - デバイスが存在しません(DirectInputDevice::GetDeviceState)¥n");
		return FALSE;
	}

	HRESULT hr = m_device->GetDeviceState(
		sizeOfByte, 
		lpData
	);
	if (hr != DI_OK) //失敗
	{
		::OutputDebugString(L"Error - 状態の取得に失敗しました(DirectInputDevice::GetDeviceState)¥n");
	}

	return 0;
}
