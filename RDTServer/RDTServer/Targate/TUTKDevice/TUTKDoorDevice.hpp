//
//  TUTKDoorDevice.h
//  RDTServer
//
//  Created by jakey on 2015/6/24.
//  Copyright (c) 2015年 jakey. All rights reserved.
//

#ifndef __RDTServer__TUTKDoorDevice__
#define __RDTServer__TUTKDoorDevice__

#include <stdio.h>
#include "Device.hpp"

class TUTKDoorDevice : public Device
{
public:
    static void generatorDataInfoList(Command* pCommand);
    
#pragma mark - Device
    virtual void constructorFinish() { generatorDataInfoList(m_pCommand); }
    
protected:
    virtual unsigned short onCommandGetProductCode();
    virtual const char* onCommandGetProductName();
    
private:
    bool m_bOnOff;
};

#endif /* defined(__RDTServer__TUTKDoorDevice__) */
