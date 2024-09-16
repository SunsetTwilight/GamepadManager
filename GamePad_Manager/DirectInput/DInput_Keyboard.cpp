#include "DInput_Keyboard.h"

//------------------------------------------------------------------------------------------------
//Constructor
//------------------------------------------------------------------------------------------------
DInput_Keyboard::DInput_Keyboard() : DirectInputDevice(
    GUID_SysKeyboard,
    &c_dfDIKeyboard
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
DInput_Keyboard::~DInput_Keyboard() 
{

}

void DInput_Keyboard::GetState()
{
    GetDeviceState(
        256,
        m_keyState
    );
}