#ifndef _DIRECT_INPUT
#define _DIRECT_INPUT

#define DIRECTINPUT_VERSION 0x0800	// DirectInputのバージョン指定
#include <dinput.h>
#include <wrl.h>

#pragma comment(lib, "dinput8.lib") 

enum DirectInputDataFormat
{
    Keyboard,
    Mouse,
    Joystick,
};


class DirectInput
{
public:
    DirectInput();
    ~DirectInput();

    BOOL CreateDirectInputDevice(
        IDirectInputDevice8** dInputDevice,
        const GUID guid
    );

    HWND GetHWND();

private:
    BOOL Initialize(HWND hWnd);
    void Release();

public:


private:

    static Microsoft::WRL::ComPtr<IDirectInput8> m_directInput;
    static HWND m_hWnd;
};



LPDIRECTINPUT8 lpDI = NULL;        // DirectInputオブジェクト

BOOL CreateDirectInput(HINSTANCE hInstance)
{
    // IDirectInput8の作成
    HRESULT ret = DirectInput8Create(
        hInstance,          
        DIRECTINPUT_VERSION, 
        IID_IDirectInput8, 
        (LPVOID*)&lpDI, 
        NULL
    );

    if (FAILED(ret)) {
        // 作成に失敗
        //DEBUG("DirectInput8の作成に失敗\n");
        return FALSE;
    }

    return TRUE;
}

// IDirectInputDevice8の取得
BOOL DInput_GetInterface()
{
    LPDIRECTINPUTDEVICE8 lpJoystick;

    HRESULT hr = lpDI->CreateDevice(GUID_Joystick, &lpJoystick, NULL);
    if (FAILED(hr)) {
        //DEBUG("キーボードデバイスの作成に失敗\n");
        lpDI->Release();
    
        return FALSE;
    }

    return TRUE;
}

// 入力データ形式のセット
BOOL DInput_SetInputFormat(LPDIRECTINPUTDEVICE8 lpDInputDevice)
{
    HRESULT hr = lpDInputDevice->SetDataFormat(&c_dfDIJoystick2);

    if (FAILED(hr)) {
        //DEBUG("入力データ形式のセット失敗\n");
        lpDInputDevice->Release();
        lpDI->Release();
        return FALSE;
    }

    return TRUE;
}


// 排他制御のセット
BOOL DInput_SetCooperativeLevel(
    LPDIRECTINPUTDEVICE8 lpDInputDevice,    //排他制御のセットをするデバイス
    HWND hWnd                               //フォーカスするウィンドウのハンドル
)
{
    HRESULT hr = lpDInputDevice->SetCooperativeLevel(
        hWnd,
        DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
    );
    if (FAILED(hr)) {
        //DEBUG("排他制御のセット失敗\n");
        lpDInputDevice->Release();
        lpDI->Release();

        return FALSE;
    }

    return TRUE;

    
}


// DirectInputDevice8オブジェクトの生成
LPDIRECTINPUTDEVICE8 pDIDGamePad;

// ゲームパッド列挙コールバック関数
BOOL CALLBACK DIEnumGamePadProc(LPDIDEVICEINSTANCE pDIDInst, LPVOID pRef) 
{ 
    // デバイスオブジェクト生成
    if(lpDI->CreateDevice(pDIDInst->guidInstance, &pDIDGamePad, NULL) != DI_OK)
        // 生成失敗
        return DIENUM_CONTINUE;
}

void CreateVabrationEffect(
    LPDIRECTINPUTDEVICE8 lpDInputDevice
)
{

    // フォースフィードバック対応デバイスの列挙
    //LPDIRECTINPUT8 lpDInput8; // DirectInput8オブジェクト(初期化済みとする) 
    lpDI->EnumDevices(
        DI8DEVCLASS_GAMECTRL,
        (LPDIENUMDEVICESCALLBACK)DIEnumGamePadProc,
        NULL,
        DIEDFL_FORCEFEEDBACK | DIEDFL_ATTACHEDONLY
    );

    // エフェクト周期設定
    DIPERIODIC diprd;
    ZeroMemory(&diprd, sizeof(diprd));
    diprd.dwMagnitude = 10000; // エフェクトの強さ(0～10,000)
    diprd.lOffset = 0;
    diprd.dwPhase = 0;
    diprd.dwPeriod = (DWORD)(0.5 * DI_SECONDS); // エフェクトの間隔

    // 振動エフェクト設定
    DWORD dwAxes[] = { DIJOFS_X, DIJOFS_Y };    // エフェクト軸
    LONG lDirection[] = { 0, 0 };                 // エフェクト方向

    DIEFFECT diEffect;
    ZeroMemory(&diEffect, sizeof(diEffect));
    diEffect.dwSize = sizeof(diEffect);
    diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
    diEffect.dwDuration = (DWORD)(0.5 * DI_SECONDS);        // エフェクト継続時間
    diEffect.dwSamplePeriod = 0;
    diEffect.dwGain = DI_FFNOMINALMAX;
    diEffect.dwTriggerButton = DIEB_NOTRIGGER;
    diEffect.dwTriggerRepeatInterval = 0;
    diEffect.cAxes = sizeof(dwAxes) / sizeof(dwAxes[0]);
    diEffect.rgdwAxes = dwAxes;
    diEffect.rglDirection = lDirection;
    diEffect.lpEnvelope = NULL;                     // エンベロープ構造体
    diEffect.cbTypeSpecificParams = sizeof(diprd);  // エフェクト周期構造体のサイズ
    diEffect.lpvTypeSpecificParams = &diprd;        // エフェクト周期構造体

    // エフェクト生成(lpDIDGamePadはフォースフィードバック対応の初期化済みデバイスオブジェクト)
    LPDIEFFECT lpDIEffect = NULL; // エフェクトオブジェクト
    lpDInputDevice->CreateEffect(GUID_Square, &diEffect, &lpDIEffect, NULL);

}


#endif // !_DIRECT_INPUT

