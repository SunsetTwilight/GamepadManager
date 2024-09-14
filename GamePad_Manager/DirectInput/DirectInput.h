#ifndef _DIRECT_INPUT
#define _DIRECT_INPUT

#define DIRECTINPUT_VERSION     0x0800	// DirectInput�̃o�[�W�����w��
#include <dinput.h>

LPDIRECTINPUT8 lpDI = NULL;        // DirectInput�I�u�W�F�N�g

BOOL CreateDirectInput(HINSTANCE hInstance)
{
    // IDirectInput8�̍쐬
    HRESULT ret = DirectInput8Create(
        hInstance,          
        DIRECTINPUT_VERSION, 
        IID_IDirectInput8, 
        (LPVOID*)&lpDI, 
        NULL
    );

    if (FAILED(ret)) {
        // �쐬�Ɏ��s
        //DEBUG("DirectInput8�̍쐬�Ɏ��s\n");
        return FALSE;
    }

    return TRUE;
}

// IDirectInputDevice8�̎擾
BOOL DInput_GetInterface()
{
    LPDIRECTINPUTDEVICE8 lpJoystick;

    HRESULT hr = lpDI->CreateDevice(GUID_Joystick, &lpJoystick, NULL);
    if (FAILED(hr)) {
        //DEBUG("�L�[�{�[�h�f�o�C�X�̍쐬�Ɏ��s\n");
        lpDI->Release();
    
        return FALSE;
    }

    return TRUE;
}

// ���̓f�[�^�`���̃Z�b�g
BOOL DInput_SetInputFormat(LPDIRECTINPUTDEVICE8 lpDInputDevice)
{
    HRESULT hr = lpDInputDevice->SetDataFormat(&c_dfDIJoystick2);

    if (FAILED(hr)) {
        //DEBUG("���̓f�[�^�`���̃Z�b�g���s\n");
        lpDInputDevice->Release();
        lpDI->Release();
        return FALSE;
    }

    return TRUE;
}


// �r������̃Z�b�g
BOOL DInput_SetCooperativeLevel(
    LPDIRECTINPUTDEVICE8 lpDInputDevice,    //�r������̃Z�b�g������f�o�C�X
    HWND hWnd                               //�t�H�[�J�X����E�B���h�E�̃n���h��
)
{
    HRESULT hr = lpDInputDevice->SetCooperativeLevel(
        hWnd,
        DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY
    );
    if (FAILED(hr)) {
        //DEBUG("�r������̃Z�b�g���s\n");
        lpDInputDevice->Release();
        lpDI->Release();

        return FALSE;
    }

    return TRUE;

    
}




#endif // !_DIRECT_INPUT

