#ifndef _DINPUT_MOUSE
#define _DINPUT_MOUSE

#include "DirectInputDevice.h"

class DInput_Mouse : public DirectInputDevice
{
public:
    DInput_Mouse();
    virtual ~DInput_Mouse();

protected:
    void GetState() override;

public:

private:
    DIMOUSESTATE2 m_mouseState;
};

#endif // !_DINPUT_MOUSE
