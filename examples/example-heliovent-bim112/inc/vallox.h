/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_BIM112_HELIOVENT_1 BCU BIM112 Maskversion 0x0705 Example
 * @ingroup SBLIB_EXAMPLES
 * @brief
 * @details
 *
 * @{
 *
 * @file   vallox.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */


#ifndef KNXPRODEDITORHEADER_VALLOX_H
#define KNXPRODEDITORHEADER_VALLOX_H

/**
 * Header File for sblib (Selfbus ARM KNX Library)
 * generated with KNXprodEditor 0.44
 *
 * Format:
 * MANUFACTURER = knxMaster->Manufacturer->KnxManufacturerId = sblib manufacturer [dez]
 * DEVICETYPE = ApplicationProgram->ApplicationNumber = sblib deviceType [dez]
 * APPVERSION = ApplicationProgram->ApplicationVersion = sblib version [dez]
 *
 * Parameter: EE_...
 * einfacher Parameter: EE_PARAMETER_[Name]   [Adresse in hex] //Addr:[Adresse in hex], Size:[Größe in hex], Beschreibung in Sprache 1
 * Union Parameter: EE_UNIONPARAMETER_[Adresse in hex]   [Adresse in hex] //Addr:[Adresse in hex], Size:[Größe in hex], Beschreibung Parameter 1 in Sprache 1, Beschreibung Parameter 2 in Sprache 1, ...
 *
 * Kommunikations Objekte: COMOBJ...
 * COMOBJ_[Beschreibung]_[Funktionstext]   [Kommunikations-Objekt Nummer] //Com-Objekt Nummer: [Nummer] ,Beschreibung: [in Sprache 1], Funktion [in Sprache 1]
 *
 *
 *
 * Geräteinformationen:
 * Device: MV KNX-Bus-Unit
 * ApplicationProgram: M-0115_A-0002-10-4506
 *
 */

#define MANUFACTURER 0x0115 //!< Manufacturer ID
#define DEVICETYPE 0x002    //!< Device Type
#define APPVERSION 0x10     //!< Application Version

const byte hardwareVersion[6] = { 0, 0, 0, 0, 0, 0 }; // The hardware identification number hardwareVersion

#define EE_PARAMETER_0014_STOSSLUEFTUNG_INTERVALLZEIT_MIN     0x0014 //!< relative Addr: 0x0014, Size: 0x0010, Stoßlüftung - Intervallzeit (min)
#define EE_PARAMETER_0016_KAMINFUNKTION_INTERVALLZEIT_MIN     0x0016 //!< relative Addr: 0x0016, Size: 0x0010, Kaminfunktion - Intervallzeit (min)
#define EE_PARAMETER_0018_DELTA_TEMPERATUR     0x0018 //!< relative Addr: 0x0018, Size: 0x0008, Delta Temperatur
#define EE_PARAMETER_0019_DELTA_RELATIVE_FEUCHTE     0x0019 //!< relative Addr: 0x0019, Size: 0x0008, Delta relative Feuchte
#define EE_PARAMETER_001A_DELTA_CO2_KONZENTRATION     0x001A //!< relative Addr: 0x001A, Size: 0x0008, Delta CO2 Konzentration
#define EE_PARAMETER_001B_DELTA_VOC_KONZENTRATION     0x001B //!< relative Addr: 0x001B, Size: 0x0008,     Delta VOC Konzentration
#define EE_PARAMETER_001C_ZYKLISCHE_UEBERTRAGUNG     0x001C //!< relative Addr: 0x001C, Size: 0x0008, Zyklische Übertragung
#define EE_PARAMETER_001D_ZYKLUSZEIT     0x001D //!< relative Addr: 0x001D, Size: 0x0008,     Zykluszeit
#define EE_PARAMETER_001E_WARTEZEIT_NACH_BUSSPANNUNGSWIEDERKEHR     0x001E //!< relative Addr: 0x001E, Size: 0x0008, Wartezeit nach Busspannungswiederkehr
#define EE_PARAMETER_001F_WARTEZEIT     0x001F //!< relative Addr: 0x001F, Size: 0x0008,     Wartezeit


#define COMOBJ_1_GO_BASE_1_GO_BASE_1     1 //!< Com-Objekt Nummer: 1, Beschreibung: GO_BASE_1, Funktion: GO_BASE_1
#define COMOBJ_2_GO_BASE_2_GO_BASE_2     2 //!< Com-Objekt Nummer: 2, Beschreibung: GO_BASE_2, Funktion: GO_BASE_2
#define COMOBJ_3_GO_BASE_3_GO_BASE_3     3 //!< Com-Objekt Nummer: 3, Beschreibung: GO_BASE_3, Funktion: GO_BASE_3
#define COMOBJ_4_GO_BASE_4_GO_BASE_4     4 //!< Com-Objekt Nummer: 4, Beschreibung: GO_BASE_4, Funktion: GO_BASE_4
#define COMOBJ_5_GO_BASE_5_GO_BASE_5     5 //!< Com-Objekt Nummer: 5, Beschreibung: GO_BASE_5, Funktion: GO_BASE_5
#define COMOBJ_6_GO_BASE_6_GO_BASE_6     6 //!< Com-Objekt Nummer: 6, Beschreibung: GO_BASE_6, Funktion: GO_BASE_6
#define COMOBJ_7_GO_BASE_7_GO_BASE_7     7 //!< Com-Objekt Nummer: 7, Beschreibung: GO_BASE_7, Funktion: GO_BASE_7
#define COMOBJ_8_GO_BASE_8_GO_BASE_8     8 //!< Com-Objekt Nummer: 8, Beschreibung: GO_BASE_8, Funktion: GO_BASE_8
#define COMOBJ_9_GO_BASE_9_GO_BASE_9     9 //!< Com-Objekt Nummer: 9, Beschreibung: GO_BASE_9, Funktion: GO_BASE_9
#define COMOBJ_10_GO_BASE_10_GO_BASE_10     10 //!< Com-Objekt Nummer: 10, Beschreibung: GO_BASE_10, Funktion: GO_BASE_10
#define COMOBJ_11_GO_BASE_11_GO_BASE_11     11 //!< Com-Objekt Nummer: 11, Beschreibung: GO_BASE_11, Funktion: GO_BASE_11
#define COMOBJ_12_GO_BASE_12_GO_BASE_12     12 //!< Com-Objekt Nummer: 12, Beschreibung: GO_BASE_12, Funktion: GO_BASE_12
#define COMOBJ_13_GO_BASE_13_GO_BASE_13     13 //!< Com-Objekt Nummer: 13, Beschreibung: GO_BASE_13, Funktion: GO_BASE_13
#define COMOBJ_14_GO_BASE_14_GO_BASE_14     14 //!< Com-Objekt Nummer: 14, Beschreibung: GO_BASE_14, Funktion: GO_BASE_14
#define COMOBJ_15_GO_BASE_15_GO_BASE_15     15 //!< Com-Objekt Nummer: 15, Beschreibung: GO_BASE_15, Funktion: GO_BASE_15
#define COMOBJ_16_GO_BASE_16_GO_BASE_16     16 //!< Com-Objekt Nummer: 16, Beschreibung: GO_BASE_16, Funktion: GO_BASE_16
#define COMOBJ_17_GO_BASE_17_GO_BASE_17     17 //!< Com-Objekt Nummer: 17, Beschreibung: GO_BASE_17, Funktion: GO_BASE_17
#define COMOBJ_18_GO_BASE_18_GO_BASE_18     18 //!< Com-Objekt Nummer: 18, Beschreibung: GO_BASE_18, Funktion: GO_BASE_18
#define COMOBJ_19_GO_BASE_19_GO_BASE_19     19 //!< Com-Objekt Nummer: 19, Beschreibung: GO_BASE_19, Funktion: GO_BASE_19
#define COMOBJ_20_GO_BASE_20_GO_BASE_20     20 //!< Com-Objekt Nummer: 20, Beschreibung: GO_BASE_20, Funktion: GO_BASE_20
#define COMOBJ_21_GO_BASE_21_GO_BASE_21     21 //!< Com-Objekt Nummer: 21, Beschreibung: GO_BASE_21, Funktion: GO_BASE_21
#define COMOBJ_22_GO_BASE_22_GO_BASE_22     22 //!< Com-Objekt Nummer: 22, Beschreibung: GO_BASE_22, Funktion: GO_BASE_22
#define COMOBJ_23_GO_BASE_23_GO_BASE_23     23 //!< Com-Objekt Nummer: 23, Beschreibung: GO_BASE_23, Funktion: GO_BASE_23
#define COMOBJ_24_GO_BASE_24_GO_BASE_24     24 //!< Com-Objekt Nummer: 24, Beschreibung: GO_BASE_24, Funktion: GO_BASE_24
#define COMOBJ_25_GO_BASE_25_GO_BASE_25     25 //!< Com-Objekt Nummer: 25, Beschreibung: GO_BASE_25, Funktion: GO_BASE_25
#define COMOBJ_26_GO_BASE_26_GO_BASE_26     26 //!< Com-Objekt Nummer: 26, Beschreibung: GO_BASE_26, Funktion: GO_BASE_26
#define COMOBJ_27_GO_BASE_27_GO_BASE_27     27 //!< Com-Objekt Nummer: 27, Beschreibung: GO_BASE_27, Funktion: GO_BASE_27
#define COMOBJ_28_GO_BASE_28_GO_BASE_28     28 //!< Com-Objekt Nummer: 28, Beschreibung: GO_BASE_28, Funktion: GO_BASE_28
#define COMOBJ_29_GO_BASE_29_GO_BASE_29     29 //!< Com-Objekt Nummer: 29, Beschreibung: GO_BASE_29, Funktion: GO_BASE_29
#define COMOBJ_30_GO_BASE_30_GO_BASE_30     30 //!< Com-Objekt Nummer: 30, Beschreibung: GO_BASE_30, Funktion: GO_BASE_30
#define COMOBJ_31_GO_BASE_31_GO_BASE_31     31 //!< Com-Objekt Nummer: 31, Beschreibung: GO_BASE_31, Funktion: GO_BASE_31
#define COMOBJ_32_GO_BASE_32_GO_BASE_32     32 //!< Com-Objekt Nummer: 32, Beschreibung: GO_BASE_32, Funktion: GO_BASE_32
#define COMOBJ_33_GO_BASE_33_GO_BASE_33     33 //!< Com-Objekt Nummer: 33, Beschreibung: GO_BASE_33, Funktion: GO_BASE_33
#define COMOBJ_34_GO_BASE_34_GO_BASE_34     34 //!< Com-Objekt Nummer: 34, Beschreibung: GO_BASE_34, Funktion: GO_BASE_34
#define COMOBJ_35_GO_BASE_35_GO_BASE_35     35 //!< Com-Objekt Nummer: 35, Beschreibung: GO_BASE_35, Funktion: GO_BASE_35
#define COMOBJ_36_GO_BASE_36_GO_BASE_36     36 //!< Com-Objekt Nummer: 36, Beschreibung: GO_BASE_36, Funktion: GO_BASE_36
#define COMOBJ_37_GO_BASE_37_GO_BASE_37     37 //!< Com-Objekt Nummer: 37, Beschreibung: GO_BASE_37, Funktion: GO_BASE_37
#define COMOBJ_38_GO_BASE_38_GO_BASE_38     38 //!< Com-Objekt Nummer: 38, Beschreibung: GO_BASE_38, Funktion: GO_BASE_38
#define COMOBJ_39_GO_BASE_39_GO_BASE_39     39 //!< Com-Objekt Nummer: 39, Beschreibung: GO_BASE_39, Funktion: GO_BASE_39
#define COMOBJ_40_GO_BASE_40_GO_BASE_40     40 //!< Com-Objekt Nummer: 40, Beschreibung: GO_BASE_40, Funktion: GO_BASE_40
#define COMOBJ_41_GO_BASE_41_GO_BASE_41     41 //!< Com-Objekt Nummer: 41, Beschreibung: GO_BASE_41, Funktion: GO_BASE_41
#define COMOBJ_42_GO_BASE_42_GO_BASE_42     42 //!< Com-Objekt Nummer: 42, Beschreibung: GO_BASE_42, Funktion: GO_BASE_42
#define COMOBJ_43_GO_BASE_43_GO_BASE_43     43 //!< Com-Objekt Nummer: 43, Beschreibung: GO_BASE_43, Funktion: GO_BASE_43
#define COMOBJ_44_GO_BASE_44_GO_BASE_44     44 //!< Com-Objekt Nummer: 44, Beschreibung: GO_BASE_44, Funktion: GO_BASE_44
#define COMOBJ_51_GO_BASE_51_GO_BASE_51     51 //!< Com-Objekt Nummer: 51, Beschreibung: GO_BASE_51, Funktion: GO_BASE_51
#define COMOBJ_52_GO_BASE_52_GO_BASE_52     52 //!< Com-Objekt Nummer: 52, Beschreibung: GO_BASE_52, Funktion: GO_BASE_52
#define COMOBJ_53_GO_BASE_53_GO_BASE_53     53 //!< Com-Objekt Nummer: 53, Beschreibung: GO_BASE_53, Funktion: GO_BASE_53
#define COMOBJ_54_GO_BASE_54_GO_BASE_54     54 //!< Com-Objekt Nummer: 54, Beschreibung: GO_BASE_54, Funktion: GO_BASE_54
#define COMOBJ_55_GO_BASE_55_GO_BASE_55     55 //!< Com-Objekt Nummer: 55, Beschreibung: GO_BASE_55, Funktion: GO_BASE_55
#define COMOBJ_56_GO_BASE_56_GO_BASE_56     56 //!< Com-Objekt Nummer: 56, Beschreibung: GO_BASE_56, Funktion: GO_BASE_56
#define COMOBJ_57_GO_BASE_57_GO_BASE_57     57 //!< Com-Objekt Nummer: 57, Beschreibung: GO_BASE_57, Funktion: GO_BASE_57
#define COMOBJ_58_GO_BASE_58_GO_BASE_58     58 //!< Com-Objekt Nummer: 58, Beschreibung: GO_BASE_58, Funktion: GO_BASE_58
#define COMOBJ_59_GO_BASE_59_GO_BASE_59     59 //!< Com-Objekt Nummer: 59, Beschreibung: GO_BASE_59, Funktion: GO_BASE_59
#define COMOBJ_61_GO_BASE_61_GO_BASE_61     61 //!< Com-Objekt Nummer: 61, Beschreibung: GO_BASE_61, Funktion: GO_BASE_61
#define COMOBJ_62_GO_BASE_62_GO_BASE_62     62 //!< Com-Objekt Nummer: 62, Beschreibung: GO_BASE_62, Funktion: GO_BASE_62
#define COMOBJ_63_GO_BASE_63_GO_BASE_63     63 //!< Com-Objekt Nummer: 63, Beschreibung: GO_BASE_63, Funktion: GO_BASE_63
#define COMOBJ_64_GO_BASE_64_GO_BASE_64     64 //!< Com-Objekt Nummer: 64, Beschreibung: GO_BASE_64, Funktion: GO_BASE_64

#endif

/** @}*/
