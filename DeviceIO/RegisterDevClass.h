#ifndef REGISTERDEVCLASS_H
#define REGISTERDEVCLASS_H
#include "ReflectClass.h"
#include "peakCAN.h"
#include "CANalyst.h"
#include "peakLIN.h"
#include "zlgCAN.h"

class RegisterDevReflect
{
public:
    static void RegisterReflect()
    {
        CReflectClass<DeviceIO>::registerClass<peakCAN>();
        CReflectClass<DeviceIO>::registerClass<peakLIN>();
        CReflectClass<DeviceIO>::registerClass<CANalyst>();
        CReflectClass<DeviceIO>::registerClass<zlgCAN>();
        //CReflectClass<DeviceIO>::registerClass<tsMasterLIN>();
    }
};

#endif // REGISTERDEVCLASS_H
