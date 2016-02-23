//
//  TUTKPlug.cpp
//  RDTServer
//
//  Created by jakey on 2015/6/4.
//  Copyright (c) 2015年 jakey. All rights reserved.
//

#include "TUTKPlugDevice.hpp"
#include "TUTKPlugEnum.hpp"
#include "AccessoryTypeEnum.hpp"
#include "BinraryRDTServerCommand.hpp"

#pragma mark - Normal Method

void TUTKPlugDevice::generatorDataInfoList(Command* pCommand)
{
    BinraryRDTServerCommand *pBinraryRDTServerCommand = (BinraryRDTServerCommand*) pCommand;
    FunctionInfo* pFunctionInfo = NULL;
    
    pFunctionInfo = pBinraryRDTServerCommand->createFunctionInfo(Accessory_Type_Plug);
    
    // 查詢開關(on, off)
    pBinraryRDTServerCommand->addFunctionStatus(pFunctionInfo, TUTKPlugFunctionCode_QuerySwitch, 1);
    
    // 瓦數詢問 (整數， 小數)
    pBinraryRDTServerCommand->addFunctionStatus(pFunctionInfo, TUTKPlugFunctionCode_QueryWattage, 52, 4996);
    
    // 安培數詢問 (整數， 小數)
    pBinraryRDTServerCommand->addFunctionStatus(pFunctionInfo, TUTKPlugFunctionCode_QueryAmpere, 0, 2587);
    
    // 伏特數詢問 (整數， 小數)
    pBinraryRDTServerCommand->addFunctionStatus(pFunctionInfo, TUTKPlugFunctionCode_QueryVolt, 220, 235);
    
    // 裝置名稱詢問 (N)
    pBinraryRDTServerCommand->addFunctionStatus(pFunctionInfo, TUTKCommonFunctionCode_QueryDeviceName, u'插', u'座');
}

#pragma mark - CommandEvent

unsigned short TUTKPlugDevice::onCommandGetProductCode()
{
    return 0x0F2;
}

const char* TUTKPlugDevice::onCommandGetProductName()
{
    return "TUTKPlugDevice";
}

