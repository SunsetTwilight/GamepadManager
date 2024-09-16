#ifndef _DINPUT_KEYBOARD
#define _DINPUT_KEYBOARD

#include "DirectInputDevice.h"

class DInput_Keyboard : public DirectInputDevice
{
public:
    DInput_Keyboard();
    virtual ~DInput_Keyboard();

protected:
    void GetState() override;

public:

private:
    BYTE m_keyState[256];
};

#endif // !_DINPUT_KEYBOARD

