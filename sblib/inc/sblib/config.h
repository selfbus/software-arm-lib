/**************************************************************************//**
 * @file     config.h
 * @brief    set pre-processor symbols which apply to all sblib build-configs
 * @date     2021/04/02
 *
 * @note
 *
 ******************************************************************************/

#ifndef SBLIB_CONFIG_H_
#define SBLIB_CONFIG_H_

#define DEBUG_BUS           //!>
#define DEBUG_BUS_BITLEVEL  //!>
#define DEBUG_COM_OBJ       //!>
#define DUMP_TELEGRAMS      //!>
#define DUMP_TELEGRAMS_BUS  //!>

#define DEBUG_OUT8_BI       //!>

#define DUMP_MEM_OPS    //!> dump APCI_MEMORY_WRITE_PDU and APCI_MEMORY_READ_PDU over serial interface
#define DEBUG_SERIAL    //!> dump KNX-serialnumber over serial interface
#define DUMP_PROPERTIES //!> dump interface object and property handling over serial interface


#endif /* SBLIB_CONFIG_H_ */
