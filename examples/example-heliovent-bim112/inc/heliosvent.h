/**************************************************************************//**
 * @addtogroup SBLIB_EXAMPLES Selfbus library usage examples
 * @defgroup SBLIB_EXAMPLE_BIM112_HELIOVENT_1 BCU BIM112 Maskversion 0x0705 Example
 * @ingroup SBLIB_EXAMPLES
 * @brief   Header File for Selfbus ARM KNX Library, Device: KWL-EB, ApplicationProgram: M-0112_A-0001-10-C995
 * @details Header File for sblib (Selfbus ARM KNX Library) generated with KNXprodEditor 0.44
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
 * Geräteinformationen:
 * Device: KWL-EB
 * ApplicationProgram: M-0112_A-0001-10-C995
 *
 * @{
 *
 * @file   heliosvent.h
 * @author Darthyson <darth@maptrack.de> Copyright (c) 2021
 * @bug No known bugs.
 ******************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 */

#ifndef KNXPRODEDITORHEADER_H
#define KNXPRODEDITORHEADER_H

const byte hardwareVersion[6] = { 0, 0, 0, 0, 0, 0 }; // The hardware identification number hardwareVersion

#define MANUFACTURER 274 //!< Manufacturer ID
#define DEVICETYPE 1 //!< Device Type
#define APPVERSION 16 //!< Application Version




#define EE_PARAMETER_4600_VENTILATOR_EINSCHALTVERZOEGERUNG     0x4600 //!< Addr: 0x4600, Size: 0x0008, Ventilator Einschaltverzögerung
#define EE_PARAMETER_4601_VENTILATOR_AUSSCHALTVERZOEGERUNG     0x4601 //!< Addr: 0x4601, Size: 0x0008, Ventilator Ausschaltverzögerung
#define EE_PARAMETER_4610_DELTA_TEMPERATUR     0x4610 //!< Addr: 0x4610, Size: 0x0008, Delta Temperatur
#define EE_PARAMETER_4611_DELTA_CO2_KONZENTRATION     0x4611 //!< Addr: 0x4611, Size: 0x0008, Delta CO2-Konzentration
#define EE_PARAMETER_4612_DELTA_RELATIVE_FEUCHTE     0x4612 //!< Addr: 0x4612, Size: 0x0008, Delta relative Feuchte
#define EE_UNIONPARAMETER_4613     0x4613 //!< Addr: 0x4613, Size: 0x0008,     Übertragungszyklus  oder Nein Übertragungszyklus
#define EE_UNIONPARAMETER_4614     0x4614 //!< Addr: 0x4614, Size: 0x0008,     Wartezeit  oder Nein Wartezeit


#define COMOBJ_0_OBJECT_0_DATENPUNKT_0     0 //!< Com-Objekt Nummer: 0, Beschreibung: Object 0, Funktion: Datenpunkt 0
#define COMOBJ_1_LUEFTUNGSGERAET_HAUPTSCHALTER_SCHALTEN     1 //!< Com-Objekt Nummer: 1, Beschreibung: Lüftungsgerät - Hauptschalter, Funktion: Schalten
#define COMOBJ_2_LUEFTUNGSGERAET_HAUPTSCHALTER_STATUS     2 //!< Com-Objekt Nummer: 2, Beschreibung: Lüftungsgerät - Hauptschalter, Funktion: Status
#define COMOBJ_3_LUEFTUNGSGERAET_EXTERNER_KONTAKT_KNX_SCHALTEN     3 //!< Com-Objekt Nummer: 3, Beschreibung: Lüftungsgerät - Externer Kontakt KNX, Funktion: Schalten
#define COMOBJ_4_LUEFTUNGSGERAET_LEISTUNGSSTUFE_SOLL_STELLWERT     4 //!< Com-Objekt Nummer: 4, Beschreibung: Lüftungsgerät - Leistungsstufe Soll, Funktion: Stellwert
#define COMOBJ_5_LUEFTUNGSGERAET_LEISTUNGSSTUFE_IST_STATUSWERT     5 //!< Com-Objekt Nummer: 5, Beschreibung: Lüftungsgerät - Leistungsstufe Ist, Funktion: Statuswert
#define COMOBJ_6_LUEFTUNGSGERAET_LEISTUNGSSTUFE_MINIMUM_STELLWERT     6 //!< Com-Objekt Nummer: 6, Beschreibung: Lüftungsgerät - Leistungsstufe Minimum, Funktion: Stellwert
#define COMOBJ_7_LUEFTUNGSGERAET_LEISTUNGSSTUFE_MINIMUM_STATUSWERT     7 //!< Com-Objekt Nummer: 7, Beschreibung: Lüftungsgerät - Leistungsstufe Minimum, Funktion: Statuswert
#define COMOBJ_8_LUEFTUNGSGERAET_LEISTUNGSSTUFE_MAXIMUM_STELLWERT     8 //!< Com-Objekt Nummer: 8, Beschreibung: Lüftungsgerät - Leistungsstufe Maximum, Funktion: Stellwert
#define COMOBJ_9_LUEFTUNGSGERAET_LEISTUNGSSTUFE_MAXIMUM_STATUSWERT     9 //!< Com-Objekt Nummer: 9, Beschreibung: Lüftungsgerät - Leistungsstufe Maximum, Funktion: Statuswert
#define COMOBJ_10_LUEFTUNGSGERAET_ABLUFTVENTILATOR_EIN_AUS_STATUS     10 //!< Com-Objekt Nummer: 10, Beschreibung: Lüftungsgerät - Abluftventilator Ein/Aus, Funktion: Status
#define COMOBJ_11_LUEFTUNGSGERAET_ZULUFTVENTILATOR_EIN_AUS_STATUS     11 //!< Com-Objekt Nummer: 11, Beschreibung: Lüftungsgerät - Zuluftventilator Ein/Aus, Funktion: Status
#define COMOBJ_12_LUEFTUNGSGERAET_ZUSATZFUNKTION_STATUS     12 //!< Com-Objekt Nummer: 12, Beschreibung: Lüftungsgerät - Zusatzfunktion, Funktion: Status
#define COMOBJ_13_OBJECT_13_DATENPUNKT_13     13 //!< Com-Objekt Nummer: 13, Beschreibung: Object 13, Funktion: Datenpunkt 13
#define COMOBJ_14_OBJECT_14_DATENPUNKT_14     14 //!< Com-Objekt Nummer: 14, Beschreibung: Object 14, Funktion: Datenpunkt 14
#define COMOBJ_15_OBJECT_15_DATENPUNKT_15     15 //!< Com-Objekt Nummer: 15, Beschreibung: Object 15, Funktion: Datenpunkt 15
#define COMOBJ_16_OBJECT_16_DATENPUNKT_16     16 //!< Com-Objekt Nummer: 16, Beschreibung: Object 16, Funktion: Datenpunkt 16
#define COMOBJ_17_OBJECT_17_DATENPUNKT_17     17 //!< Com-Objekt Nummer: 17, Beschreibung: Object 17, Funktion: Datenpunkt 17
#define COMOBJ_18_OBJECT_18_DATENPUNKT_18     18 //!< Com-Objekt Nummer: 18, Beschreibung: Object 18, Funktion: Datenpunkt 18
#define COMOBJ_19_OBJECT_19_DATENPUNKT_19     19 //!< Com-Objekt Nummer: 19, Beschreibung: Object 19, Funktion: Datenpunkt 19
#define COMOBJ_20_OBJECT_20_DATENPUNKT_20     20 //!< Com-Objekt Nummer: 20, Beschreibung: Object 20, Funktion: Datenpunkt 20
#define COMOBJ_21_REGELUNG_INTERVALL_STELLWERT     21 //!< Com-Objekt Nummer: 21, Beschreibung: Regelung - Intervall, Funktion: Stellwert
#define COMOBJ_22_REGELUNG_INTERVALL_STATUSWERT     22 //!< Com-Objekt Nummer: 22, Beschreibung: Regelung - Intervall, Funktion: Statuswert
#define COMOBJ_23_CO2_KONZENTRATION_REGELUNG_EIN_AUS_SCHALTEN     23 //!< Com-Objekt Nummer: 23, Beschreibung: CO2 Konzentration - Regelung Ein/Aus, Funktion: Schalten
#define COMOBJ_24_CO2_KONZENTRATION_REGELUNG_EIN_AUS_STATUS     24 //!< Com-Objekt Nummer: 24, Beschreibung: CO2 Konzentration - Regelung Ein/Aus, Funktion: Status
#define COMOBJ_25_CO2_KONZENTRATION_GRENZWERT_STELLWERT     25 //!< Com-Objekt Nummer: 25, Beschreibung: CO2 Konzentration - Grenzwert, Funktion: Stellwert
#define COMOBJ_26_CO2_KONZENTRATION_GRENZWERT_STATUSWERT     26 //!< Com-Objekt Nummer: 26, Beschreibung: CO2 Konzentration - Grenzwert, Funktion: Statuswert
#define COMOBJ_27_CO2_KONZENTRATION_MAXIMUM_STATUSWERT     27 //!< Com-Objekt Nummer: 27, Beschreibung: CO2 Konzentration - Maximum, Funktion: Statuswert
#define COMOBJ_28_RELATIVE_FEUCHTE_REGELUNG_EIN_AUS_SCHALTEN     28 //!< Com-Objekt Nummer: 28, Beschreibung: Relative Feuchte - Regelung Ein/Aus, Funktion: Schalten
#define COMOBJ_29_RELATIVE_FEUCHTE_REGELUNG_EIN_AUS_STATUS     29 //!< Com-Objekt Nummer: 29, Beschreibung: Relative Feuchte - Regelung Ein/Aus, Funktion: Status
#define COMOBJ_30_RELATIVE_FEUCHTE_GRENZWERT_STELLWERT     30 //!< Com-Objekt Nummer: 30, Beschreibung: Relative Feuchte - Grenzwert, Funktion: Stellwert
#define COMOBJ_31_RELATIVE_FEUCHTE_GRENZWERT_STATUSWERT     31 //!< Com-Objekt Nummer: 31, Beschreibung: Relative Feuchte - Grenzwert, Funktion: Statuswert
#define COMOBJ_32_RELATIVE_FEUCHTE_FUEHLER_1_STATUSWERT     32 //!< Com-Objekt Nummer: 32, Beschreibung: Relative Feuchte - Fühler 1, Funktion: Statuswert
#define COMOBJ_33_RELATIVE_FEUCHTE_FUEHLER_2_STATUSWERT     33 //!< Com-Objekt Nummer: 33, Beschreibung: Relative Feuchte - Fühler 2, Funktion: Statuswert
#define COMOBJ_34_OBJECT_34_DATENPUNKT_34     34 //!< Com-Objekt Nummer: 34, Beschreibung: Object 34, Funktion: Datenpunkt 34
#define COMOBJ_35_OBJECT_35_DATENPUNKT_35     35 //!< Com-Objekt Nummer: 35, Beschreibung: Object 35, Funktion: Datenpunkt 35
#define COMOBJ_36_OBJECT_36_DATENPUNKT_36     36 //!< Com-Objekt Nummer: 36, Beschreibung: Object 36, Funktion: Datenpunkt 36
#define COMOBJ_37_OBJECT_37_DATENPUNKT_37     37 //!< Com-Objekt Nummer: 37, Beschreibung: Object 37, Funktion: Datenpunkt 37
#define COMOBJ_38_OBJECT_38_DATENPUNKT_38     38 //!< Com-Objekt Nummer: 38, Beschreibung: Object 38, Funktion: Datenpunkt 38
#define COMOBJ_39_OBJECT_39_DATENPUNKT_39     39 //!< Com-Objekt Nummer: 39, Beschreibung: Object 39, Funktion: Datenpunkt 39
#define COMOBJ_40_OBJECT_40_DATENPUNKT_40     40 //!< Com-Objekt Nummer: 40, Beschreibung: Object 40, Funktion: Datenpunkt 40
#define COMOBJ_41_TEMPERATURFUEHLER_AUSSENLUFT_STATUSWERT     41 //!< Com-Objekt Nummer: 41, Beschreibung: Temperaturfühler - Außenluft, Funktion: Statuswert
#define COMOBJ_42_TEMPERATURFUEHLER_FORTLUFT_STATUSWERT     42 //!< Com-Objekt Nummer: 42, Beschreibung: Temperaturfühler - Fortluft, Funktion: Statuswert
#define COMOBJ_43_TEMPERATURFUEHLER_ABLUFT_STATUSWERT     43 //!< Com-Objekt Nummer: 43, Beschreibung: Temperaturfühler - Abluft, Funktion: Statuswert
#define COMOBJ_44_TEMPERATURFUEHLER_ZULUFT_STATUSWERT     44 //!< Com-Objekt Nummer: 44, Beschreibung: Temperaturfühler - Zuluft, Funktion: Statuswert
#define COMOBJ_45_FROSTSCHUTZ_GRENZWERT_STELLWERT     45 //!< Com-Objekt Nummer: 45, Beschreibung: Frostschutz - Grenzwert, Funktion: Stellwert
#define COMOBJ_46_FROSTSCHUTZ_GRENZWERT_STATUSWERT     46 //!< Com-Objekt Nummer: 46, Beschreibung: Frostschutz - Grenzwert, Funktion: Statuswert
#define COMOBJ_47_FROSTSCHUTZ_HYSTERESEWERT_STELLWERT     47 //!< Com-Objekt Nummer: 47, Beschreibung: Frostschutz - Hysteresewert, Funktion: Stellwert
#define COMOBJ_48_FROSTSCHUTZ_HYSTERESEWERT_STATUSWERT     48 //!< Com-Objekt Nummer: 48, Beschreibung: Frostschutz - Hysteresewert, Funktion: Statuswert
#define COMOBJ_49_BYPASSFUNKTION_EIN_AUS_STATUS     49 //!< Com-Objekt Nummer: 49, Beschreibung: Bypassfunktion - Ein/Aus, Funktion: Status
#define COMOBJ_50_BYPASSFUNKTION_GRENZWERT_STELLWERT     50 //!< Com-Objekt Nummer: 50, Beschreibung: Bypassfunktion - Grenzwert, Funktion: Stellwert
#define COMOBJ_51_BYPASSFUNKTION_GRENZWERT_STATUSWERT     51 //!< Com-Objekt Nummer: 51, Beschreibung: Bypassfunktion - Grenzwert, Funktion: Statuswert
#define COMOBJ_52_VORHEIZUNG_EIN_AUS_STATUS     52 //!< Com-Objekt Nummer: 52, Beschreibung: Vorheizung - Ein/Aus, Funktion: Status
#define COMOBJ_53_VORHEIZUNG_EINSCHALTTEMPERATUR_STELLWERT     53 //!< Com-Objekt Nummer: 53, Beschreibung: Vorheizung - Einschalttemperatur, Funktion: Stellwert
#define COMOBJ_54_VORHEIZUNG_EINSCHALTTEMPERATUR_STATUSWERT     54 //!< Com-Objekt Nummer: 54, Beschreibung: Vorheizung - Einschalttemperatur, Funktion: Statuswert
#define COMOBJ_55_NACHHEIZUNG_EIN_AUS_SCHALTEN     55 //!< Com-Objekt Nummer: 55, Beschreibung: Nachheizung - Ein/Aus, Funktion: Schalten
#define COMOBJ_56_NACHHEIZUNG_EIN_AUS_STATUS     56 //!< Com-Objekt Nummer: 56, Beschreibung: Nachheizung - Ein/Aus, Funktion: Status
#define COMOBJ_57_NACHHEIZUNG_HEIZUNGSTYP_SCHALTEN     57 //!< Com-Objekt Nummer: 57, Beschreibung: Nachheizung - Heizungstyp, Funktion: Schalten
#define COMOBJ_58_NACHHEIZUNG_HEIZUNGSTYP_STATUS     58 //!< Com-Objekt Nummer: 58, Beschreibung: Nachheizung - Heizungstyp, Funktion: Status
#define COMOBJ_59_NACHHEIZUNG_SOLLTEMPERATUR_STELLWERT     59 //!< Com-Objekt Nummer: 59, Beschreibung: Nachheizung - Solltemperatur, Funktion: Stellwert
#define COMOBJ_60_NACHHEIZUNG_SOLLTEMPERATUR_STATUSWERT     60 //!< Com-Objekt Nummer: 60, Beschreibung: Nachheizung - Solltemperatur, Funktion: Statuswert
#define COMOBJ_61_ALARM_NOTFALLSCHALTER_SCHALTEN     61 //!< Com-Objekt Nummer: 61, Beschreibung: Alarm - Notfallschalter, Funktion: Schalten
#define COMOBJ_62_ALARM_FILTER_WARTUNG_STATUS     62 //!< Com-Objekt Nummer: 62, Beschreibung: Alarm - Filter Wartung, Funktion: Status
#define COMOBJ_63_ALARM_FEHLERCODE_WERT     63 //!< Com-Objekt Nummer: 63, Beschreibung: Alarm - Fehlercode, Funktion: Wert

#endif /* KNXPRODEDITORHEADER_H */
/** @}*/
