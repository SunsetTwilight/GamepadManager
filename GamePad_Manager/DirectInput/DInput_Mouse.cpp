#include "DInput_Mouse.h"

//------------------------------------------------------------------------------------------------
//Constructor
//------------------------------------------------------------------------------------------------
DInput_Mouse::DInput_Mouse() : DirectInputDevice(
    GUID_SysMouse,
    &c_dfDIMouse2
)
{
    SetCooperativeLevel(
        CL_GROUND::FORE_GROUND,     //Active���A���͂��󂯎��
        CL_EXCLUSIVE::NO_EXCLUSIVE  //�r����L���Ȃ�
    );
}

//------------------------------------------------------------------------------------------------
//Destructor
//------------------------------------------------------------------------------------------------
DInput_Mouse::~DInput_Mouse()
{

}

void DInput_Mouse::GetState()
{
    GetDeviceState(
        sizeof(DIMOUSESTATE2),
        &m_mouseState
    );
}