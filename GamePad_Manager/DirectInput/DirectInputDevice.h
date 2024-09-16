#ifndef _DIRECTINPUTDEVICE
#define _DIRECTINPUTDEVICE

#include "DirectInput.h"

//------------------------------------------------------------------------------------------------
//Windowの状態により、デバイス入力の制限をする設定
//------------------------------------------------------------------------------------------------
enum CL_GROUND : DWORD
{
    FORE_GROUND = DISCL_FOREGROUND, //ウィンドウがActiveの時のみ、　入力する。
    BACK_GROUND = DISCL_BACKGROUND  //ウィンドウが非Activeの時のみ、入力する。

    //上記２つは共存出来ない（関数　IDirectInputDevice8::SetCooperativeLevel()実行時、エラー）
};

//------------------------------------------------------------------------------------------------
//排他占有設定
//------------------------------------------------------------------------------------------------
enum CL_EXCLUSIVE : DWORD
{
    EXCLUSIVE       = DISCL_EXCLUSIVE,      //排他占有：入力デバイスのアクセス権を放棄しない限り、他のアプリケーションで制御できなくする。（使用注意）
    NO_EXCLUSIVE    = DISCL_NONEXCLUSIVE,   //非排他占有しない（キーボード、マウスはこちらを使用）

    //上記２つは共存出来ない（関数　IDirectInputDevice8::SetCooperativeLevel()実行時、エラー）
    
    /*
    排他占有すると、アクセス権を解放するまでDirectInputでしか制御できなくなり、
    協調レベルを設定した以外のウィンドウはAPIを使っても入力を受け取ることができなくなります。
    キーボードやマウスを排他占有すると、それらに関するメッセージが発生しなくなり、
    メッセージボックスのOKボタンをクリックできなくなったりします。
    最悪の場合はOSを終了できなくなります。
    
    ほとんどの入力デバイスは非排他モードで十分ですが、
    ゲームパッドなどのフォースフィードバック機能を制御するときは、排他占有モードに設定します。
    */
};

//------------------------------------------------------------------------------------------------
//入力デバイスインターフェース定義
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

