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
        CL_GROUND::FORE_GROUND,     //Active時、入力を受け取る
        CL_EXCLUSIVE::NO_EXCLUSIVE  //排他占有しない
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