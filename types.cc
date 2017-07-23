#include "types.h"

void ControlData::operator= (const ControlData &other)
{
    motorA = other.motorA;
    motorB = other.motorB;
    motorC = other.motorC;
    motorD = other.motorD;
    servo1 = other.servo1;
    servo2 = other.servo2;
    servo3 = other.servo3;
    servo4 = other.servo4;
    servo5 = other.servo5;
    servo6 = other.servo6;
    servo7 = other.servo7;
    servo8 = other.servo8;
    extLed = other.extLed;
    motorPower = other.motorPower;
    hostPower = other.hostPower;
}

bool ControlData::operator== (const ControlData &other)
{
    return (motorA == other.motorA &&
            motorB == other.motorB &&
            motorC == other.motorC &&
            motorD == other.motorD &&
            servo1 == other.servo1 &&
            servo2 == other.servo2 &&
            servo3 == other.servo3 &&
            servo4 == other.servo4 &&
            servo5 == other.servo5 &&
            servo6 == other.servo6 &&
            servo7 == other.servo7 &&
            servo8 == other.servo8 &&
            extLed == other.extLed &&
            motorPower == other.motorPower &&
            hostPower == other.hostPower );
}

bool ControlData::operator!= (const ControlData &other)
{
    return !(*this==other);
}

bool ControlData::diffTypePosition(const ControlData *other)
{
    return (motorA == other->motorA &&
            motorB == other->motorB &&
            motorC == other->motorC &&
            motorD == other->motorD &&
            servo1 == other->servo1 &&
            servo2 == other->servo2 &&
            servo3 == other->servo3 &&
            servo4 == other->servo4 &&
            servo5 == other->servo5 &&
            servo6 == other->servo6 &&
            servo7 == other->servo7 &&
            servo8 == other->servo8 &&
            extLed == other->extLed );
}

bool ControlData::diffTypePower(const ControlData *other)
{
    return ( motorPower == other->motorPower &&
             hostPower == other->hostPower );
}

void ControlData::Init(void)
{
    motorA = SAFE_MOTOR;
    motorB = SAFE_MOTOR;
    motorC = SAFE_MOTOR;
    motorD = SAFE_MOTOR;
    servo1 = SAFE_SERVO;
    servo2 = SAFE_SERVO;
    servo3 = SAFE_SERVO;
    servo4 = SAFE_SERVO;
    servo5 = SAFE_SERVO;
    servo6 = SAFE_SERVO;
    servo7 = SAFE_SERVO;
    servo8 = SAFE_SERVO;
    extLed = SAFE_LED;
    motorPower = false;
    hostPower = true;
}

