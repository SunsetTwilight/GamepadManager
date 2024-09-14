#ifndef _DIRECT_INPUT
#define _DIRECT_INPUT

#define DIRECTINPUT_VERSION     0x0800	// DirectInputのバージョン指定
#include <dinput.h>

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




#endif // !_DIRECT_INPUT

