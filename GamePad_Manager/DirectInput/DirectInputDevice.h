#ifndef _DIRECTINPUTDEVICE
#define _DIRECTINPUTDEVICE

#include "DirectInput.h"

//------------------------------------------------------------------------------------------------
//Window�̏�Ԃɂ��A�f�o�C�X���͂̐���������ݒ�
//------------------------------------------------------------------------------------------------
enum CL_GROUND : DWORD
{
    FORE_GROUND = DISCL_FOREGROUND, //�E�B���h�E��Active�̎��̂݁A�@���͂���B
    BACK_GROUND = DISCL_BACKGROUND  //�E�B���h�E����Active�̎��̂݁A���͂���B

    //��L�Q�͋����o���Ȃ��i�֐��@IDirectInputDevice8::SetCooperativeLevel()���s���A�G���[�j
};

//------------------------------------------------------------------------------------------------
//�r����L�ݒ�
//------------------------------------------------------------------------------------------------
enum CL_EXCLUSIVE : DWORD
{
    EXCLUSIVE       = DISCL_EXCLUSIVE,      //�r����L�F���̓f�o�C�X�̃A�N�Z�X����������Ȃ�����A���̃A�v���P�[�V�����Ő���ł��Ȃ�����B�i�g�p���Ӂj
    NO_EXCLUSIVE    = DISCL_NONEXCLUSIVE,   //��r����L���Ȃ��i�L�[�{�[�h�A�}�E�X�͂�������g�p�j

    //��L�Q�͋����o���Ȃ��i�֐��@IDirectInputDevice8::SetCooperativeLevel()���s���A�G���[�j
    
    /*
    �r����L����ƁA�A�N�Z�X�����������܂�DirectInput�ł�������ł��Ȃ��Ȃ�A
    �������x����ݒ肵���ȊO�̃E�B���h�E��API���g���Ă����͂��󂯎�邱�Ƃ��ł��Ȃ��Ȃ�܂��B
    �L�[�{�[�h��}�E�X��r����L����ƁA�����Ɋւ��郁�b�Z�[�W���������Ȃ��Ȃ�A
    ���b�Z�[�W�{�b�N�X��OK�{�^�����N���b�N�ł��Ȃ��Ȃ����肵�܂��B
    �ň��̏ꍇ��OS���I���ł��Ȃ��Ȃ�܂��B
    
    �قƂ�ǂ̓��̓f�o�C�X�͔�r�����[�h�ŏ\���ł����A
    �Q�[���p�b�h�Ȃǂ̃t�H�[�X�t�B�[�h�o�b�N�@�\�𐧌䂷��Ƃ��́A�r����L���[�h�ɐݒ肵�܂��B
    */
};

//------------------------------------------------------------------------------------------------
//���̓f�o�C�X�C���^�[�t�F�[�X��`
//------------------------------------------------------------------------------------------------
class DirectInputDevice
{
public:
    DirectInputDevice(
        const GUID guid,
        LPCDIDATAFORMAT lpDataFormat
    );
    virtual ~DirectInputDevice();

    void UpdateState();

protected:
    virtual void GetState() = 0;

    BOOL SetCooperativeLevel(
        CL_GROUND ground, 
        CL_EXCLUSIVE exclusive
    );

    BOOL GetDeviceState(
        DWORD sizeOfByte,
        LPVOID pData
    );

    void Acquire();
    void Unacquire();

protected:

private:
    DirectInput m_directInput;
    Microsoft::WRL::ComPtr<IDirectInputDevice8> m_device;

    LPCDIDATAFORMAT m_dataFormat;
};

#endif // !_DIRECTINPUTDEVICE

