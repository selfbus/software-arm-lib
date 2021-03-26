/*
 *  apci.h - Telegram application types.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */
#ifndef sblib_apci_h
#define sblib_apci_h

enum
{
    // Application commands (see KNX 3/3/7 p.8 Application Layer control field)

    APCI_GROUP_MASK = 0x3c0,

    APCI_GROUP_VALUE_READ_PDU                   = 0x000, //!> A_GroupValue_Read-PDU, multicast
    APCI_GROUP_VALUE_RESPONSE_PDU               = 0x040, //!> A_GroupValue_Response-PDU, multicast
    APCI_GROUP_VALUE_WRITE_PDU                  = 0x080, //!> A_GroupValue_Write-PDU, multicast

    APCI_INDIVIDUAL_ADDRESS_WRITE_PDU           = 0x0c0, //!> A_IndividualAddress_Write-PDU, broadcast
    APCI_INDIVIDUAL_ADDRESS_READ_PDU            = 0x100, //!> A_IndividualAddress_Read-PDU, broadcast
    APCI_INDIVIDUAL_ADDRESS_RESPONSE_PDU        = 0x140, //!> A_IndividualAddress_Response-PDU, broadcast

    APCI_ADC_READ_PDU                           = 0x180, //!> A_ADC_Read-PDU, connection-oriented
    APCI_ADC_RESPONSE_PDU                       = 0x1C0, //!> A_ADC_Response-PDU, connection-oriented

    APCI_SYSTEM_NETWORK_PARAMETER_READ_PDU      = 0x1C8, //!> A_SystemNetworkParameter_Read-PDU, system broadcast
    APCI_SYSTEM_NETWORK_PARAMETER_RESPONSE_PDU  = 0x1C9, //!> A_SystemNetworkParameter_Response-PDU, system broadcast
    APCI_SYSTEM_NETWORK_PARAMETER_WRITE_PDU     = 0x1CA, //!> A_SystemNetworkParameter_Write-PDU, system broadcast
    APCI_SYSTEM_NETWORK_PARAMETER_RESERVED_PDU  = 0x1CB, //!> planned for future system broadcast service, system broadcast

    APCI_MEMORY_READ_PDU                        = 0x200, //!> A_Memory_Read-PDU, connection-oriented
    APCI_MEMORY_RESPONSE_PDU                    = 0x240, //!> A_Memory_Response-PDU, connection-oriented
    APCI_MEMORY_WRITE_PDU                       = 0x280, //!> A_Memory_Write-PDU, connection-oriented

    APCI_USER_MEMORY_READ_PDU                   = 0x2C0, //!> A_UserMemory_Read-PDU, connection-oriented
    APCI_USER_MEMORY_RESPONSE_PDU               = 0x2C1, //!> A_UserMemory_Response-PDU, connection-oriented
    APCI_USER_MEMORY_WRITE_PDU                  = 0x2C2, //!> A_UserMemory_Write-PDU, connection-oriented

    APCI_USER_MEMORY_BIT_WRITE_PDU              = 0x2C4, //!> deprecated, A_UserMemoryBit_Write-PDU, connection-oriented

    APCI_USER_MANUFACTURER_INFO_READ_PDU        = 0x2C5, //!> A_UserManufacturerInfo_Read-PDU, connection-oriented
    APCI_USER_MANUFACTURER_INFO_RESPONSE_PDU    = 0x2C6, //!> A_UserManufacturerInfo_Response-PDU, connection-oriented

    APCI_FUNCTIONPROPERTY_COMMAND_PDU           = 0x2C7, //!> A_FunctionPropertyCommand-PDU, connectionless and connection-oriented
    APCI_FUNCTIONPROPERTY_STATE_READ_PDU        = 0x2C8, //!> A_FunctionPropertyState_Read-PDU, connectionless and connection-oriented
    APCI_FUNCTIONPROPERTY_STATE_RESPONSE_PDU    = 0x2C9, //!> A_FunctionPropertyState_Response-PDU, connectionless and connection-oriented

    // 0x2CA -> 0x2F7 reserved usermsg
    // 0x2F8 -> 0x2FE manufacturer specific area for usermsg

    APCI_DEVICEDESCRIPTOR_READ_PDU              = 0x300, //!> A_DeviceDescriptor_Read-PDU, connection-oriented
    APCI_DEVICEDESCRIPTOR_RESPONSE_PDU          = 0x340, //!> A_DeviceDescriptor_Response-PDU, connection-oriented

    APCI_RESTART_PDU                            = 0x380, //!> A_Restart-PDU, connection-oriented
	APCI_RESTART_TYPE1_PDU                      = 0x381, //!>

	// 0x3C0 -> 0x3D0 coupler specific?

    APCI_AUTHORIZE_REQUEST_PDU                  = 0x3d1, //!> A_Authorize_Request-PDU, connection-oriented
    APCI_AUTHORIZE_RESPONSE_PDU                 = 0x3d2, //!> A_Authorize_Response-PDU, connection-oriented

    APCI_KEY_WRITE_PDU                          = 0x3D3, //!> A_Key_Write-PDU, connection-oriented
    APCI_KEY_RESPONSE                           = 0x3D4, //!> A_Key_Response-PDU, connection-oriented

    APCI_PROPERTY_VALUE_READ_PDU                = 0x3d5, //!> A_PropertyValue_Read-PDU, connectionless and connection-oriented
    APCI_PROPERTY_VALUE_RESPONSE_PDU            = 0x3d6, //!> A_PropertyValue_Response-PDU, connectionless and connection-oriented
    APCI_PROPERTY_VALUE_WRITE_PDU               = 0x3d7, //!> A_PropertyValue_Write-PDU, connectionless and connection-oriented

    APCI_PROPERTY_DESCRIPTION_READ_PDU          = 0x3d8, //!> A_PropertyDescription_Read-PDU, connectionless and connection-oriented
    APCI_PROPERTY_DESCRIPTION_RESPONSE_PDU      = 0x3d9, //!> A_PropertyDescription_Response-PDU, connectionless and connection-oriented

    APCI_NETWORKPARAMETER_READ_PDU              = 0x3DA, //!> A_NetworkParameter_Read-PDU, broadcast
    APCI_NETWORKPARAMETER_RESPONSE_PDU          = 0x3DB, //!> A_NetworkParameter_Response-PDU, broadcast and connectionless

    APCI_INDIVIDUALADDRESS_SERIALNUMBER_READ_PDU        = 0x3DC, //!> A_IndividualAddressSerialNumber_Read-PDU, broadcast
    APCI_INDIVIDUALADDRESS_SERIALNUMBER_RESPONSE_PDU    = 0x3DD, //!> A_IndividualAddressSerialNumber_Response-PDU, broadcast
    APCI_INDIVIDUALADDRESS_SERIALNUMBER_WRITE_PDU       = 0x3DE, //!> A_IndividualAddressSerialNumber_Write-PDU, broadcast

    // Transport commands

    T_CONNECT_PDU = 0x80,
    T_DISCONNECT_PDU = 0x81,
    T_ACK_PDU = 0xc2,
    T_NACK_PDU = 0xc3,

    // TPCI (TPDU) commands

    T_GROUP_PDU = 0x00
};


#endif /*sblib_apci_h*/
