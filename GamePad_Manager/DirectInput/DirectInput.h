#ifndef _DIRECT_INPUT
#define _DIRECT_INPUT

#define DIRECTINPUT_VERSION 0x0800	// DirectInput�̃o�[�W�����w��
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


// DirectInputDevice8�I�u�W�F�N�g�̐���
LPDIRECTINPUTDEVICE8 pDIDGamePad;

// �Q�[���p�b�h�񋓃R�[���o�b�N�֐�
BOOL CALLBACK DIEnumGamePadProc(LPDIDEVICEINSTANCE pDIDInst, LPVOID pRef) 
{ 
    // �f�o�C�X�I�u�W�F�N�g����
    if(lpDI->CreateDevice(pDIDInst->guidInstance, &pDIDGamePad, NULL) != DI_OK)
        // �������s
        return DIENUM_CONTINUE;
}

void CreateVabrationEffect(
    LPDIRECTINPUTDEVICE8 lpDInputDevice
)
{

    // �t�H�[�X�t�B�[�h�o�b�N�Ή��f�o�C�X�̗�
    //LPDIRECTINPUT8 lpDInput8; // DirectInput8�I�u�W�F�N�g(�������ς݂Ƃ���) 
    lpDI->EnumDevices(
        DI8DEVCLASS_GAMECTRL,
        (LPDIENUMDEVICESCALLBACK)DIEnumGamePadProc,
        NULL,
        DIEDFL_FORCEFEEDBACK | DIEDFL_ATTACHEDONLY
    );

    // �G�t�F�N�g�����ݒ�
    DIPERIODIC diprd;
    ZeroMemory(&diprd, sizeof(diprd));
    diprd.dwMagnitude = 10000; // �G�t�F�N�g�̋���(0�`10,000)
    diprd.lOffset = 0;
    diprd.dwPhase = 0;
    diprd.dwPeriod = (DWORD)(0.5 * DI_SECONDS); // �G�t�F�N�g�̊Ԋu

    // �U���G�t�F�N�g�ݒ�
    DWORD dwAxes[] = { DIJOFS_X, DIJOFS_Y };    // �G�t�F�N�g��
    LONG lDirection[] = { 0, 0 };                 // �G�t�F�N�g����

    DIEFFECT diEffect;
    ZeroMemory(&diEffect, sizeof(diEffect));
    diEffect.dwSize = sizeof(diEffect);
    diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
    diEffect.dwDuration = (DWORD)(0.5 * DI_SECONDS);        // �G�t�F�N�g�p������
    diEffect.dwSamplePeriod = 0;
    diEffect.dwGain = DI_FFNOMINALMAX;
    diEffect.dwTriggerButton = DIEB_NOTRIGGER;
    diEffect.dwTriggerRepeatInterval = 0;
    diEffect.cAxes = sizeof(dwAxes) / sizeof(dwAxes[0]);
    diEffect.rgdwAxes = dwAxes;
    diEffect.rglDirection = lDirection;
    diEffect.lpEnvelope = NULL;                     // �G���x���[�v�\����
    diEffect.cbTypeSpecificParams = sizeof(diprd);  // �G�t�F�N�g�����\���̂̃T�C�Y
    diEffect.lpvTypeSpecificParams = &diprd;        // �G�t�F�N�g�����\����

    // �G�t�F�N�g����(lpDIDGamePad�̓t�H�[�X�t�B�[�h�o�b�N�Ή��̏������ς݃f�o�C�X�I�u�W�F�N�g)
    LPDIEFFECT lpDIEffect = NULL; // �G�t�F�N�g�I�u�W�F�N�g
    lpDInputDevice->CreateEffect(GUID_Square, &diEffect, &lpDIEffect, NULL);

}


#endif // !_DIRECT_INPUT

