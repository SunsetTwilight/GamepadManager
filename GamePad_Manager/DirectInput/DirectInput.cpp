#include "DirectInput.h"

Microsoft::WRL::ComPtr<IDirectInput8> DirectInput::m_directInput = NULL;
HWND DirectInput::m_hWnd =  NULL;

//------------------------------------------------------------------------------------------------
//Constructor
//------------------------------------------------------------------------------------------------
DirectInput::DirectInput()
{
	
}

//------------------------------------------------------------------------------------------------
//Destructor
//------------------------------------------------------------------------------------------------
DirectInput::~DirectInput()
{

}

//------------------------------------------------------------------------------------------------
//使用中のウィンドウハンドルを返す
//------------------------------------------------------------------------------------------------
HWND DirectInput::GetHWND()
{
	return m_hWnd == nullptr ? nullptr : m_hWnd;
}

//------------------------------------------------------------------------------------------------
//DirectInputDeviceの生成
// 引数１　：IDirectInputDevice8** 生成したデバイスのアドレス
// 引数２　：生成したいデバイスのGUID
// 戻り値　：BOOL 成功：TRUE、失敗：FALSE
//------------------------------------------------------------------------------------------------
BOOL DirectInput::CreateDirectInputDevice(
	IDirectInputDevice8** dInputDevice,
	const GUID guid
)
{
	if(!m_directInput.operator Microsoft::WRL::Details::BoolType()) //DirectInputが初期化されているか
	{ 
		::OutputDebugString(L"Error - DirectInput8オブジェクトが初期化されていません(CDInput8_CreateKeyboard)¥n");
		return FALSE;
	}
	HRESULT hr = m_directInput->CreateDevice(
		guid,
		dInputDevice, 
		NULL
	);
	if (hr != DI_OK) { //DI_OK(S_OK)でないなら（失敗）
		::OutputDebugString(L"Error - デバイス生成失敗(CDInput8_CreateKeyboard)¥n");
		return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------------------------------
//DirectInputの初期化
// 引数　：HWND ウィンドウハンドル
// 戻り値：BOOL 成功：TRUE、失敗：FALSE
//------------------------------------------------------------------------------------------------
BOOL DirectInput::Initialize(HWND hWnd)
{
	if (m_directInput == NULL) 
	{
	 	HRESULT hr = DirectInput8Create(
			(HINSTANCE)::GetWindowLongPtr(hWnd, GWL_HINSTANCE), //HINSTANCE取得
			DIRECTINPUT_VERSION,								//DirectInput の バージョン
			IID_IDirectInput8, 
			(LPVOID*)m_directInput.GetAddressOf(),
			NULL
		);
		if (hr != DI_OK) //DI_OK(S_OK)でないなら（失敗）
		{ 
			m_hWnd = NULL;
			::OutputDebugString(L"Error - DirectInput8オブジェクト初期化失敗(CDInput8_Init)¥n");
			
			return FALSE; //エラー
		}
		else //成功
		{
			m_hWnd = hWnd; //ウィンドウハンドル使用
		}
	}

	return TRUE; //正常終了
}

//------------------------------------------------------------------------------------------------
//DirectInputの解放
//------------------------------------------------------------------------------------------------
void DirectInput::Release()
{
	m_directInput.Reset();
	m_hWnd = NULL;
}

