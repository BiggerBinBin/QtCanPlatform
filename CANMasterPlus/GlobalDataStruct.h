#ifndef GLOBALDATASTRUCT_H
#define GLOBALDATASTRUCT_H
struct DeviceParameter
{
    short devType;
    short devChannel;
    short baudRate;
    short devModel;
    std::vector<int>v;
    DeviceParameter()
    {
        devType = 0;
        devChannel = 0;
        baudRate = 0;
        devModel = 0;
        v.clear();
    }
};

#endif // GLOBALDATASTRUCT_H
