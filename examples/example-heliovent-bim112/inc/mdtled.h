/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_BIM112_HELIOVENT_1 BCU BIM112 Maskversion 0x0705 Example
 * @ingroup SBLIB_EXAMPLES
 * @brief   Header File for sblib (Selfbus ARM KNX Library), Device: AKD-0424R.02 LED Controller 4 Ch/RGBW, MDRC, ApplicationProgram: M-0083_A-0040-24-1835
 * @details Header File for sblib (Selfbus ARM KNX Library) generated with KNXprodEditor 0.44
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
 * @{
 *
 * @file   mdtled.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef KNXPRODEDITORHEADER_MDTLED_H
#define KNXPRODEDITORHEADER_MDTLED_H

#define MANUFACTURER 131 //!< Manufacturer ID
#define DEVICETYPE 64 //!< Device Type
#define APPVERSION 36 //!< Application Version

const unsigned char hardwareVersion[6] = { 0x00, 0x00, 0x00, 0x00, 0x03, 0x40 }; //!< The hardware identification number hardwareVersion

#define EE_PARAMETER_46A6_EINSCHALTVERZOEGERUNG     0x46A6 //!< Addr: 0x46A6, Size: 0x0010, Einschaltverzögerung 
#define EE_UNIONPARAMETER_4684     0x4684 //!< Addr: 0x4684, Size: 0x0008, RelaisSwitchOFFTime0 oder     Ausschaltverzögerung des Relais 
#define EE_PARAMETER_46AE_MINIMALE_HELLIGKEIT     0x46AE //!< Addr: 0x46AE, Size: 0x0008, Minimale Helligkeit 
#define EE_PARAMETER_46AF_MAXIMALE_HELLIGKEIT     0x46AF //!< Addr: 0x46AF, Size: 0x0008, Maximale Helligkeit 
#define EE_PARAMETER_46B0_EINSCHALTWERT     0x46B0 //!< Addr: 0x46B0, Size: 0x0008,     Einschaltwert 
#define EE_PARAMETER_46B3_MANUELLES_AUSSCHALTEN     0x46B3 //!< Addr: 0x46B3, Size: 0x0001, Manuelles Ausschalten 
#define EE_PARAMETER_46A5_TREPPENLICHTZEIT_VERLAENGERN     0x46A5 //!< Addr: 0x46A5, Size: 0x0008, Treppenlichtzeit verlängern 
#define EE_PARAMETER_46B2_DIMMGESCHWINDIGKEIT_REL_DIMMEN_S     0x46B2 //!< Addr: 0x46B2, Size: 0x0010,     Dimmgeschwindigkeit rel. Dimmen s 
#define EE_PARAMETER_46BB_SPERROBJEKT_1_AKTION_BEIM_ENTSPERREN     0x46BB //!< Addr: 0x46BB, Size: 0x0008,     Sperrobjekt 1 -> Aktion beim Entsperren 
#define EE_PARAMETER_46BA_SPERROBJEKT_1_AKTION_BEIM_SPERREN     0x46BA //!< Addr: 0x46BA, Size: 0x0008,     Sperrobjekt 1 -> Aktion beim Sperren 
#define EE_PARAMETER_46CD_HELLIGKEITSWERT     0x46CD //!< Addr: 0x46CD, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_46CE_HELLIGKEITSWERT     0x46CE //!< Addr: 0x46CE, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_46CF_HELLIGKEITSWERT     0x46CF //!< Addr: 0x46CF, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_46D0_HELLIGKEITSWERT     0x46D0 //!< Addr: 0x46D0, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_46D1_HELLIGKEITSWERT     0x46D1 //!< Addr: 0x46D1, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_46D2_HELLIGKEITSWERT     0x46D2 //!< Addr: 0x46D2, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_46D3_HELLIGKEITSWERT     0x46D3 //!< Addr: 0x46D3, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_46D4_HELLIGKEITSWERT     0x46D4 //!< Addr: 0x46D4, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_46D5_HELLIGKEITSWERT     0x46D5 //!< Addr: 0x46D5, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_46D6_HELLIGKEITSWERT     0x46D6 //!< Addr: 0x46D6, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_46D7_HELLIGKEITSWERT     0x46D7 //!< Addr: 0x46D7, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_46D8_HELLIGKEITSWERT     0x46D8 //!< Addr: 0x46D8, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_46D9_HELLIGKEITSWERT     0x46D9 //!< Addr: 0x46D9, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_46DA_HELLIGKEITSWERT     0x46DA //!< Addr: 0x46DA, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_46DB_HELLIGKEITSWERT     0x46DB //!< Addr: 0x46DB, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_46DC_HELLIGKEITSWERT     0x46DC //!< Addr: 0x46DC, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_46DD_SZENENUMMER_A     0x46DD //!< Addr: 0x46DD, Size: 0x0008, Szenenummer A 
#define EE_PARAMETER_46DE_SZENENUMMER_B     0x46DE //!< Addr: 0x46DE, Size: 0x0008, Szenenummer B 
#define EE_PARAMETER_46DF_SZENENUMMER_C     0x46DF //!< Addr: 0x46DF, Size: 0x0008, Szenenummer C 
#define EE_PARAMETER_46E0_SZENENUMMER_D     0x46E0 //!< Addr: 0x46E0, Size: 0x0008, Szenenummer D 
#define EE_PARAMETER_46E1_SZENENUMMER_E     0x46E1 //!< Addr: 0x46E1, Size: 0x0008, Szenenummer E 
#define EE_PARAMETER_46E2_SZENENUMMER_F     0x46E2 //!< Addr: 0x46E2, Size: 0x0008, Szenenummer F 
#define EE_PARAMETER_46E3_SZENENUMMER_G     0x46E3 //!< Addr: 0x46E3, Size: 0x0008, Szenenummer G 
#define EE_PARAMETER_46E4_SZENENUMMER_H     0x46E4 //!< Addr: 0x46E4, Size: 0x0008, Szenenummer H 
#define EE_PARAMETER_46EA_STATUS_DIMMWERT_SENDEN     0x46EA //!< Addr: 0x46EA, Size: 0x0008, Status Dimmwert senden 
#define EE_PARAMETER_46EE_SZENE_SPEICHERN     0x46EE //!< Addr: 0x46EE, Size: 0x0008, Szene speichern 
#define EE_PARAMETER_46EF_OM_AKTORPARAM_ACTIVEDELAYOBJECTS_0     0x46EF //!< Addr: 0x46EF, Size: 0x0008, OM_aktorParam_activeDelayObjects_0
#define EE_PARAMETER_47DE_VERHALTEN_BEI_SPERROBJEKT_1_WERT_1     0x47DE //!< Addr: 0x47DE, Size: 0x0008, Verhalten bei Sperrobjekt 1 = Wert 1 
#define EE_PARAMETER_4870_VERHALTEN_BEI_SPERROBJEKT_1_WERT_1     0x4870 //!< Addr: 0x4870, Size: 0x0008, Verhalten bei Sperrobjekt 1 = Wert 1 
#define EE_PARAMETER_4697_ABSOLUTES_DIMMEN     0x4697 //!< Addr: 0x4697, Size: 0x0001,     Absolutes Dimmen 
#define EE_PARAMETER_469A_AUSSCHALTEN     0x469A //!< Addr: 0x469A, Size: 0x0001,     Ausschalten 
#define EE_PARAMETER_46F2_EINSCHALTGESCHWINDIGKEIT_S     0x46F2 //!< Addr: 0x46F2, Size: 0x0010,     Einschaltgeschwindigkeit s 
#define EE_PARAMETER_46F4_AUSSCHALTGESCHWINDIGKEIT_S     0x46F4 //!< Addr: 0x46F4, Size: 0x0010,     Ausschaltgeschwindigkeit s 
#define EE_PARAMETER_4681_FUNKTIONSAUSWAHL     0x4681 //!< Addr: 0x4681, Size: 0x0008, Funktionsauswahl 
#define EE_PARAMETER_468B_DIMMKURVE     0x468B //!< Addr: 0x468B, Size: 0x0008, Dimmkurve 
#define EE_PARAMETER_492A_AUSGABE_RGBW_STATUS_4X_1BYTE     0x492A //!< Addr: 0x492A, Size: 0x0008,     Ausgabe RGBW Status - 4x 1Byte 
#define EE_PARAMETER_492B_AUSGABE_HSV_STATUS_3X_1BYTE     0x492B //!< Addr: 0x492B, Size: 0x0008,     Ausgabe HSV Status - 3x 1Byte 
#define EE_PARAMETER_4930_RELATIVES_DIMMEN_FARBTON_H_S     0x4930 //!< Addr: 0x4930, Size: 0x0010,     Relatives Dimmen Farbton (H) s 
#define EE_PARAMETER_493E_AUSSCHALTGESCHWINDIGKEIT_S     0x493E //!< Addr: 0x493E, Size: 0x0010,     Ausschaltgeschwindigkeit s 
#define EE_PARAMETER_4938_ABSOLUTES_DIMMEN_S     0x4938 //!< Addr: 0x4938, Size: 0x0010,     Absolutes Dimmen s 
#define EE_PARAMETER_493A_EINSCHALTGESCHWINDIGKEIT_S     0x493A //!< Addr: 0x493A, Size: 0x0010,     Einschaltgeschwindigkeit s 
#define EE_PARAMETER_46FC_DIMMGESCHWINDIGKEIT_ABS_DIMMEN_S     0x46FC //!< Addr: 0x46FC, Size: 0x0010,     Dimmgeschwindigkeit abs. Dimmen s 
#define EE_PARAMETER_4B93_OM_SEQPARAM0_RGBHHSV     0x4B93 //!< Addr: 0x4B93, Size: 0x0008, OM_SeqParam[0]_RgbHhsv
#define EE_PARAMETER_4B68_OM_SEQPARAM0_DIMMMODECOLOR_STEPA     0x4B68 //!< Addr: 0x4B68, Size: 0x0008, OM_SeqParam[0]_DimmModeColor_StepA
#define EE_PARAMETER_4B67_OM_SEQPARAM0_DIMMMODEW_0_STEPA     0x4B67 //!< Addr: 0x4B67, Size: 0x0008, OM_SeqParam[0]_DimmModeW/0_StepA
#define EE_PARAMETER_4B64_OM_SEQPARAM0_DIMMMODER_H_STEPA     0x4B64 //!< Addr: 0x4B64, Size: 0x0008, OM_SeqParam[0]_DimmModeR/H_StepA
#define EE_PARAMETER_4B65_OM_SEQPARAM0_DIMMMODEG_S_STEPA     0x4B65 //!< Addr: 0x4B65, Size: 0x0008, OM_SeqParam[0]_DimmModeG/S_StepA
#define EE_PARAMETER_4B66_OM_SEQPARAM0_DIMMMODEB_V_STEPA     0x4B66 //!< Addr: 0x4B66, Size: 0x0008, OM_SeqParam[0]_DimmModeB/V_StepA
#define EE_PARAMETER_4B6D_OM_SEQPARAM0_DIMMMODECOLOR_STEPB     0x4B6D //!< Addr: 0x4B6D, Size: 0x0008, OM_SeqParam[0]_DimmModeColor_StepB
#define EE_PARAMETER_4B6C_OM_SEQPARAM0_DIMMMODEW_0_STEPB     0x4B6C //!< Addr: 0x4B6C, Size: 0x0008, OM_SeqParam[0]_DimmModeW/0_StepB
#define EE_PARAMETER_4B6B_OM_SEQPARAM0_DIMMMODEB_V_STEPB     0x4B6B //!< Addr: 0x4B6B, Size: 0x0008, OM_SeqParam[0]_DimmModeB/V_StepB
#define EE_PARAMETER_4B6A_OM_SEQPARAM0_DIMMMODEG_S_STEPB     0x4B6A //!< Addr: 0x4B6A, Size: 0x0008, OM_SeqParam[0]_DimmModeG/S_StepB
#define EE_PARAMETER_4B69_OM_SEQPARAM0_DIMMMODER_H_STEPB     0x4B69 //!< Addr: 0x4B69, Size: 0x0008, OM_SeqParam[0]_DimmModeR/H_StepB
#define EE_PARAMETER_4B72_OM_SEQPARAM0_DIMMMODECOLOR_STEPC     0x4B72 //!< Addr: 0x4B72, Size: 0x0008, OM_SeqParam[0]_DimmModeColor_StepC
#define EE_PARAMETER_4B71_OM_SEQPARAM0_DIMMMODEW_0_STEPC     0x4B71 //!< Addr: 0x4B71, Size: 0x0008, OM_SeqParam[0]_DimmModeW/0_StepC
#define EE_PARAMETER_4B70_OM_SEQPARAM0_DIMMMODEB_V_STEPC     0x4B70 //!< Addr: 0x4B70, Size: 0x0008, OM_SeqParam[0]_DimmModeB/V_StepC
#define EE_PARAMETER_4B6F_OM_SEQPARAM0_DIMMMODEG_S_STEPC     0x4B6F //!< Addr: 0x4B6F, Size: 0x0008, OM_SeqParam[0]_DimmModeG/S_StepC
#define EE_PARAMETER_4B6E_OM_SEQPARAM0_DIMMMODER_H_STEPC     0x4B6E //!< Addr: 0x4B6E, Size: 0x0008, OM_SeqParam[0]_DimmModeR/H_StepC
#define EE_PARAMETER_4B77_OM_SEQPARAM0_DIMMMODECOLOR_STEPD     0x4B77 //!< Addr: 0x4B77, Size: 0x0008, OM_SeqParam[0]_DimmModeColor_StepD
#define EE_PARAMETER_4B76_OM_SEQPARAM0_DIMMMODEW_0_STEPD     0x4B76 //!< Addr: 0x4B76, Size: 0x0008, OM_SeqParam[0]_DimmModeW/0_StepD
#define EE_PARAMETER_4B75_OM_SEQPARAM0_DIMMMODEB_V_STEPD     0x4B75 //!< Addr: 0x4B75, Size: 0x0008, OM_SeqParam[0]_DimmModeB/V_StepD
#define EE_PARAMETER_4B74_OM_SEQPARAM0_DIMMMODEG_S_STEPD     0x4B74 //!< Addr: 0x4B74, Size: 0x0008, OM_SeqParam[0]_DimmModeG/S_StepD
#define EE_PARAMETER_4B73_OM_SEQPARAM0_DIMMMODER_H_STEPD     0x4B73 //!< Addr: 0x4B73, Size: 0x0008, OM_SeqParam[0]_DimmModeR/H_StepD
#define EE_PARAMETER_4B7C_OM_SEQPARAM0_DIMMMODECOLOR_STEPE     0x4B7C //!< Addr: 0x4B7C, Size: 0x0008, OM_SeqParam[0]_DimmModeColor_StepE
#define EE_PARAMETER_4B7B_OM_SEQPARAM0_DIMMMODEW_0_STEPE     0x4B7B //!< Addr: 0x4B7B, Size: 0x0008, OM_SeqParam[0]_DimmModeW/0_StepE
#define EE_PARAMETER_4B7A_OM_SEQPARAM0_DIMMMODEB_V_STEPE     0x4B7A //!< Addr: 0x4B7A, Size: 0x0008, OM_SeqParam[0]_DimmModeB/V_StepE
#define EE_PARAMETER_4B79_OM_SEQPARAM0_DIMMMODEG_S_STEPE     0x4B79 //!< Addr: 0x4B79, Size: 0x0008, OM_SeqParam[0]_DimmModeG/S_StepE
#define EE_PARAMETER_4B78_OM_SEQPARAM0_DIMMMODER_H_STEPE     0x4B78 //!< Addr: 0x4B78, Size: 0x0008, OM_SeqParam[0]_DimmModeR/H_StepE
#define EE_PARAMETER_4B7E_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4B7E //!< Addr: 0x4B7E, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4B80_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4B80 //!< Addr: 0x4B80, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4B82_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4B82 //!< Addr: 0x4B82, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4B84_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4B84 //!< Addr: 0x4B84, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4B86_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4B86 //!< Addr: 0x4B86, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_UNIONPARAMETER_46B4     0x46B4 //!< Addr: 0x46B4, Size: 0x0002, Einschaltverhalten  oder Einschaltverhalten 
#define EE_UNIONPARAMETER_4746     0x4746 //!< Addr: 0x4746, Size: 0x0002, Einschaltverhalten  oder Einschaltverhalten 
#define EE_UNIONPARAMETER_47D8     0x47D8 //!< Addr: 0x47D8, Size: 0x0002, Einschaltverhalten  oder Einschaltverhalten 
#define EE_UNIONPARAMETER_486A     0x486A //!< Addr: 0x486A, Size: 0x0002, Einschaltverhalten  oder Einschaltverhalten 
#define EE_UNIONPARAMETER_4929     0x4929 //!< Addr: 0x4929, Size: 0x0008, DPT für RGB/RGBW Kombiobjekt  oder DPT für RGB Kombiobjekt 
#define EE_UNIONPARAMETER_4945     0x4945 //!< Addr: 0x4945, Size: 0x0008,     Aktion  oder     Aktion  oder     Aktion  oder     Aktion  oder     Aktion 
#define EE_UNIONPARAMETER_4946     0x4946 //!< Addr: 0x4946, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_494F     0x494F //!< Addr: 0x494F, Size: 0x0008,     Aktion  oder     Aktion  oder     Aktion  oder     Aktion  oder     Aktion 
#define EE_UNIONPARAMETER_4950     0x4950 //!< Addr: 0x4950, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4959     0x4959 //!< Addr: 0x4959, Size: 0x0008,     Aktion  oder     Aktion  oder     Aktion  oder     Aktion  oder     Aktion 
#define EE_UNIONPARAMETER_495A     0x495A //!< Addr: 0x495A, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4963     0x4963 //!< Addr: 0x4963, Size: 0x0008,     Aktion  oder     Aktion  oder     Aktion  oder     Aktion  oder     Aktion 
#define EE_UNIONPARAMETER_4964     0x4964 //!< Addr: 0x4964, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_496D     0x496D //!< Addr: 0x496D, Size: 0x0008,     Aktion  oder     Aktion  oder     Aktion  oder     Aktion  oder     Aktion 
#define EE_UNIONPARAMETER_496E     0x496E //!< Addr: 0x496E, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4977     0x4977 //!< Addr: 0x4977, Size: 0x0008,     Aktion  oder     Aktion  oder     Aktion  oder     Aktion  oder     Aktion 
#define EE_UNIONPARAMETER_4978     0x4978 //!< Addr: 0x4978, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4981     0x4981 //!< Addr: 0x4981, Size: 0x0008,     Aktion  oder     Aktion  oder     Aktion  oder     Aktion  oder     Aktion 
#define EE_UNIONPARAMETER_4982     0x4982 //!< Addr: 0x4982, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_498B     0x498B //!< Addr: 0x498B, Size: 0x0008,     Aktion  oder     Aktion  oder     Aktion  oder     Aktion  oder     Aktion 
#define EE_UNIONPARAMETER_498C     0x498C //!< Addr: 0x498C, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4994     0x4994 //!< Addr: 0x4994, Size: 0x0008,     Aktion bei Wert = 0  oder     Aktion bei Wert = 0  oder     Aktion bei Wert = 0  oder     Aktion bei Wert = 0  oder     Aktion bei Wert = 0 
#define EE_UNIONPARAMETER_4995     0x4995 //!< Addr: 0x4995, Size: 0x0008,     Aktion bei Wert = 1  oder     Aktion bei Wert = 1  oder     Aktion bei Wert = 1  oder     Aktion bei Wert = 1  oder     Aktion bei Wert = 1 
#define EE_UNIONPARAMETER_4996     0x4996 //!< Addr: 0x4996, Size: 0x0008,         Farbton H Bit Wert 0  oder         Farbton H Bit Wert 0  oder         Farbtemperatur Bit Wert 0 
#define EE_UNIONPARAMETER_499A     0x499A //!< Addr: 0x499A, Size: 0x0008,         Farbton H Bit Wert 1  oder         Farbton H Bit Wert 1  oder         Farbtemperatur Bit Wert 1 
#define EE_UNIONPARAMETER_49A2     0x49A2 //!< Addr: 0x49A2, Size: 0x0008,     Aktion bei Wert = 0  oder     Aktion bei Wert = 0  oder     Aktion bei Wert = 0  oder     Aktion bei Wert = 0  oder     Aktion bei Wert = 0 
#define EE_UNIONPARAMETER_49A3     0x49A3 //!< Addr: 0x49A3, Size: 0x0008,     Aktion bei Wert = 1  oder     Aktion bei Wert = 1  oder     Aktion bei Wert = 1  oder     Aktion bei Wert = 1  oder     Aktion bei Wert = 1 
#define EE_UNIONPARAMETER_49A4     0x49A4 //!< Addr: 0x49A4, Size: 0x0008,         Farbton H Bit Wert 0  oder         Farbton H Bit Wert 0  oder         Farbtemperatur Bit Wert 0 
#define EE_UNIONPARAMETER_49A8     0x49A8 //!< Addr: 0x49A8, Size: 0x0008,         Farbton H Bit Wert 1  oder         Farbton H Bit Wert 1  oder         Farbtemperatur Bit Wert 1 
#define EE_UNIONPARAMETER_49B0     0x49B0 //!< Addr: 0x49B0, Size: 0x0008,     Aktion bei Wert = 0  oder     Aktion bei Wert = 0  oder     Aktion bei Wert = 0  oder     Aktion bei Wert = 0  oder     Aktion bei Wert = 0 
#define EE_UNIONPARAMETER_49B1     0x49B1 //!< Addr: 0x49B1, Size: 0x0008,     Aktion bei Wert = 1  oder     Aktion bei Wert = 1  oder     Aktion bei Wert = 1  oder     Aktion bei Wert = 1  oder     Aktion bei Wert = 1 
#define EE_UNIONPARAMETER_49B2     0x49B2 //!< Addr: 0x49B2, Size: 0x0008,         Farbton H Bit Wert 0  oder         Farbton H Bit Wert 0  oder         Farbtemperatur Bit Wert 0 
#define EE_UNIONPARAMETER_49B6     0x49B6 //!< Addr: 0x49B6, Size: 0x0008,         Farbton H Bit Wert 1  oder         Farbton H Bit Wert 1  oder         Farbtemperatur Bit Wert 1 
#define EE_UNIONPARAMETER_49BE     0x49BE //!< Addr: 0x49BE, Size: 0x0008,     Aktion bei Wert = 0  oder     Aktion bei Wert = 0  oder     Aktion bei Wert = 0  oder     Aktion bei Wert = 0  oder     Aktion bei Wert = 0 
#define EE_UNIONPARAMETER_49BF     0x49BF //!< Addr: 0x49BF, Size: 0x0008,     Aktion bei Wert = 1  oder     Aktion bei Wert = 1  oder     Aktion bei Wert = 1  oder     Aktion bei Wert = 1  oder     Aktion bei Wert = 1 
#define EE_UNIONPARAMETER_49C0     0x49C0 //!< Addr: 0x49C0, Size: 0x0008,         Farbton H Bit Wert 0  oder         Farbton H Bit Wert 0  oder         Farbtemperatur Bit Wert 0 
#define EE_UNIONPARAMETER_49C4     0x49C4 //!< Addr: 0x49C4, Size: 0x0008,         Farbton H Bit Wert 1  oder         Farbton H Bit Wert 1  oder         Farbtemperatur Bit Wert 1 
#define EE_UNIONPARAMETER_49D6     0x49D6 //!< Addr: 0x49D6, Size: 0x0008, Einschaltverhalten  oder Einschaltverhalten  oder Einschaltverhalten  oder Einschaltverhalten  oder Einschaltverhalten 
#define EE_UNIONPARAMETER_49D7     0x49D7 //!< Addr: 0x49D7, Size: 0x0008, Einschaltverhalten Nacht  oder Einschaltverhalten Nacht  oder Einschaltverhalten Nacht  oder Einschaltverhalten Nacht  oder Einschaltverhalten Nacht 
#define EE_UNIONPARAMETER_49D8     0x49D8 //!< Addr: 0x49D8, Size: 0x0008,     Einschaltwert Rot  oder     Einschaltwert Farbton H  oder     Einschaltwert Farbtemperatur 
#define EE_UNIONPARAMETER_49DC     0x49DC //!< Addr: 0x49DC, Size: 0x0008,     Einschaltwert Rot  Nacht  oder     Einschaltwert Farbton H  Nacht  oder     Einschaltwert Farbtemperatur  Nacht 
#define EE_UNIONPARAMETER_49E0     0x49E0 //!< Addr: 0x49E0, Size: 0x0008, Verhalten nach Reset  oder Verhalten nach Reset  oder Verhalten nach Reset  oder Verhalten nach Reset  oder Verhalten nach Reset 
#define EE_UNIONPARAMETER_49E1     0x49E1 //!< Addr: 0x49E1, Size: 0x0008,     Resetwert Rot  oder     Resetwert Farbton H  oder     Resetwert Farbtemperatur 
#define EE_UNIONPARAMETER_49EC     0x49EC //!< Addr: 0x49EC, Size: 0x0008,     Aktion beim Entsperren  oder     Aktion beim Entsperren  oder     Aktion beim Entsperren  oder     Aktion beim Entsperren  oder     Aktion beim Entsperren 
#define EE_UNIONPARAMETER_49ED     0x49ED //!< Addr: 0x49ED, Size: 0x0008,     Aktion beim Sperren  oder     Aktion beim Sperren  oder     Aktion beim Sperren  oder     Aktion beim Sperren  oder     Aktion beim Sperren  oder     Aktion beim Sperren  oder     Aktion beim Sperren  oder     Aktion beim Sperren 
#define EE_UNIONPARAMETER_49EE     0x49EE //!< Addr: 0x49EE, Size: 0x0008,         Farbwert Rot  oder         Farbton H  oder         Farbtemperatur 
#define EE_UNIONPARAMETER_49F2     0x49F2 //!< Addr: 0x49F2, Size: 0x0008,         Farbwert Rot  oder         Farbton H  oder         Farbtemperatur 
#define EE_UNIONPARAMETER_49FC     0x49FC //!< Addr: 0x49FC, Size: 0x0008,     Aktion beim Entsperren  oder     Aktion beim Entsperren  oder     Aktion beim Entsperren  oder     Aktion beim Entsperren  oder     Aktion beim Entsperren 
#define EE_UNIONPARAMETER_49FD     0x49FD //!< Addr: 0x49FD, Size: 0x0008,     Aktion beim Sperren  oder     Aktion beim Sperren  oder     Aktion beim Sperren  oder     Aktion beim Sperren  oder     Aktion beim Sperren  oder     Aktion beim Sperren  oder     Aktion beim Sperren  oder     Aktion beim Sperren 
#define EE_UNIONPARAMETER_49FE     0x49FE //!< Addr: 0x49FE, Size: 0x0008,         Farbwert Rot  oder         Farbton H  oder         Farbtemperatur 
#define EE_UNIONPARAMETER_4A02     0x4A02 //!< Addr: 0x4A02, Size: 0x0008,         Farbwert Rot  oder         Farbton H  oder         Farbtemperatur 
#define EE_UNIONPARAMETER_4A2A     0x4A2A //!< Addr: 0x4A2A, Size: 0x0008,     Regelung der Farbtemperatur gültig  oder     Regelung der Farbtemperatur gültig 
#define EE_UNIONPARAMETER_4A2D     0x4A2D //!< Addr: 0x4A2D, Size: 0x0008, OM_RGBParam[0]_TW_ValMidTemp
#define EE_UNIONPARAMETER_4A5A     0x4A5A //!< Addr: 0x4A5A, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4A64     0x4A64 //!< Addr: 0x4A64, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4A6E     0x4A6E //!< Addr: 0x4A6E, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4A78     0x4A78 //!< Addr: 0x4A78, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4A82     0x4A82 //!< Addr: 0x4A82, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4A8C     0x4A8C //!< Addr: 0x4A8C, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4A96     0x4A96 //!< Addr: 0x4A96, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4AA0     0x4AA0 //!< Addr: 0x4AA0, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4AAA     0x4AAA //!< Addr: 0x4AAA, Size: 0x0008,         Farbton H Bit Wert 0  oder         Farbtemperatur Bit Wert 0 
#define EE_UNIONPARAMETER_4AAE     0x4AAE //!< Addr: 0x4AAE, Size: 0x0008,         Farbton H Bit Wert 1  oder         Farbtemperatur Bit Wert 1 
#define EE_PARAMETER_4AB9_SAETTIGUNG_S_BIT_WERT_0     0x4AB9 //!< Addr: 0x4AB9, Size: 0x0008,         Sättigung S Bit Wert 0 
#define EE_UNIONPARAMETER_4AB8     0x4AB8 //!< Addr: 0x4AB8, Size: 0x0008,         Farbton H Bit Wert 0  oder         Farbtemperatur Bit Wert 0 
#define EE_UNIONPARAMETER_4ABC     0x4ABC //!< Addr: 0x4ABC, Size: 0x0008,         Farbton H Bit Wert 1  oder         Farbtemperatur Bit Wert 1 
#define EE_UNIONPARAMETER_4AC6     0x4AC6 //!< Addr: 0x4AC6, Size: 0x0008,         Farbton H Bit Wert 0  oder         Farbtemperatur Bit Wert 0 
#define EE_UNIONPARAMETER_4ACA     0x4ACA //!< Addr: 0x4ACA, Size: 0x0008,         Farbton H Bit Wert 1  oder         Farbtemperatur Bit Wert 1 
#define EE_UNIONPARAMETER_4AD4     0x4AD4 //!< Addr: 0x4AD4, Size: 0x0008,         Farbton H Bit Wert 0  oder         Farbtemperatur Bit Wert 0 
#define EE_UNIONPARAMETER_4AD8     0x4AD8 //!< Addr: 0x4AD8, Size: 0x0008,         Farbton H Bit Wert 1  oder         Farbtemperatur Bit Wert 1 
#define EE_UNIONPARAMETER_4AEC     0x4AEC //!< Addr: 0x4AEC, Size: 0x0008,     Einschaltwert Rot  oder     Einschaltwert Farbtemperatur 
#define EE_UNIONPARAMETER_4AF0     0x4AF0 //!< Addr: 0x4AF0, Size: 0x0008,     Einschaltwert Rot  Nacht  oder     Einschaltwert Farbtemperatur  Nacht 
#define EE_UNIONPARAMETER_4AF5     0x4AF5 //!< Addr: 0x4AF5, Size: 0x0008,     Resetwert Rot  oder     Resetwert Farbtemperatur 
#define EE_UNIONPARAMETER_4B01     0x4B01 //!< Addr: 0x4B01, Size: 0x0008,     Aktion beim Sperren  oder     Aktion beim Sperren 
#define EE_UNIONPARAMETER_4B02     0x4B02 //!< Addr: 0x4B02, Size: 0x0008,         Farbwert Rot  oder         Farbtemperatur 
#define EE_UNIONPARAMETER_4B06     0x4B06 //!< Addr: 0x4B06, Size: 0x0008,         Farbwert Rot  oder         Farbtemperatur 
#define EE_UNIONPARAMETER_4B11     0x4B11 //!< Addr: 0x4B11, Size: 0x0008,     Aktion beim Sperren  oder     Aktion beim Sperren 
#define EE_UNIONPARAMETER_4B12     0x4B12 //!< Addr: 0x4B12, Size: 0x0008,         Farbwert Rot  oder         Farbtemperatur 
#define EE_UNIONPARAMETER_4B16     0x4B16 //!< Addr: 0x4B16, Size: 0x0008,         Farbwert Rot  oder         Farbtemperatur 
#define EE_UNIONPARAMETER_4B41     0x4B41 //!< Addr: 0x4B41, Size: 0x0008, OM_RGBParam[1]_TW_ValMidTemp
#define EE_UNIONPARAMETER_4B50     0x4B50 //!< Addr: 0x4B50, Size: 0x0008,     Farbwert Rot  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Farbton H  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot 
#define EE_UNIONPARAMETER_4B51     0x4B51 //!< Addr: 0x4B51, Size: 0x0008,     Sättigung S  oder     Oberer Grenzwert Sättigung S  oder     Unterer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4B52     0x4B52 //!< Addr: 0x4B52, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4B53     0x4B53 //!< Addr: 0x4B53, Size: 0x0008,     Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4B54     0x4B54 //!< Addr: 0x4B54, Size: 0x0008,     Farbwert Rot  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbton H  oder     Farbton H  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot 
#define EE_UNIONPARAMETER_4B55     0x4B55 //!< Addr: 0x4B55, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4B56     0x4B56 //!< Addr: 0x4B56, Size: 0x0008,     Helligkeit V  oder     Oberer Grenzwert Helligkeit V  oder     Unterer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4B57     0x4B57 //!< Addr: 0x4B57, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4B58     0x4B58 //!< Addr: 0x4B58, Size: 0x0008,     Farbwert Rot  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Farbton H  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbwert Rot  oder     Unterer Grenzwert Farbwert Rot 
#define EE_UNIONPARAMETER_4B59     0x4B59 //!< Addr: 0x4B59, Size: 0x0008,     Sättigung S  oder     Oberer Grenzwert Sättigung S  oder     Unterer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4B5A     0x4B5A //!< Addr: 0x4B5A, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4B5B     0x4B5B //!< Addr: 0x4B5B, Size: 0x0008,     Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4B5C     0x4B5C //!< Addr: 0x4B5C, Size: 0x0008,     Farbwert Rot  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbton H  oder     Farbton H  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot 
#define EE_UNIONPARAMETER_4B5D     0x4B5D //!< Addr: 0x4B5D, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4B5E     0x4B5E //!< Addr: 0x4B5E, Size: 0x0008,     Helligkeit V  oder     Oberer Grenzwert Helligkeit V  oder     Unterer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4B5F     0x4B5F //!< Addr: 0x4B5F, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4B60     0x4B60 //!< Addr: 0x4B60, Size: 0x0008,     Farbwert Rot  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Farbton H  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbwert Rot  oder     Unterer Grenzwert Farbwert Rot 
#define EE_UNIONPARAMETER_4B61     0x4B61 //!< Addr: 0x4B61, Size: 0x0008,     Sättigung S  oder     Oberer Grenzwert Sättigung S  oder     Unterer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4B62     0x4B62 //!< Addr: 0x4B62, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4B63     0x4B63 //!< Addr: 0x4B63, Size: 0x0008,     Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4B88     0x4B88 //!< Addr: 0x4B88, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4B8A     0x4B8A //!< Addr: 0x4B8A, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4B8C     0x4B8C //!< Addr: 0x4B8C, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4B8E     0x4B8E //!< Addr: 0x4B8E, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4B90     0x4B90 //!< Addr: 0x4B90, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_PARAMETER_4B95_UEBERGANGSZEIT_ZUFAELLIG     0x4B95 //!< Addr: 0x4B95, Size: 0x0008,     Übergangszeit zufällig 
#define EE_PARAMETER_492C_AUSGABE_RGBW_HSV_STATUS_KOMBIOBJEKTE     0x492C //!< Addr: 0x492C, Size: 0x0008,     Ausgabe RGBW/HSV Status - Kombiobjekte 
#define EE_PARAMETER_4997_SAETTIGUNG_S_BIT_WERT_0     0x4997 //!< Addr: 0x4997, Size: 0x0008,         Sättigung S Bit Wert 0 
#define EE_PARAMETER_4998_HELLIGKEIT_V_BIT_WERT_0     0x4998 //!< Addr: 0x4998, Size: 0x0008,         Helligkeit V Bit Wert 0 
#define EE_PARAMETER_499C_HELLIGKEIT_V_BIT_WERT_1     0x499C //!< Addr: 0x499C, Size: 0x0008,         Helligkeit V Bit Wert 1 
#define EE_PARAMETER_499B_SAETTIGUNG_S_BIT_WERT_1     0x499B //!< Addr: 0x499B, Size: 0x0008,         Sättigung S Bit Wert 1 
#define EE_PARAMETER_4999_FARBWERT_WEISS_BIT_WERT_0     0x4999 //!< Addr: 0x4999, Size: 0x0008,         Farbwert Weiß Bit Wert 0 
#define EE_PARAMETER_499D_FARBWERT_WEISS_BIT_WERT_1     0x499D //!< Addr: 0x499D, Size: 0x0008,         Farbwert Weiß Bit Wert 1 
#define EE_PARAMETER_4947_SAETTIGUNG_S     0x4947 //!< Addr: 0x4947, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4948_HELLIGKEIT_V     0x4948 //!< Addr: 0x4948, Size: 0x0008,     Helligkeit V 
#define EE_PARAMETER_492F_AENDERUNG_SENDEN_WAEHREND_DES_DIMMVORGANGS     0x492F //!< Addr: 0x492F, Size: 0x0008,     Änderung senden während des Dimmvorgangs 
#define EE_PARAMETER_494A_DIMMGESCHWINDIGKEIT_S     0x494A //!< Addr: 0x494A, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4942_SZENE_SPEICHERN     0x4942 //!< Addr: 0x4942, Size: 0x0008, Szene speichern 
#define EE_PARAMETER_4949_FARBWERT_WEISS     0x4949 //!< Addr: 0x4949, Size: 0x0008,     Farbwert Weiß 
#define EE_PARAMETER_4B96_OM_SEQPARAM0_STARTSTEP     0x4B96 //!< Addr: 0x4B96, Size: 0x0008, OM_SeqParam[0]_StartStep
#define EE_UNIONPARAMETER_4B92     0x4B92 //!< Addr: 0x4B92, Size: 0x0008, Anzahl parametrierte Schritte  oder OM_SeqParam[0]_Steps
#define EE_UNIONPARAMETER_4B94     0x4B94 //!< Addr: 0x4B94, Size: 0x0008, OM_SegParam[0]_LoopOff oder     Anzahl der Ausführungen 
#define EE_UNIONPARAMETER_4B97     0x4B97 //!< Addr: 0x4B97, Size: 0x0008,     Verhalten nach Sequenz  oder     Verhalten nach Sequenz  oder     Verhalten nach Sequenz 
#define EE_UNIONPARAMETER_4B9B     0x4B9B //!< Addr: 0x4B9B, Size: 0x0008, OM_SeqParam[0]_DimBehaviorOff oder     Rückfallzeit der Helligkeit  oder     Rückfallzeit auf Uhrzeitabhängiges Dimmen nach absoluten/relativen Dimmen (ab R5.0) 
#define EE_PARAMETER_4687_TEMPMAX     0x4687 //!< Addr: 0x4687, Size: 0x0008, TempMax
#define EE_PARAMETER_4688_CURRENTMAX     0x4688 //!< Addr: 0x4688, Size: 0x0008, CurrentMax
#define EE_PARAMETER_4689_SECUREMODE     0x4689 //!< Addr: 0x4689, Size: 0x0008, SecureMode
#define EE_PARAMETER_467E_GERAETEANLAUFZEIT_S     0x467E //!< Addr: 0x467E, Size: 0x0010, Geräteanlaufzeit s 
#define EE_PARAMETER_46B7_KANAL_AUSSCHALTEN_MIT_REL_DIMMEN     0x46B7 //!< Addr: 0x46B7, Size: 0x0001, Kanal ausschalten mit rel. Dimmen 
#define EE_PARAMETER_4682_SINGLEWHITE     0x4682 //!< Addr: 0x4682, Size: 0x0008, SingleWhite
#define EE_PARAMETER_468A_PWM_FREQUENZ     0x468A //!< Addr: 0x468A, Size: 0x0008, PWM Frequenz 
#define EE_PARAMETER_467C_IN_BETRIEB_ZYKLISCH_SENDEN     0x467C //!< Addr: 0x467C, Size: 0x0010, "In Betrieb" zyklisch senden 
#define EE_PARAMETER_4683_EINSTELLUNG_KANAELE     0x4683 //!< Addr: 0x4683, Size: 0x0008, Einstellung Kanäle 
#define EE_PARAMETER_4685_AUSSCHALTVERZOEGERUNG_DES_RELAIS_IN_SEQUENZ     0x4685 //!< Addr: 0x4685, Size: 0x0008,     Ausschaltverzögerung des Relais in Sequenz 
#define EE_PARAMETER_46EB_VERHALTEN_BEI_BUSSPANNUNGSAUSFALL     0x46EB //!< Addr: 0x46EB, Size: 0x0008,     Verhalten bei Busspannungsausfall 
#define EE_PARAMETER_4686_EINSCHALTHELLIGKEIT_BEI_HANDBEDIENUNG     0x4686 //!< Addr: 0x4686, Size: 0x0008, Einschalthelligkeit bei Handbedienung 
#define EE_PARAMETER_468D_TAG_NACHT_OBJEKT     0x468D //!< Addr: 0x468D, Size: 0x0008, Tag/Nacht Objekt 
#define EE_PARAMETER_468E_WERT_FUER_TAG_NACHT     0x468E //!< Addr: 0x468E, Size: 0x0008,     Wert für Tag/Nacht 
#define EE_PARAMETER_468F_TAG_NACHT_LICHT_UMSCHALTEN     0x468F //!< Addr: 0x468F, Size: 0x0008,     Tag/Nacht Licht umschalten 
#define EE_PARAMETER_4DB3_AUTOMATISCHE_UMSCHALTUNG_DER_SOMMERZEIT     0x4DB3 //!< Addr: 0x4DB3, Size: 0x0008, Automatische Umschaltung der Sommerzeit 
#define EE_PARAMETER_4DB4_BREITE     0x4DB4 //!< Addr: 0x4DB4, Size: 0x0008, Breite 
#define EE_PARAMETER_4DB5_BREITE_IN_GRAD_0_GRAD_90_GRAD_GRAD     0x4DB5 //!< Addr: 0x4DB5, Size: 0x0008, Breite in Grad [0° - 90°] ° 
#define EE_PARAMETER_4DB6_BREITE_IN_MINUTE_0_59     0x4DB6 //!< Addr: 0x4DB6, Size: 0x0008, Breite in Minute [0' - 59'] ' 
#define EE_PARAMETER_4DB7_LAENGE     0x4DB7 //!< Addr: 0x4DB7, Size: 0x0008, Länge 
#define EE_PARAMETER_4DB8_LAENGE_IN_GRAD_0_GRAD_180_GRAD_GRAD     0x4DB8 //!< Addr: 0x4DB8, Size: 0x0008, Länge in Grad [0° - 180°] ° 
#define EE_PARAMETER_4DB9_LAENGE_IN_MINUTE_0_59     0x4DB9 //!< Addr: 0x4DB9, Size: 0x0008, Länge in Minute [0' - 59'] ' 
#define EE_PARAMETER_4DBA_ZEITDIFFERENZ_ZUR_WELTZEIT_UTC     0x4DBA //!< Addr: 0x4DBA, Size: 0x0008, Zeitdifferenz zur Weltzeit (UTC + ...) 
#define EE_PARAMETER_46FE_EINSCHALTWERT_NACHT     0x46FE //!< Addr: 0x46FE, Size: 0x0008,     Einschaltwert Nacht 
#define EE_PARAMETER_4700_EINSCHALTGESCHWINDIGKEIT_NACHT_S     0x4700 //!< Addr: 0x4700, Size: 0x0010,     Einschaltgeschwindigkeit Nacht s 
#define EE_PARAMETER_4702_AUSSCHALTGESCHWINDIGKEIT_NACHT_S     0x4702 //!< Addr: 0x4702, Size: 0x0010,     Ausschaltgeschwindigkeit Nacht s 
#define EE_PARAMETER_4704_MAXIMALE_HELLIGKEIT_NACHT     0x4704 //!< Addr: 0x4704, Size: 0x0008, Maximale Helligkeit Nacht 
#define EE_PARAMETER_470D_STATUS_DIMMWERT_BEI_GESPERRTER_AKTION_SENDEN     0x470D //!< Addr: 0x470D, Size: 0x0001, Status Dimmwert bei gesperrter Aktion senden 
#define EE_PARAMETER_470C_DIMMBEREICH_UNTER_MINIMALWERT_BEIM_EIN_AUSSCHALTEN     0x470C //!< Addr: 0x470C, Size: 0x0001, Dimmbereich unter Minimalwert beim Ein-/Ausschalten 
#define EE_PARAMETER_4699_EINSCHALTEN     0x4699 //!< Addr: 0x4699, Size: 0x0001,     Einschalten 
#define EE_PARAMETER_4698_RELATIVES_DIMMEN     0x4698 //!< Addr: 0x4698, Size: 0x0001,     Relatives Dimmen 
#define EE_PARAMETER_4696_SZENEN     0x4696 //!< Addr: 0x4696, Size: 0x0001,     Szenen 
#define EE_PARAMETER_4707_AKTION     0x4707 //!< Addr: 0x4707, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4708_AKTION     0x4708 //!< Addr: 0x4708, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4709_AKTION     0x4709 //!< Addr: 0x4709, Size: 0x0008,     Aktion 
#define EE_PARAMETER_470A_AKTION     0x470A //!< Addr: 0x470A, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4718_DIMMGESCHWINDIGKEIT_S     0x4718 //!< Addr: 0x4718, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_471A_DIMMGESCHWINDIGKEIT_S     0x471A //!< Addr: 0x471A, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_471C_DIMMGESCHWINDIGKEIT_S     0x471C //!< Addr: 0x471C, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_471E_DIMMGESCHWINDIGKEIT_S     0x471E //!< Addr: 0x471E, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_470B_AKTION     0x470B //!< Addr: 0x470B, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4720_DIMMGESCHWINDIGKEIT_S     0x4720 //!< Addr: 0x4720, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_470C_AKTION     0x470C //!< Addr: 0x470C, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4722_DIMMGESCHWINDIGKEIT_S     0x4722 //!< Addr: 0x4722, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_470D_AKTION     0x470D //!< Addr: 0x470D, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4724_DIMMGESCHWINDIGKEIT_S     0x4724 //!< Addr: 0x4724, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_470E_AKTION     0x470E //!< Addr: 0x470E, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4726_DIMMGESCHWINDIGKEIT_S     0x4726 //!< Addr: 0x4726, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_470F_AKTION_BEI_AUS     0x470F //!< Addr: 0x470F, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_4710_AKTION_BEI_EIN     0x4710 //!< Addr: 0x4710, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_4728_DIMMGESCHWINDIGKEIT_S     0x4728 //!< Addr: 0x4728, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4712_AKTION_BEI_EIN     0x4712 //!< Addr: 0x4712, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_4711_AKTION_BEI_AUS     0x4711 //!< Addr: 0x4711, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_472A_DIMMGESCHWINDIGKEIT_S     0x472A //!< Addr: 0x472A, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4714_AKTION_BEI_EIN     0x4714 //!< Addr: 0x4714, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_4713_AKTION_BEI_AUS     0x4713 //!< Addr: 0x4713, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_472C_DIMMGESCHWINDIGKEIT_S     0x472C //!< Addr: 0x472C, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4716_AKTION_BEI_EIN     0x4716 //!< Addr: 0x4716, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_4715_AKTION_BEI_AUS     0x4715 //!< Addr: 0x4715, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_472E_DIMMGESCHWINDIGKEIT_S     0x472E //!< Addr: 0x472E, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_46C0_SPERROBJEKT_1_DATENPUNKTTYP     0x46C0 //!< Addr: 0x46C0, Size: 0x0002, Sperrobjekt 1 - Datenpunkttyp 
#define EE_PARAMETER_46C3_AKTION_BEI_OBJEKTWERT_1     0x46C3 //!< Addr: 0x46C3, Size: 0x0001,     Aktion bei Objektwert = 1 
#define EE_PARAMETER_46C2_AKTION_BEI_OBJEKTWERT_ZWANG_AUS     0x46C2 //!< Addr: 0x46C2, Size: 0x0001,     Aktion bei Objektwert Zwang AUS 
#define EE_PARAMETER_46CA_SPERROBJEKT_2_DATENPUNKTTYP     0x46CA //!< Addr: 0x46CA, Size: 0x0002, Sperrobjekt 2 - Datenpunkttyp 
#define EE_PARAMETER_46CD_AKTION_BEI_OBJEKTWERT_1     0x46CD //!< Addr: 0x46CD, Size: 0x0001,     Aktion bei Objektwert = 1 
#define EE_PARAMETER_46CC_AKTION_BEI_OBJEKTWERT_ZWANG_AUS     0x46CC //!< Addr: 0x46CC, Size: 0x0001,     Aktion bei Objektwert Zwang AUS 
#define EE_PARAMETER_46C4_SPERROBJEKT_2_AKTION_BEIM_SPERREN     0x46C4 //!< Addr: 0x46C4, Size: 0x0008,     Sperrobjekt 2 -> Aktion beim Sperren 
#define EE_PARAMETER_46C5_SPERROBJEKT_2_AKTION_BEIM_ENTSPERREN     0x46C5 //!< Addr: 0x46C5, Size: 0x0008,     Sperrobjekt 2 -> Aktion beim Entsperren 
#define EE_PARAMETER_46BE_DIMMGESCHWINDIGKEIT_S     0x46BE //!< Addr: 0x46BE, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_46C8_DIMMGESCHWINDIGKEIT_S     0x46C8 //!< Addr: 0x46C8, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4694_HELLIGKEITSWERT     0x4694 //!< Addr: 0x4694, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_46A8_AUSSCHALTVERZOEGERUNG     0x46A8 //!< Addr: 0x46A8, Size: 0x0010, Ausschaltverzögerung 
#define EE_PARAMETER_46AA_TREPPENLICHTDAUER_S     0x46AA //!< Addr: 0x46AA, Size: 0x0010, Treppenlichtdauer s 
#define EE_PARAMETER_46B4_ABDIMMWERT     0x46B4 //!< Addr: 0x46B4, Size: 0x0008,     Abdimmwert 
#define EE_PARAMETER_46B8_HELLIGKEITSWERT     0x46B8 //!< Addr: 0x46B8, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_46B9_HELLIGKEITSWERT     0x46B9 //!< Addr: 0x46B9, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_4781_OM_AKTORPARAM_ACTIVEDELAYOBJECTS_1     0x4781 //!< Addr: 0x4781, Size: 0x0008, OM_aktorParam_activeDelayObjects_1
#define EE_PARAMETER_4738_EINSCHALTVERZOEGERUNG     0x4738 //!< Addr: 0x4738, Size: 0x0010, Einschaltverzögerung 
#define EE_PARAMETER_473A_AUSSCHALTVERZOEGERUNG     0x473A //!< Addr: 0x473A, Size: 0x0010, Ausschaltverzögerung 
#define EE_PARAMETER_4742_EINSCHALTWERT     0x4742 //!< Addr: 0x4742, Size: 0x0008,     Einschaltwert 
#define EE_PARAMETER_4790_EINSCHALTWERT_NACHT     0x4790 //!< Addr: 0x4790, Size: 0x0008,     Einschaltwert Nacht 
#define EE_PARAMETER_4784_EINSCHALTGESCHWINDIGKEIT_S     0x4784 //!< Addr: 0x4784, Size: 0x0010,     Einschaltgeschwindigkeit s 
#define EE_PARAMETER_4786_AUSSCHALTGESCHWINDIGKEIT_S     0x4786 //!< Addr: 0x4786, Size: 0x0010,     Ausschaltgeschwindigkeit s 
#define EE_PARAMETER_4741_MAXIMALE_HELLIGKEIT     0x4741 //!< Addr: 0x4741, Size: 0x0008, Maximale Helligkeit 
#define EE_PARAMETER_4792_EINSCHALTGESCHWINDIGKEIT_NACHT_S     0x4792 //!< Addr: 0x4792, Size: 0x0010,     Einschaltgeschwindigkeit Nacht s 
#define EE_PARAMETER_4794_AUSSCHALTGESCHWINDIGKEIT_NACHT_S     0x4794 //!< Addr: 0x4794, Size: 0x0010,     Ausschaltgeschwindigkeit Nacht s 
#define EE_PARAMETER_4796_MAXIMALE_HELLIGKEIT_NACHT     0x4796 //!< Addr: 0x4796, Size: 0x0008, Maximale Helligkeit Nacht 
#define EE_PARAMETER_4740_MINIMALE_HELLIGKEIT     0x4740 //!< Addr: 0x4740, Size: 0x0008, Minimale Helligkeit 
#define EE_PARAMETER_4744_DIMMGESCHWINDIGKEIT_REL_DIMMEN_S     0x4744 //!< Addr: 0x4744, Size: 0x0010,     Dimmgeschwindigkeit rel. Dimmen s 
#define EE_PARAMETER_478E_DIMMGESCHWINDIGKEIT_ABS_DIMMEN_S     0x478E //!< Addr: 0x478E, Size: 0x0010,     Dimmgeschwindigkeit abs. Dimmen s 
#define EE_PARAMETER_4749_KANAL_AUSSCHALTEN_MIT_REL_DIMMEN     0x4749 //!< Addr: 0x4749, Size: 0x0001, Kanal ausschalten mit rel. Dimmen 
#define EE_PARAMETER_479E_DIMMBEREICH_UNTER_MINIMALWERT_BEIM_EIN_AUSSCHALTEN     0x479E //!< Addr: 0x479E, Size: 0x0001, Dimmbereich unter Minimalwert beim Ein-/Ausschalten 
#define EE_PARAMETER_477C_STATUS_DIMMWERT_SENDEN     0x477C //!< Addr: 0x477C, Size: 0x0008, Status Dimmwert senden 
#define EE_PARAMETER_479F_STATUS_DIMMWERT_BEI_GESPERRTER_AKTION_SENDEN     0x479F //!< Addr: 0x479F, Size: 0x0001, Status Dimmwert bei gesperrter Aktion senden 
#define EE_PARAMETER_4699_HELLIGKEITSWERT     0x4699 //!< Addr: 0x4699, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_469F_AUSSCHALTEN     0x469F //!< Addr: 0x469F, Size: 0x0001,     Ausschalten 
#define EE_PARAMETER_469E_EINSCHALTEN     0x469E //!< Addr: 0x469E, Size: 0x0001,     Einschalten 
#define EE_PARAMETER_469D_RELATIVES_DIMMEN     0x469D //!< Addr: 0x469D, Size: 0x0001,     Relatives Dimmen 
#define EE_PARAMETER_469C_ABSOLUTES_DIMMEN     0x469C //!< Addr: 0x469C, Size: 0x0001,     Absolutes Dimmen 
#define EE_PARAMETER_469B_SZENEN     0x469B //!< Addr: 0x469B, Size: 0x0001,     Szenen 
#define EE_PARAMETER_4752_SPERROBJEKT_1_DATENPUNKTTYP     0x4752 //!< Addr: 0x4752, Size: 0x0002, Sperrobjekt 1 - Datenpunkttyp 
#define EE_PARAMETER_4755_AKTION_BEI_OBJEKTWERT_1     0x4755 //!< Addr: 0x4755, Size: 0x0001,     Aktion bei Objektwert = 1 
#define EE_PARAMETER_4754_AKTION_BEI_OBJEKTWERT_ZWANG_AUS     0x4754 //!< Addr: 0x4754, Size: 0x0001,     Aktion bei Objektwert Zwang AUS 
#define EE_PARAMETER_474C_SPERROBJEKT_1_AKTION_BEIM_SPERREN     0x474C //!< Addr: 0x474C, Size: 0x0008,     Sperrobjekt 1 -> Aktion beim Sperren 
#define EE_PARAMETER_474A_HELLIGKEITSWERT     0x474A //!< Addr: 0x474A, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_474D_SPERROBJEKT_1_AKTION_BEIM_ENTSPERREN     0x474D //!< Addr: 0x474D, Size: 0x0008,     Sperrobjekt 1 -> Aktion beim Entsperren 
#define EE_PARAMETER_474B_HELLIGKEITSWERT     0x474B //!< Addr: 0x474B, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_4750_DIMMGESCHWINDIGKEIT_S     0x4750 //!< Addr: 0x4750, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_475C_SPERROBJEKT_2_DATENPUNKTTYP     0x475C //!< Addr: 0x475C, Size: 0x0002, Sperrobjekt 2 - Datenpunkttyp 
#define EE_PARAMETER_475F_AKTION_BEI_OBJEKTWERT_1     0x475F //!< Addr: 0x475F, Size: 0x0001,     Aktion bei Objektwert = 1 
#define EE_PARAMETER_475E_AKTION_BEI_OBJEKTWERT_ZWANG_AUS     0x475E //!< Addr: 0x475E, Size: 0x0001,     Aktion bei Objektwert Zwang AUS 
#define EE_PARAMETER_4756_SPERROBJEKT_2_AKTION_BEIM_SPERREN     0x4756 //!< Addr: 0x4756, Size: 0x0008,     Sperrobjekt 2 -> Aktion beim Sperren 
#define EE_PARAMETER_4757_SPERROBJEKT_2_AKTION_BEIM_ENTSPERREN     0x4757 //!< Addr: 0x4757, Size: 0x0008,     Sperrobjekt 2 -> Aktion beim Entsperren 
#define EE_PARAMETER_475A_DIMMGESCHWINDIGKEIT_S     0x475A //!< Addr: 0x475A, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_473C_TREPPENLICHTDAUER_S     0x473C //!< Addr: 0x473C, Size: 0x0010, Treppenlichtdauer s 
#define EE_PARAMETER_4746_ABDIMMWERT     0x4746 //!< Addr: 0x4746, Size: 0x0008,     Abdimmwert 
#define EE_PARAMETER_4737_TREPPENLICHTZEIT_VERLAENGERN     0x4737 //!< Addr: 0x4737, Size: 0x0008, Treppenlichtzeit verlängern 
#define EE_PARAMETER_4745_MANUELLES_AUSSCHALTEN     0x4745 //!< Addr: 0x4745, Size: 0x0001, Manuelles Ausschalten 
#define EE_PARAMETER_4780_SZENE_SPEICHERN     0x4780 //!< Addr: 0x4780, Size: 0x0008, Szene speichern 
#define EE_PARAMETER_476F_SZENENUMMER_A     0x476F //!< Addr: 0x476F, Size: 0x0008, Szenenummer A 
#define EE_PARAMETER_4799_AKTION     0x4799 //!< Addr: 0x4799, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4767_HELLIGKEITSWERT     0x4767 //!< Addr: 0x4767, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_47AA_DIMMGESCHWINDIGKEIT_S     0x47AA //!< Addr: 0x47AA, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4770_SZENENUMMER_B     0x4770 //!< Addr: 0x4770, Size: 0x0008, Szenenummer B 
#define EE_PARAMETER_479A_AKTION     0x479A //!< Addr: 0x479A, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4768_HELLIGKEITSWERT     0x4768 //!< Addr: 0x4768, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_47AC_DIMMGESCHWINDIGKEIT_S     0x47AC //!< Addr: 0x47AC, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4771_SZENENUMMER_C     0x4771 //!< Addr: 0x4771, Size: 0x0008, Szenenummer C 
#define EE_PARAMETER_479B_AKTION     0x479B //!< Addr: 0x479B, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4769_HELLIGKEITSWERT     0x4769 //!< Addr: 0x4769, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_47AE_DIMMGESCHWINDIGKEIT_S     0x47AE //!< Addr: 0x47AE, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4772_SZENENUMMER_D     0x4772 //!< Addr: 0x4772, Size: 0x0008, Szenenummer D 
#define EE_PARAMETER_479C_AKTION     0x479C //!< Addr: 0x479C, Size: 0x0008,     Aktion 
#define EE_PARAMETER_476A_HELLIGKEITSWERT     0x476A //!< Addr: 0x476A, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_47B0_DIMMGESCHWINDIGKEIT_S     0x47B0 //!< Addr: 0x47B0, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4773_SZENENUMMER_E     0x4773 //!< Addr: 0x4773, Size: 0x0008, Szenenummer E 
#define EE_PARAMETER_479D_AKTION     0x479D //!< Addr: 0x479D, Size: 0x0008,     Aktion 
#define EE_PARAMETER_476B_HELLIGKEITSWERT     0x476B //!< Addr: 0x476B, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_47B2_DIMMGESCHWINDIGKEIT_S     0x47B2 //!< Addr: 0x47B2, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4774_SZENENUMMER_F     0x4774 //!< Addr: 0x4774, Size: 0x0008, Szenenummer F 
#define EE_PARAMETER_479E_AKTION     0x479E //!< Addr: 0x479E, Size: 0x0008,     Aktion 
#define EE_PARAMETER_476C_HELLIGKEITSWERT     0x476C //!< Addr: 0x476C, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_47B4_DIMMGESCHWINDIGKEIT_S     0x47B4 //!< Addr: 0x47B4, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4775_SZENENUMMER_G     0x4775 //!< Addr: 0x4775, Size: 0x0008, Szenenummer G 
#define EE_PARAMETER_479F_AKTION     0x479F //!< Addr: 0x479F, Size: 0x0008,     Aktion 
#define EE_PARAMETER_476D_HELLIGKEITSWERT     0x476D //!< Addr: 0x476D, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_47B6_DIMMGESCHWINDIGKEIT_S     0x47B6 //!< Addr: 0x47B6, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4776_SZENENUMMER_H     0x4776 //!< Addr: 0x4776, Size: 0x0008, Szenenummer H 
#define EE_PARAMETER_47A0_AKTION     0x47A0 //!< Addr: 0x47A0, Size: 0x0008,     Aktion 
#define EE_PARAMETER_476E_HELLIGKEITSWERT     0x476E //!< Addr: 0x476E, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_47B8_DIMMGESCHWINDIGKEIT_S     0x47B8 //!< Addr: 0x47B8, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_47A2_AKTION_BEI_EIN     0x47A2 //!< Addr: 0x47A2, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_4760_HELLIGKEITSWERT     0x4760 //!< Addr: 0x4760, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_47A1_AKTION_BEI_AUS     0x47A1 //!< Addr: 0x47A1, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_475F_HELLIGKEITSWERT     0x475F //!< Addr: 0x475F, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_47BA_DIMMGESCHWINDIGKEIT_S     0x47BA //!< Addr: 0x47BA, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_47A4_AKTION_BEI_EIN     0x47A4 //!< Addr: 0x47A4, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_4762_HELLIGKEITSWERT     0x4762 //!< Addr: 0x4762, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_47A3_AKTION_BEI_AUS     0x47A3 //!< Addr: 0x47A3, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_4761_HELLIGKEITSWERT     0x4761 //!< Addr: 0x4761, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_47BC_DIMMGESCHWINDIGKEIT_S     0x47BC //!< Addr: 0x47BC, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_47A6_AKTION_BEI_EIN     0x47A6 //!< Addr: 0x47A6, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_4764_HELLIGKEITSWERT     0x4764 //!< Addr: 0x4764, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_47A5_AKTION_BEI_AUS     0x47A5 //!< Addr: 0x47A5, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_4763_HELLIGKEITSWERT     0x4763 //!< Addr: 0x4763, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_47BE_DIMMGESCHWINDIGKEIT_S     0x47BE //!< Addr: 0x47BE, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_47A8_AKTION_BEI_EIN     0x47A8 //!< Addr: 0x47A8, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_4766_HELLIGKEITSWERT     0x4766 //!< Addr: 0x4766, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_47A7_AKTION_BEI_AUS     0x47A7 //!< Addr: 0x47A7, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_4765_HELLIGKEITSWERT     0x4765 //!< Addr: 0x4765, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_47C0_DIMMGESCHWINDIGKEIT_S     0x47C0 //!< Addr: 0x47C0, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4696_VERHALTEN_NACH_RESET     0x4696 //!< Addr: 0x4696, Size: 0x0004, Verhalten nach Reset 
#define EE_PARAMETER_469B_VERHALTEN_NACH_RESET     0x469B //!< Addr: 0x469B, Size: 0x0004, Verhalten nach Reset 
#define EE_PARAMETER_46C2_HELLIGKEITSWERT     0x46C2 //!< Addr: 0x46C2, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_46C3_HELLIGKEITSWERT     0x46C3 //!< Addr: 0x46C3, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_4754_HELLIGKEITSWERT     0x4754 //!< Addr: 0x4754, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_4755_HELLIGKEITSWERT     0x4755 //!< Addr: 0x4755, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_4813_OM_AKTORPARAM_ACTIVEDELAYOBJECTS_2     0x4813 //!< Addr: 0x4813, Size: 0x0008, OM_aktorParam_activeDelayObjects_2
#define EE_PARAMETER_47CA_EINSCHALTVERZOEGERUNG     0x47CA //!< Addr: 0x47CA, Size: 0x0010, Einschaltverzögerung 
#define EE_PARAMETER_47CC_AUSSCHALTVERZOEGERUNG     0x47CC //!< Addr: 0x47CC, Size: 0x0010, Ausschaltverzögerung 
#define EE_PARAMETER_47D4_EINSCHALTWERT     0x47D4 //!< Addr: 0x47D4, Size: 0x0008,     Einschaltwert 
#define EE_PARAMETER_4822_EINSCHALTWERT_NACHT     0x4822 //!< Addr: 0x4822, Size: 0x0008,     Einschaltwert Nacht 
#define EE_PARAMETER_4816_EINSCHALTGESCHWINDIGKEIT_S     0x4816 //!< Addr: 0x4816, Size: 0x0010,     Einschaltgeschwindigkeit s 
#define EE_PARAMETER_4818_AUSSCHALTGESCHWINDIGKEIT_S     0x4818 //!< Addr: 0x4818, Size: 0x0010,     Ausschaltgeschwindigkeit s 
#define EE_PARAMETER_47D3_MAXIMALE_HELLIGKEIT     0x47D3 //!< Addr: 0x47D3, Size: 0x0008, Maximale Helligkeit 
#define EE_PARAMETER_4824_EINSCHALTGESCHWINDIGKEIT_NACHT_S     0x4824 //!< Addr: 0x4824, Size: 0x0010,     Einschaltgeschwindigkeit Nacht s 
#define EE_PARAMETER_4826_AUSSCHALTGESCHWINDIGKEIT_NACHT_S     0x4826 //!< Addr: 0x4826, Size: 0x0010,     Ausschaltgeschwindigkeit Nacht s 
#define EE_PARAMETER_4828_MAXIMALE_HELLIGKEIT_NACHT     0x4828 //!< Addr: 0x4828, Size: 0x0008, Maximale Helligkeit Nacht 
#define EE_PARAMETER_47D2_MINIMALE_HELLIGKEIT     0x47D2 //!< Addr: 0x47D2, Size: 0x0008, Minimale Helligkeit 
#define EE_PARAMETER_47D6_DIMMGESCHWINDIGKEIT_REL_DIMMEN_S     0x47D6 //!< Addr: 0x47D6, Size: 0x0010,     Dimmgeschwindigkeit rel. Dimmen s 
#define EE_PARAMETER_4820_DIMMGESCHWINDIGKEIT_ABS_DIMMEN_S     0x4820 //!< Addr: 0x4820, Size: 0x0010,     Dimmgeschwindigkeit abs. Dimmen s 
#define EE_PARAMETER_47DB_KANAL_AUSSCHALTEN_MIT_REL_DIMMEN     0x47DB //!< Addr: 0x47DB, Size: 0x0001, Kanal ausschalten mit rel. Dimmen 
#define EE_PARAMETER_4830_DIMMBEREICH_UNTER_MINIMALWERT_BEIM_EIN_AUSSCHALTEN     0x4830 //!< Addr: 0x4830, Size: 0x0001, Dimmbereich unter Minimalwert beim Ein-/Ausschalten 
#define EE_PARAMETER_480E_STATUS_DIMMWERT_SENDEN     0x480E //!< Addr: 0x480E, Size: 0x0008, Status Dimmwert senden 
#define EE_PARAMETER_4831_STATUS_DIMMWERT_BEI_GESPERRTER_AKTION_SENDEN     0x4831 //!< Addr: 0x4831, Size: 0x0001, Status Dimmwert bei gesperrter Aktion senden 
#define EE_PARAMETER_46A0_VERHALTEN_NACH_RESET     0x46A0 //!< Addr: 0x46A0, Size: 0x0004, Verhalten nach Reset 
#define EE_PARAMETER_469E_HELLIGKEITSWERT     0x469E //!< Addr: 0x469E, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_46A4_AUSSCHALTEN     0x46A4 //!< Addr: 0x46A4, Size: 0x0001,     Ausschalten 
#define EE_PARAMETER_46A3_EINSCHALTEN     0x46A3 //!< Addr: 0x46A3, Size: 0x0001,     Einschalten 
#define EE_PARAMETER_46A2_RELATIVES_DIMMEN     0x46A2 //!< Addr: 0x46A2, Size: 0x0001,     Relatives Dimmen 
#define EE_PARAMETER_46A1_ABSOLUTES_DIMMEN     0x46A1 //!< Addr: 0x46A1, Size: 0x0001,     Absolutes Dimmen 
#define EE_PARAMETER_46A0_SZENEN     0x46A0 //!< Addr: 0x46A0, Size: 0x0001,     Szenen 
#define EE_PARAMETER_47E4_SPERROBJEKT_1_DATENPUNKTTYP     0x47E4 //!< Addr: 0x47E4, Size: 0x0002, Sperrobjekt 1 - Datenpunkttyp 
#define EE_PARAMETER_47E7_AKTION_BEI_OBJEKTWERT_1     0x47E7 //!< Addr: 0x47E7, Size: 0x0001,     Aktion bei Objektwert = 1 
#define EE_PARAMETER_47E6_AKTION_BEI_OBJEKTWERT_ZWANG_AUS     0x47E6 //!< Addr: 0x47E6, Size: 0x0001,     Aktion bei Objektwert Zwang AUS 
#define EE_PARAMETER_47DC_HELLIGKEITSWERT     0x47DC //!< Addr: 0x47DC, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_47DF_SPERROBJEKT_1_AKTION_BEIM_ENTSPERREN     0x47DF //!< Addr: 0x47DF, Size: 0x0008,     Sperrobjekt 1 -> Aktion beim Entsperren 
#define EE_PARAMETER_47DD_HELLIGKEITSWERT     0x47DD //!< Addr: 0x47DD, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_47E2_DIMMGESCHWINDIGKEIT_S     0x47E2 //!< Addr: 0x47E2, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_47EE_SPERROBJEKT_2_DATENPUNKTTYP     0x47EE //!< Addr: 0x47EE, Size: 0x0002, Sperrobjekt 2 - Datenpunkttyp 
#define EE_PARAMETER_47F1_AKTION_BEI_OBJEKTWERT_1     0x47F1 //!< Addr: 0x47F1, Size: 0x0001,     Aktion bei Objektwert = 1 
#define EE_PARAMETER_47F0_AKTION_BEI_OBJEKTWERT_ZWANG_AUS     0x47F0 //!< Addr: 0x47F0, Size: 0x0001,     Aktion bei Objektwert Zwang AUS 
#define EE_PARAMETER_47E8_SPERROBJEKT_2_AKTION_BEIM_SPERREN     0x47E8 //!< Addr: 0x47E8, Size: 0x0008,     Sperrobjekt 2 -> Aktion beim Sperren 
#define EE_PARAMETER_47E6_HELLIGKEITSWERT     0x47E6 //!< Addr: 0x47E6, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_47E9_SPERROBJEKT_2_AKTION_BEIM_ENTSPERREN     0x47E9 //!< Addr: 0x47E9, Size: 0x0008,     Sperrobjekt 2 -> Aktion beim Entsperren 
#define EE_PARAMETER_47E7_HELLIGKEITSWERT     0x47E7 //!< Addr: 0x47E7, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_47EC_DIMMGESCHWINDIGKEIT_S     0x47EC //!< Addr: 0x47EC, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_47CE_TREPPENLICHTDAUER_S     0x47CE //!< Addr: 0x47CE, Size: 0x0010, Treppenlichtdauer s 
#define EE_PARAMETER_47D8_ABDIMMWERT     0x47D8 //!< Addr: 0x47D8, Size: 0x0008,     Abdimmwert 
#define EE_PARAMETER_47C9_TREPPENLICHTZEIT_VERLAENGERN     0x47C9 //!< Addr: 0x47C9, Size: 0x0008, Treppenlichtzeit verlängern 
#define EE_PARAMETER_47D7_MANUELLES_AUSSCHALTEN     0x47D7 //!< Addr: 0x47D7, Size: 0x0001, Manuelles Ausschalten 
#define EE_PARAMETER_4812_SZENE_SPEICHERN     0x4812 //!< Addr: 0x4812, Size: 0x0008, Szene speichern 
#define EE_PARAMETER_4801_SZENENUMMER_A     0x4801 //!< Addr: 0x4801, Size: 0x0008, Szenenummer A 
#define EE_PARAMETER_482B_AKTION     0x482B //!< Addr: 0x482B, Size: 0x0008,     Aktion 
#define EE_PARAMETER_47F9_HELLIGKEITSWERT     0x47F9 //!< Addr: 0x47F9, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_483C_DIMMGESCHWINDIGKEIT_S     0x483C //!< Addr: 0x483C, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4802_SZENENUMMER_B     0x4802 //!< Addr: 0x4802, Size: 0x0008, Szenenummer B 
#define EE_PARAMETER_482C_AKTION     0x482C //!< Addr: 0x482C, Size: 0x0008,     Aktion 
#define EE_PARAMETER_47FA_HELLIGKEITSWERT     0x47FA //!< Addr: 0x47FA, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_483E_DIMMGESCHWINDIGKEIT_S     0x483E //!< Addr: 0x483E, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4803_SZENENUMMER_C     0x4803 //!< Addr: 0x4803, Size: 0x0008, Szenenummer C 
#define EE_PARAMETER_482D_AKTION     0x482D //!< Addr: 0x482D, Size: 0x0008,     Aktion 
#define EE_PARAMETER_47FB_HELLIGKEITSWERT     0x47FB //!< Addr: 0x47FB, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_4840_DIMMGESCHWINDIGKEIT_S     0x4840 //!< Addr: 0x4840, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4804_SZENENUMMER_D     0x4804 //!< Addr: 0x4804, Size: 0x0008, Szenenummer D 
#define EE_PARAMETER_482E_AKTION     0x482E //!< Addr: 0x482E, Size: 0x0008,     Aktion 
#define EE_PARAMETER_47FC_HELLIGKEITSWERT     0x47FC //!< Addr: 0x47FC, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_4842_DIMMGESCHWINDIGKEIT_S     0x4842 //!< Addr: 0x4842, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4805_SZENENUMMER_E     0x4805 //!< Addr: 0x4805, Size: 0x0008, Szenenummer E 
#define EE_PARAMETER_482F_AKTION     0x482F //!< Addr: 0x482F, Size: 0x0008,     Aktion 
#define EE_PARAMETER_47FD_HELLIGKEITSWERT     0x47FD //!< Addr: 0x47FD, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_4844_DIMMGESCHWINDIGKEIT_S     0x4844 //!< Addr: 0x4844, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4806_SZENENUMMER_F     0x4806 //!< Addr: 0x4806, Size: 0x0008, Szenenummer F 
#define EE_PARAMETER_4830_AKTION     0x4830 //!< Addr: 0x4830, Size: 0x0008,     Aktion 
#define EE_PARAMETER_47FE_HELLIGKEITSWERT     0x47FE //!< Addr: 0x47FE, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_4846_DIMMGESCHWINDIGKEIT_S     0x4846 //!< Addr: 0x4846, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4807_SZENENUMMER_G     0x4807 //!< Addr: 0x4807, Size: 0x0008, Szenenummer G 
#define EE_PARAMETER_4831_AKTION     0x4831 //!< Addr: 0x4831, Size: 0x0008,     Aktion 
#define EE_PARAMETER_47FF_HELLIGKEITSWERT     0x47FF //!< Addr: 0x47FF, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_4848_DIMMGESCHWINDIGKEIT_S     0x4848 //!< Addr: 0x4848, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4808_SZENENUMMER_H     0x4808 //!< Addr: 0x4808, Size: 0x0008, Szenenummer H 
#define EE_PARAMETER_4832_AKTION     0x4832 //!< Addr: 0x4832, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4800_HELLIGKEITSWERT     0x4800 //!< Addr: 0x4800, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_484A_DIMMGESCHWINDIGKEIT_S     0x484A //!< Addr: 0x484A, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4834_AKTION_BEI_EIN     0x4834 //!< Addr: 0x4834, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_47F2_HELLIGKEITSWERT     0x47F2 //!< Addr: 0x47F2, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_4833_AKTION_BEI_AUS     0x4833 //!< Addr: 0x4833, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_47F1_HELLIGKEITSWERT     0x47F1 //!< Addr: 0x47F1, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_484C_DIMMGESCHWINDIGKEIT_S     0x484C //!< Addr: 0x484C, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4836_AKTION_BEI_EIN     0x4836 //!< Addr: 0x4836, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_47F4_HELLIGKEITSWERT     0x47F4 //!< Addr: 0x47F4, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_4835_AKTION_BEI_AUS     0x4835 //!< Addr: 0x4835, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_47F3_HELLIGKEITSWERT     0x47F3 //!< Addr: 0x47F3, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_484E_DIMMGESCHWINDIGKEIT_S     0x484E //!< Addr: 0x484E, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4838_AKTION_BEI_EIN     0x4838 //!< Addr: 0x4838, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_47F6_HELLIGKEITSWERT     0x47F6 //!< Addr: 0x47F6, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_4837_AKTION_BEI_AUS     0x4837 //!< Addr: 0x4837, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_47F5_HELLIGKEITSWERT     0x47F5 //!< Addr: 0x47F5, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_4850_DIMMGESCHWINDIGKEIT_S     0x4850 //!< Addr: 0x4850, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_483A_AKTION_BEI_EIN     0x483A //!< Addr: 0x483A, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_47F8_HELLIGKEITSWERT     0x47F8 //!< Addr: 0x47F8, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_4839_AKTION_BEI_AUS     0x4839 //!< Addr: 0x4839, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_47F7_HELLIGKEITSWERT     0x47F7 //!< Addr: 0x47F7, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_4852_DIMMGESCHWINDIGKEIT_S     0x4852 //!< Addr: 0x4852, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_48A5_OM_AKTORPARAM_ACTIVEDELAYOBJECTS_3     0x48A5 //!< Addr: 0x48A5, Size: 0x0008, OM_aktorParam_activeDelayObjects_3
#define EE_PARAMETER_485C_EINSCHALTVERZOEGERUNG     0x485C //!< Addr: 0x485C, Size: 0x0010, Einschaltverzögerung 
#define EE_PARAMETER_485E_AUSSCHALTVERZOEGERUNG     0x485E //!< Addr: 0x485E, Size: 0x0010, Ausschaltverzögerung 
#define EE_PARAMETER_4866_EINSCHALTWERT     0x4866 //!< Addr: 0x4866, Size: 0x0008,     Einschaltwert 
#define EE_PARAMETER_48B4_EINSCHALTWERT_NACHT     0x48B4 //!< Addr: 0x48B4, Size: 0x0008,     Einschaltwert Nacht 
#define EE_PARAMETER_48A8_EINSCHALTGESCHWINDIGKEIT_S     0x48A8 //!< Addr: 0x48A8, Size: 0x0010,     Einschaltgeschwindigkeit s 
#define EE_PARAMETER_48AA_AUSSCHALTGESCHWINDIGKEIT_S     0x48AA //!< Addr: 0x48AA, Size: 0x0010,     Ausschaltgeschwindigkeit s 
#define EE_PARAMETER_4865_MAXIMALE_HELLIGKEIT     0x4865 //!< Addr: 0x4865, Size: 0x0008, Maximale Helligkeit 
#define EE_PARAMETER_48B6_EINSCHALTGESCHWINDIGKEIT_NACHT_S     0x48B6 //!< Addr: 0x48B6, Size: 0x0010,     Einschaltgeschwindigkeit Nacht s 
#define EE_PARAMETER_48B8_AUSSCHALTGESCHWINDIGKEIT_NACHT_S     0x48B8 //!< Addr: 0x48B8, Size: 0x0010,     Ausschaltgeschwindigkeit Nacht s 
#define EE_PARAMETER_48BA_MAXIMALE_HELLIGKEIT_NACHT     0x48BA //!< Addr: 0x48BA, Size: 0x0008, Maximale Helligkeit Nacht 
#define EE_PARAMETER_4864_MINIMALE_HELLIGKEIT     0x4864 //!< Addr: 0x4864, Size: 0x0008, Minimale Helligkeit 
#define EE_PARAMETER_4868_DIMMGESCHWINDIGKEIT_REL_DIMMEN_S     0x4868 //!< Addr: 0x4868, Size: 0x0010,     Dimmgeschwindigkeit rel. Dimmen s 
#define EE_PARAMETER_48B2_DIMMGESCHWINDIGKEIT_ABS_DIMMEN_S     0x48B2 //!< Addr: 0x48B2, Size: 0x0010,     Dimmgeschwindigkeit abs. Dimmen s 
#define EE_PARAMETER_486D_KANAL_AUSSCHALTEN_MIT_REL_DIMMEN     0x486D //!< Addr: 0x486D, Size: 0x0001, Kanal ausschalten mit rel. Dimmen 
#define EE_PARAMETER_48C2_DIMMBEREICH_UNTER_MINIMALWERT_BEIM_EIN_AUSSCHALTEN     0x48C2 //!< Addr: 0x48C2, Size: 0x0001, Dimmbereich unter Minimalwert beim Ein-/Ausschalten 
#define EE_PARAMETER_48A0_STATUS_DIMMWERT_SENDEN     0x48A0 //!< Addr: 0x48A0, Size: 0x0008, Status Dimmwert senden 
#define EE_PARAMETER_48C3_STATUS_DIMMWERT_BEI_GESPERRTER_AKTION_SENDEN     0x48C3 //!< Addr: 0x48C3, Size: 0x0001, Status Dimmwert bei gesperrter Aktion senden 
#define EE_PARAMETER_46A5_VERHALTEN_NACH_RESET     0x46A5 //!< Addr: 0x46A5, Size: 0x0004, Verhalten nach Reset 
#define EE_PARAMETER_46A3_HELLIGKEITSWERT     0x46A3 //!< Addr: 0x46A3, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_46A9_AUSSCHALTEN     0x46A9 //!< Addr: 0x46A9, Size: 0x0001,     Ausschalten 
#define EE_PARAMETER_46A8_EINSCHALTEN     0x46A8 //!< Addr: 0x46A8, Size: 0x0001,     Einschalten 
#define EE_PARAMETER_46A7_RELATIVES_DIMMEN     0x46A7 //!< Addr: 0x46A7, Size: 0x0001,     Relatives Dimmen 
#define EE_PARAMETER_46A6_ABSOLUTES_DIMMEN     0x46A6 //!< Addr: 0x46A6, Size: 0x0001,     Absolutes Dimmen 
#define EE_PARAMETER_46A5_SZENEN     0x46A5 //!< Addr: 0x46A5, Size: 0x0001,     Szenen 
#define EE_PARAMETER_4876_SPERROBJEKT_1_DATENPUNKTTYP     0x4876 //!< Addr: 0x4876, Size: 0x0002, Sperrobjekt 1 - Datenpunkttyp 
#define EE_PARAMETER_4879_AKTION_BEI_OBJEKTWERT_1     0x4879 //!< Addr: 0x4879, Size: 0x0001,     Aktion bei Objektwert = 1 
#define EE_PARAMETER_4878_AKTION_BEI_OBJEKTWERT_ZWANG_AUS     0x4878 //!< Addr: 0x4878, Size: 0x0001,     Aktion bei Objektwert Zwang AUS 
#define EE_PARAMETER_486E_HELLIGKEITSWERT     0x486E //!< Addr: 0x486E, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_4871_SPERROBJEKT_1_AKTION_BEIM_ENTSPERREN     0x4871 //!< Addr: 0x4871, Size: 0x0008,     Sperrobjekt 1 -> Aktion beim Entsperren 
#define EE_PARAMETER_486F_HELLIGKEITSWERT     0x486F //!< Addr: 0x486F, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_4874_DIMMGESCHWINDIGKEIT_S     0x4874 //!< Addr: 0x4874, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4880_SPERROBJEKT_2_DATENPUNKTTYP     0x4880 //!< Addr: 0x4880, Size: 0x0002, Sperrobjekt 2 - Datenpunkttyp 
#define EE_PARAMETER_4883_AKTION_BEI_OBJEKTWERT_1     0x4883 //!< Addr: 0x4883, Size: 0x0001,     Aktion bei Objektwert = 1 
#define EE_PARAMETER_4882_AKTION_BEI_OBJEKTWERT_ZWANG_AUS     0x4882 //!< Addr: 0x4882, Size: 0x0001,     Aktion bei Objektwert Zwang AUS 
#define EE_PARAMETER_487A_SPERROBJEKT_2_AKTION_BEIM_SPERREN     0x487A //!< Addr: 0x487A, Size: 0x0008,     Sperrobjekt 2 -> Aktion beim Sperren 
#define EE_PARAMETER_4878_HELLIGKEITSWERT     0x4878 //!< Addr: 0x4878, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_487B_SPERROBJEKT_2_AKTION_BEIM_ENTSPERREN     0x487B //!< Addr: 0x487B, Size: 0x0008,     Sperrobjekt 2 -> Aktion beim Entsperren 
#define EE_PARAMETER_4879_HELLIGKEITSWERT     0x4879 //!< Addr: 0x4879, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_487E_DIMMGESCHWINDIGKEIT_S     0x487E //!< Addr: 0x487E, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4860_TREPPENLICHTDAUER_S     0x4860 //!< Addr: 0x4860, Size: 0x0010, Treppenlichtdauer s 
#define EE_PARAMETER_486A_ABDIMMWERT     0x486A //!< Addr: 0x486A, Size: 0x0008,     Abdimmwert 
#define EE_PARAMETER_485B_TREPPENLICHTZEIT_VERLAENGERN     0x485B //!< Addr: 0x485B, Size: 0x0008, Treppenlichtzeit verlängern 
#define EE_PARAMETER_4869_MANUELLES_AUSSCHALTEN     0x4869 //!< Addr: 0x4869, Size: 0x0001, Manuelles Ausschalten 
#define EE_PARAMETER_48A4_SZENE_SPEICHERN     0x48A4 //!< Addr: 0x48A4, Size: 0x0008, Szene speichern 
#define EE_PARAMETER_4893_SZENENUMMER_A     0x4893 //!< Addr: 0x4893, Size: 0x0008, Szenenummer A 
#define EE_PARAMETER_48BD_AKTION     0x48BD //!< Addr: 0x48BD, Size: 0x0008,     Aktion 
#define EE_PARAMETER_488B_HELLIGKEITSWERT     0x488B //!< Addr: 0x488B, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_48CE_DIMMGESCHWINDIGKEIT_S     0x48CE //!< Addr: 0x48CE, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4894_SZENENUMMER_B     0x4894 //!< Addr: 0x4894, Size: 0x0008, Szenenummer B 
#define EE_PARAMETER_48BE_AKTION     0x48BE //!< Addr: 0x48BE, Size: 0x0008,     Aktion 
#define EE_PARAMETER_488C_HELLIGKEITSWERT     0x488C //!< Addr: 0x488C, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_48D0_DIMMGESCHWINDIGKEIT_S     0x48D0 //!< Addr: 0x48D0, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4895_SZENENUMMER_C     0x4895 //!< Addr: 0x4895, Size: 0x0008, Szenenummer C 
#define EE_PARAMETER_48BF_AKTION     0x48BF //!< Addr: 0x48BF, Size: 0x0008,     Aktion 
#define EE_PARAMETER_488D_HELLIGKEITSWERT     0x488D //!< Addr: 0x488D, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_48D2_DIMMGESCHWINDIGKEIT_S     0x48D2 //!< Addr: 0x48D2, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4896_SZENENUMMER_D     0x4896 //!< Addr: 0x4896, Size: 0x0008, Szenenummer D 
#define EE_PARAMETER_48C0_AKTION     0x48C0 //!< Addr: 0x48C0, Size: 0x0008,     Aktion 
#define EE_PARAMETER_488E_HELLIGKEITSWERT     0x488E //!< Addr: 0x488E, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_48D4_DIMMGESCHWINDIGKEIT_S     0x48D4 //!< Addr: 0x48D4, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4897_SZENENUMMER_E     0x4897 //!< Addr: 0x4897, Size: 0x0008, Szenenummer E 
#define EE_PARAMETER_48C1_AKTION     0x48C1 //!< Addr: 0x48C1, Size: 0x0008,     Aktion 
#define EE_PARAMETER_488F_HELLIGKEITSWERT     0x488F //!< Addr: 0x488F, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_48D6_DIMMGESCHWINDIGKEIT_S     0x48D6 //!< Addr: 0x48D6, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4898_SZENENUMMER_F     0x4898 //!< Addr: 0x4898, Size: 0x0008, Szenenummer F 
#define EE_PARAMETER_48C2_AKTION     0x48C2 //!< Addr: 0x48C2, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4890_HELLIGKEITSWERT     0x4890 //!< Addr: 0x4890, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_48D8_DIMMGESCHWINDIGKEIT_S     0x48D8 //!< Addr: 0x48D8, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4899_SZENENUMMER_G     0x4899 //!< Addr: 0x4899, Size: 0x0008, Szenenummer G 
#define EE_PARAMETER_48C3_AKTION     0x48C3 //!< Addr: 0x48C3, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4891_HELLIGKEITSWERT     0x4891 //!< Addr: 0x4891, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_48DA_DIMMGESCHWINDIGKEIT_S     0x48DA //!< Addr: 0x48DA, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_489A_SZENENUMMER_H     0x489A //!< Addr: 0x489A, Size: 0x0008, Szenenummer H 
#define EE_PARAMETER_48C4_AKTION     0x48C4 //!< Addr: 0x48C4, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4892_HELLIGKEITSWERT     0x4892 //!< Addr: 0x4892, Size: 0x0008,     Helligkeitswert 
#define EE_PARAMETER_48DC_DIMMGESCHWINDIGKEIT_S     0x48DC //!< Addr: 0x48DC, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_48C6_AKTION_BEI_EIN     0x48C6 //!< Addr: 0x48C6, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_4884_HELLIGKEITSWERT     0x4884 //!< Addr: 0x4884, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_48C5_AKTION_BEI_AUS     0x48C5 //!< Addr: 0x48C5, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_4883_HELLIGKEITSWERT     0x4883 //!< Addr: 0x4883, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_48DE_DIMMGESCHWINDIGKEIT_S     0x48DE //!< Addr: 0x48DE, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_48C8_AKTION_BEI_EIN     0x48C8 //!< Addr: 0x48C8, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_4886_HELLIGKEITSWERT     0x4886 //!< Addr: 0x4886, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_48C7_AKTION_BEI_AUS     0x48C7 //!< Addr: 0x48C7, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_4885_HELLIGKEITSWERT     0x4885 //!< Addr: 0x4885, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_48E0_DIMMGESCHWINDIGKEIT_S     0x48E0 //!< Addr: 0x48E0, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_48CA_AKTION_BEI_EIN     0x48CA //!< Addr: 0x48CA, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_4888_HELLIGKEITSWERT     0x4888 //!< Addr: 0x4888, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_48C9_AKTION_BEI_AUS     0x48C9 //!< Addr: 0x48C9, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_4887_HELLIGKEITSWERT     0x4887 //!< Addr: 0x4887, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_48E2_DIMMGESCHWINDIGKEIT_S     0x48E2 //!< Addr: 0x48E2, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_48CC_AKTION_BEI_EIN     0x48CC //!< Addr: 0x48CC, Size: 0x0008,     Aktion bei "Ein" 
#define EE_PARAMETER_488A_HELLIGKEITSWERT     0x488A //!< Addr: 0x488A, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_48CB_AKTION_BEI_AUS     0x48CB //!< Addr: 0x48CB, Size: 0x0008,     Aktion bei "Aus" 
#define EE_PARAMETER_4889_HELLIGKEITSWERT     0x4889 //!< Addr: 0x4889, Size: 0x0008,         Helligkeitswert 
#define EE_PARAMETER_48E4_DIMMGESCHWINDIGKEIT_S     0x48E4 //!< Addr: 0x48E4, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_49D2_EINSCHALTVERZOEGERUNG_S     0x49D2 //!< Addr: 0x49D2, Size: 0x0010,     Einschaltverzögerung s 
#define EE_PARAMETER_4A14_TREPPENLICHTDAUER_S     0x4A14 //!< Addr: 0x4A14, Size: 0x0010,     Treppenlichtdauer s 
#define EE_PARAMETER_4A1B_TREPPENLICHTZEIT_VERLAENGERN     0x4A1B //!< Addr: 0x4A1B, Size: 0x0008,     Treppenlichtzeit verlängern 
#define EE_PARAMETER_4A1C_MANUELLES_AUSSCHALTEN     0x4A1C //!< Addr: 0x4A1C, Size: 0x0008,     Manuelles Ausschalten 
#define EE_PARAMETER_49D4_AUSSCHALTVERZOEGERUNG_S     0x49D4 //!< Addr: 0x49D4, Size: 0x0010,     Ausschaltverzögerung s 
#define EE_PARAMETER_49D9_EINSCHALTWERT_GRUEN     0x49D9 //!< Addr: 0x49D9, Size: 0x0008,     Einschaltwert Grün 
#define EE_PARAMETER_49DA_EINSCHALTWERT_BLAU     0x49DA //!< Addr: 0x49DA, Size: 0x0008,     Einschaltwert Blau 
#define EE_PARAMETER_49DB_EINSCHALTWERT_WEISS     0x49DB //!< Addr: 0x49DB, Size: 0x0008,     Einschaltwert Weiß 
#define EE_PARAMETER_49DD_EINSCHALTWERT_GRUEN_NACHT     0x49DD //!< Addr: 0x49DD, Size: 0x0008,     Einschaltwert Grün  Nacht 
#define EE_PARAMETER_49DE_EINSCHALTWERT_BLAU_NACHT     0x49DE //!< Addr: 0x49DE, Size: 0x0008,     Einschaltwert Blau  Nacht 
#define EE_PARAMETER_49DF_EINSCHALTWERT_WEISS_NACHT     0x49DF //!< Addr: 0x49DF, Size: 0x0008,     Einschaltwert Weiß  Nacht 
#define EE_PARAMETER_493C_EINSCHALTGESCHWINDIGKEIT_S     0x493C //!< Addr: 0x493C, Size: 0x0010,     Einschaltgeschwindigkeit s 
#define EE_PARAMETER_4940_AUSSCHALTGESCHWINDIGKEIT_S     0x4940 //!< Addr: 0x4940, Size: 0x0010,     Ausschaltgeschwindigkeit s 
#define EE_PARAMETER_49E5_EINSCHALTEN_MIT_REL_DIMMEN_FARBTON_H     0x49E5 //!< Addr: 0x49E5, Size: 0x0008, Einschalten mit rel. Dimmen Farbton H 
#define EE_PARAMETER_49E6_EINSCHALTEN_MIT_REL_DIMMEN_FARBSAETTIGUNG_S     0x49E6 //!< Addr: 0x49E6, Size: 0x0008, Einschalten mit rel. Dimmen Farbsättigung S 
#define EE_PARAMETER_49E7_AUSSCHALTEN_MIT_REL_DIMMEN_HELLIGKEIT_V     0x49E7 //!< Addr: 0x49E7, Size: 0x0008, Ausschalten mit rel. Dimmen Helligkeit V 
#define EE_PARAMETER_49E8_EINSCHALTEN_MIT_REL_DIMMEN_FARBTEMPERATUR     0x49E8 //!< Addr: 0x49E8, Size: 0x0008, Einschalten mit rel. Dimmen Farbtemperatur 
#define EE_PARAMETER_4932_RELATIVES_DIMMEN_FARBSAETTIGUNG_S_S     0x4932 //!< Addr: 0x4932, Size: 0x0010,     Relatives Dimmen Farbsättigung S s 
#define EE_PARAMETER_4934_RELATIVES_DIMMEN_HELLIGKEIT_V_S     0x4934 //!< Addr: 0x4934, Size: 0x0010,     Relatives Dimmen Helligkeit V s 
#define EE_PARAMETER_4936_RELATIVES_DIMMEN_FARBTEMPERATUR_S     0x4936 //!< Addr: 0x4936, Size: 0x0010,     Relatives Dimmen Farbtemperatur s 
#define EE_PARAMETER_4922_WEISSABGLEICH_SKALIERUNG     0x4922 //!< Addr: 0x4922, Size: 0x0008, Weißabgleich / Skalierung 
#define EE_PARAMETER_4923_SKALIERUNG_ROT     0x4923 //!< Addr: 0x4923, Size: 0x0008,     Skalierung Rot 
#define EE_PARAMETER_4924_SKALIERUNG_GRUEN     0x4924 //!< Addr: 0x4924, Size: 0x0008,     Skalierung Grün 
#define EE_PARAMETER_4925_SKALIERUNG_BLAU     0x4925 //!< Addr: 0x4925, Size: 0x0008,     Skalierung Blau 
#define EE_PARAMETER_4926_SKALIERUNG_WEISS     0x4926 //!< Addr: 0x4926, Size: 0x0008,     Skalierung Weiß 
#define EE_PARAMETER_49E2_RESETWERT_GRUEN     0x49E2 //!< Addr: 0x49E2, Size: 0x0008,     Resetwert Grün 
#define EE_PARAMETER_49E3_RESETWERT_BLAU     0x49E3 //!< Addr: 0x49E3, Size: 0x0008,     Resetwert Blau 
#define EE_PARAMETER_49E4_RESETWERT_WEISS     0x49E4 //!< Addr: 0x49E4, Size: 0x0008,     Resetwert Weiß 
#define EE_PARAMETER_4A26_FARBTEMPERATUR_VON_WARMWEISS_KELVIN     0x4A26 //!< Addr: 0x4A26, Size: 0x0010, Farbtemperatur von Warmweiß Kelvin 
#define EE_PARAMETER_4A28_FARBTEMPERATUR_VON_KALTWEISS_KELVIN     0x4A28 //!< Addr: 0x4A28, Size: 0x0010, Farbtemperatur von Kaltweiß Kelvin 
#define EE_PARAMETER_4A21_KANAL_A_ROT_ANTEIL     0x4A21 //!< Addr: 0x4A21, Size: 0x0008,     Kanal A (Rot-Anteil) 
#define EE_PARAMETER_4A22_KANAL_B_GRUEN_ANTEIL     0x4A22 //!< Addr: 0x4A22, Size: 0x0008,     Kanal B (Grün-Anteil) 
#define EE_PARAMETER_4A23_KANAL_C_BLAU_ANTEIL     0x4A23 //!< Addr: 0x4A23, Size: 0x0008,     Kanal C (Blau-Anteil) 
#define EE_PARAMETER_4A24_KANAL_D_WEISS_ANTEIL     0x4A24 //!< Addr: 0x4A24, Size: 0x0008,     Kanal D (Weiß-Anteil) 
#define EE_PARAMETER_4A1D_KANAL_A_ROT_ANTEIL     0x4A1D //!< Addr: 0x4A1D, Size: 0x0008,     Kanal A (Rot-Anteil) 
#define EE_PARAMETER_4A1E_KANAL_B_GRUEN_ANTEIL     0x4A1E //!< Addr: 0x4A1E, Size: 0x0008,     Kanal B (Grün-Anteil) 
#define EE_PARAMETER_4A1F_KANAL_C_BLAU_ANTEIL     0x4A1F //!< Addr: 0x4A1F, Size: 0x0008,     Kanal C (Blau-Anteil) 
#define EE_PARAMETER_4A20_KANAL_D_WEISS_ANTEIL     0x4A20 //!< Addr: 0x4A20, Size: 0x0008,     Kanal D (Weiß-Anteil) 
#define EE_PARAMETER_4A2B_FARBTEMPERATUR_WENN_KLEINER_HELLIGKEITSSCHWELLE_1_DUNKEL     0x4A2B //!< Addr: 0x4A2B, Size: 0x0008,     Farbtemperatur, wenn kleiner Helligkeitsschwelle 1 (dunkel) 
#define EE_PARAMETER_4A2C_FARBTEMPERATUR_WENN_GROESSER_HELLIGKEITSSCHWELLE_2_HELL     0x4A2C //!< Addr: 0x4A2C, Size: 0x0008,     Farbtemperatur, wenn größer Helligkeitsschwelle 2 (hell) 
#define EE_PARAMETER_499E_DIMMGESCHWINDIGKEIT_S     0x499E //!< Addr: 0x499E, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_49A9_SAETTIGUNG_S_BIT_WERT_1     0x49A9 //!< Addr: 0x49A9, Size: 0x0008,         Sättigung S Bit Wert 1 
#define EE_PARAMETER_49AA_HELLIGKEIT_V_BIT_WERT_1     0x49AA //!< Addr: 0x49AA, Size: 0x0008,         Helligkeit V Bit Wert 1 
#define EE_PARAMETER_49AB_FARBWERT_WEISS_BIT_WERT_1     0x49AB //!< Addr: 0x49AB, Size: 0x0008,         Farbwert Weiß Bit Wert 1 
#define EE_PARAMETER_49A5_SAETTIGUNG_S_BIT_WERT_0     0x49A5 //!< Addr: 0x49A5, Size: 0x0008,         Sättigung S Bit Wert 0 
#define EE_PARAMETER_49A6_HELLIGKEIT_V_BIT_WERT_0     0x49A6 //!< Addr: 0x49A6, Size: 0x0008,         Helligkeit V Bit Wert 0 
#define EE_PARAMETER_49A7_FARBWERT_WEISS_BIT_WERT_0     0x49A7 //!< Addr: 0x49A7, Size: 0x0008,         Farbwert Weiß Bit Wert 0 
#define EE_PARAMETER_49AC_DIMMGESCHWINDIGKEIT_S     0x49AC //!< Addr: 0x49AC, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_49B7_SAETTIGUNG_S_BIT_WERT_1     0x49B7 //!< Addr: 0x49B7, Size: 0x0008,         Sättigung S Bit Wert 1 
#define EE_PARAMETER_49B8_HELLIGKEIT_V_BIT_WERT_1     0x49B8 //!< Addr: 0x49B8, Size: 0x0008,         Helligkeit V Bit Wert 1 
#define EE_PARAMETER_49B9_FARBWERT_WEISS_BIT_WERT_1     0x49B9 //!< Addr: 0x49B9, Size: 0x0008,         Farbwert Weiß Bit Wert 1 
#define EE_PARAMETER_49B3_SAETTIGUNG_S_BIT_WERT_0     0x49B3 //!< Addr: 0x49B3, Size: 0x0008,         Sättigung S Bit Wert 0 
#define EE_PARAMETER_49B4_HELLIGKEIT_V_BIT_WERT_0     0x49B4 //!< Addr: 0x49B4, Size: 0x0008,         Helligkeit V Bit Wert 0 
#define EE_PARAMETER_49B5_FARBWERT_WEISS_BIT_WERT_0     0x49B5 //!< Addr: 0x49B5, Size: 0x0008,         Farbwert Weiß Bit Wert 0 
#define EE_PARAMETER_49BA_DIMMGESCHWINDIGKEIT_S     0x49BA //!< Addr: 0x49BA, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_49C5_SAETTIGUNG_S_BIT_WERT_1     0x49C5 //!< Addr: 0x49C5, Size: 0x0008,         Sättigung S Bit Wert 1 
#define EE_PARAMETER_49C6_HELLIGKEIT_V_BIT_WERT_1     0x49C6 //!< Addr: 0x49C6, Size: 0x0008,         Helligkeit V Bit Wert 1 
#define EE_PARAMETER_49C7_FARBWERT_WEISS_BIT_WERT_1     0x49C7 //!< Addr: 0x49C7, Size: 0x0008,         Farbwert Weiß Bit Wert 1 
#define EE_PARAMETER_49C1_SAETTIGUNG_S_BIT_WERT_0     0x49C1 //!< Addr: 0x49C1, Size: 0x0008,         Sättigung S Bit Wert 0 
#define EE_PARAMETER_49C2_HELLIGKEIT_V_BIT_WERT_0     0x49C2 //!< Addr: 0x49C2, Size: 0x0008,         Helligkeit V Bit Wert 0 
#define EE_PARAMETER_49C3_FARBWERT_WEISS_BIT_WERT_0     0x49C3 //!< Addr: 0x49C3, Size: 0x0008,         Farbwert Weiß Bit Wert 0 
#define EE_PARAMETER_49C8_DIMMGESCHWINDIGKEIT_S     0x49C8 //!< Addr: 0x49C8, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4928_OM_RGBWPARAM0_CHVALID     0x4928 //!< Addr: 0x4928, Size: 0x0008, OM_RGBWParam[0]_ChValid
#define EE_PARAMETER_4A3C_OM_RGBWPARAM1_CHVALID     0x4A3C //!< Addr: 0x4A3C, Size: 0x0008, OM_RGBWParam[1]_ChValid
#define EE_PARAMETER_46C0_RUECKFALLZEIT_SPERROBJEKT_1_0_NICHT_AKTIV_S     0x46C0 //!< Addr: 0x46C0, Size: 0x0010,     Rückfallzeit Sperrobjekt 1 (0 = nicht aktiv) s 
#define EE_PARAMETER_46CA_RUECKFALLZEIT_SPERROBJEKT_2_0_NICHT_AKTIV_S     0x46CA //!< Addr: 0x46CA, Size: 0x0010,     Rückfallzeit Sperrobjekt 2 (0 = nicht aktiv) s 
#define EE_PARAMETER_4752_RUECKFALLZEIT_SPERROBJEKT_1_0_NICHT_AKTIV_S     0x4752 //!< Addr: 0x4752, Size: 0x0010,     Rückfallzeit Sperrobjekt 1 (0 = nicht aktiv) s 
#define EE_PARAMETER_475C_RUECKFALLZEIT_SPERROBJEKT_2_0_NICHT_AKTIV_S     0x475C //!< Addr: 0x475C, Size: 0x0010,     Rückfallzeit Sperrobjekt 2 (0 = nicht aktiv) s 
#define EE_PARAMETER_47E4_RUECKFALLZEIT_SPERROBJEKT_1_0_NICHT_AKTIV_S     0x47E4 //!< Addr: 0x47E4, Size: 0x0010,     Rückfallzeit Sperrobjekt 1 (0 = nicht aktiv) s 
#define EE_PARAMETER_47EE_RUECKFALLZEIT_SPERROBJEKT_2_0_NICHT_AKTIV_S     0x47EE //!< Addr: 0x47EE, Size: 0x0010,     Rückfallzeit Sperrobjekt 2 (0 = nicht aktiv) s 
#define EE_PARAMETER_4876_RUECKFALLZEIT_SPERROBJEKT_1_0_NICHT_AKTIV_S     0x4876 //!< Addr: 0x4876, Size: 0x0010,     Rückfallzeit Sperrobjekt 1 (0 = nicht aktiv) s 
#define EE_PARAMETER_4880_RUECKFALLZEIT_SPERROBJEKT_2_0_NICHT_AKTIV_S     0x4880 //!< Addr: 0x4880, Size: 0x0010,     Rückfallzeit Sperrobjekt 2 (0 = nicht aktiv) s 
#define EE_PARAMETER_470A_DIMMGESCHWINDIGKEITEN     0x470A //!< Addr: 0x470A, Size: 0x0001, Dimmgeschwindigkeiten 
#define EE_PARAMETER_479C_DIMMGESCHWINDIGKEITEN     0x479C //!< Addr: 0x479C, Size: 0x0001, Dimmgeschwindigkeiten 
#define EE_PARAMETER_482E_DIMMGESCHWINDIGKEITEN     0x482E //!< Addr: 0x482E, Size: 0x0001, Dimmgeschwindigkeiten 
#define EE_PARAMETER_48C0_DIMMGESCHWINDIGKEITEN     0x48C0 //!< Addr: 0x48C0, Size: 0x0001, Dimmgeschwindigkeiten 
#define EE_PARAMETER_4951_SAETTIGUNG_S     0x4951 //!< Addr: 0x4951, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4952_HELLIGKEIT_V     0x4952 //!< Addr: 0x4952, Size: 0x0008,     Helligkeit V 
#define EE_PARAMETER_4953_FARBWERT_WEISS     0x4953 //!< Addr: 0x4953, Size: 0x0008,     Farbwert Weiß 
#define EE_PARAMETER_4954_DIMMGESCHWINDIGKEIT_S     0x4954 //!< Addr: 0x4954, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_495B_SAETTIGUNG_S     0x495B //!< Addr: 0x495B, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_495C_HELLIGKEIT_V     0x495C //!< Addr: 0x495C, Size: 0x0008,     Helligkeit V 
#define EE_PARAMETER_495D_FARBWERT_WEISS     0x495D //!< Addr: 0x495D, Size: 0x0008,     Farbwert Weiß 
#define EE_PARAMETER_495E_DIMMGESCHWINDIGKEIT_S     0x495E //!< Addr: 0x495E, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4965_SAETTIGUNG_S     0x4965 //!< Addr: 0x4965, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4966_HELLIGKEIT_V     0x4966 //!< Addr: 0x4966, Size: 0x0008,     Helligkeit V 
#define EE_PARAMETER_4967_FARBWERT_WEISS     0x4967 //!< Addr: 0x4967, Size: 0x0008,     Farbwert Weiß 
#define EE_PARAMETER_4968_DIMMGESCHWINDIGKEIT_S     0x4968 //!< Addr: 0x4968, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_496F_SAETTIGUNG_S     0x496F //!< Addr: 0x496F, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4970_HELLIGKEIT_V     0x4970 //!< Addr: 0x4970, Size: 0x0008,     Helligkeit V 
#define EE_PARAMETER_4971_FARBWERT_WEISS     0x4971 //!< Addr: 0x4971, Size: 0x0008,     Farbwert Weiß 
#define EE_PARAMETER_4972_DIMMGESCHWINDIGKEIT_S     0x4972 //!< Addr: 0x4972, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_496C_SZENENUMMER_E     0x496C //!< Addr: 0x496C, Size: 0x0008, Szenenummer E 
#define EE_PARAMETER_4944_SZENENUMMER_A     0x4944 //!< Addr: 0x4944, Size: 0x0008, Szenenummer A 
#define EE_PARAMETER_494E_SZENENUMMER_B     0x494E //!< Addr: 0x494E, Size: 0x0008, Szenenummer B 
#define EE_PARAMETER_4958_SZENENUMMER_C     0x4958 //!< Addr: 0x4958, Size: 0x0008, Szenenummer C 
#define EE_PARAMETER_4962_SZENENUMMER_D     0x4962 //!< Addr: 0x4962, Size: 0x0008, Szenenummer D 
#define EE_PARAMETER_4976_SZENENUMMER_F     0x4976 //!< Addr: 0x4976, Size: 0x0008, Szenenummer F 
#define EE_PARAMETER_4979_SAETTIGUNG_S     0x4979 //!< Addr: 0x4979, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_497A_HELLIGKEIT_V     0x497A //!< Addr: 0x497A, Size: 0x0008,     Helligkeit V 
#define EE_PARAMETER_497B_FARBWERT_WEISS     0x497B //!< Addr: 0x497B, Size: 0x0008,     Farbwert Weiß 
#define EE_PARAMETER_497C_DIMMGESCHWINDIGKEIT_S     0x497C //!< Addr: 0x497C, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4980_SZENENUMMER_G     0x4980 //!< Addr: 0x4980, Size: 0x0008, Szenenummer G 
#define EE_PARAMETER_4983_SAETTIGUNG_S     0x4983 //!< Addr: 0x4983, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4984_HELLIGKEIT_V     0x4984 //!< Addr: 0x4984, Size: 0x0008,     Helligkeit V 
#define EE_PARAMETER_4985_FARBWERT_WEISS     0x4985 //!< Addr: 0x4985, Size: 0x0008,     Farbwert Weiß 
#define EE_PARAMETER_4986_DIMMGESCHWINDIGKEIT_S     0x4986 //!< Addr: 0x4986, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_498A_SZENENUMMER_H     0x498A //!< Addr: 0x498A, Size: 0x0008, Szenenummer H 
#define EE_PARAMETER_498D_SAETTIGUNG_S     0x498D //!< Addr: 0x498D, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_498E_HELLIGKEIT_V     0x498E //!< Addr: 0x498E, Size: 0x0008,     Helligkeit V 
#define EE_PARAMETER_498F_FARBWERT_WEISS     0x498F //!< Addr: 0x498F, Size: 0x0008,     Farbwert Weiß 
#define EE_PARAMETER_4990_DIMMGESCHWINDIGKEIT_S     0x4990 //!< Addr: 0x4990, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_49EA_SPERROBJEKT_1_DATENPUNKTTYP     0x49EA //!< Addr: 0x49EA, Size: 0x0008, Sperrobjekt 1 - Datenpunkttyp 
#define EE_PARAMETER_49F2_AKTION_BEI_OBJEKTWERT_1     0x49F2 //!< Addr: 0x49F2, Size: 0x0001,     Aktion bei Objektwert = 1 
#define EE_PARAMETER_49F1_AKTION_BEI_OBJEKTWERT_ZWANG_AUS     0x49F1 //!< Addr: 0x49F1, Size: 0x0001,     Aktion bei Objektwert Zwang AUS 
#define EE_PARAMETER_49F3_SAETTIGUNG_S     0x49F3 //!< Addr: 0x49F3, Size: 0x0008,         Sättigung S 
#define EE_PARAMETER_49F4_HELLIGKEIT_V     0x49F4 //!< Addr: 0x49F4, Size: 0x0008,         Helligkeit V 
#define EE_PARAMETER_49F5_FARBWERT_WEISS     0x49F5 //!< Addr: 0x49F5, Size: 0x0008,         Farbwert Weiß 
#define EE_PARAMETER_49EF_SAETTIGUNG_S     0x49EF //!< Addr: 0x49EF, Size: 0x0008,         Sättigung S 
#define EE_PARAMETER_49F0_HELLIGKEIT_V     0x49F0 //!< Addr: 0x49F0, Size: 0x0008,         Helligkeit V 
#define EE_PARAMETER_49F6_DIMMGESCHWINDIGKEIT_S     0x49F6 //!< Addr: 0x49F6, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_49F8_RUECKFALLZEIT_0S_NICHT_AKTIV_S     0x49F8 //!< Addr: 0x49F8, Size: 0x0010,     Rückfallzeit (0s = nicht aktiv) s 
#define EE_PARAMETER_49FA_SPERROBJEKT_2_DATENPUNKTTYP     0x49FA //!< Addr: 0x49FA, Size: 0x0008, Sperrobjekt 2 - Datenpunkttyp 
#define EE_PARAMETER_4A02_AKTION_BEI_OBJEKTWERT_1     0x4A02 //!< Addr: 0x4A02, Size: 0x0001,     Aktion bei Objektwert = 1 
#define EE_PARAMETER_4A01_AKTION_BEI_OBJEKTWERT_ZWANG_AUS     0x4A01 //!< Addr: 0x4A01, Size: 0x0001,     Aktion bei Objektwert Zwang AUS 
#define EE_PARAMETER_4A03_SAETTIGUNG_S     0x4A03 //!< Addr: 0x4A03, Size: 0x0008,         Sättigung S 
#define EE_PARAMETER_4A04_HELLIGKEIT_V     0x4A04 //!< Addr: 0x4A04, Size: 0x0008,         Helligkeit V 
#define EE_PARAMETER_4A05_FARBWERT_WEISS     0x4A05 //!< Addr: 0x4A05, Size: 0x0008,         Farbwert Weiß 
#define EE_PARAMETER_49FF_SAETTIGUNG_S     0x49FF //!< Addr: 0x49FF, Size: 0x0008,         Sättigung S 
#define EE_PARAMETER_4A00_HELLIGKEIT_V     0x4A00 //!< Addr: 0x4A00, Size: 0x0008,         Helligkeit V 
#define EE_PARAMETER_4A06_DIMMGESCHWINDIGKEIT_S     0x4A06 //!< Addr: 0x4A06, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4A08_RUECKFALLZEIT_0S_NICHT_AKTIV_S     0x4A08 //!< Addr: 0x4A08, Size: 0x0010,     Rückfallzeit (0s = nicht aktiv) s 
#define EE_PARAMETER_492D_AUSGABE_TUNABLE_WHITE_STATUS     0x492D //!< Addr: 0x492D, Size: 0x0008,     Ausgabe Tunable White Status 
#define EE_PARAMETER_49CF_EINSCHALTEN_MIT_ABS_FARBTON_SAETTIGUNG     0x49CF //!< Addr: 0x49CF, Size: 0x0008, Einschalten mit abs. Farbton/Sättigung 
#define EE_PARAMETER_49CC_MINIMALE_HELLIGKEIT_FUER_RELATIVES_DIMMEN     0x49CC //!< Addr: 0x49CC, Size: 0x0008,     Minimale Helligkeit für relatives Dimmen 
#define EE_PARAMETER_4DB0_STATUS_WAEHREND_DER_SEQUENZ_HCL_AUSGEBEN     0x4DB0 //!< Addr: 0x4DB0, Size: 0x0008, Status während der Sequenz/HCL ausgeben 
#define EE_PARAMETER_4B9D_VERHALTEN_BEI_STEUEROBJEKT_AUS     0x4B9D //!< Addr: 0x4B9D, Size: 0x0001, Verhalten bei Steuerobjekt "Aus" 
#define EE_PARAMETER_4B99_STATUSOBJEKT_SEQUENZ     0x4B99 //!< Addr: 0x4B99, Size: 0x0002, Statusobjekt Sequenz 
#define EE_PARAMETER_4B9B_SEQUENZUEBERGANG     0x4B9B //!< Addr: 0x4B9B, Size: 0x0001, Sequenzübergang 
#define EE_UNIONPARAMETER_4B9C     0x4B9C //!< Addr: 0x4B9C, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4B9D     0x4B9D //!< Addr: 0x4B9D, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4B9E     0x4B9E //!< Addr: 0x4B9E, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4B9F     0x4B9F //!< Addr: 0x4B9F, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4BA0     0x4BA0 //!< Addr: 0x4BA0, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4BA1     0x4BA1 //!< Addr: 0x4BA1, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4BA2     0x4BA2 //!< Addr: 0x4BA2, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4BA3     0x4BA3 //!< Addr: 0x4BA3, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4BA4     0x4BA4 //!< Addr: 0x4BA4, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4BA5     0x4BA5 //!< Addr: 0x4BA5, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4BA6     0x4BA6 //!< Addr: 0x4BA6, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4BA7     0x4BA7 //!< Addr: 0x4BA7, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4BA8     0x4BA8 //!< Addr: 0x4BA8, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4BA9     0x4BA9 //!< Addr: 0x4BA9, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4BAA     0x4BAA //!< Addr: 0x4BAA, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4BAB     0x4BAB //!< Addr: 0x4BAB, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4BAC     0x4BAC //!< Addr: 0x4BAC, Size: 0x0008,     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Farbwert Rot  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4BAD     0x4BAD //!< Addr: 0x4BAD, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4BAE     0x4BAE //!< Addr: 0x4BAE, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4BAF     0x4BAF //!< Addr: 0x4BAF, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4BD4     0x4BD4 //!< Addr: 0x4BD4, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_PARAMETER_4BE9_VERHALTEN_BEI_STEUEROBJEKT_AUS     0x4BE9 //!< Addr: 0x4BE9, Size: 0x0001, Verhalten bei Steuerobjekt "Aus" 
#define EE_PARAMETER_4BDF_OM_SEQPARAM1_RGBHHSV     0x4BDF //!< Addr: 0x4BDF, Size: 0x0008, OM_SeqParam[1]_RgbHhsv
#define EE_PARAMETER_4BB4_OM_SEQPARAM1_DIMMMODECOLOR_STEPA     0x4BB4 //!< Addr: 0x4BB4, Size: 0x0008, OM_SeqParam[1]_DimmModeColor_StepA
#define EE_PARAMETER_4BB9_OM_SEQPARAM1_DIMMMODECOLOR_STEPB     0x4BB9 //!< Addr: 0x4BB9, Size: 0x0008, OM_SeqParam[1]_DimmModeColor_StepB
#define EE_PARAMETER_4BBE_OM_SEQPARAM1_DIMMMODECOLOR_STEPC     0x4BBE //!< Addr: 0x4BBE, Size: 0x0008, OM_SeqParam[1]_DimmModeColor_StepC
#define EE_PARAMETER_4BC3_OM_SEQPARAM1_DIMMMODECOLOR_STEPD     0x4BC3 //!< Addr: 0x4BC3, Size: 0x0008, OM_SeqParam[1]_DimmModeColor_StepD
#define EE_PARAMETER_4BC8_OM_SEQPARAM1_DIMMMODECOLOR_STEPE     0x4BC8 //!< Addr: 0x4BC8, Size: 0x0008, OM_SeqParam[1]_DimmModeColor_StepE
#define EE_PARAMETER_4BDE_ANZAHL_PARAMETRIERTE_SCHRITTE     0x4BDE //!< Addr: 0x4BDE, Size: 0x0008, Anzahl parametrierte Schritte 
#define EE_PARAMETER_4BE1_UEBERGANGSZEIT_ZUFAELLIG     0x4BE1 //!< Addr: 0x4BE1, Size: 0x0008,     Übergangszeit zufällig 
#define EE_PARAMETER_4BE2_OM_SEQPARAM1_STARTSTEP     0x4BE2 //!< Addr: 0x4BE2, Size: 0x0008, OM_SeqParam[1]_StartStep
#define EE_PARAMETER_4BCA_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4BCA //!< Addr: 0x4BCA, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4BCC_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4BCC //!< Addr: 0x4BCC, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4BCE_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4BCE //!< Addr: 0x4BCE, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4BD0_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4BD0 //!< Addr: 0x4BD0, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4BB0_OM_SEQPARAM1_DIMMMODER_H_STEPA     0x4BB0 //!< Addr: 0x4BB0, Size: 0x0008, OM_SeqParam[1]_DimmModeR/H_StepA
#define EE_PARAMETER_4BB1_OM_SEQPARAM1_DIMMMODEG_S_STEPA     0x4BB1 //!< Addr: 0x4BB1, Size: 0x0008, OM_SeqParam[1]_DimmModeG/S_StepA
#define EE_PARAMETER_4BB2_OM_SEQPARAM1_DIMMMODEB_V_STEPA     0x4BB2 //!< Addr: 0x4BB2, Size: 0x0008, OM_SeqParam[1]_DimmModeB/V_StepA
#define EE_PARAMETER_4BB3_OM_SEQPARAM1_DIMMMODEW_0_STEPA     0x4BB3 //!< Addr: 0x4BB3, Size: 0x0008, OM_SeqParam[1]_DimmModeW/0_StepA
#define EE_PARAMETER_4BB5_OM_SEQPARAM1_DIMMMODER_H_STEPB     0x4BB5 //!< Addr: 0x4BB5, Size: 0x0008, OM_SeqParam[1]_DimmModeR/H_StepB
#define EE_PARAMETER_4BB6_OM_SEQPARAM1_DIMMMODEG_S_STEPB     0x4BB6 //!< Addr: 0x4BB6, Size: 0x0008, OM_SeqParam[1]_DimmModeG/S_StepB
#define EE_PARAMETER_4BB7_OM_SEQPARAM1_DIMMMODEB_V_STEPB     0x4BB7 //!< Addr: 0x4BB7, Size: 0x0008, OM_SeqParam[1]_DimmModeB/V_StepB
#define EE_PARAMETER_4BE7_SEQUENZUEBERGANG     0x4BE7 //!< Addr: 0x4BE7, Size: 0x0001, Sequenzübergang 
#define EE_PARAMETER_4BD2_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4BD2 //!< Addr: 0x4BD2, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4BB8_OM_SEQPARAM1_DIMMMODEW_0_STEPB     0x4BB8 //!< Addr: 0x4BB8, Size: 0x0008, OM_SeqParam[1]_DimmModeW/0_StepB
#define EE_PARAMETER_4BBA_OM_SEQPARAM1_DIMMMODER_H_STEPC     0x4BBA //!< Addr: 0x4BBA, Size: 0x0008, OM_SeqParam[1]_DimmModeR/H_StepC
#define EE_PARAMETER_4BBB_OM_SEQPARAM1_DIMMMODEG_S_STEPC     0x4BBB //!< Addr: 0x4BBB, Size: 0x0008, OM_SeqParam[1]_DimmModeG/S_StepC
#define EE_PARAMETER_4BBC_OM_SEQPARAM1_DIMMMODEB_V_STEPC     0x4BBC //!< Addr: 0x4BBC, Size: 0x0008, OM_SeqParam[1]_DimmModeB/V_StepC
#define EE_PARAMETER_4BBD_OM_SEQPARAM1_DIMMMODEW_0_STEPC     0x4BBD //!< Addr: 0x4BBD, Size: 0x0008, OM_SeqParam[1]_DimmModeW/0_StepC
#define EE_PARAMETER_4BBF_OM_SEQPARAM1_DIMMMODER_H_STEPD     0x4BBF //!< Addr: 0x4BBF, Size: 0x0008, OM_SeqParam[1]_DimmModeR/H_StepD
#define EE_PARAMETER_4BC0_OM_SEQPARAM1_DIMMMODEG_S_STEPD     0x4BC0 //!< Addr: 0x4BC0, Size: 0x0008, OM_SeqParam[1]_DimmModeG/S_StepD
#define EE_PARAMETER_4BC1_OM_SEQPARAM1_DIMMMODEB_V_STEPD     0x4BC1 //!< Addr: 0x4BC1, Size: 0x0008, OM_SeqParam[1]_DimmModeB/V_StepD
#define EE_PARAMETER_4BC2_OM_SEQPARAM1_DIMMMODEW_0_STEPD     0x4BC2 //!< Addr: 0x4BC2, Size: 0x0008, OM_SeqParam[1]_DimmModeW/0_StepD
#define EE_PARAMETER_4BC4_OM_SEQPARAM1_DIMMMODER_H_STEPE     0x4BC4 //!< Addr: 0x4BC4, Size: 0x0008, OM_SeqParam[1]_DimmModeR/H_StepE
#define EE_PARAMETER_4BC5_OM_SEQPARAM1_DIMMMODEG_S_STEPE     0x4BC5 //!< Addr: 0x4BC5, Size: 0x0008, OM_SeqParam[1]_DimmModeG/S_StepE
#define EE_PARAMETER_4BC6_OM_SEQPARAM1_DIMMMODEB_V_STEPE     0x4BC6 //!< Addr: 0x4BC6, Size: 0x0008, OM_SeqParam[1]_DimmModeB/V_StepE
#define EE_PARAMETER_4BC7_OM_SEQPARAM1_DIMMMODEW_0_STEPE     0x4BC7 //!< Addr: 0x4BC7, Size: 0x0008, OM_SeqParam[1]_DimmModeW/0_StepE
#define EE_PARAMETER_4BE5_STATUSOBJEKT_SEQUENZ     0x4BE5 //!< Addr: 0x4BE5, Size: 0x0002, Statusobjekt Sequenz 
#define EE_UNIONPARAMETER_4BD6     0x4BD6 //!< Addr: 0x4BD6, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4BD8     0x4BD8 //!< Addr: 0x4BD8, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4BDA     0x4BDA //!< Addr: 0x4BDA, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4BDC     0x4BDC //!< Addr: 0x4BDC, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4BE0     0x4BE0 //!< Addr: 0x4BE0, Size: 0x0008, OM_SegParam[1]_LoopOff oder     Anzahl der Ausführungen 
#define EE_UNIONPARAMETER_4BE3     0x4BE3 //!< Addr: 0x4BE3, Size: 0x0008,     Verhalten nach Sequenz  oder     Verhalten nach Sequenz  oder     Verhalten nach Sequenz 
#define EE_UNIONPARAMETER_4BE7     0x4BE7 //!< Addr: 0x4BE7, Size: 0x0008, OM_SeqParam[1]_DimBehaviorOff oder     Rückfallzeit der Helligkeit 
#define EE_PARAMETER_4C35_VERHALTEN_BEI_STEUEROBJEKT_AUS     0x4C35 //!< Addr: 0x4C35, Size: 0x0001, Verhalten bei Steuerobjekt "Aus" 
#define EE_PARAMETER_4C2B_OM_SEQPARAM2_RGBHHSV     0x4C2B //!< Addr: 0x4C2B, Size: 0x0008, OM_SeqParam[2]_RgbHhsv
#define EE_PARAMETER_4C00_OM_SEQPARAM2_DIMMMODECOLOR_STEPA     0x4C00 //!< Addr: 0x4C00, Size: 0x0008, OM_SeqParam[2]_DimmModeColor_StepA
#define EE_PARAMETER_4C05_OM_SEQPARAM2_DIMMMODECOLOR_STEPB     0x4C05 //!< Addr: 0x4C05, Size: 0x0008, OM_SeqParam[2]_DimmModeColor_StepB
#define EE_PARAMETER_4C0A_OM_SEQPARAM2_DIMMMODECOLOR_STEPC     0x4C0A //!< Addr: 0x4C0A, Size: 0x0008, OM_SeqParam[2]_DimmModeColor_StepC
#define EE_PARAMETER_4C0F_OM_SEQPARAM2_DIMMMODECOLOR_STEPD     0x4C0F //!< Addr: 0x4C0F, Size: 0x0008, OM_SeqParam[2]_DimmModeColor_StepD
#define EE_PARAMETER_4C14_OM_SEQPARAM2_DIMMMODECOLOR_STEPE     0x4C14 //!< Addr: 0x4C14, Size: 0x0008, OM_SeqParam[2]_DimmModeColor_StepE
#define EE_PARAMETER_4C2D_UEBERGANGSZEIT_ZUFAELLIG     0x4C2D //!< Addr: 0x4C2D, Size: 0x0008,     Übergangszeit zufällig 
#define EE_PARAMETER_4C2E_OM_SEQPARAM2_STARTSTEP     0x4C2E //!< Addr: 0x4C2E, Size: 0x0008, OM_SeqParam[2]_StartStep
#define EE_PARAMETER_4C16_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4C16 //!< Addr: 0x4C16, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4C18_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4C18 //!< Addr: 0x4C18, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4C1A_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4C1A //!< Addr: 0x4C1A, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4C1C_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4C1C //!< Addr: 0x4C1C, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4BFC_OM_SEQPARAM2_DIMMMODER_H_STEPA     0x4BFC //!< Addr: 0x4BFC, Size: 0x0008, OM_SeqParam[2]_DimmModeR/H_StepA
#define EE_PARAMETER_4BFD_OM_SEQPARAM2_DIMMMODEG_S_STEPA     0x4BFD //!< Addr: 0x4BFD, Size: 0x0008, OM_SeqParam[2]_DimmModeG/S_StepA
#define EE_PARAMETER_4BFE_OM_SEQPARAM2_DIMMMODEB_V_STEPA     0x4BFE //!< Addr: 0x4BFE, Size: 0x0008, OM_SeqParam[2]_DimmModeB/V_StepA
#define EE_PARAMETER_4BFF_OM_SEQPARAM2_DIMMMODEW_0_STEPA     0x4BFF //!< Addr: 0x4BFF, Size: 0x0008, OM_SeqParam[2]_DimmModeW/0_StepA
#define EE_PARAMETER_4C01_OM_SEQPARAM2_DIMMMODER_H_STEPB     0x4C01 //!< Addr: 0x4C01, Size: 0x0008, OM_SeqParam[2]_DimmModeR/H_StepB
#define EE_PARAMETER_4C02_OM_SEQPARAM2_DIMMMODEG_S_STEPB     0x4C02 //!< Addr: 0x4C02, Size: 0x0008, OM_SeqParam[2]_DimmModeG/S_StepB
#define EE_PARAMETER_4C03_OM_SEQPARAM2_DIMMMODEB_V_STEPB     0x4C03 //!< Addr: 0x4C03, Size: 0x0008, OM_SeqParam[2]_DimmModeB/V_StepB
#define EE_PARAMETER_4C33_SEQUENZUEBERGANG     0x4C33 //!< Addr: 0x4C33, Size: 0x0001, Sequenzübergang 
#define EE_PARAMETER_4C1E_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4C1E //!< Addr: 0x4C1E, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4C04_OM_SEQPARAM2_DIMMMODEW_0_STEPB     0x4C04 //!< Addr: 0x4C04, Size: 0x0008, OM_SeqParam[2]_DimmModeW/0_StepB
#define EE_PARAMETER_4C06_OM_SEQPARAM2_DIMMMODER_H_STEPC     0x4C06 //!< Addr: 0x4C06, Size: 0x0008, OM_SeqParam[2]_DimmModeR/H_StepC
#define EE_PARAMETER_4C07_OM_SEQPARAM2_DIMMMODEG_S_STEPC     0x4C07 //!< Addr: 0x4C07, Size: 0x0008, OM_SeqParam[2]_DimmModeG/S_StepC
#define EE_PARAMETER_4C08_OM_SEQPARAM2_DIMMMODEB_V_STEPC     0x4C08 //!< Addr: 0x4C08, Size: 0x0008, OM_SeqParam[2]_DimmModeB/V_StepC
#define EE_PARAMETER_4C09_OM_SEQPARAM2_DIMMMODEW_0_STEPC     0x4C09 //!< Addr: 0x4C09, Size: 0x0008, OM_SeqParam[2]_DimmModeW/0_StepC
#define EE_PARAMETER_4C0B_OM_SEQPARAM2_DIMMMODER_H_STEPD     0x4C0B //!< Addr: 0x4C0B, Size: 0x0008, OM_SeqParam[2]_DimmModeR/H_StepD
#define EE_PARAMETER_4C0C_OM_SEQPARAM2_DIMMMODEG_S_STEPD     0x4C0C //!< Addr: 0x4C0C, Size: 0x0008, OM_SeqParam[2]_DimmModeG/S_StepD
#define EE_PARAMETER_4C0D_OM_SEQPARAM2_DIMMMODEB_V_STEPD     0x4C0D //!< Addr: 0x4C0D, Size: 0x0008, OM_SeqParam[2]_DimmModeB/V_StepD
#define EE_PARAMETER_4C0E_OM_SEQPARAM2_DIMMMODEW_0_STEPD     0x4C0E //!< Addr: 0x4C0E, Size: 0x0008, OM_SeqParam[2]_DimmModeW/0_StepD
#define EE_PARAMETER_4C10_OM_SEQPARAM2_DIMMMODER_H_STEPE     0x4C10 //!< Addr: 0x4C10, Size: 0x0008, OM_SeqParam[2]_DimmModeR/H_StepE
#define EE_PARAMETER_4C11_OM_SEQPARAM2_DIMMMODEG_S_STEPE     0x4C11 //!< Addr: 0x4C11, Size: 0x0008, OM_SeqParam[2]_DimmModeG/S_StepE
#define EE_PARAMETER_4C12_OM_SEQPARAM2_DIMMMODEB_V_STEPE     0x4C12 //!< Addr: 0x4C12, Size: 0x0008, OM_SeqParam[2]_DimmModeB/V_StepE
#define EE_PARAMETER_4C13_OM_SEQPARAM2_DIMMMODEW_0_STEPE     0x4C13 //!< Addr: 0x4C13, Size: 0x0008, OM_SeqParam[2]_DimmModeW/0_StepE
#define EE_PARAMETER_4C31_STATUSOBJEKT_SEQUENZ     0x4C31 //!< Addr: 0x4C31, Size: 0x0002, Statusobjekt Sequenz 
#define EE_UNIONPARAMETER_4BE8     0x4BE8 //!< Addr: 0x4BE8, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4BE9     0x4BE9 //!< Addr: 0x4BE9, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4BEA     0x4BEA //!< Addr: 0x4BEA, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4BEB     0x4BEB //!< Addr: 0x4BEB, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4BEC     0x4BEC //!< Addr: 0x4BEC, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4BED     0x4BED //!< Addr: 0x4BED, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4BEE     0x4BEE //!< Addr: 0x4BEE, Size: 0x0008,     Oberer Grenzwert Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4BEF     0x4BEF //!< Addr: 0x4BEF, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4BF0     0x4BF0 //!< Addr: 0x4BF0, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4BF1     0x4BF1 //!< Addr: 0x4BF1, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4BF2     0x4BF2 //!< Addr: 0x4BF2, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4BF3     0x4BF3 //!< Addr: 0x4BF3, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4BF4     0x4BF4 //!< Addr: 0x4BF4, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4BF5     0x4BF5 //!< Addr: 0x4BF5, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4BF6     0x4BF6 //!< Addr: 0x4BF6, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4BF7     0x4BF7 //!< Addr: 0x4BF7, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4BF8     0x4BF8 //!< Addr: 0x4BF8, Size: 0x0008,     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Farbwert Rot  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4BF9     0x4BF9 //!< Addr: 0x4BF9, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4BFA     0x4BFA //!< Addr: 0x4BFA, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4BFB     0x4BFB //!< Addr: 0x4BFB, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4C20     0x4C20 //!< Addr: 0x4C20, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4C22     0x4C22 //!< Addr: 0x4C22, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4C24     0x4C24 //!< Addr: 0x4C24, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4C26     0x4C26 //!< Addr: 0x4C26, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4C28     0x4C28 //!< Addr: 0x4C28, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder         Verschiebung min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4C2A     0x4C2A //!< Addr: 0x4C2A, Size: 0x0008, OM_SeqParam[2]_Steps oder Anzahl parametrierte Schritte 
#define EE_UNIONPARAMETER_4C2C     0x4C2C //!< Addr: 0x4C2C, Size: 0x0008, OM_SegParam[2]_LoopOff oder     Anzahl der Ausführungen 
#define EE_UNIONPARAMETER_4C2F     0x4C2F //!< Addr: 0x4C2F, Size: 0x0008,     Verhalten nach Sequenz  oder     Verhalten nach Sequenz 
#define EE_UNIONPARAMETER_4C33     0x4C33 //!< Addr: 0x4C33, Size: 0x0008, OM_SeqParam[2]_DimBehaviorOff oder     Rückfallzeit der Helligkeit  oder     Rückfallzeit auf Uhrzeitabhängiges Dimmen nach absoluten/relativen Dimmen (ab R5.0) 
#define EE_UNIONPARAMETER_4C34     0x4C34 //!< Addr: 0x4C34, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4C35     0x4C35 //!< Addr: 0x4C35, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4C36     0x4C36 //!< Addr: 0x4C36, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4C37     0x4C37 //!< Addr: 0x4C37, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4C38     0x4C38 //!< Addr: 0x4C38, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4C39     0x4C39 //!< Addr: 0x4C39, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_PARAMETER_4C81_VERHALTEN_BEI_STEUEROBJEKT_AUS     0x4C81 //!< Addr: 0x4C81, Size: 0x0001, Verhalten bei Steuerobjekt "Aus" 
#define EE_PARAMETER_4C77_OM_SEQPARAM3_RGBHHSV     0x4C77 //!< Addr: 0x4C77, Size: 0x0008, OM_SeqParam[3]_RgbHhsv
#define EE_PARAMETER_4C4C_OM_SEQPARAM3_DIMMMODECOLOR_STEPA     0x4C4C //!< Addr: 0x4C4C, Size: 0x0008, OM_SeqParam[3]_DimmModeColor_StepA
#define EE_PARAMETER_4C51_OM_SEQPARAM3_DIMMMODECOLOR_STEPB     0x4C51 //!< Addr: 0x4C51, Size: 0x0008, OM_SeqParam[3]_DimmModeColor_StepB
#define EE_PARAMETER_4C56_OM_SEQPARAM3_DIMMMODECOLOR_STEPC     0x4C56 //!< Addr: 0x4C56, Size: 0x0008, OM_SeqParam[3]_DimmModeColor_StepC
#define EE_PARAMETER_4C5B_OM_SEQPARAM3_DIMMMODECOLOR_STEPD     0x4C5B //!< Addr: 0x4C5B, Size: 0x0008, OM_SeqParam[3]_DimmModeColor_StepD
#define EE_PARAMETER_4C60_OM_SEQPARAM3_DIMMMODECOLOR_STEPE     0x4C60 //!< Addr: 0x4C60, Size: 0x0008, OM_SeqParam[3]_DimmModeColor_StepE
#define EE_PARAMETER_4C76_ANZAHL_PARAMETRIERTE_SCHRITTE     0x4C76 //!< Addr: 0x4C76, Size: 0x0008, Anzahl parametrierte Schritte 
#define EE_PARAMETER_4C79_UEBERGANGSZEIT_ZUFAELLIG     0x4C79 //!< Addr: 0x4C79, Size: 0x0008,     Übergangszeit zufällig 
#define EE_PARAMETER_4C7A_OM_SEQPARAM3_STARTSTEP     0x4C7A //!< Addr: 0x4C7A, Size: 0x0008, OM_SeqParam[3]_StartStep
#define EE_PARAMETER_4C62_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4C62 //!< Addr: 0x4C62, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4C64_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4C64 //!< Addr: 0x4C64, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4C66_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4C66 //!< Addr: 0x4C66, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4C68_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4C68 //!< Addr: 0x4C68, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4C48_OM_SEQPARAM3_DIMMMODER_H_STEPA     0x4C48 //!< Addr: 0x4C48, Size: 0x0008, OM_SeqParam[3]_DimmModeR/H_StepA
#define EE_PARAMETER_4C49_OM_SEQPARAM3_DIMMMODEG_S_STEPA     0x4C49 //!< Addr: 0x4C49, Size: 0x0008, OM_SeqParam[3]_DimmModeG/S_StepA
#define EE_PARAMETER_4C4A_OM_SEQPARAM3_DIMMMODEB_V_STEPA     0x4C4A //!< Addr: 0x4C4A, Size: 0x0008, OM_SeqParam[3]_DimmModeB/V_StepA
#define EE_PARAMETER_4C4B_OM_SEQPARAM3_DIMMMODEW_0_STEPA     0x4C4B //!< Addr: 0x4C4B, Size: 0x0008, OM_SeqParam[3]_DimmModeW/0_StepA
#define EE_PARAMETER_4C4D_OM_SEQPARAM3_DIMMMODER_H_STEPB     0x4C4D //!< Addr: 0x4C4D, Size: 0x0008, OM_SeqParam[3]_DimmModeR/H_StepB
#define EE_PARAMETER_4C4E_OM_SEQPARAM3_DIMMMODEG_S_STEPB     0x4C4E //!< Addr: 0x4C4E, Size: 0x0008, OM_SeqParam[3]_DimmModeG/S_StepB
#define EE_PARAMETER_4C4F_OM_SEQPARAM3_DIMMMODEB_V_STEPB     0x4C4F //!< Addr: 0x4C4F, Size: 0x0008, OM_SeqParam[3]_DimmModeB/V_StepB
#define EE_PARAMETER_4C7F_SEQUENZUEBERGANG     0x4C7F //!< Addr: 0x4C7F, Size: 0x0001, Sequenzübergang 
#define EE_UNIONPARAMETER_4C3A     0x4C3A //!< Addr: 0x4C3A, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4C3B     0x4C3B //!< Addr: 0x4C3B, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4C3C     0x4C3C //!< Addr: 0x4C3C, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4C3D     0x4C3D //!< Addr: 0x4C3D, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4C3E     0x4C3E //!< Addr: 0x4C3E, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4C3F     0x4C3F //!< Addr: 0x4C3F, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4C40     0x4C40 //!< Addr: 0x4C40, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4C41     0x4C41 //!< Addr: 0x4C41, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4C42     0x4C42 //!< Addr: 0x4C42, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4C43     0x4C43 //!< Addr: 0x4C43, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4C44     0x4C44 //!< Addr: 0x4C44, Size: 0x0008,     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Farbwert Rot  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_PARAMETER_4C6A_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4C6A //!< Addr: 0x4C6A, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4C50_OM_SEQPARAM3_DIMMMODEW_0_STEPB     0x4C50 //!< Addr: 0x4C50, Size: 0x0008, OM_SeqParam[3]_DimmModeW/0_StepB
#define EE_PARAMETER_4C52_OM_SEQPARAM3_DIMMMODER_H_STEPC     0x4C52 //!< Addr: 0x4C52, Size: 0x0008, OM_SeqParam[3]_DimmModeR/H_StepC
#define EE_PARAMETER_4C53_OM_SEQPARAM3_DIMMMODEG_S_STEPC     0x4C53 //!< Addr: 0x4C53, Size: 0x0008, OM_SeqParam[3]_DimmModeG/S_StepC
#define EE_PARAMETER_4C54_OM_SEQPARAM3_DIMMMODEB_V_STEPC     0x4C54 //!< Addr: 0x4C54, Size: 0x0008, OM_SeqParam[3]_DimmModeB/V_StepC
#define EE_PARAMETER_4C55_OM_SEQPARAM3_DIMMMODEW_0_STEPC     0x4C55 //!< Addr: 0x4C55, Size: 0x0008, OM_SeqParam[3]_DimmModeW/0_StepC
#define EE_PARAMETER_4C57_OM_SEQPARAM3_DIMMMODER_H_STEPD     0x4C57 //!< Addr: 0x4C57, Size: 0x0008, OM_SeqParam[3]_DimmModeR/H_StepD
#define EE_PARAMETER_4C58_OM_SEQPARAM3_DIMMMODEG_S_STEPD     0x4C58 //!< Addr: 0x4C58, Size: 0x0008, OM_SeqParam[3]_DimmModeG/S_StepD
#define EE_PARAMETER_4C59_OM_SEQPARAM3_DIMMMODEB_V_STEPD     0x4C59 //!< Addr: 0x4C59, Size: 0x0008, OM_SeqParam[3]_DimmModeB/V_StepD
#define EE_PARAMETER_4C5A_OM_SEQPARAM3_DIMMMODEW_0_STEPD     0x4C5A //!< Addr: 0x4C5A, Size: 0x0008, OM_SeqParam[3]_DimmModeW/0_StepD
#define EE_PARAMETER_4C5C_OM_SEQPARAM3_DIMMMODER_H_STEPE     0x4C5C //!< Addr: 0x4C5C, Size: 0x0008, OM_SeqParam[3]_DimmModeR/H_StepE
#define EE_PARAMETER_4C5D_OM_SEQPARAM3_DIMMMODEG_S_STEPE     0x4C5D //!< Addr: 0x4C5D, Size: 0x0008, OM_SeqParam[3]_DimmModeG/S_StepE
#define EE_PARAMETER_4C5E_OM_SEQPARAM3_DIMMMODEB_V_STEPE     0x4C5E //!< Addr: 0x4C5E, Size: 0x0008, OM_SeqParam[3]_DimmModeB/V_StepE
#define EE_PARAMETER_4C5F_OM_SEQPARAM3_DIMMMODEW_0_STEPE     0x4C5F //!< Addr: 0x4C5F, Size: 0x0008, OM_SeqParam[3]_DimmModeW/0_StepE
#define EE_UNIONPARAMETER_4C45     0x4C45 //!< Addr: 0x4C45, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4C46     0x4C46 //!< Addr: 0x4C46, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4C47     0x4C47 //!< Addr: 0x4C47, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_PARAMETER_4C7D_STATUSOBJEKT_SEQUENZ     0x4C7D //!< Addr: 0x4C7D, Size: 0x0002, Statusobjekt Sequenz 
#define EE_UNIONPARAMETER_4C6C     0x4C6C //!< Addr: 0x4C6C, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4C6E     0x4C6E //!< Addr: 0x4C6E, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4C70     0x4C70 //!< Addr: 0x4C70, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4C72     0x4C72 //!< Addr: 0x4C72, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4C74     0x4C74 //!< Addr: 0x4C74, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4C78     0x4C78 //!< Addr: 0x4C78, Size: 0x0008, OM_SegParam[3]_LoopOff oder     Anzahl der Ausführungen 
#define EE_UNIONPARAMETER_4C7B     0x4C7B //!< Addr: 0x4C7B, Size: 0x0008,     Verhalten nach Sequenz  oder     Verhalten nach Sequenz 
#define EE_UNIONPARAMETER_4C7F     0x4C7F //!< Addr: 0x4C7F, Size: 0x0008, OM_SeqParam[3]_DimBehaviorOff oder     Rückfallzeit der Helligkeit 
#define EE_PARAMETER_4CCD_VERHALTEN_BEI_STEUEROBJEKT_AUS     0x4CCD //!< Addr: 0x4CCD, Size: 0x0001, Verhalten bei Steuerobjekt "Aus" 
#define EE_PARAMETER_4CC3_OM_SEQPARAM4_RGBHHSV     0x4CC3 //!< Addr: 0x4CC3, Size: 0x0008, OM_SeqParam[4]_RgbHhsv
#define EE_PARAMETER_4C98_OM_SEQPARAM4_DIMMMODECOLOR_STEPA     0x4C98 //!< Addr: 0x4C98, Size: 0x0008, OM_SeqParam[4]_DimmModeColor_StepA
#define EE_PARAMETER_4C9D_OM_SEQPARAM4_DIMMMODECOLOR_STEPB     0x4C9D //!< Addr: 0x4C9D, Size: 0x0008, OM_SeqParam[4]_DimmModeColor_StepB
#define EE_PARAMETER_4CA2_OM_SEQPARAM4_DIMMMODECOLOR_STEPC     0x4CA2 //!< Addr: 0x4CA2, Size: 0x0008, OM_SeqParam[4]_DimmModeColor_StepC
#define EE_PARAMETER_4CA7_OM_SEQPARAM4_DIMMMODECOLOR_STEPD     0x4CA7 //!< Addr: 0x4CA7, Size: 0x0008, OM_SeqParam[4]_DimmModeColor_StepD
#define EE_PARAMETER_4CAC_OM_SEQPARAM4_DIMMMODECOLOR_STEPE     0x4CAC //!< Addr: 0x4CAC, Size: 0x0008, OM_SeqParam[4]_DimmModeColor_StepE
#define EE_PARAMETER_4CC5_UEBERGANGSZEIT_ZUFAELLIG     0x4CC5 //!< Addr: 0x4CC5, Size: 0x0008,     Übergangszeit zufällig 
#define EE_PARAMETER_4CC6_OM_SEQPARAM4_STARTSTEP     0x4CC6 //!< Addr: 0x4CC6, Size: 0x0008, OM_SeqParam[4]_StartStep
#define EE_PARAMETER_4CAE_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4CAE //!< Addr: 0x4CAE, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4CB0_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4CB0 //!< Addr: 0x4CB0, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4CB2_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4CB2 //!< Addr: 0x4CB2, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4CB4_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4CB4 //!< Addr: 0x4CB4, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4C94_OM_SEQPARAM4_DIMMMODER_H_STEPA     0x4C94 //!< Addr: 0x4C94, Size: 0x0008, OM_SeqParam[4]_DimmModeR/H_StepA
#define EE_PARAMETER_4C95_OM_SEQPARAM4_DIMMMODEG_S_STEPA     0x4C95 //!< Addr: 0x4C95, Size: 0x0008, OM_SeqParam[4]_DimmModeG/S_StepA
#define EE_PARAMETER_4C96_OM_SEQPARAM4_DIMMMODEB_V_STEPA     0x4C96 //!< Addr: 0x4C96, Size: 0x0008, OM_SeqParam[4]_DimmModeB/V_StepA
#define EE_PARAMETER_4C97_OM_SEQPARAM4_DIMMMODEW_0_STEPA     0x4C97 //!< Addr: 0x4C97, Size: 0x0008, OM_SeqParam[4]_DimmModeW/0_StepA
#define EE_PARAMETER_4C99_OM_SEQPARAM4_DIMMMODER_H_STEPB     0x4C99 //!< Addr: 0x4C99, Size: 0x0008, OM_SeqParam[4]_DimmModeR/H_StepB
#define EE_PARAMETER_4C9A_OM_SEQPARAM4_DIMMMODEG_S_STEPB     0x4C9A //!< Addr: 0x4C9A, Size: 0x0008, OM_SeqParam[4]_DimmModeG/S_StepB
#define EE_PARAMETER_4C9B_OM_SEQPARAM4_DIMMMODEB_V_STEPB     0x4C9B //!< Addr: 0x4C9B, Size: 0x0008, OM_SeqParam[4]_DimmModeB/V_StepB
#define EE_PARAMETER_4CCB_SEQUENZUEBERGANG     0x4CCB //!< Addr: 0x4CCB, Size: 0x0001, Sequenzübergang 
#define EE_PARAMETER_4CB6_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4CB6 //!< Addr: 0x4CB6, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4C9C_OM_SEQPARAM4_DIMMMODEW_0_STEPB     0x4C9C //!< Addr: 0x4C9C, Size: 0x0008, OM_SeqParam[4]_DimmModeW/0_StepB
#define EE_PARAMETER_4C9E_OM_SEQPARAM4_DIMMMODER_H_STEPC     0x4C9E //!< Addr: 0x4C9E, Size: 0x0008, OM_SeqParam[4]_DimmModeR/H_StepC
#define EE_PARAMETER_4C9F_OM_SEQPARAM4_DIMMMODEG_S_STEPC     0x4C9F //!< Addr: 0x4C9F, Size: 0x0008, OM_SeqParam[4]_DimmModeG/S_StepC
#define EE_PARAMETER_4CA0_OM_SEQPARAM4_DIMMMODEB_V_STEPC     0x4CA0 //!< Addr: 0x4CA0, Size: 0x0008, OM_SeqParam[4]_DimmModeB/V_StepC
#define EE_PARAMETER_4CA1_OM_SEQPARAM4_DIMMMODEW_0_STEPC     0x4CA1 //!< Addr: 0x4CA1, Size: 0x0008, OM_SeqParam[4]_DimmModeW/0_StepC
#define EE_PARAMETER_4CA3_OM_SEQPARAM4_DIMMMODER_H_STEPD     0x4CA3 //!< Addr: 0x4CA3, Size: 0x0008, OM_SeqParam[4]_DimmModeR/H_StepD
#define EE_PARAMETER_4CA4_OM_SEQPARAM4_DIMMMODEG_S_STEPD     0x4CA4 //!< Addr: 0x4CA4, Size: 0x0008, OM_SeqParam[4]_DimmModeG/S_StepD
#define EE_PARAMETER_4CA5_OM_SEQPARAM4_DIMMMODEB_V_STEPD     0x4CA5 //!< Addr: 0x4CA5, Size: 0x0008, OM_SeqParam[4]_DimmModeB/V_StepD
#define EE_PARAMETER_4CA6_OM_SEQPARAM4_DIMMMODEW_0_STEPD     0x4CA6 //!< Addr: 0x4CA6, Size: 0x0008, OM_SeqParam[4]_DimmModeW/0_StepD
#define EE_PARAMETER_4CA8_OM_SEQPARAM4_DIMMMODER_H_STEPE     0x4CA8 //!< Addr: 0x4CA8, Size: 0x0008, OM_SeqParam[4]_DimmModeR/H_StepE
#define EE_PARAMETER_4CA9_OM_SEQPARAM4_DIMMMODEG_S_STEPE     0x4CA9 //!< Addr: 0x4CA9, Size: 0x0008, OM_SeqParam[4]_DimmModeG/S_StepE
#define EE_PARAMETER_4CAA_OM_SEQPARAM4_DIMMMODEB_V_STEPE     0x4CAA //!< Addr: 0x4CAA, Size: 0x0008, OM_SeqParam[4]_DimmModeB/V_StepE
#define EE_PARAMETER_4CAB_OM_SEQPARAM4_DIMMMODEW_0_STEPE     0x4CAB //!< Addr: 0x4CAB, Size: 0x0008, OM_SeqParam[4]_DimmModeW/0_StepE
#define EE_PARAMETER_4CC9_STATUSOBJEKT_SEQUENZ     0x4CC9 //!< Addr: 0x4CC9, Size: 0x0002, Statusobjekt Sequenz 
#define EE_UNIONPARAMETER_4C80     0x4C80 //!< Addr: 0x4C80, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4C81     0x4C81 //!< Addr: 0x4C81, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4C82     0x4C82 //!< Addr: 0x4C82, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4C83     0x4C83 //!< Addr: 0x4C83, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4C84     0x4C84 //!< Addr: 0x4C84, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4C85     0x4C85 //!< Addr: 0x4C85, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4C86     0x4C86 //!< Addr: 0x4C86, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4C87     0x4C87 //!< Addr: 0x4C87, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4C88     0x4C88 //!< Addr: 0x4C88, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4C89     0x4C89 //!< Addr: 0x4C89, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4C8A     0x4C8A //!< Addr: 0x4C8A, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4C8B     0x4C8B //!< Addr: 0x4C8B, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4C8C     0x4C8C //!< Addr: 0x4C8C, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4C8D     0x4C8D //!< Addr: 0x4C8D, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4C8E     0x4C8E //!< Addr: 0x4C8E, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4C8F     0x4C8F //!< Addr: 0x4C8F, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4C90     0x4C90 //!< Addr: 0x4C90, Size: 0x0008,     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Farbwert Rot  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4C91     0x4C91 //!< Addr: 0x4C91, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4C92     0x4C92 //!< Addr: 0x4C92, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4C93     0x4C93 //!< Addr: 0x4C93, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4CB8     0x4CB8 //!< Addr: 0x4CB8, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4CBA     0x4CBA //!< Addr: 0x4CBA, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4CBC     0x4CBC //!< Addr: 0x4CBC, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4CBE     0x4CBE //!< Addr: 0x4CBE, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4CC0     0x4CC0 //!< Addr: 0x4CC0, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4CC2     0x4CC2 //!< Addr: 0x4CC2, Size: 0x0008, Anzahl parametrierte Schritte  oder OM_SeqParam[4]_Steps
#define EE_UNIONPARAMETER_4CC4     0x4CC4 //!< Addr: 0x4CC4, Size: 0x0008, OM_SegParam[4]_LoopOff oder     Anzahl der Ausführungen 
#define EE_UNIONPARAMETER_4CC7     0x4CC7 //!< Addr: 0x4CC7, Size: 0x0008,     Verhalten nach Sequenz  oder     Verhalten nach Sequenz 
#define EE_UNIONPARAMETER_4CCB     0x4CCB //!< Addr: 0x4CCB, Size: 0x0008, OM_SeqParam[4]_DimBehaviorOff oder     Rückfallzeit der Helligkeit  oder     Rückfallzeit auf Uhrzeitabhängiges Dimmen nach absoluten/relativen Dimmen (ab R5.0) 
#define EE_PARAMETER_4D18_VERHALTEN_BEI_STEUEROBJEKT_AUS     0x4D18 //!< Addr: 0x4D18, Size: 0x0001, Verhalten bei Steuerobjekt "Aus" 
#define EE_PARAMETER_4D0F_OM_SEQPARAM5_RGBHHSV     0x4D0F //!< Addr: 0x4D0F, Size: 0x0008, OM_SeqParam[5]_RgbHhsv
#define EE_PARAMETER_4CE4_OM_SEQPARAM5_DIMMMODECOLOR_STEPA     0x4CE4 //!< Addr: 0x4CE4, Size: 0x0008, OM_SeqParam[5]_DimmModeColor_StepA
#define EE_PARAMETER_4CE9_OM_SEQPARAM5_DIMMMODECOLOR_STEPB     0x4CE9 //!< Addr: 0x4CE9, Size: 0x0008, OM_SeqParam[5]_DimmModeColor_StepB
#define EE_PARAMETER_4CEE_OM_SEQPARAM5_DIMMMODECOLOR_STEPC     0x4CEE //!< Addr: 0x4CEE, Size: 0x0008, OM_SeqParam[5]_DimmModeColor_StepC
#define EE_PARAMETER_4CF3_OM_SEQPARAM5_DIMMMODECOLOR_STEPD     0x4CF3 //!< Addr: 0x4CF3, Size: 0x0008, OM_SeqParam[5]_DimmModeColor_StepD
#define EE_PARAMETER_4CF8_OM_SEQPARAM5_DIMMMODECOLOR_STEPE     0x4CF8 //!< Addr: 0x4CF8, Size: 0x0008, OM_SeqParam[5]_DimmModeColor_StepE
#define EE_PARAMETER_4D0E_ANZAHL_PARAMETRIERTE_SCHRITTE     0x4D0E //!< Addr: 0x4D0E, Size: 0x0008, Anzahl parametrierte Schritte 
#define EE_PARAMETER_4D11_UEBERGANGSZEIT_ZUFAELLIG     0x4D11 //!< Addr: 0x4D11, Size: 0x0008,     Übergangszeit zufällig 
#define EE_PARAMETER_4D12_OM_SEQPARAM5_STARTSTEP     0x4D12 //!< Addr: 0x4D12, Size: 0x0008, OM_SeqParam[5]_StartStep
#define EE_PARAMETER_4CFA_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4CFA //!< Addr: 0x4CFA, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4CFC_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4CFC //!< Addr: 0x4CFC, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4CFE_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4CFE //!< Addr: 0x4CFE, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4D00_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4D00 //!< Addr: 0x4D00, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4CE0_OM_SEQPARAM5_DIMMMODER_H_STEPA     0x4CE0 //!< Addr: 0x4CE0, Size: 0x0008, OM_SeqParam[5]_DimmModeR/H_StepA
#define EE_PARAMETER_4CE1_OM_SEQPARAM5_DIMMMODEG_S_STEPA     0x4CE1 //!< Addr: 0x4CE1, Size: 0x0008, OM_SeqParam[5]_DimmModeG/S_StepA
#define EE_PARAMETER_4CE2_OM_SEQPARAM5_DIMMMODEB_V_STEPA     0x4CE2 //!< Addr: 0x4CE2, Size: 0x0008, OM_SeqParam[5]_DimmModeB/V_StepA
#define EE_PARAMETER_4CE3_OM_SEQPARAM5_DIMMMODEW_0_STEPA     0x4CE3 //!< Addr: 0x4CE3, Size: 0x0008, OM_SeqParam[5]_DimmModeW/0_StepA
#define EE_PARAMETER_4CE5_OM_SEQPARAM5_DIMMMODER_H_STEPB     0x4CE5 //!< Addr: 0x4CE5, Size: 0x0008, OM_SeqParam[5]_DimmModeR/H_StepB
#define EE_PARAMETER_4CE6_OM_SEQPARAM5_DIMMMODEG_S_STEPB     0x4CE6 //!< Addr: 0x4CE6, Size: 0x0008, OM_SeqParam[5]_DimmModeG/S_StepB
#define EE_PARAMETER_4CE7_OM_SEQPARAM5_DIMMMODEB_V_STEPB     0x4CE7 //!< Addr: 0x4CE7, Size: 0x0008, OM_SeqParam[5]_DimmModeB/V_StepB
#define EE_PARAMETER_4D17_SEQUENZUEBERGANG     0x4D17 //!< Addr: 0x4D17, Size: 0x0001, Sequenzübergang 
#define EE_PARAMETER_4D02_UEBERGANGSZEIT_ZUM_NAECHSTEN_SCHRITT_S     0x4D02 //!< Addr: 0x4D02, Size: 0x0010,     Übergangszeit zum nächsten Schritt s 
#define EE_PARAMETER_4CE8_OM_SEQPARAM5_DIMMMODEW_0_STEPB     0x4CE8 //!< Addr: 0x4CE8, Size: 0x0008, OM_SeqParam[5]_DimmModeW/0_StepB
#define EE_PARAMETER_4CEA_OM_SEQPARAM5_DIMMMODER_H_STEPC     0x4CEA //!< Addr: 0x4CEA, Size: 0x0008, OM_SeqParam[5]_DimmModeR/H_StepC
#define EE_PARAMETER_4CEB_OM_SEQPARAM5_DIMMMODEG_S_STEPC     0x4CEB //!< Addr: 0x4CEB, Size: 0x0008, OM_SeqParam[5]_DimmModeG/S_StepC
#define EE_PARAMETER_4CEC_OM_SEQPARAM5_DIMMMODEB_V_STEPC     0x4CEC //!< Addr: 0x4CEC, Size: 0x0008, OM_SeqParam[5]_DimmModeB/V_StepC
#define EE_PARAMETER_4CED_OM_SEQPARAM5_DIMMMODEW_0_STEPC     0x4CED //!< Addr: 0x4CED, Size: 0x0008, OM_SeqParam[5]_DimmModeW/0_StepC
#define EE_PARAMETER_4CEF_OM_SEQPARAM5_DIMMMODER_H_STEPD     0x4CEF //!< Addr: 0x4CEF, Size: 0x0008, OM_SeqParam[5]_DimmModeR/H_StepD
#define EE_PARAMETER_4CF0_OM_SEQPARAM5_DIMMMODEG_S_STEPD     0x4CF0 //!< Addr: 0x4CF0, Size: 0x0008, OM_SeqParam[5]_DimmModeG/S_StepD
#define EE_PARAMETER_4CF1_OM_SEQPARAM5_DIMMMODEB_V_STEPD     0x4CF1 //!< Addr: 0x4CF1, Size: 0x0008, OM_SeqParam[5]_DimmModeB/V_StepD
#define EE_PARAMETER_4CF2_OM_SEQPARAM5_DIMMMODEW_0_STEPD     0x4CF2 //!< Addr: 0x4CF2, Size: 0x0008, OM_SeqParam[5]_DimmModeW/0_StepD
#define EE_PARAMETER_4CF4_OM_SEQPARAM5_DIMMMODER_H_STEPE     0x4CF4 //!< Addr: 0x4CF4, Size: 0x0008, OM_SeqParam[5]_DimmModeR/H_StepE
#define EE_PARAMETER_4CF5_OM_SEQPARAM5_DIMMMODEG_S_STEPE     0x4CF5 //!< Addr: 0x4CF5, Size: 0x0008, OM_SeqParam[5]_DimmModeG/S_StepE
#define EE_PARAMETER_4CF6_OM_SEQPARAM5_DIMMMODEB_V_STEPE     0x4CF6 //!< Addr: 0x4CF6, Size: 0x0008, OM_SeqParam[5]_DimmModeB/V_StepE
#define EE_PARAMETER_4CF7_OM_SEQPARAM5_DIMMMODEW_0_STEPE     0x4CF7 //!< Addr: 0x4CF7, Size: 0x0008, OM_SeqParam[5]_DimmModeW/0_StepE
#define EE_PARAMETER_4D15_STATUSOBJEKT_SEQUENZ     0x4D15 //!< Addr: 0x4D15, Size: 0x0002, Statusobjekt Sequenz 
#define EE_UNIONPARAMETER_4CCC     0x4CCC //!< Addr: 0x4CCC, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4CCD     0x4CCD //!< Addr: 0x4CCD, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4CCE     0x4CCE //!< Addr: 0x4CCE, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4CCF     0x4CCF //!< Addr: 0x4CCF, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4CD0     0x4CD0 //!< Addr: 0x4CD0, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4CD1     0x4CD1 //!< Addr: 0x4CD1, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4CD2     0x4CD2 //!< Addr: 0x4CD2, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4CD3     0x4CD3 //!< Addr: 0x4CD3, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4CD4     0x4CD4 //!< Addr: 0x4CD4, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4CD5     0x4CD5 //!< Addr: 0x4CD5, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4CD6     0x4CD6 //!< Addr: 0x4CD6, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4CD7     0x4CD7 //!< Addr: 0x4CD7, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4CD8     0x4CD8 //!< Addr: 0x4CD8, Size: 0x0008,     Farbwert Rot  oder     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4CD9     0x4CD9 //!< Addr: 0x4CD9, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4CDA     0x4CDA //!< Addr: 0x4CDA, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4CDB     0x4CDB //!< Addr: 0x4CDB, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_PARAMETER_49F1_FARBWERT_WEISS     0x49F1 //!< Addr: 0x49F1, Size: 0x0008,         Farbwert Weiß 
#define EE_PARAMETER_4A01_FARBWERT_WEISS     0x4A01 //!< Addr: 0x4A01, Size: 0x0008,         Farbwert Weiß 
#define EE_PARAMETER_4DBC_STROMVERTEILUNG_DER_KANAELE     0x4DBC //!< Addr: 0x4DBC, Size: 0x0008, Stromverteilung der Kanäle 
#define EE_PARAMETER_4A2E_100PROZENT_HELLIGKEIT_UEBERSTEUERN_BEI_RELATIV_DIMMEN     0x4A2E //!< Addr: 0x4A2E, Size: 0x0008,     100% Helligkeit übersteuern bei relativ Dimmen  
#define EE_PARAMETER_4A2F_HELLIGKEITSSCHWELLE_1_DUNKEL     0x4A2F //!< Addr: 0x4A2F, Size: 0x0008,         Helligkeitsschwelle 1 (dunkel) 
#define EE_PARAMETER_4A30_HELLIGKEITSSCHWELLE_2_HELL     0x4A30 //!< Addr: 0x4A30, Size: 0x0008,         Helligkeitsschwelle 2 (hell) 
#define EE_UNIONPARAMETER_4CDC     0x4CDC //!< Addr: 0x4CDC, Size: 0x0008,     Farbton H  oder     Unterer Grenzwert Farbton H  oder     Oberer Grenzwert Farbton H  oder     Farbwert Rot  oder     Unterer Grenzwert Farbwert Rot  oder     Oberer Grenzwert Farbwert Rot  oder     Farbtemperatur  oder     Unterer Grenzwert Farbtemperatur  oder     Oberer Grenzwert Farbtemperatur 
#define EE_UNIONPARAMETER_4CDD     0x4CDD //!< Addr: 0x4CDD, Size: 0x0008,     Sättigung S  oder     Unterer Grenzwert Sättigung S  oder     Oberer Grenzwert Sättigung S 
#define EE_UNIONPARAMETER_4CDE     0x4CDE //!< Addr: 0x4CDE, Size: 0x0008,     Helligkeit V  oder     Unterer Grenzwert Helligkeit V  oder     Oberer Grenzwert Helligkeit V 
#define EE_UNIONPARAMETER_4CDF     0x4CDF //!< Addr: 0x4CDF, Size: 0x0008,     Farbwert Weiß  oder     Unterer Grenzwert Farbwert Weiß  oder     Oberer Grenzwert Farbwert Weiß 
#define EE_UNIONPARAMETER_4D04     0x4D04 //!< Addr: 0x4D04, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4D06     0x4D06 //!< Addr: 0x4D06, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4D08     0x4D08 //!< Addr: 0x4D08, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4D0A     0x4D0A //!< Addr: 0x4D0A, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4D0C     0x4D0C //!< Addr: 0x4D0C, Size: 0x0010,     Haltezeit x100 ms  oder         Stunden h  oder         Minuten min  oder     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4D10     0x4D10 //!< Addr: 0x4D10, Size: 0x0008, OM_SegParam[5]_LoopOff oder     Anzahl der Ausführungen 
#define EE_UNIONPARAMETER_4D13     0x4D13 //!< Addr: 0x4D13, Size: 0x0008,     Verhalten nach Sequenz  oder     Verhalten nach Sequenz 
#define EE_UNIONPARAMETER_4D17     0x4D17 //!< Addr: 0x4D17, Size: 0x0008, OM_SeqParam[5]_DimBehaviorOff oder     Rückfallzeit der Helligkeit 
#define EE_UNIONPARAMETER_4D18     0x4D18 //!< Addr: 0x4D18, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4D1C     0x4D1C //!< Addr: 0x4D1C, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4D20     0x4D20 //!< Addr: 0x4D20, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4D24     0x4D24 //!< Addr: 0x4D24, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4D28     0x4D28 //!< Addr: 0x4D28, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4D50     0x4D50 //!< Addr: 0x4D50, Size: 0x0010,     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4D52     0x4D52 //!< Addr: 0x4D52, Size: 0x0010,     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4D54     0x4D54 //!< Addr: 0x4D54, Size: 0x0010,     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4D56     0x4D56 //!< Addr: 0x4D56, Size: 0x0010,     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4D58     0x4D58 //!< Addr: 0x4D58, Size: 0x0010,     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4D63     0x4D63 //!< Addr: 0x4D63, Size: 0x0008,     Rückfallzeit der Helligkeit  oder OM_SeqParam[6]_DimBehaviorOff oder     Rückfallzeit auf Uhrzeitabhängiges Dimmen nach absoluten/relativen Dimmen (ab R5.0) 
#define EE_UNIONPARAMETER_4D64     0x4D64 //!< Addr: 0x4D64, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4D68     0x4D68 //!< Addr: 0x4D68, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4D6C     0x4D6C //!< Addr: 0x4D6C, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4D70     0x4D70 //!< Addr: 0x4D70, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4D74     0x4D74 //!< Addr: 0x4D74, Size: 0x0008,     Farbwert Rot  oder     Farbtemperatur 
#define EE_UNIONPARAMETER_4D9C     0x4D9C //!< Addr: 0x4D9C, Size: 0x0010,     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4D9E     0x4D9E //!< Addr: 0x4D9E, Size: 0x0010,     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4DA0     0x4DA0 //!< Addr: 0x4DA0, Size: 0x0010,     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4DA2     0x4DA2 //!< Addr: 0x4DA2, Size: 0x0010,     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_UNIONPARAMETER_4DA4     0x4DA4 //!< Addr: 0x4DA4, Size: 0x0010,     Zeitpunkt wann der Dimmvorgang startet  oder     Zeitpunkt wann der Dimmvorgang startet 
#define EE_PARAMETER_4D5B_OM_SEQPARAM6_RGBHHSV     0x4D5B //!< Addr: 0x4D5B, Size: 0x0008, OM_SeqParam[6]_RgbHhsv
#define EE_PARAMETER_4D65_VERHALTEN_BEI_STEUEROBJEKT_AUS     0x4D65 //!< Addr: 0x4D65, Size: 0x0001, Verhalten bei Steuerobjekt "Aus" 
#define EE_PARAMETER_4D61_OM_SEQPARAM6_STATE     0x4D61 //!< Addr: 0x4D61, Size: 0x0002, OM_SeqParam[6]_State
#define EE_PARAMETER_4D63_OM_SEQPARAM6_STEADYTIMEMODE     0x4D63 //!< Addr: 0x4D63, Size: 0x0001, OM_SeqParam[6]_steadyTimeMode
#define EE_PARAMETER_4B9C_RUECKFALL_DER_HELLIGKEIT_BEI_WIEDERHOLTEM_EIN     0x4B9C //!< Addr: 0x4B9C, Size: 0x0001,     Rückfall der Helligkeit bei wiederholtem "Ein" 
#define EE_PARAMETER_4B98_VERHALTEN_BEI_STEUEROBJEKT_EIN     0x4B98 //!< Addr: 0x4B98, Size: 0x0001, Verhalten bei Steuerobjekt "Ein" 
#define EE_PARAMETER_4C30_VERHALTEN_BEI_STEUEROBJEKT_EIN     0x4C30 //!< Addr: 0x4C30, Size: 0x0001, Verhalten bei Steuerobjekt "Ein" 
#define EE_PARAMETER_4C34_RUECKFALL_DER_HELLIGKEIT_BEI_WIEDERHOLTEM_EIN     0x4C34 //!< Addr: 0x4C34, Size: 0x0001,     Rückfall der Helligkeit bei wiederholtem "Ein" 
#define EE_PARAMETER_4CC8_VERHALTEN_BEI_STEUEROBJEKT_EIN     0x4CC8 //!< Addr: 0x4CC8, Size: 0x0001, Verhalten bei Steuerobjekt "Ein" 
#define EE_PARAMETER_4CCC_RUECKFALL_DER_HELLIGKEIT_BEI_WIEDERHOLTEM_EIN     0x4CCC //!< Addr: 0x4CCC, Size: 0x0001,     Rückfall der Helligkeit bei wiederholtem "Ein" 
#define EE_PARAMETER_4D60_VERHALTEN_BEI_STEUEROBJEKT_EIN     0x4D60 //!< Addr: 0x4D60, Size: 0x0001, Verhalten bei Steuerobjekt "Ein" 
#define EE_PARAMETER_4D64_RUECKFALL_DER_HELLIGKEIT_BEI_WIEDERHOLTEM_EIN     0x4D64 //!< Addr: 0x4D64, Size: 0x0001,     Rückfall der Helligkeit bei wiederholtem "Ein" 
#define EE_PARAMETER_4BE4_VERHALTEN_BEI_STEUEROBJEKT_EIN     0x4BE4 //!< Addr: 0x4BE4, Size: 0x0001, Verhalten bei Steuerobjekt "Ein" 
#define EE_PARAMETER_4BE8_RUECKFALL_DER_HELLIGKEIT_BEI_WIEDERHOLTEM_EIN     0x4BE8 //!< Addr: 0x4BE8, Size: 0x0001,     Rückfall der Helligkeit bei wiederholtem "Ein" 
#define EE_PARAMETER_4C7C_VERHALTEN_BEI_STEUEROBJEKT_EIN     0x4C7C //!< Addr: 0x4C7C, Size: 0x0001, Verhalten bei Steuerobjekt "Ein" 
#define EE_PARAMETER_4C80_RUECKFALL_DER_HELLIGKEIT_BEI_WIEDERHOLTEM_EIN     0x4C80 //!< Addr: 0x4C80, Size: 0x0001,     Rückfall der Helligkeit bei wiederholtem "Ein" 
#define EE_PARAMETER_4D14_VERHALTEN_BEI_STEUEROBJEKT_EIN     0x4D14 //!< Addr: 0x4D14, Size: 0x0001, Verhalten bei Steuerobjekt "Ein" 
#define EE_PARAMETER_4D19_RUECKFALL_DER_HELLIGKEIT_BEI_WIEDERHOLTEM_EIN     0x4D19 //!< Addr: 0x4D19, Size: 0x0001,     Rückfall der Helligkeit bei wiederholtem "Ein" 
#define EE_PARAMETER_4B28_TREPPENLICHTDAUER_S     0x4B28 //!< Addr: 0x4B28, Size: 0x0010,     Treppenlichtdauer s 
#define EE_PARAMETER_4B2F_TREPPENLICHTZEIT_VERLAENGERN     0x4B2F //!< Addr: 0x4B2F, Size: 0x0008,     Treppenlichtzeit verlängern 
#define EE_PARAMETER_4B30_MANUELLES_AUSSCHALTEN     0x4B30 //!< Addr: 0x4B30, Size: 0x0008,     Manuelles Ausschalten 
#define EE_PARAMETER_4AE6_EINSCHALTVERZOEGERUNG_S     0x4AE6 //!< Addr: 0x4AE6, Size: 0x0010,     Einschaltverzögerung s 
#define EE_PARAMETER_4AE8_AUSSCHALTVERZOEGERUNG_S     0x4AE8 //!< Addr: 0x4AE8, Size: 0x0010,     Ausschaltverzögerung s 
#define EE_PARAMETER_4AE0_MINIMALE_HELLIGKEIT_FUER_RELATIVES_DIMMEN     0x4AE0 //!< Addr: 0x4AE0, Size: 0x0008,     Minimale Helligkeit für relatives Dimmen 
#define EE_PARAMETER_4AED_EINSCHALTWERT_GRUEN     0x4AED //!< Addr: 0x4AED, Size: 0x0008,     Einschaltwert Grün 
#define EE_PARAMETER_4A4E_EINSCHALTGESCHWINDIGKEIT_S     0x4A4E //!< Addr: 0x4A4E, Size: 0x0010,     Einschaltgeschwindigkeit s 
#define EE_PARAMETER_4A52_AUSSCHALTGESCHWINDIGKEIT_S     0x4A52 //!< Addr: 0x4A52, Size: 0x0010,     Ausschaltgeschwindigkeit s 
#define EE_PARAMETER_4AF1_EINSCHALTWERT_GRUEN_NACHT     0x4AF1 //!< Addr: 0x4AF1, Size: 0x0008,     Einschaltwert Grün  Nacht 
#define EE_PARAMETER_4A50_EINSCHALTGESCHWINDIGKEIT_S     0x4A50 //!< Addr: 0x4A50, Size: 0x0010,     Einschaltgeschwindigkeit s 
#define EE_PARAMETER_4A54_AUSSCHALTGESCHWINDIGKEIT_S     0x4A54 //!< Addr: 0x4A54, Size: 0x0010,     Ausschaltgeschwindigkeit s 
#define EE_PARAMETER_4AFB_AUSSCHALTEN_MIT_REL_DIMMEN_HELLIGKEIT_V_OBJ_105     0x4AFB //!< Addr: 0x4AFB, Size: 0x0008, Ausschalten mit rel. Dimmen Helligkeit V (Obj. 105) 
#define EE_PARAMETER_4AFC_EINSCHALTEN_MIT_REL_DIMMEN_FARBTEMPERATUR     0x4AFC //!< Addr: 0x4AFC, Size: 0x0008, Einschalten mit rel. Dimmen Farbtemperatur 
#define EE_PARAMETER_4A48_RELATIVES_DIMMEN_HELLIGKEIT_V_S     0x4A48 //!< Addr: 0x4A48, Size: 0x0010,     Relatives Dimmen Helligkeit V s 
#define EE_PARAMETER_4A4A_RELATIVES_DIMMEN_FARBTEMPERATUR_S     0x4A4A //!< Addr: 0x4A4A, Size: 0x0010,     Relatives Dimmen Farbtemperatur s 
#define EE_PARAMETER_4A4C_ABSOLUTES_DIMMEN_S     0x4A4C //!< Addr: 0x4A4C, Size: 0x0010,     Absolutes Dimmen s 
#define EE_PARAMETER_4A3E_AUSGABE_RGBW_STATUS     0x4A3E //!< Addr: 0x4A3E, Size: 0x0008,     Ausgabe RGBW Status 
#define EE_PARAMETER_4A41_AUSGABE_TUNABLE_WHITE_STATUS     0x4A41 //!< Addr: 0x4A41, Size: 0x0008,     Ausgabe Tunable White Status 
#define EE_PARAMETER_4A43_AENDERUNG_SENDEN_WAEHREND_DES_DIMMVORGANGS     0x4A43 //!< Addr: 0x4A43, Size: 0x0008,     Änderung senden während des Dimmvorgangs 
#define EE_PARAMETER_4AF6_RESETWERT_GRUEN     0x4AF6 //!< Addr: 0x4AF6, Size: 0x0008,     Resetwert Grün 
#define EE_PARAMETER_4B3A_FARBTEMPERATUR_VON_WARMWEISS_KELVIN     0x4B3A //!< Addr: 0x4B3A, Size: 0x0010, Farbtemperatur von Warmweiß Kelvin 
#define EE_PARAMETER_4B3C_FARBTEMPERATUR_VON_KALTWEISS_KELVIN     0x4B3C //!< Addr: 0x4B3C, Size: 0x0010, Farbtemperatur von Kaltweiß Kelvin 
#define EE_PARAMETER_4B37_OM_RGBPARAM1_TW_KW_VALUES_2     0x4B37 //!< Addr: 0x4B37, Size: 0x0008, OM_RGBParam[1]_TW_KW_Values_2
#define EE_PARAMETER_4B34_OM_RGBPARAM1_TW_WW_VALUES_3     0x4B34 //!< Addr: 0x4B34, Size: 0x0008, OM_RGBParam[1]_TW_WW_Values_3
#define EE_PARAMETER_4B42_100PROZENT_HELLIGKEIT_UEBERSTEUERN_BEI_RELATIV_DIMMEN     0x4B42 //!< Addr: 0x4B42, Size: 0x0008,     100% Helligkeit übersteuern bei relativ Dimmen  
#define EE_PARAMETER_4B3F_FARBTEMPERATUR_WENN_KLEINER_HELLIGKEITSSCHWELLE_1_DUNKEL     0x4B3F //!< Addr: 0x4B3F, Size: 0x0008,     Farbtemperatur, wenn kleiner Helligkeitsschwelle 1 (dunkel) 
#define EE_PARAMETER_4B43_HELLIGKEITSSCHWELLE_1_DUNKEL     0x4B43 //!< Addr: 0x4B43, Size: 0x0008,         Helligkeitsschwelle 1 (dunkel) 
#define EE_PARAMETER_4B40_FARBTEMPERATUR_WENN_GROESSER_HELLIGKEITSSCHWELLE_2_HELL     0x4B40 //!< Addr: 0x4B40, Size: 0x0008,     Farbtemperatur, wenn größer Helligkeitsschwelle 2 (hell) 
#define EE_PARAMETER_4B44_HELLIGKEITSSCHWELLE_2_HELL     0x4B44 //!< Addr: 0x4B44, Size: 0x0008,         Helligkeitsschwelle 2 (hell) 
#define EE_PARAMETER_4AFE_SPERROBJEKT_2_DATENPUNKTTYP     0x4AFE //!< Addr: 0x4AFE, Size: 0x0008, Sperrobjekt 2 - Datenpunkttyp 
#define EE_PARAMETER_4B0E_SPERROBJEKT_2_DATENPUNKTTYP     0x4B0E //!< Addr: 0x4B0E, Size: 0x0008, Sperrobjekt 2 - Datenpunkttyp 
#define EE_PARAMETER_4B06_AKTION_BEI_OBJEKTWERT_2     0x4B06 //!< Addr: 0x4B06, Size: 0x0001,     Aktion bei Objektwert = 2 
#define EE_PARAMETER_4B05_AKTION_BEI_OBJEKTWERT_ZWANG_AUS     0x4B05 //!< Addr: 0x4B05, Size: 0x0001,     Aktion bei Objektwert Zwang AUS 
#define EE_PARAMETER_4B03_SAETTIGUNG_S     0x4B03 //!< Addr: 0x4B03, Size: 0x0008,         Sättigung S 
#define EE_PARAMETER_4B07_SAETTIGUNG_S     0x4B07 //!< Addr: 0x4B07, Size: 0x0008,         Sättigung S 
#define EE_PARAMETER_4B0A_DIMMGESCHWINDIGKEIT_S     0x4B0A //!< Addr: 0x4B0A, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4B0C_RUECKFALLZEIT_0S_NICHT_AKTIV_S     0x4B0C //!< Addr: 0x4B0C, Size: 0x0010,     Rückfallzeit (0s = nicht aktiv) s 
#define EE_PARAMETER_4B16_AKTION_BEI_OBJEKTWERT_2     0x4B16 //!< Addr: 0x4B16, Size: 0x0001,     Aktion bei Objektwert = 2 
#define EE_PARAMETER_4B15_AKTION_BEI_OBJEKTWERT_ZWANG_AUS     0x4B15 //!< Addr: 0x4B15, Size: 0x0001,     Aktion bei Objektwert Zwang AUS 
#define EE_PARAMETER_4B13_SAETTIGUNG_S     0x4B13 //!< Addr: 0x4B13, Size: 0x0008,         Sättigung S 
#define EE_PARAMETER_4B17_SAETTIGUNG_S     0x4B17 //!< Addr: 0x4B17, Size: 0x0008,         Sättigung S 
#define EE_PARAMETER_4B1A_DIMMGESCHWINDIGKEIT_S     0x4B1A //!< Addr: 0x4B1A, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4B1C_RUECKFALLZEIT_0S_NICHT_AKTIV_S     0x4B1C //!< Addr: 0x4B1C, Size: 0x0010,     Rückfallzeit (0s = nicht aktiv) s 
#define EE_PARAMETER_4AAF_SAETTIGUNG_S_BIT_WERT_2     0x4AAF //!< Addr: 0x4AAF, Size: 0x0008,         Sättigung S Bit Wert 2 
#define EE_PARAMETER_4AAB_SAETTIGUNG_S_BIT_WERT_0     0x4AAB //!< Addr: 0x4AAB, Size: 0x0008,         Sättigung S Bit Wert 0 
#define EE_PARAMETER_4AB2_DIMMGESCHWINDIGKEIT_S     0x4AB2 //!< Addr: 0x4AB2, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4ABD_SAETTIGUNG_S_BIT_WERT_2     0x4ABD //!< Addr: 0x4ABD, Size: 0x0008,         Sättigung S Bit Wert 2 
#define EE_PARAMETER_4AC0_DIMMGESCHWINDIGKEIT_S     0x4AC0 //!< Addr: 0x4AC0, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4ACB_SAETTIGUNG_S_BIT_WERT_2     0x4ACB //!< Addr: 0x4ACB, Size: 0x0008,         Sättigung S Bit Wert 2 
#define EE_PARAMETER_4AC7_SAETTIGUNG_S_BIT_WERT_0     0x4AC7 //!< Addr: 0x4AC7, Size: 0x0008,         Sättigung S Bit Wert 0 
#define EE_PARAMETER_4ACE_DIMMGESCHWINDIGKEIT_S     0x4ACE //!< Addr: 0x4ACE, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4AD9_SAETTIGUNG_S_BIT_WERT_2     0x4AD9 //!< Addr: 0x4AD9, Size: 0x0008,         Sättigung S Bit Wert 2 
#define EE_PARAMETER_4AD5_SAETTIGUNG_S_BIT_WERT_0     0x4AD5 //!< Addr: 0x4AD5, Size: 0x0008,         Sättigung S Bit Wert 0 
#define EE_PARAMETER_4ADC_DIMMGESCHWINDIGKEIT_S     0x4ADC //!< Addr: 0x4ADC, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4A56_SZENE_SPEICHERN     0x4A56 //!< Addr: 0x4A56, Size: 0x0008, Szene speichern 
#define EE_PARAMETER_4A58_SZENENUMMER_A     0x4A58 //!< Addr: 0x4A58, Size: 0x0008, Szenenummer A 
#define EE_PARAMETER_4A5B_SAETTIGUNG_S     0x4A5B //!< Addr: 0x4A5B, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4A5E_DIMMGESCHWINDIGKEIT_S     0x4A5E //!< Addr: 0x4A5E, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4A62_SZENENUMMER_B     0x4A62 //!< Addr: 0x4A62, Size: 0x0008, Szenenummer B 
#define EE_PARAMETER_4A65_SAETTIGUNG_S     0x4A65 //!< Addr: 0x4A65, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4A68_DIMMGESCHWINDIGKEIT_S     0x4A68 //!< Addr: 0x4A68, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4A6C_SZENENUMMER_C     0x4A6C //!< Addr: 0x4A6C, Size: 0x0008, Szenenummer C 
#define EE_PARAMETER_4A6F_SAETTIGUNG_S     0x4A6F //!< Addr: 0x4A6F, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4A72_DIMMGESCHWINDIGKEIT_S     0x4A72 //!< Addr: 0x4A72, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4A76_SZENENUMMER_D     0x4A76 //!< Addr: 0x4A76, Size: 0x0008, Szenenummer D 
#define EE_PARAMETER_4A79_SAETTIGUNG_S     0x4A79 //!< Addr: 0x4A79, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4A7C_DIMMGESCHWINDIGKEIT_S     0x4A7C //!< Addr: 0x4A7C, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4A80_SZENENUMMER_E     0x4A80 //!< Addr: 0x4A80, Size: 0x0008, Szenenummer E 
#define EE_PARAMETER_4A83_SAETTIGUNG_S     0x4A83 //!< Addr: 0x4A83, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4A86_DIMMGESCHWINDIGKEIT_S     0x4A86 //!< Addr: 0x4A86, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4A8A_SZENENUMMER_F     0x4A8A //!< Addr: 0x4A8A, Size: 0x0008, Szenenummer F 
#define EE_PARAMETER_4A8D_SAETTIGUNG_S     0x4A8D //!< Addr: 0x4A8D, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4A90_DIMMGESCHWINDIGKEIT_S     0x4A90 //!< Addr: 0x4A90, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4A94_SZENENUMMER_G     0x4A94 //!< Addr: 0x4A94, Size: 0x0008, Szenenummer G 
#define EE_PARAMETER_4A97_SAETTIGUNG_S     0x4A97 //!< Addr: 0x4A97, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4A9A_DIMMGESCHWINDIGKEIT_S     0x4A9A //!< Addr: 0x4A9A, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4A9E_SZENENUMMER_H     0x4A9E //!< Addr: 0x4A9E, Size: 0x0008, Szenenummer H 
#define EE_PARAMETER_4AA1_SAETTIGUNG_S     0x4AA1 //!< Addr: 0x4AA1, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4AA4_DIMMGESCHWINDIGKEIT_S     0x4AA4 //!< Addr: 0x4AA4, Size: 0x0010,     Dimmgeschwindigkeit s 
#define EE_PARAMETER_4AEA_EINSCHALTVERHALTEN     0x4AEA //!< Addr: 0x4AEA, Size: 0x0008, Einschaltverhalten 
#define EE_PARAMETER_4B00_AKTION_BEIM_ENTSPERREN     0x4B00 //!< Addr: 0x4B00, Size: 0x0008,     Aktion beim Entsperren 
#define EE_PARAMETER_4B10_AKTION_BEIM_ENTSPERREN     0x4B10 //!< Addr: 0x4B10, Size: 0x0008,     Aktion beim Entsperren 
#define EE_PARAMETER_4DBD_HCL_SEQUENZEN_AKTIV_HALTEN     0x4DBD //!< Addr: 0x4DBD, Size: 0x0008, HCL/Sequenzen aktiv halten 
#define EE_PARAMETER_4709_HELLIGKEITSWERT_BEI_AUS_IN_SPEICHER_UEBERNEHMEN     0x4709 //!< Addr: 0x4709, Size: 0x0001,     Helligkeitswert bei "Aus" in Speicher übernehmen 
#define EE_PARAMETER_479B_HELLIGKEITSWERT_BEI_AUS_IN_SPEICHER_UEBERNEHMEN     0x479B //!< Addr: 0x479B, Size: 0x0001,     Helligkeitswert bei "Aus" in Speicher übernehmen 
#define EE_PARAMETER_482D_HELLIGKEITSWERT_BEI_AUS_IN_SPEICHER_UEBERNEHMEN     0x482D //!< Addr: 0x482D, Size: 0x0001,     Helligkeitswert bei "Aus" in Speicher übernehmen 
#define EE_PARAMETER_48BF_HELLIGKEITSWERT_BEI_AUS_IN_SPEICHER_UEBERNEHMEN     0x48BF //!< Addr: 0x48BF, Size: 0x0001,     Helligkeitswert bei "Aus" in Speicher übernehmen 
#define EE_PARAMETER_468C_SPARMODUS_LEDS_AM_GERAET_ABSCHALTEN_NACH     0x468C //!< Addr: 0x468C, Size: 0x0008, Sparmodus, LED's  am Gerät abschalten nach 
#define EE_PARAMETER_4A59_AKTION     0x4A59 //!< Addr: 0x4A59, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4A63_AKTION     0x4A63 //!< Addr: 0x4A63, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4A6D_AKTION     0x4A6D //!< Addr: 0x4A6D, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4A77_AKTION     0x4A77 //!< Addr: 0x4A77, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4A81_AKTION     0x4A81 //!< Addr: 0x4A81, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4A8B_AKTION     0x4A8B //!< Addr: 0x4A8B, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4A95_AKTION     0x4A95 //!< Addr: 0x4A95, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4A9F_AKTION     0x4A9F //!< Addr: 0x4A9F, Size: 0x0008,     Aktion 
#define EE_PARAMETER_4AA8_AKTION_BEI_WERT_0     0x4AA8 //!< Addr: 0x4AA8, Size: 0x0008,     Aktion bei Wert = 0 
#define EE_PARAMETER_4AA9_AKTION_BEI_WERT_1     0x4AA9 //!< Addr: 0x4AA9, Size: 0x0008,     Aktion bei Wert = 1 
#define EE_PARAMETER_4AB6_AKTION_BEI_WERT_0     0x4AB6 //!< Addr: 0x4AB6, Size: 0x0008,     Aktion bei Wert = 0 
#define EE_PARAMETER_4AB7_AKTION_BEI_WERT_1     0x4AB7 //!< Addr: 0x4AB7, Size: 0x0008,     Aktion bei Wert = 1 
#define EE_PARAMETER_4AC4_AKTION_BEI_WERT_0     0x4AC4 //!< Addr: 0x4AC4, Size: 0x0008,     Aktion bei Wert = 0 
#define EE_PARAMETER_4AC5_AKTION_BEI_WERT_1     0x4AC5 //!< Addr: 0x4AC5, Size: 0x0008,     Aktion bei Wert = 1 
#define EE_PARAMETER_4AD2_AKTION_BEI_WERT_0     0x4AD2 //!< Addr: 0x4AD2, Size: 0x0008,     Aktion bei Wert = 0 
#define EE_PARAMETER_4AD3_AKTION_BEI_WERT_1     0x4AD3 //!< Addr: 0x4AD3, Size: 0x0008,     Aktion bei Wert = 1 
#define EE_PARAMETER_4AEB_EINSCHALTVERHALTEN_NACHT     0x4AEB //!< Addr: 0x4AEB, Size: 0x0008, Einschaltverhalten Nacht 
#define EE_PARAMETER_4AF4_VERHALTEN_NACH_RESET     0x4AF4 //!< Addr: 0x4AF4, Size: 0x0008, Verhalten nach Reset 
#define EE_PARAMETER_4B3E_REGELUNG_DER_FARBTEMPERATUR_GUELTIG     0x4B3E //!< Addr: 0x4B3E, Size: 0x0008,     Regelung der Farbtemperatur gültig 
#define EE_PARAMETER_4D75_SAETTIGUNG_S     0x4D75 //!< Addr: 0x4D75, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4D71_SAETTIGUNG_S     0x4D71 //!< Addr: 0x4D71, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4D6D_SAETTIGUNG_S     0x4D6D //!< Addr: 0x4D6D, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4D69_SAETTIGUNG_S     0x4D69 //!< Addr: 0x4D69, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4D65_SAETTIGUNG_S     0x4D65 //!< Addr: 0x4D65, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4D5C_OM_SEGPARAM6_LOOPOFF     0x4D5C //!< Addr: 0x4D5C, Size: 0x0008, OM_SegParam[6]_LoopOff
#define EE_PARAMETER_4D5A_OM_SEQPARAM6_STEPS     0x4D5A //!< Addr: 0x4D5A, Size: 0x0008, OM_SeqParam[6]_Steps
#define EE_PARAMETER_4D29_SAETTIGUNG_S     0x4D29 //!< Addr: 0x4D29, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4D25_SAETTIGUNG_S     0x4D25 //!< Addr: 0x4D25, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4D21_SAETTIGUNG_S     0x4D21 //!< Addr: 0x4D21, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4D1D_SAETTIGUNG_S     0x4D1D //!< Addr: 0x4D1D, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_4D19_SAETTIGUNG_S     0x4D19 //!< Addr: 0x4D19, Size: 0x0008,     Sättigung S 
#define EE_PARAMETER_46B6_VORWARNDAUER_S     0x46B6 //!< Addr: 0x46B6, Size: 0x0010,     Vorwarndauer s 
#define EE_PARAMETER_4748_VORWARNDAUER_S     0x4748 //!< Addr: 0x4748, Size: 0x0010,     Vorwarndauer s 
#define EE_PARAMETER_486C_VORWARNDAUER_S     0x486C //!< Addr: 0x486C, Size: 0x0010,     Vorwarndauer s 
#define EE_PARAMETER_47DA_VORWARNDAUER_S     0x47DA //!< Addr: 0x47DA, Size: 0x0010,     Vorwarndauer s 
#define EE_PARAMETER_4DBE_VERRINGERUNG_DER_HELLIGKEIT_KANAL_A     0x4DBE //!< Addr: 0x4DBE, Size: 0x0008, Verringerung der Helligkeit Kanal A 
#define EE_PARAMETER_4DBF_VERRINGERUNG_DER_HELLIGKEIT_KANAL_B     0x4DBF //!< Addr: 0x4DBF, Size: 0x0008, Verringerung der Helligkeit Kanal B 
#define EE_PARAMETER_4DC0_VERRINGERUNG_DER_HELLIGKEIT_KANAL_C     0x4DC0 //!< Addr: 0x4DC0, Size: 0x0008, Verringerung der Helligkeit Kanal C 
#define EE_PARAMETER_4DC1_VERRINGERUNG_DER_HELLIGKEIT_KANAL_D     0x4DC1 //!< Addr: 0x4DC1, Size: 0x0008, Verringerung der Helligkeit Kanal D 
#define EE_PARAMETER_4DCD_AUSGAENGE_AB_BZW_CD_AUF_100PROZENT_BEGRENZEN_AB_R5_0     0x4DCD //!< Addr: 0x4DCD, Size: 0x0001, Ausgänge A+B bzw C+D auf 100% begrenzen (Ab R5.0) 
#define EE_PARAMETER_4DCE_AUSGAENGE_AB_AUF_100PROZENT_BEGRENZEN_AB_R5_0     0x4DCE //!< Addr: 0x4DCE, Size: 0x0001, Ausgänge A+B auf 100% begrenzen (Ab R5.0) 
#define EE_PARAMETER_4DCC_AUSGAENGE_AB_CD_AUF_100PROZENT_BEGRENZEN_AB_R5_0     0x4DCC //!< Addr: 0x4DCC, Size: 0x0001, Ausgänge AB + CD auf 100% begrenzen (Ab R5.0) 
#define EE_PARAMETER_4DC9_RELAISANFORDERUNG_TYP     0x4DC9 //!< Addr: 0x4DC9, Size: 0x0002, Relaisanforderung Typ 
#define EE_PARAMETER_46E8_STATUSOBJEKTE_ZYKLISCH_SENDEN     0x46E8 //!< Addr: 0x46E8, Size: 0x0010, Statusobjekte zyklisch senden 
#define EE_PARAMETER_477A_STATUSOBJEKTE_ZYKLISCH_SENDEN     0x477A //!< Addr: 0x477A, Size: 0x0010, Statusobjekte zyklisch senden 
#define EE_PARAMETER_480C_STATUSOBJEKTE_ZYKLISCH_SENDEN     0x480C //!< Addr: 0x480C, Size: 0x0010, Statusobjekte zyklisch senden 
#define EE_PARAMETER_489E_STATUSOBJEKTE_ZYKLISCH_SENDEN     0x489E //!< Addr: 0x489E, Size: 0x0010, Statusobjekte zyklisch senden 
#define EE_PARAMETER_4708_ABDIMMZEIT     0x4708 //!< Addr: 0x4708, Size: 0x0001,     Abdimmzeit 
#define EE_PARAMETER_479A_ABDIMMZEIT     0x479A //!< Addr: 0x479A, Size: 0x0001,     Abdimmzeit 
#define EE_PARAMETER_482C_ABDIMMZEIT     0x482C //!< Addr: 0x482C, Size: 0x0001,     Abdimmzeit 
#define EE_PARAMETER_48BE_ABDIMMZEIT     0x48BE //!< Addr: 0x48BE, Size: 0x0001,     Abdimmzeit 
#define EE_PARAMETER_4B99_OM_SEQPARAM0_PAUSESEQONDIMMING     0x4B99 //!< Addr: 0x4B99, Size: 0x0008, OM_SeqParam[0]_PauseSeqOnDimming
#define EE_PARAMETER_4C31_OM_SEQPARAM2_PAUSESEQONDIMMING     0x4C31 //!< Addr: 0x4C31, Size: 0x0008, OM_SeqParam[2]_PauseSeqOnDimming
#define EE_PARAMETER_4CC9_OM_SEQPARAM4_PAUSESEQONDIMMING     0x4CC9 //!< Addr: 0x4CC9, Size: 0x0008, OM_SeqParam[4]_PauseSeqOnDimming
#define EE_PARAMETER_4D61_OM_SEQPARAM6_PAUSESEQONDIMMING     0x4D61 //!< Addr: 0x4D61, Size: 0x0008, OM_SeqParam[6]_PauseSeqOnDimming
#define EE_PARAMETER_4DC8_RELAYREQUESTMASTER     0x4DC8 //!< Addr: 0x4DC8, Size: 0x0001, RelayRequestMaster
#define EE_UNIONPARAMETER_4DC4     0x4DC4 //!< Addr: 0x4DC4, Size: 0x0010,     Relaisanforderung zyklisch senden  oder     Überwachungszeitraum 
#define EE_PARAMETER_46E6_OM_AKTORPARAM_POWERUPMESSAGEDELAY_0     0x46E6 //!< Addr: 0x46E6, Size: 0x0010, OM_aktorParam_powerUpMessageDelay_0
#define EE_PARAMETER_4778_OM_AKTORPARAM_POWERUPMESSAGEDELAY_1     0x4778 //!< Addr: 0x4778, Size: 0x0010, OM_aktorParam_powerUpMessageDelay_1
#define EE_PARAMETER_480A_OM_AKTORPARAM_POWERUPMESSAGEDELAY_2     0x480A //!< Addr: 0x480A, Size: 0x0010, OM_aktorParam_powerUpMessageDelay_2
#define EE_PARAMETER_489C_OM_AKTORPARAM_POWERUPMESSAGEDELAY_3     0x489C //!< Addr: 0x489C, Size: 0x0010, OM_aktorParam_powerUpMessageDelay_3


#define COMOBJ_0_KANAL_A_SCHALTEN     0 //!< Com-Objekt Nummer: 0, Beschreibung: Kanal A, Funktion: Schalten
#define COMOBJ_1_KANAL_A_TREPPENLICHT     1 //!< Com-Objekt Nummer: 1, Beschreibung: Kanal A, Funktion: Treppenlicht
#define COMOBJ_2_KANAL_A_DIMMEN_RELATIV     2 //!< Com-Objekt Nummer: 2, Beschreibung: Kanal A, Funktion: Dimmen relativ
#define COMOBJ_3_KANAL_A_DIMMEN_ABSOLUT     3 //!< Com-Objekt Nummer: 3, Beschreibung: Kanal A, Funktion: Dimmen absolut
#define COMOBJ_4_KANAL_A_STATUS_EIN_AUS     4 //!< Com-Objekt Nummer: 4, Beschreibung: Kanal A, Funktion: Status Ein/Aus
#define COMOBJ_5_KANAL_A_STATUS_DIMMWERT     5 //!< Com-Objekt Nummer: 5, Beschreibung: Kanal A, Funktion: Status Dimmwert
#define COMOBJ_6_KANAL_A_SPERRE_1     6 //!< Com-Objekt Nummer: 6, Beschreibung: Kanal A, Funktion: Sperre 1
#define COMOBJ_7_KANAL_A_SPERRE_2     7 //!< Com-Objekt Nummer: 7, Beschreibung: Kanal A, Funktion: Sperre 2
#define COMOBJ_8_KANAL_A_STATUS_SPERRE     8 //!< Com-Objekt Nummer: 8, Beschreibung: Kanal A, Funktion: Status Sperre
#define COMOBJ_9_KANAL_A_SZENE     9 //!< Com-Objekt Nummer: 9, Beschreibung: Kanal A, Funktion: Szene
#define COMOBJ_10_DUMMY     10 //!< Com-Objekt Nummer: 10, Beschreibung: Dummy, Funktion: 
#define COMOBJ_11_DUMMY     11 //!< Com-Objekt Nummer: 11, Beschreibung: Dummy, Funktion: 
#define COMOBJ_12_KANAL_A_BIT_SZENE_1     12 //!< Com-Objekt Nummer: 12, Beschreibung: Kanal A, Funktion: Bit Szene 1
#define COMOBJ_13_KANAL_A_BIT_SZENE_2     13 //!< Com-Objekt Nummer: 13, Beschreibung: Kanal A, Funktion: Bit Szene 2
#define COMOBJ_14_KANAL_A_BIT_SZENE_3     14 //!< Com-Objekt Nummer: 14, Beschreibung: Kanal A, Funktion: Bit Szene 3
#define COMOBJ_15_KANAL_A_BIT_SZENE_4     15 //!< Com-Objekt Nummer: 15, Beschreibung: Kanal A, Funktion: Bit Szene 4
#define COMOBJ_16_KANAL_B_SCHALTEN     16 //!< Com-Objekt Nummer: 16, Beschreibung: Kanal B, Funktion: Schalten
#define COMOBJ_17_KANAL_B_TREPPENLICHT     17 //!< Com-Objekt Nummer: 17, Beschreibung: Kanal B, Funktion: Treppenlicht
#define COMOBJ_18_KANAL_B_DIMMEN_RELATIV     18 //!< Com-Objekt Nummer: 18, Beschreibung: Kanal B, Funktion: Dimmen relativ
#define COMOBJ_19_KANAL_B_DIMMEN_ABSOLUT     19 //!< Com-Objekt Nummer: 19, Beschreibung: Kanal B, Funktion: Dimmen absolut
#define COMOBJ_20_KANAL_B_STATUS_EIN_AUS     20 //!< Com-Objekt Nummer: 20, Beschreibung: Kanal B, Funktion: Status Ein/Aus
#define COMOBJ_21_KANAL_B_STATUS_DIMMWERT     21 //!< Com-Objekt Nummer: 21, Beschreibung: Kanal B, Funktion: Status Dimmwert
#define COMOBJ_22_KANAL_B_SPERRE_1     22 //!< Com-Objekt Nummer: 22, Beschreibung: Kanal B, Funktion: Sperre 1
#define COMOBJ_23_KANAL_B_SPERRE_2     23 //!< Com-Objekt Nummer: 23, Beschreibung: Kanal B, Funktion: Sperre 2
#define COMOBJ_24_KANAL_B_STATUS_SPERRE     24 //!< Com-Objekt Nummer: 24, Beschreibung: Kanal B, Funktion: Status Sperre
#define COMOBJ_25_KANAL_B_SZENE     25 //!< Com-Objekt Nummer: 25, Beschreibung: Kanal B, Funktion: Szene
#define COMOBJ_26_DUMMY     26 //!< Com-Objekt Nummer: 26, Beschreibung: Dummy, Funktion: 
#define COMOBJ_27_DUMMY     27 //!< Com-Objekt Nummer: 27, Beschreibung: Dummy, Funktion: 
#define COMOBJ_28_KANAL_B_BIT_SZENE_1     28 //!< Com-Objekt Nummer: 28, Beschreibung: Kanal B, Funktion: Bit Szene 1
#define COMOBJ_29_KANAL_B_BIT_SZENE_2     29 //!< Com-Objekt Nummer: 29, Beschreibung: Kanal B, Funktion: Bit Szene 2
#define COMOBJ_30_KANAL_B_BIT_SZENE_3     30 //!< Com-Objekt Nummer: 30, Beschreibung: Kanal B, Funktion: Bit Szene 3
#define COMOBJ_31_KANAL_B_BIT_SZENE_4     31 //!< Com-Objekt Nummer: 31, Beschreibung: Kanal B, Funktion: Bit Szene 4
#define COMOBJ_32_KANAL_C_SCHALTEN     32 //!< Com-Objekt Nummer: 32, Beschreibung: Kanal C, Funktion: Schalten
#define COMOBJ_33_KANAL_C_TREPPENLICHT     33 //!< Com-Objekt Nummer: 33, Beschreibung: Kanal C, Funktion: Treppenlicht
#define COMOBJ_34_KANAL_C_DIMMEN_RELATIV     34 //!< Com-Objekt Nummer: 34, Beschreibung: Kanal C, Funktion: Dimmen relativ
#define COMOBJ_35_KANAL_C_DIMMEN_ABSOLUT     35 //!< Com-Objekt Nummer: 35, Beschreibung: Kanal C, Funktion: Dimmen absolut
#define COMOBJ_36_KANAL_C_STATUS_EIN_AUS     36 //!< Com-Objekt Nummer: 36, Beschreibung: Kanal C, Funktion: Status Ein/Aus
#define COMOBJ_37_KANAL_C_STATUS_DIMMWERT     37 //!< Com-Objekt Nummer: 37, Beschreibung: Kanal C, Funktion: Status Dimmwert
#define COMOBJ_38_KANAL_C_SPERRE_1     38 //!< Com-Objekt Nummer: 38, Beschreibung: Kanal C, Funktion: Sperre 1
#define COMOBJ_39_KANAL_C_SPERRE_2     39 //!< Com-Objekt Nummer: 39, Beschreibung: Kanal C, Funktion: Sperre 2
#define COMOBJ_40_KANAL_C_STATUS_SPERRE     40 //!< Com-Objekt Nummer: 40, Beschreibung: Kanal C, Funktion: Status Sperre
#define COMOBJ_41_KANAL_C_SZENE     41 //!< Com-Objekt Nummer: 41, Beschreibung: Kanal C, Funktion: Szene
#define COMOBJ_42_DUMMY     42 //!< Com-Objekt Nummer: 42, Beschreibung: Dummy, Funktion: 
#define COMOBJ_43_DUMMY     43 //!< Com-Objekt Nummer: 43, Beschreibung: Dummy, Funktion: 
#define COMOBJ_44_KANAL_C_BIT_SZENE_1     44 //!< Com-Objekt Nummer: 44, Beschreibung: Kanal C, Funktion: Bit Szene 1
#define COMOBJ_45_KANAL_C_BIT_SZENE_2     45 //!< Com-Objekt Nummer: 45, Beschreibung: Kanal C, Funktion: Bit Szene 2
#define COMOBJ_46_KANAL_C_BIT_SZENE_3     46 //!< Com-Objekt Nummer: 46, Beschreibung: Kanal C, Funktion: Bit Szene 3
#define COMOBJ_47_KANAL_C_BIT_SZENE_4     47 //!< Com-Objekt Nummer: 47, Beschreibung: Kanal C, Funktion: Bit Szene 4
#define COMOBJ_48_KANAL_D_SCHALTEN     48 //!< Com-Objekt Nummer: 48, Beschreibung: Kanal D, Funktion: Schalten
#define COMOBJ_49_KANAL_D_TREPPENLICHT     49 //!< Com-Objekt Nummer: 49, Beschreibung: Kanal D, Funktion: Treppenlicht
#define COMOBJ_50_KANAL_D_DIMMEN_RELATIV     50 //!< Com-Objekt Nummer: 50, Beschreibung: Kanal D, Funktion: Dimmen relativ
#define COMOBJ_51_KANAL_D_DIMMEN_ABSOLUT     51 //!< Com-Objekt Nummer: 51, Beschreibung: Kanal D, Funktion: Dimmen absolut
#define COMOBJ_52_KANAL_D_STATUS_EIN_AUS     52 //!< Com-Objekt Nummer: 52, Beschreibung: Kanal D, Funktion: Status Ein/Aus
#define COMOBJ_53_KANAL_D_STATUS_DIMMWERT     53 //!< Com-Objekt Nummer: 53, Beschreibung: Kanal D, Funktion: Status Dimmwert
#define COMOBJ_54_KANAL_D_SPERRE_1     54 //!< Com-Objekt Nummer: 54, Beschreibung: Kanal D, Funktion: Sperre 1
#define COMOBJ_55_KANAL_D_SPERRE_2     55 //!< Com-Objekt Nummer: 55, Beschreibung: Kanal D, Funktion: Sperre 2
#define COMOBJ_56_KANAL_D_STATUS_SPERRE     56 //!< Com-Objekt Nummer: 56, Beschreibung: Kanal D, Funktion: Status Sperre
#define COMOBJ_57_KANAL_D_SZENE     57 //!< Com-Objekt Nummer: 57, Beschreibung: Kanal D, Funktion: Szene
#define COMOBJ_58_DUMMY     58 //!< Com-Objekt Nummer: 58, Beschreibung: Dummy, Funktion: 
#define COMOBJ_59_DUMMY     59 //!< Com-Objekt Nummer: 59, Beschreibung: Dummy, Funktion: 
#define COMOBJ_60_KANAL_D_BIT_SZENE_1     60 //!< Com-Objekt Nummer: 60, Beschreibung: Kanal D, Funktion: Bit Szene 1
#define COMOBJ_61_KANAL_D_BIT_SZENE_2     61 //!< Com-Objekt Nummer: 61, Beschreibung: Kanal D, Funktion: Bit Szene 2
#define COMOBJ_62_KANAL_D_BIT_SZENE_3     62 //!< Com-Objekt Nummer: 62, Beschreibung: Kanal D, Funktion: Bit Szene 3
#define COMOBJ_63_KANAL_D_BIT_SZENE_4     63 //!< Com-Objekt Nummer: 63, Beschreibung: Kanal D, Funktion: Bit Szene 4
#define COMOBJ_64_LED_RGBW_HSV_TW_SCHALTEN     64 //!< Com-Objekt Nummer: 64, Beschreibung: LED RGBW / HSV / TW, Funktion: Schalten
#define COMOBJ_65_LED_RGBW_HSV_TW_TREPPENLICHT     65 //!< Com-Objekt Nummer: 65, Beschreibung: LED RGBW / HSV / TW, Funktion: Treppenlicht
#define COMOBJ_66_LED_RGBW_FARBEINSTELLUNG     66 //!< Com-Objekt Nummer: 66, Beschreibung: LED RGBW, Funktion: Farbeinstellung
#define COMOBJ_67_LED_HSV_FARBEINSTELLUNG     67 //!< Com-Objekt Nummer: 67, Beschreibung: LED HSV, Funktion: Farbeinstellung
#define COMOBJ_68_LED_HSV_FARBTON_H_DIMMEN_ABSOLUT     68 //!< Com-Objekt Nummer: 68, Beschreibung: LED HSV Farbton (H), Funktion: Dimmen absolut
#define COMOBJ_69_LED_HSV_SAETTIGUNG_S_DIMMEN_ABSOLUT     69 //!< Com-Objekt Nummer: 69, Beschreibung: LED HSV Sättigung (S), Funktion: Dimmen absolut
#define COMOBJ_70_LED_HSV_HELLIGKEIT_V_DIMMEN_ABSOLUT     70 //!< Com-Objekt Nummer: 70, Beschreibung: LED HSV Helligkeit (V), Funktion: Dimmen absolut
#define COMOBJ_71_LED_HSV_FARBTON_H_DIMMEN_RELATIV     71 //!< Com-Objekt Nummer: 71, Beschreibung: LED HSV Farbton (H), Funktion: Dimmen relativ
#define COMOBJ_72_LED_HSV_SAETTIGUNG_S_DIMMEN_RELATIV     72 //!< Com-Objekt Nummer: 72, Beschreibung: LED HSV Sättigung (S), Funktion: Dimmen relativ
#define COMOBJ_73_LED_HSV_HELLIGKEIT_V_DIMMEN_RELATIV     73 //!< Com-Objekt Nummer: 73, Beschreibung: LED HSV Helligkeit (V), Funktion: Dimmen relativ
#define COMOBJ_74_LED_TW_FARBTEMPERATUR_ANTEIL_KW_IN_PROZENT_DIMMEN_ABSOLUT     74 //!< Com-Objekt Nummer: 74, Beschreibung: LED TW Farbtemperatur (Anteil KW in %), Funktion: Dimmen absolut
#define COMOBJ_75_LED_TW_FARBTEMPERATUR_KELVIN_DIMMEN_ABSOLUT     75 //!< Com-Objekt Nummer: 75, Beschreibung: LED TW Farbtemperatur (Kelvin), Funktion: Dimmen absolut
#define COMOBJ_76_LED_TW_HELLIGKEIT_DIMMEN_ABSOLUT     76 //!< Com-Objekt Nummer: 76, Beschreibung: LED TW Helligkeit, Funktion: Dimmen absolut
#define COMOBJ_77_LED_TW_UEBERGANG_FARBTEMPERATUR_UND_HELLIGKEIT_DIMMEN_ABSOLUT     77 //!< Com-Objekt Nummer: 77, Beschreibung: LED TW Übergang (Farbtemperatur und Helligkeit), Funktion: Dimmen absolut
#define COMOBJ_78_LED_TW_FARBTEMPERATUR_ANTEIL_IN_PROZENT_DIMMEN_RELATIV     78 //!< Com-Objekt Nummer: 78, Beschreibung: LED TW Farbtemperatur (Anteil in %), Funktion: Dimmen relativ
#define COMOBJ_79_LED_TW_HELLIGKEIT_DIMMEN_RELATIV     79 //!< Com-Objekt Nummer: 79, Beschreibung: LED TW Helligkeit, Funktion: Dimmen relativ
#define COMOBJ_80_LED_RGBW_HSV_TW_STATUS_EIN_AUS     80 //!< Com-Objekt Nummer: 80, Beschreibung: LED RGBW / HSV / TW, Funktion: Status Ein/Aus
#define COMOBJ_81_LED_RGBW_6BYTE_STATUS_DIMMWERT     81 //!< Com-Objekt Nummer: 81, Beschreibung: LED RGBW, Funktion: 6Byte Status Dimmwert
#define COMOBJ_82_LED_HSV_3BYTE_STATUS_DIMMWERT     82 //!< Com-Objekt Nummer: 82, Beschreibung: LED HSV, Funktion: 3Byte Status Dimmwert
#define COMOBJ_83_LED_HSV_FARBTON_H_STATUS_DIMMWERT     83 //!< Com-Objekt Nummer: 83, Beschreibung: LED HSV Farbton (H), Funktion: Status Dimmwert
#define COMOBJ_84_LED_HSV_SAETTIGUNG_S_STATUS_DIMMWERT     84 //!< Com-Objekt Nummer: 84, Beschreibung: LED HSV Sättigung (S), Funktion: Status Dimmwert
#define COMOBJ_85_LED_HSV_HELLIGKEIT_V_STATUS_DIMMWERT     85 //!< Com-Objekt Nummer: 85, Beschreibung: LED HSV Helligkeit (V), Funktion: Status Dimmwert
#define COMOBJ_86_LED_TW_FARBTEMPERATUR_ANTEIL_KW_IN_PROZENT_STATUS_DIMMWERT     86 //!< Com-Objekt Nummer: 86, Beschreibung: LED TW Farbtemperatur (Anteil KW in %), Funktion: Status Dimmwert
#define COMOBJ_87_LED_TW_FARBTEMPERATUR_KELVIN_STATUS_DIMMWERT     87 //!< Com-Objekt Nummer: 87, Beschreibung: LED TW Farbtemperatur (Kelvin), Funktion: Status Dimmwert
#define COMOBJ_88_LED_TW_HELLIGKEIT_STATUS_DIMMWERT     88 //!< Com-Objekt Nummer: 88, Beschreibung: LED TW Helligkeit, Funktion: Status Dimmwert
#define COMOBJ_89_LED_RGBW_SZENE     89 //!< Com-Objekt Nummer: 89, Beschreibung: LED RGBW, Funktion: Szene
#define COMOBJ_90_LED_RGBW_BIT_SZENE_1_STARTEN     90 //!< Com-Objekt Nummer: 90, Beschreibung: LED RGBW, Funktion: Bit Szene 1 starten
#define COMOBJ_91_LED_RGBW_BIT_SZENE_2_STARTEN     91 //!< Com-Objekt Nummer: 91, Beschreibung: LED RGBW, Funktion: Bit Szene 2 starten
#define COMOBJ_92_LED_RGBW_BIT_SZENE_3_STARTEN     92 //!< Com-Objekt Nummer: 92, Beschreibung: LED RGBW, Funktion: Bit Szene 3 starten
#define COMOBJ_93_LED_RGBW_BIT_SZENE_4_STARTEN     93 //!< Com-Objekt Nummer: 93, Beschreibung: LED RGBW, Funktion: Bit Szene 4 starten
#define COMOBJ_94_LED_RGBW_SPERRE_1     94 //!< Com-Objekt Nummer: 94, Beschreibung: LED RGBW, Funktion: Sperre 1
#define COMOBJ_95_LED_RGBW_SPERRE_2     95 //!< Com-Objekt Nummer: 95, Beschreibung: LED RGBW, Funktion: Sperre 2
#define COMOBJ_96_LED_RGBW_STATUS_SPERRE     96 //!< Com-Objekt Nummer: 96, Beschreibung: LED RGBW, Funktion: Status Sperre
#define COMOBJ_97_LED_RGBW_HSV_TW_TEACH_IN_FUER_WEISSABGLEICH     97 //!< Com-Objekt Nummer: 97, Beschreibung: LED RGBW / HSV / TW, Funktion: Teach-In für Weißabgleich
#define COMOBJ_98_LED_TW_2_SCHALTEN_EIN_AUS     98 //!< Com-Objekt Nummer: 98, Beschreibung: LED TW 2, Funktion: Schalten Ein/Aus
#define COMOBJ_99_LED_TW_2_TREPPENLICHT     99 //!< Com-Objekt Nummer: 99, Beschreibung: LED TW 2, Funktion: Treppenlicht
#define COMOBJ_100_LED_TW_2_FARBTEMPERATUR_ANTEIL_KW_IN_PROZENT_DIMMEN_ABSOLUT     100 //!< Com-Objekt Nummer: 100, Beschreibung: LED TW 2 Farbtemperatur (Anteil KW in %), Funktion: Dimmen absolut
#define COMOBJ_101_LED_TW_2_FARBTEMPERATUR_KELVIN_DIMMEN_ABSOLUT     101 //!< Com-Objekt Nummer: 101, Beschreibung: LED TW 2 Farbtemperatur (Kelvin), Funktion: Dimmen absolut
#define COMOBJ_102_LED_TW_2_HELLIGKEIT_DIMMEN_ABSOLUT     102 //!< Com-Objekt Nummer: 102, Beschreibung: LED TW 2 Helligkeit, Funktion: Dimmen absolut
#define COMOBJ_103_LED_TW_2_UEBERGANG_FARBTEMPERATUR_UND_HELLIGKEIT_DIMMEN_ABSOLUT     103 //!< Com-Objekt Nummer: 103, Beschreibung: LED TW 2 Übergang (Farbtemperatur und Helligkeit), Funktion: Dimmen absolut
#define COMOBJ_104_LED_TW_2_FARBTEMPERATUR_ANTEIL_IN_PROZENT_DIMMEN_RELATIV     104 //!< Com-Objekt Nummer: 104, Beschreibung: LED TW 2 Farbtemperatur (Anteil in %), Funktion: Dimmen relativ
#define COMOBJ_105_LED_TW_2_HELLIGKEIT_DIMMEN_RELATIV     105 //!< Com-Objekt Nummer: 105, Beschreibung: LED TW 2 Helligkeit, Funktion: Dimmen relativ
#define COMOBJ_106_LED_TW_2_STATUS_EIN_AUS     106 //!< Com-Objekt Nummer: 106, Beschreibung: LED TW 2, Funktion: Status Ein/Aus
#define COMOBJ_107_LED_TW_2_FARBTEMPERATUR_ANTEIL_KW_IN_PROZENT_STATUS_DIMMWERT     107 //!< Com-Objekt Nummer: 107, Beschreibung: LED TW 2 Farbtemperatur (Anteil KW in %), Funktion: Status Dimmwert
#define COMOBJ_108_LED_TW_2_FARBTEMPERATUR_KELVIN_STATUS_DIMMWERT     108 //!< Com-Objekt Nummer: 108, Beschreibung: LED TW 2 Farbtemperatur (Kelvin), Funktion: Status Dimmwert
#define COMOBJ_109_LED_TW_2_HELLIGKEIT_STATUS_DIMMWERT     109 //!< Com-Objekt Nummer: 109, Beschreibung: LED TW 2 Helligkeit, Funktion: Status Dimmwert
#define COMOBJ_110_LED_TW_2_SZENE     110 //!< Com-Objekt Nummer: 110, Beschreibung: LED TW 2, Funktion: Szene
#define COMOBJ_111_LED_TW_2_BIT_SZENE_1_STARTEN     111 //!< Com-Objekt Nummer: 111, Beschreibung: LED TW 2, Funktion: Bit Szene 1 starten
#define COMOBJ_112_LED_TW_2_BIT_SZENE_2_STARTEN     112 //!< Com-Objekt Nummer: 112, Beschreibung: LED TW 2, Funktion: Bit Szene 2 starten
#define COMOBJ_113_LED_TW_2_BIT_SZENE_3_STARTEN     113 //!< Com-Objekt Nummer: 113, Beschreibung: LED TW 2, Funktion: Bit Szene 3 starten
#define COMOBJ_114_LED_TW_2_BIT_SZENE_4_STARTEN     114 //!< Com-Objekt Nummer: 114, Beschreibung: LED TW 2, Funktion: Bit Szene 4 starten
#define COMOBJ_115_LED_TW_2_SPERRE_1     115 //!< Com-Objekt Nummer: 115, Beschreibung: LED TW 2, Funktion: Sperre 1
#define COMOBJ_116_LED_TW_2_SPERRE_2     116 //!< Com-Objekt Nummer: 116, Beschreibung: LED TW 2, Funktion: Sperre 2
#define COMOBJ_117_LED_TW_2_STATUS_SPERRE     117 //!< Com-Objekt Nummer: 117, Beschreibung: LED TW 2, Funktion: Status Sperre
#define COMOBJ_118_DUMMY     118 //!< Com-Objekt Nummer: 118, Beschreibung: Dummy, Funktion: 
#define COMOBJ_119_LED_RGBW_HSV_TW_SEQUENZ_1_STARTEN     119 //!< Com-Objekt Nummer: 119, Beschreibung: LED RGBW / HSV / TW, Funktion: Sequenz 1 starten
#define COMOBJ_120_LED_RGBW_HSV_TW_SEQUENZ_1_STATUS     120 //!< Com-Objekt Nummer: 120, Beschreibung: LED RGBW / HSV / TW, Funktion: Sequenz 1 Status
#define COMOBJ_121_LED_RGBW_HSV_TW_SEQUENZ_2_STARTEN     121 //!< Com-Objekt Nummer: 121, Beschreibung: LED RGBW / HSV / TW, Funktion: Sequenz 2 starten
#define COMOBJ_122_LED_RGBW_HSV_TW_SEQUENZ_2_STATUS     122 //!< Com-Objekt Nummer: 122, Beschreibung: LED RGBW / HSV / TW, Funktion: Sequenz 2 Status
#define COMOBJ_123_LED_RGBW_HSV_TW_SEQUENZ_3_STARTEN     123 //!< Com-Objekt Nummer: 123, Beschreibung: LED RGBW / HSV / TW, Funktion: Sequenz 3 starten
#define COMOBJ_124_LED_RGBW_HSV_TW_SEQUENZ_3_STATUS     124 //!< Com-Objekt Nummer: 124, Beschreibung: LED RGBW / HSV / TW, Funktion: Sequenz 3 Status
#define COMOBJ_125_LED_RGBW_HSV_TW_SEQUENZ_4_STARTEN     125 //!< Com-Objekt Nummer: 125, Beschreibung: LED RGBW / HSV / TW, Funktion: Sequenz 4 starten
#define COMOBJ_126_LED_RGBW_HSV_TW_SEQUENZ_4_STATUS     126 //!< Com-Objekt Nummer: 126, Beschreibung: LED RGBW / HSV / TW, Funktion: Sequenz 4 Status
#define COMOBJ_127_LED_RGBW_HSV_TW_SEQUENZ_5_STARTEN     127 //!< Com-Objekt Nummer: 127, Beschreibung: LED RGBW / HSV / TW, Funktion: Sequenz 5 starten
#define COMOBJ_128_LED_RGBW_HSV_TW_SEQUENZ_5_STATUS     128 //!< Com-Objekt Nummer: 128, Beschreibung: LED RGBW / HSV / TW, Funktion: Sequenz 5 Status
#define COMOBJ_129_LED_RGBW_HSV_TW_SEQUENZ_6_STARTEN     129 //!< Com-Objekt Nummer: 129, Beschreibung: LED RGBW / HSV / TW, Funktion: Sequenz 6 starten
#define COMOBJ_130_LED_RGBW_HSV_TW_SEQUENZ_6_STATUS     130 //!< Com-Objekt Nummer: 130, Beschreibung: LED RGBW / HSV / TW, Funktion: Sequenz 6 Status
#define COMOBJ_131_LED_TW_2_HUMAN_CENTRIC_LIGHT_HCL_HCL_STARTEN     131 //!< Com-Objekt Nummer: 131, Beschreibung: LED TW 2 Human Centric Light (HCL), Funktion: HCL starten
#define COMOBJ_132_LED_TW_2_HUMAN_CENTRIC_LIGHT_HCL_HCL_STATUS     132 //!< Com-Objekt Nummer: 132, Beschreibung: LED TW 2 Human Centric Light (HCL), Funktion: HCL Status
#define COMOBJ_133_DUMMY     133 //!< Com-Objekt Nummer: 133, Beschreibung: Dummy, Funktion: 
#define COMOBJ_134_DUMMY     134 //!< Com-Objekt Nummer: 134, Beschreibung: Dummy, Funktion: 
#define COMOBJ_135_ZENTRAL_SCHALTEN     135 //!< Com-Objekt Nummer: 135, Beschreibung: Zentral, Funktion: Schalten
#define COMOBJ_136_ZENTRAL_DIMMEN_RELATIV     136 //!< Com-Objekt Nummer: 136, Beschreibung: Zentral, Funktion: Dimmen relativ
#define COMOBJ_137_ZENTRAL_DIMMEN_ABSOLUT     137 //!< Com-Objekt Nummer: 137, Beschreibung: Zentral, Funktion: Dimmen absolut
#define COMOBJ_138_ZENTRAL_SZENE     138 //!< Com-Objekt Nummer: 138, Beschreibung: Zentral, Funktion: Szene
#define COMOBJ_139_ZENTRAL_UEBERSTROM_ALARM     139 //!< Com-Objekt Nummer: 139, Beschreibung: Zentral, Funktion: Überstrom Alarm
#define COMOBJ_140_ZENTRAL_UEBERTEMPERATUR_ALARM     140 //!< Com-Objekt Nummer: 140, Beschreibung: Zentral, Funktion: Übertemperatur Alarm
#define COMOBJ_141_RELAIS_SCHALTEN_EIN_AUS     141 //!< Com-Objekt Nummer: 141, Beschreibung: Relais, Funktion: Schalten Ein/Aus
#define COMOBJ_142_RELAIS_STATUS     142 //!< Com-Objekt Nummer: 142, Beschreibung: Relais, Funktion: Status
#define COMOBJ_143_ZENTRAL_STATUS_12_24V_SPANNUNGSVERSORGUNG     143 //!< Com-Objekt Nummer: 143, Beschreibung: Zentral, Funktion: Status 12/24V Spannungsversorgung
#define COMOBJ_144_UHRZEIT_EINGANG     144 //!< Com-Objekt Nummer: 144, Beschreibung: Uhrzeit, Funktion: Eingang
#define COMOBJ_145_DATUM_EINGANG     145 //!< Com-Objekt Nummer: 145, Beschreibung: Datum, Funktion: Eingang
#define COMOBJ_146_DATUM_UHRZEIT_EINGANG     146 //!< Com-Objekt Nummer: 146, Beschreibung: Datum/Uhrzeit, Funktion: Eingang
#define COMOBJ_147_IN_BETRIEB_AUSGANG     147 //!< Com-Objekt Nummer: 147, Beschreibung: In Betrieb, Funktion: Ausgang
#define COMOBJ_148_TAG_NACHT_TAG1_NACHT0     148 //!< Com-Objekt Nummer: 148, Beschreibung: Tag / Nacht, Funktion: Tag=1 / Nacht=0

#endif
/** @}*/
