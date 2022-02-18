#ifndef KNXPRODEDITORHEADER_GLASTASTER_H
#define KNXPRODEDITORHEADER_GLASTASTER_H
/**
 * Header File for DIY KNX Device
 * generated with KNXprodEditor 0.50
 * 
 * Format: 
 * MANUFACTURER = knxMaster->Manufacturer->KnxManufacturerId
 * DEVICETYPE = ApplicationProgram->ApplicationNumber
 * APPVERSION = ApplicationProgram->ApplicationVersion
 * 
 * Parameter: EE_...
 * Simple parameter: EE_PARAMETER_[Name]   [Address in hex] //Addr:[Address in hex], Size:[Size in hex], Description in language 1
 * Union Parameter: EE_UNIONPARAMETER_[Address in hex]   [Address in hex] //Addr:[Address in hex], Size:[Size in hex], Description Parameter 1 in language 1, Description Parameter 2 in language 1, ...
 * 
 * Communication objects: COMOBJ...
 * COMOBJ_[Description]_[Function text]   [Communication object number] //Com-Objekt number: [number] ,Description: [in language 1], Function [in language 1]
 *
 *
 *
 * Device information:
 * Device: BE-GT2Tx.01 Glass Push Button II Smart with Temperature sensor
 * ApplicationProgram: M-0083_A-008A-28-C2AA
 *
 */


#define MANUFACTURER 131 //!< Manufacturer ID
#define DEVICETYPE 138 //!< Device Type
#define APPVERSION 40 //!< Application Version

const unsigned char hardwareVersion[6] = { 0x00, 0x00, 0x00, 0x00, 0x02, 0x27 }; //!< The hardware identification number hardwareVersion



#define EE_PARAMETER_4622_REAKTIONSZEIT_BEI_TASTENDRUCK     0x4622 //!< Addr: 0x4622, Size: 0x0010, Reaktionszeit bei Tastendruck 
#define EE_PARAMETER_4624_ZEIT_LANGER_TASTENDRUCK     0x4624 //!< Addr: 0x4624, Size: 0x0010, Zeit langer Tastendruck 
#define EE_UNIONPARAMETER_462E     0x462E //!< Addr: 0x462E, Size: 0x0020, OM_InputUsage_Values00_0 oder Szenen Nummer  oder Wert für losgelassene Taste  oder Wert für losgelassene Taste  oder     1. Umschaltwert  oder     1. Umschaltwert  oder     1. Umschaltwert °C  oder     1. Umschaltwert Lux  oder     RGB-Wert  oder     Schrittweite  oder     Schrittweite  oder Schrittweite K  oder Wert für losgelassene Taste Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4632     0x4632 //!< Addr: 0x4632, Size: 0x0020, OM_InputUsage_Values01_0 oder Wert für betätigte Taste  oder Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert  oder     2. Umschaltwert  oder     2. Umschaltwert °C  oder     2. Umschaltwert Lux  oder     RGB-Wert  oder Farbtemperaturwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4636     0x4636 //!< Addr: 0x4636, Size: 0x0020,     Szene Nummer  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert °C  oder     3. Umschaltwert Lux  oder     RGB-Wert  oder     Wert rechte Taste  oder Unterer Grenzwert K  oder Unterer Grenzwert °C  oder     3. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_463A     0x463A //!< Addr: 0x463A, Size: 0x0020,     1Byte-Wert (0...255)  oder     Szene Nummer  oder     4. Umschaltwert  oder     4. Umschaltwert  oder     4. Umschaltwert  oder     4. Umschaltwert °C  oder     4. Umschaltwert Lux  oder     RGB-Wert  oder     Wert linke Taste  oder Oberer Grenzwert K  oder Oberer Grenzwert °C  oder     4. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_PARAMETER_463E_OM_INPUTUSAGE_VALUETYPE_00_0     0x463E //!< Addr: 0x463E, Size: 0x0008, OM_InputUsage_valueType_00_0
#define EE_UNIONPARAMETER_4640     0x4640 //!< Addr: 0x4640, Size: 0x0008, Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder OM_InputUsage_valueType02_0 oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_4641     0x4641 //!< Addr: 0x4641, Size: 0x0008, Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder OM_InputUsage_valueType03_0 oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_4648     0x4648 //!< Addr: 0x4648, Size: 0x0010, Zeitverzögerung  oder OM_inputUsage_timeDuration_0 oder     Wiederholungszeit  oder Individuelle Zeit für langen Tastendruck  oder Zeitverzögerung zwischen den Szenenumschaltungen 
#define EE_PARAMETER_4650_OM_INPUTUSAGE_TIMERONACTIVE_0     0x4650 //!< Addr: 0x4650, Size: 0x0001, OM_InputUsage_timerOnActive_0
#define EE_PARAMETER_4651_OM_INPUTUSAGE_TIMEROFFACTIVE_0     0x4651 //!< Addr: 0x4651, Size: 0x0001, OM_InputUsage_timerOffActive_0
#define EE_PARAMETER_464B_OM_INPUTUSAGE_OBJTYPE_0     0x464B //!< Addr: 0x464B, Size: 0x0008, OM_inputUsage_objType_0
#define EE_UNIONPARAMETER_465A     0x465A //!< Addr: 0x465A, Size: 0x0006,     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol 
#define EE_UNIONPARAMETER_4659     0x4659 //!< Addr: 0x4659, Size: 0x00A0, FunctionDescription_0 oder     Text  oder     Text für "Aus"  oder     Text für "Ein" 
#define EE_UNIONPARAMETER_4673     0x4673 //!< Addr: 0x4673, Size: 0x0008,     Symbol für 0%  oder     Sonderdarstellung  oder     Sonderdarstellung  oder     Sonderdarstellung  oder HSV Funktionsauswahl  oder Datenpunkttyp für Statusobjekt  oder FunctionIconsZero_0
#define EE_UNIONPARAMETER_4684     0x4684 //!< Addr: 0x4684, Size: 0x0020, Szenen Nummer  oder Wert für losgelassene Taste  oder Wert für losgelassene Taste  oder OM_inputUsage_values00_1 oder     1. Umschaltwert  oder     1. Umschaltwert  oder     1. Umschaltwert °C  oder     1. Umschaltwert Lux  oder     RGB-Wert  oder Wert für losgelassene Taste Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4688     0x4688 //!< Addr: 0x4688, Size: 0x0020, Wert für betätigte Taste  oder Wert für betätigte Taste  oder OM_InputUsage_Values01_1 oder     2. Umschaltwert  oder     2. Umschaltwert  oder     2. Umschaltwert  oder     2. Umschaltwert °C  oder     2. Umschaltwert Lux  oder     RGB-Wert  oder Farbtemperaturwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_468C     0x468C //!< Addr: 0x468C, Size: 0x0020,     Szene Nummer  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert °C  oder     3. Umschaltwert Lux  oder     RGB-Wert  oder     3. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4690     0x4690 //!< Addr: 0x4690, Size: 0x0020,     1Byte-Wert (0...255)  oder     Szene Nummer  oder     4. Umschaltwert  oder     4. Umschaltwert  oder     4. Umschaltwert °C  oder     4. Umschaltwert Lux  oder     RGB-Wert  oder     4. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_PARAMETER_4694_OM_INPUTUSAGE_VALUETYPE_00_1     0x4694 //!< Addr: 0x4694, Size: 0x0008, OM_InputUsage_valueType_00_1
#define EE_UNIONPARAMETER_4697     0x4697 //!< Addr: 0x4697, Size: 0x0008, OM_InputUsage_valueType03_1 oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_469E     0x469E //!< Addr: 0x469E, Size: 0x0010, Zeitverzögerung  oder OM_inputUsage_timeDuration_1 oder     Wiederholungszeit  oder Individuelle Zeit für langen Tastendruck  oder Zeitverzögerung zwischen den Szenenumschaltungen 
#define EE_PARAMETER_46A6_OM_INPUTUSAGE_TIMERCONFIGON_1     0x46A6 //!< Addr: 0x46A6, Size: 0x0001, OM_InputUsage_timerConfigOn_1
#define EE_PARAMETER_46A7_OM_INPUTUSAGE_TIMERCONFIGOFF_1     0x46A7 //!< Addr: 0x46A7, Size: 0x0001, OM_InputUsage_timerConfigOff_1
#define EE_PARAMETER_46A1_OM_INPUTUSAGE_OBJTYPE_1     0x46A1 //!< Addr: 0x46A1, Size: 0x0008, OM_inputUsage_objType_1
#define EE_PARAMETER_4AAB_OBJEKT_FUER_PRIORITAET     0x4AAB //!< Addr: 0x4AAB, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4AAC_BEI_TAG     0x4AAC //!< Addr: 0x4AAC, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4ABE_OBJEKT_FUER_PRIORITAET     0x4ABE //!< Addr: 0x4ABE, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4ABF_BEI_TAG     0x4ABF //!< Addr: 0x4ABF, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4B9A_LC_LOGICPARAMETER_LOGICOPERATION_0     0x4B9A //!< Addr: 0x4B9A, Size: 0x0008, LC_logicParameter_logicOperation_0
#define EE_UNIONPARAMETER_46B0     0x46B0 //!< Addr: 0x46B0, Size: 0x0006,     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol 
#define EE_UNIONPARAMETER_46AF     0x46AF //!< Addr: 0x46AF, Size: 0x00A0, FunctionDescription_1 oder     Text  oder     Text für "Aus"  oder     Text für "Ein" 
#define EE_UNIONPARAMETER_46C9     0x46C9 //!< Addr: 0x46C9, Size: 0x0008,     Symbol für 0%  oder     Sonderdarstellung  oder     Sonderdarstellung  oder HSV Funktionsauswahl  oder Datenpunkttyp für Statusobjekt  oder FunctionIconsZero_1
#define EE_UNIONPARAMETER_46DA     0x46DA //!< Addr: 0x46DA, Size: 0x0020, Wert für losgelassene Taste  oder Wert für losgelassene Taste  oder     1. Umschaltwert  oder     1. Umschaltwert  oder Szenen Nummer  oder     1. Umschaltwert °C  oder     1. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values00_2 oder     Schrittweite  oder     Schrittweite  oder Schrittweite K  oder Wert für losgelassene Taste Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_46DE     0x46DE //!< Addr: 0x46DE, Size: 0x0020, Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert  oder Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert °C  oder     2. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values01_2 oder Farbtemperaturwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_46E2     0x46E2 //!< Addr: 0x46E2, Size: 0x0020,     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     Szene Nummer  oder     3. Umschaltwert °C  oder     3. Umschaltwert Lux  oder     RGB-Wert  oder     3. Umschaltwert  oder     Wert rechte Taste  oder Unterer Grenzwert K  oder Unterer Grenzwert °C  oder     3. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_46E6     0x46E6 //!< Addr: 0x46E6, Size: 0x0020,     1Byte-Wert (0...255)  oder     4. Umschaltwert  oder     4. Umschaltwert  oder     Szene Nummer  oder     4. Umschaltwert °C  oder     4. Umschaltwert Lux  oder     RGB-Wert  oder     4. Umschaltwert  oder     Wert linke Taste  oder Oberer Grenzwert K  oder Oberer Grenzwert °C  oder     4. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_46EC     0x46EC //!< Addr: 0x46EC, Size: 0x0008, Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder OM_InputUsage_valueType02_2 oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_46ED     0x46ED //!< Addr: 0x46ED, Size: 0x0008, OM_InputUsage_valueType03_2 oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_46F4     0x46F4 //!< Addr: 0x46F4, Size: 0x0010, Zeitverzögerung  oder Individuelle Zeit für langen Tastendruck  oder OM_inputUsage_timeDuration_2 oder Zeitverzögerung zwischen den Szenenumschaltungen  oder     Wiederholungszeit 
#define EE_UNIONPARAMETER_4706     0x4706 //!< Addr: 0x4706, Size: 0x0006,     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol 
#define EE_UNIONPARAMETER_4705     0x4705 //!< Addr: 0x4705, Size: 0x00A0, FunctionDescription_2 oder     Text  oder     Text für "Aus"  oder     Text für "Ein" 
#define EE_UNIONPARAMETER_471F     0x471F //!< Addr: 0x471F, Size: 0x0008,     Symbol für 0%  oder     Sonderdarstellung  oder     Sonderdarstellung  oder     Sonderdarstellung  oder HSV Funktionsauswahl  oder Datenpunkttyp für Statusobjekt  oder FunctionIconsZero_2
#define EE_UNIONPARAMETER_4730     0x4730 //!< Addr: 0x4730, Size: 0x0020, Wert für losgelassene Taste  oder Wert für losgelassene Taste  oder     1. Umschaltwert  oder     1. Umschaltwert  oder Szenen Nummer  oder     1. Umschaltwert °C  oder     1. Umschaltwert Lux  oder     RGB-Wert  oder OM_inputUsage_values00_3 oder Wert für losgelassene Taste Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4734     0x4734 //!< Addr: 0x4734, Size: 0x0020, Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert  oder Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert °C  oder     2. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values01_3 oder Farbtemperaturwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4738     0x4738 //!< Addr: 0x4738, Size: 0x0020,     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     Szene Nummer  oder     3. Umschaltwert °C  oder     3. Umschaltwert Lux  oder     RGB-Wert  oder     3. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_473C     0x473C //!< Addr: 0x473C, Size: 0x0020,     1Byte-Wert (0...255)  oder     4. Umschaltwert  oder     4. Umschaltwert  oder     Szene Nummer  oder     4. Umschaltwert °C  oder     4. Umschaltwert Lux  oder     RGB-Wert  oder     4. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4743     0x4743 //!< Addr: 0x4743, Size: 0x0008, OM_InputUsage_valueType03_3 oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_474A     0x474A //!< Addr: 0x474A, Size: 0x0010, Zeitverzögerung  oder Individuelle Zeit für langen Tastendruck  oder Zeitverzögerung zwischen den Szenenumschaltungen  oder OM_inputUsage_timeDuration_3 oder     Wiederholungszeit 
#define EE_UNIONPARAMETER_475C     0x475C //!< Addr: 0x475C, Size: 0x0006,     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol 
#define EE_UNIONPARAMETER_475B     0x475B //!< Addr: 0x475B, Size: 0x00A0, FunctionDescription_3 oder     Text  oder     Text für "Aus"  oder     Text für "Ein" 
#define EE_UNIONPARAMETER_4775     0x4775 //!< Addr: 0x4775, Size: 0x0008,     Symbol für 0%  oder     Sonderdarstellung  oder     Sonderdarstellung  oder HSV Funktionsauswahl  oder Datenpunkttyp für Statusobjekt  oder FunctionIconsZero_3
#define EE_UNIONPARAMETER_4786     0x4786 //!< Addr: 0x4786, Size: 0x0020, Wert für losgelassene Taste  oder     1. Umschaltwert  oder     1. Umschaltwert  oder Wert für losgelassene Taste  oder Szenen Nummer  oder     1. Umschaltwert °C  oder     1. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values00_4 oder     Schrittweite  oder     Schrittweite  oder Schrittweite K  oder Wert für losgelassene Taste Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_478A     0x478A //!< Addr: 0x478A, Size: 0x0020, Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert  oder Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert °C  oder     2. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values01_4 oder Farbtemperaturwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_478E     0x478E //!< Addr: 0x478E, Size: 0x0020,     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     Szene Nummer  oder     3. Umschaltwert °C  oder     3. Umschaltwert Lux  oder     RGB-Wert  oder     3. Umschaltwert  oder     Wert rechte Taste  oder Unterer Grenzwert K  oder Unterer Grenzwert °C  oder     3. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4792     0x4792 //!< Addr: 0x4792, Size: 0x0020,     4. Umschaltwert  oder     4. Umschaltwert  oder     1Byte-Wert (0...255)  oder     Szene Nummer  oder     4. Umschaltwert °C  oder     4. Umschaltwert Lux  oder     RGB-Wert  oder     4. Umschaltwert  oder     Wert linke Taste  oder Oberer Grenzwert K  oder Oberer Grenzwert °C  oder     4. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4798     0x4798 //!< Addr: 0x4798, Size: 0x0008, Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder OM_InputUsage_valueType02_4 oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_4799     0x4799 //!< Addr: 0x4799, Size: 0x0008,     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder OM_InputUsage_valueType03_4 oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_47A0     0x47A0 //!< Addr: 0x47A0, Size: 0x0010, Zeitverzögerung  oder Individuelle Zeit für langen Tastendruck  oder OM_inputUsage_timeDuration_4 oder Zeitverzögerung zwischen den Szenenumschaltungen  oder     Wiederholungszeit 
#define EE_UNIONPARAMETER_47B2     0x47B2 //!< Addr: 0x47B2, Size: 0x0006,     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol 
#define EE_UNIONPARAMETER_47B1     0x47B1 //!< Addr: 0x47B1, Size: 0x00A0, FunctionDescription_4 oder     Text  oder     Text für "Aus"  oder     Text für "Ein" 
#define EE_UNIONPARAMETER_47CB     0x47CB //!< Addr: 0x47CB, Size: 0x0008,     Symbol für 0%  oder     Sonderdarstellung  oder     Sonderdarstellung  oder HSV Funktionsauswahl  oder Datenpunkttyp für Statusobjekt  oder     Sonderdarstellung  oder FunctionIconsZero_4
#define EE_UNIONPARAMETER_47DC     0x47DC //!< Addr: 0x47DC, Size: 0x0020, Wert für losgelassene Taste  oder Wert für losgelassene Taste  oder     1. Umschaltwert  oder     1. Umschaltwert  oder Szenen Nummer  oder     1. Umschaltwert °C  oder     1. Umschaltwert Lux  oder     RGB-Wert  oder OM_inputUsage_values00_5 oder Wert für losgelassene Taste Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_47E0     0x47E0 //!< Addr: 0x47E0, Size: 0x0020, Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert  oder Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert °C  oder     2. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values01_5 oder Farbtemperaturwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_47E4     0x47E4 //!< Addr: 0x47E4, Size: 0x0020,     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     Szene Nummer  oder     3. Umschaltwert °C  oder     3. Umschaltwert Lux  oder     RGB-Wert  oder     3. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_47E8     0x47E8 //!< Addr: 0x47E8, Size: 0x0020,     4. Umschaltwert  oder     4. Umschaltwert  oder     1Byte-Wert (0...255)  oder     Szene Nummer  oder     4. Umschaltwert °C  oder     4. Umschaltwert Lux  oder     RGB-Wert  oder     4. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_47EF     0x47EF //!< Addr: 0x47EF, Size: 0x0008,     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder OM_InputUsage_valueType03_5 oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_47F6     0x47F6 //!< Addr: 0x47F6, Size: 0x0010, Zeitverzögerung  oder Zeitverzögerung zwischen den Szenenumschaltungen  oder Individuelle Zeit für langen Tastendruck  oder OM_inputUsage_timeDuration_5 oder     Wiederholungszeit 
#define EE_UNIONPARAMETER_4808     0x4808 //!< Addr: 0x4808, Size: 0x0006,     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol 
#define EE_UNIONPARAMETER_4807     0x4807 //!< Addr: 0x4807, Size: 0x00A0, FunctionDescription_5 oder     Text  oder     Text für "Aus"  oder     Text für "Ein" 
#define EE_UNIONPARAMETER_4821     0x4821 //!< Addr: 0x4821, Size: 0x0008,     Symbol für 0%  oder     Sonderdarstellung  oder     Sonderdarstellung  oder HSV Funktionsauswahl  oder Datenpunkttyp für Statusobjekt  oder FunctionIconsZero_5
#define EE_UNIONPARAMETER_4832     0x4832 //!< Addr: 0x4832, Size: 0x0020, Wert für losgelassene Taste  oder     1. Umschaltwert  oder     1. Umschaltwert  oder Wert für losgelassene Taste  oder Szenen Nummer  oder     1. Umschaltwert °C  oder     1. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values00_6 oder     Schrittweite  oder     Schrittweite  oder Schrittweite K  oder Wert für losgelassene Taste Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4836     0x4836 //!< Addr: 0x4836, Size: 0x0020, Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert  oder Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert °C  oder     2. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values01_6 oder Farbtemperaturwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_483A     0x483A //!< Addr: 0x483A, Size: 0x0020,     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     Szene Nummer  oder     3. Umschaltwert °C  oder     3. Umschaltwert Lux  oder     RGB-Wert  oder     3. Umschaltwert  oder     Wert rechte Taste  oder Unterer Grenzwert K  oder Unterer Grenzwert °C  oder     3. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_483E     0x483E //!< Addr: 0x483E, Size: 0x0020,     4. Umschaltwert  oder     4. Umschaltwert  oder     1Byte-Wert (0...255)  oder     Szene Nummer  oder     4. Umschaltwert °C  oder     4. Umschaltwert Lux  oder     RGB-Wert  oder     4. Umschaltwert  oder     Wert linke Taste  oder Oberer Grenzwert K  oder Oberer Grenzwert °C  oder     4. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4844     0x4844 //!< Addr: 0x4844, Size: 0x0008, Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder OM_InputUsage_valueType02_6 oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_4845     0x4845 //!< Addr: 0x4845, Size: 0x0008,     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder OM_InputUsage_valueType03_6 oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_484C     0x484C //!< Addr: 0x484C, Size: 0x0010, Zeitverzögerung  oder Individuelle Zeit für langen Tastendruck  oder OM_InputUsage_timeDuration_6 oder Zeitverzögerung zwischen den Szenenumschaltungen  oder     Wiederholungszeit 
#define EE_UNIONPARAMETER_485E     0x485E //!< Addr: 0x485E, Size: 0x0006,     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol 
#define EE_UNIONPARAMETER_485D     0x485D //!< Addr: 0x485D, Size: 0x00A0, FunctionDescription_6 oder     Text  oder     Text für "Aus"  oder     Text für "Ein" 
#define EE_UNIONPARAMETER_4877     0x4877 //!< Addr: 0x4877, Size: 0x0008,     Symbol für 0%  oder     Sonderdarstellung  oder     Sonderdarstellung  oder HSV Funktionsauswahl  oder Datenpunkttyp für Statusobjekt  oder     Sonderdarstellung  oder FunctionIconsZero_6
#define EE_UNIONPARAMETER_4888     0x4888 //!< Addr: 0x4888, Size: 0x0020, Wert für losgelassene Taste  oder Wert für losgelassene Taste  oder     1. Umschaltwert  oder     1. Umschaltwert  oder Szenen Nummer  oder     1. Umschaltwert °C  oder     1. Umschaltwert Lux  oder     RGB-Wert  oder OM_inputUsage_values00_7 oder Wert für losgelassene Taste Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_488C     0x488C //!< Addr: 0x488C, Size: 0x0020, Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert  oder Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert °C  oder     2. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values01_7 oder Farbtemperaturwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4890     0x4890 //!< Addr: 0x4890, Size: 0x0020,     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     Szene Nummer  oder     3. Umschaltwert °C  oder     3. Umschaltwert Lux  oder     RGB-Wert  oder     3. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4894     0x4894 //!< Addr: 0x4894, Size: 0x0020,     4. Umschaltwert  oder     4. Umschaltwert  oder     1Byte-Wert (0...255)  oder     Szene Nummer  oder     4. Umschaltwert °C  oder     4. Umschaltwert Lux  oder     RGB-Wert  oder     4. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_489B     0x489B //!< Addr: 0x489B, Size: 0x0008,     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder OM_InputUsage_valueType03_7 oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_48A2     0x48A2 //!< Addr: 0x48A2, Size: 0x0010, Zeitverzögerung  oder Zeitverzögerung zwischen den Szenenumschaltungen  oder Individuelle Zeit für langen Tastendruck  oder OM_inputUsage_timeDuration_7 oder     Wiederholungszeit 
#define EE_UNIONPARAMETER_48B4     0x48B4 //!< Addr: 0x48B4, Size: 0x0006,     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol 
#define EE_UNIONPARAMETER_48B3     0x48B3 //!< Addr: 0x48B3, Size: 0x00A0, FunctionDescription_7 oder     Text  oder     Text für "Aus"  oder     Text für "Ein" 
#define EE_UNIONPARAMETER_48CD     0x48CD //!< Addr: 0x48CD, Size: 0x0008,     Symbol für 0%  oder     Sonderdarstellung  oder     Sonderdarstellung  oder HSV Funktionsauswahl  oder Datenpunkttyp für Statusobjekt  oder FunctionIconsZero_7
#define EE_UNIONPARAMETER_48DE     0x48DE //!< Addr: 0x48DE, Size: 0x0020, Wert für losgelassene Taste  oder     1. Umschaltwert  oder     1. Umschaltwert  oder Wert für losgelassene Taste  oder Szenen Nummer  oder     1. Umschaltwert °C  oder     1. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values00_8 oder     Schrittweite  oder     Schrittweite  oder Schrittweite K  oder Wert für losgelassene Taste Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_48E2     0x48E2 //!< Addr: 0x48E2, Size: 0x0020, Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert  oder Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert °C  oder     2. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values01_8 oder Farbtemperaturwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_48E6     0x48E6 //!< Addr: 0x48E6, Size: 0x0020,     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     Szene Nummer  oder     3. Umschaltwert °C  oder     3. Umschaltwert Lux  oder     RGB-Wert  oder     3. Umschaltwert  oder     Wert rechte Taste  oder Unterer Grenzwert K  oder Unterer Grenzwert °C  oder     3. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_48EA     0x48EA //!< Addr: 0x48EA, Size: 0x0020,     4. Umschaltwert  oder     4. Umschaltwert  oder     1Byte-Wert (0...255)  oder     Szene Nummer  oder     4. Umschaltwert °C  oder     4. Umschaltwert Lux  oder     RGB-Wert  oder     4. Umschaltwert  oder     Wert linke Taste  oder Oberer Grenzwert K  oder Oberer Grenzwert °C  oder     4. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_48F0     0x48F0 //!< Addr: 0x48F0, Size: 0x0008, Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder OM_InputUsage_valueType02_8 oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_48F1     0x48F1 //!< Addr: 0x48F1, Size: 0x0008,     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder OM_InputUsage_valueType03_8 oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_48F8     0x48F8 //!< Addr: 0x48F8, Size: 0x0010, Zeitverzögerung  oder Individuelle Zeit für langen Tastendruck  oder OM_inputUsage_timeDuration_8 oder Zeitverzögerung zwischen den Szenenumschaltungen  oder     Wiederholungszeit 
#define EE_UNIONPARAMETER_490A     0x490A //!< Addr: 0x490A, Size: 0x0006,     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol 
#define EE_UNIONPARAMETER_4909     0x4909 //!< Addr: 0x4909, Size: 0x00A0, FunctionDescription_8 oder     Text  oder     Text für "Aus"  oder     Text für "Ein" 
#define EE_UNIONPARAMETER_4923     0x4923 //!< Addr: 0x4923, Size: 0x0008,     Symbol für 0%  oder     Sonderdarstellung  oder     Sonderdarstellung  oder HSV Funktionsauswahl  oder Datenpunkttyp für Statusobjekt  oder     Sonderdarstellung  oder FunctionIconsZero_8
#define EE_UNIONPARAMETER_4934     0x4934 //!< Addr: 0x4934, Size: 0x0020, Wert für losgelassene Taste  oder Wert für losgelassene Taste  oder     1. Umschaltwert  oder     1. Umschaltwert  oder Szenen Nummer  oder     1. Umschaltwert °C  oder     1. Umschaltwert Lux  oder     RGB-Wert  oder OM_inputUsage_values00_9 oder Wert für losgelassene Taste Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4938     0x4938 //!< Addr: 0x4938, Size: 0x0020, Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert  oder Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert °C  oder     2. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values01_9 oder Farbtemperaturwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_493C     0x493C //!< Addr: 0x493C, Size: 0x0020,     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     Szene Nummer  oder     3. Umschaltwert °C  oder     3. Umschaltwert Lux  oder     RGB-Wert  oder     3. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4940     0x4940 //!< Addr: 0x4940, Size: 0x0020,     4. Umschaltwert  oder     4. Umschaltwert  oder     1Byte-Wert (0...255)  oder     Szene Nummer  oder     4. Umschaltwert °C  oder     4. Umschaltwert Lux  oder     RGB-Wert  oder     4. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4947     0x4947 //!< Addr: 0x4947, Size: 0x0008,     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder OM_InputUsage_valueType03_9 oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_494E     0x494E //!< Addr: 0x494E, Size: 0x0010, Zeitverzögerung  oder Zeitverzögerung zwischen den Szenenumschaltungen  oder Individuelle Zeit für langen Tastendruck  oder OM_InputUsage_timeDuration_9 oder     Wiederholungszeit 
#define EE_UNIONPARAMETER_4960     0x4960 //!< Addr: 0x4960, Size: 0x0006,     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol 
#define EE_UNIONPARAMETER_495F     0x495F //!< Addr: 0x495F, Size: 0x00A0, FunctionDescription_9 oder     Text  oder     Text für "Aus"  oder     Text für "Ein" 
#define EE_UNIONPARAMETER_4979     0x4979 //!< Addr: 0x4979, Size: 0x0008,     Symbol für 0%  oder     Sonderdarstellung  oder     Sonderdarstellung  oder HSV Funktionsauswahl  oder Datenpunkttyp für Statusobjekt  oder FunctionIconsZero_9
#define EE_UNIONPARAMETER_498A     0x498A //!< Addr: 0x498A, Size: 0x0020, Wert für losgelassene Taste  oder     1. Umschaltwert  oder     1. Umschaltwert  oder Wert für losgelassene Taste  oder Szenen Nummer  oder     1. Umschaltwert °C  oder     1. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values00_10 oder     Schrittweite  oder     Schrittweite  oder Schrittweite K  oder Wert für losgelassene Taste Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_498E     0x498E //!< Addr: 0x498E, Size: 0x0020, Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert  oder Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert °C  oder     2. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values01_10 oder Farbtemperaturwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4992     0x4992 //!< Addr: 0x4992, Size: 0x0020,     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     Szene Nummer  oder     3. Umschaltwert °C  oder     3. Umschaltwert Lux  oder     RGB-Wert  oder     3. Umschaltwert  oder     Wert rechte Taste  oder Unterer Grenzwert K  oder Unterer Grenzwert °C  oder     3. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4996     0x4996 //!< Addr: 0x4996, Size: 0x0020,     4. Umschaltwert  oder     4. Umschaltwert  oder     1Byte-Wert (0...255)  oder     Szene Nummer  oder     4. Umschaltwert °C  oder     4. Umschaltwert Lux  oder     RGB-Wert  oder     4. Umschaltwert  oder     Wert linke Taste  oder Oberer Grenzwert K  oder Oberer Grenzwert °C  oder     4. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_499C     0x499C //!< Addr: 0x499C, Size: 0x0008, Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder OM_InputUsage_valueType02_10 oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck  oder     Linke Taste: Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_499D     0x499D //!< Addr: 0x499D, Size: 0x0008,     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder OM_InputUsage_valueType03_10 oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder Linke Taste: Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_49A4     0x49A4 //!< Addr: 0x49A4, Size: 0x0010, Zeitverzögerung  oder Individuelle Zeit für langen Tastendruck  oder OM_inputUsage_timeDuration_10 oder Zeitverzögerung zwischen den Szenenumschaltungen  oder     Wiederholungszeit 
#define EE_UNIONPARAMETER_49B6     0x49B6 //!< Addr: 0x49B6, Size: 0x0006,     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol 
#define EE_UNIONPARAMETER_49B5     0x49B5 //!< Addr: 0x49B5, Size: 0x00A0, FunctionDescription_10 oder     Text  oder     Text für "Aus"  oder     Text für "Ein" 
#define EE_UNIONPARAMETER_49CF     0x49CF //!< Addr: 0x49CF, Size: 0x0008,     Symbol für 0%  oder     Sonderdarstellung  oder     Sonderdarstellung  oder HSV Funktionsauswahl  oder Datenpunkttyp für Statusobjekt  oder     Sonderdarstellung  oder FunctionIconsZero_10
#define EE_UNIONPARAMETER_49E0     0x49E0 //!< Addr: 0x49E0, Size: 0x0020, Wert für losgelassene Taste  oder Wert für losgelassene Taste  oder     1. Umschaltwert  oder     1. Umschaltwert  oder Szenen Nummer  oder     1. Umschaltwert °C  oder     1. Umschaltwert Lux  oder     RGB-Wert  oder OM_inputUsage_values00_11 oder Wert für losgelassene Taste Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_49E4     0x49E4 //!< Addr: 0x49E4, Size: 0x0020, Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert  oder Wert für betätigte Taste  oder     2. Umschaltwert  oder     2. Umschaltwert °C  oder     2. Umschaltwert Lux  oder     RGB-Wert  oder OM_InputUsage_Values01_11 oder Farbtemperaturwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_49E8     0x49E8 //!< Addr: 0x49E8, Size: 0x0020,     3. Umschaltwert  oder     3. Umschaltwert  oder     3. Umschaltwert  oder     Szene Nummer  oder     3. Umschaltwert °C  oder     3. Umschaltwert Lux  oder     RGB-Wert  oder     3. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_49EC     0x49EC //!< Addr: 0x49EC, Size: 0x0020,     4. Umschaltwert  oder     4. Umschaltwert  oder     1Byte-Wert (0...255)  oder     Szene Nummer  oder     4. Umschaltwert °C  oder     4. Umschaltwert Lux  oder     RGB-Wert  oder     4. Umschaltwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_49F3     0x49F3 //!< Addr: 0x49F3, Size: 0x0008,     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder OM_InputUsage_valueType03_11 oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck  oder     Aktion bei langem Tastendruck 
#define EE_UNIONPARAMETER_49FA     0x49FA //!< Addr: 0x49FA, Size: 0x0010, Zeitverzögerung  oder Zeitverzögerung zwischen den Szenenumschaltungen  oder Individuelle Zeit für langen Tastendruck  oder OM_InputUsage_timeDuration_11 oder     Wiederholungszeit 
#define EE_UNIONPARAMETER_4A0C     0x4A0C //!< Addr: 0x4A0C, Size: 0x0006,     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder     Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol  oder Statuswert als Text unter Symbol 
#define EE_UNIONPARAMETER_4A0B     0x4A0B //!< Addr: 0x4A0B, Size: 0x00A0, FunctionDescription_11 oder     Text  oder     Text für "Aus"  oder     Text für "Ein" 
#define EE_UNIONPARAMETER_4A25     0x4A25 //!< Addr: 0x4A25, Size: 0x0008,     Symbol für 0%  oder     Sonderdarstellung  oder     Sonderdarstellung  oder HSV Funktionsauswahl  oder Datenpunkttyp für Statusobjekt  oder FunctionIconsZero_11
#define EE_PARAMETER_4A91_AUSWAHL_DER_OBJEKTNUMMER     0x4A91 //!< Addr: 0x4A91, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4A92_BEI_TAG_WERT_EIN     0x4A92 //!< Addr: 0x4A92, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4A93_BEI_NACHT_WERT_EIN     0x4A93 //!< Addr: 0x4A93, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4A94_BEI_TAG_WERT_AUS     0x4A94 //!< Addr: 0x4A94, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4A95_BEI_NACHT_WERT_AUS     0x4A95 //!< Addr: 0x4A95, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4A96_VERHALTEN_BEI_TAG_WERT_EIN     0x4A96 //!< Addr: 0x4A96, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4A97_VERHALTEN_BEI_NACHT_WERT_EIN     0x4A97 //!< Addr: 0x4A97, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4A98_OBJEKT_FUER_PRIORITAET     0x4A98 //!< Addr: 0x4A98, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4A99_BEI_TAG     0x4A99 //!< Addr: 0x4A99, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4A9A_BEI_NACHT     0x4A9A //!< Addr: 0x4A9A, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4A9B_VERHALTEN_BEI_TAG_WERT_EIN     0x4A9B //!< Addr: 0x4A9B, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4A9C_VERHALTEN_BEI_NACHT_WERT_EIN     0x4A9C //!< Addr: 0x4A9C, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_UNIONPARAMETER_4A36     0x4A36 //!< Addr: 0x4A36, Size: 0x0018,     1. Umschaltwert  oder     1. Umschaltwert  oder Wert für losgelassene Taste  oder Szenen Nummer  oder Wert für losgelassene Taste Kelvin  oder     1. Umschaltwert °C  oder     1. Umschaltwert Lux  oder     RGB-Wert  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4A42     0x4A42 //!< Addr: 0x4A42, Size: 0x0020,     1Byte-Wert (0...255)  oder     Szene Nummer  oder     Wert für Zwangsführung  oder     Prozentwert (0...100%)  oder     Temperaturwert °C  oder     Helligkeitswert Lux  oder     RGB-Wert  oder     Farbtemperaturwert Kelvin  oder     HSV-Wert 
#define EE_UNIONPARAMETER_4A49     0x4A49 //!< Addr: 0x4A49, Size: 0x0008, OM_InputUsage_valueType03_Panic
#define EE_UNIONPARAMETER_4A50     0x4A50 //!< Addr: 0x4A50, Size: 0x0010, Individuelle Zeit für langen Tastendruck 
#define EE_UNIONPARAMETER_4A90     0x4A90 //!< Addr: 0x4A90, Size: 0x0008, LED reagiert auf:  oder LED1_Function oder LED1_Function
#define EE_UNIONPARAMETER_4A9E     0x4A9E //!< Addr: 0x4A9E, Size: 0x0008,     EIN, wenn größer  oder LED1_ThresholdObjFFh
#define EE_UNIONPARAMETER_4AA3     0x4AA3 //!< Addr: 0x4AA3, Size: 0x0008, LED reagiert auf:  oder LED2_Function oder LED2_Function
#define EE_UNIONPARAMETER_4AB1     0x4AB1 //!< Addr: 0x4AB1, Size: 0x0008,     EIN, wenn größer  oder LED2_ThresholdObjFFh
#define EE_UNIONPARAMETER_4AB6     0x4AB6 //!< Addr: 0x4AB6, Size: 0x0008, LED reagiert auf:  oder LED3_Function oder LED3_Function
#define EE_UNIONPARAMETER_4AC4     0x4AC4 //!< Addr: 0x4AC4, Size: 0x0008,     EIN, wenn größer  oder LED3_ThresholdObjFFh
#define EE_UNIONPARAMETER_4AC9     0x4AC9 //!< Addr: 0x4AC9, Size: 0x0008, LED reagiert auf:  oder LED4_Function oder LED4_Function
#define EE_UNIONPARAMETER_4AD7     0x4AD7 //!< Addr: 0x4AD7, Size: 0x0008,     EIN, wenn größer  oder LED4_ThresholdObjFFh
#define EE_UNIONPARAMETER_4ADC     0x4ADC //!< Addr: 0x4ADC, Size: 0x0008, LED reagiert auf:  oder LED5_Function oder LED5_Function
#define EE_UNIONPARAMETER_4AEA     0x4AEA //!< Addr: 0x4AEA, Size: 0x0008,     EIN, wenn größer  oder LED5_ThresholdObjFFh
#define EE_UNIONPARAMETER_4AEF     0x4AEF //!< Addr: 0x4AEF, Size: 0x0008, LED reagiert auf:  oder LED6_Function oder LED6_Function
#define EE_UNIONPARAMETER_4AFD     0x4AFD //!< Addr: 0x4AFD, Size: 0x0008,     EIN, wenn größer  oder LED6_ThresholdObjFFh
#define EE_UNIONPARAMETER_4B02     0x4B02 //!< Addr: 0x4B02, Size: 0x0008, LED7_Function oder LED reagiert auf:  oder LED7_Function
#define EE_UNIONPARAMETER_4B15     0x4B15 //!< Addr: 0x4B15, Size: 0x0008, LED8_Function oder LED reagiert auf:  oder LED8_Function
#define EE_UNIONPARAMETER_4B10     0x4B10 //!< Addr: 0x4B10, Size: 0x0008,     EIN, wenn größer  oder LED7_ThresholdObjFFh
#define EE_UNIONPARAMETER_4B23     0x4B23 //!< Addr: 0x4B23, Size: 0x0008,     EIN, wenn größer  oder LED8_ThresholdObjFFh
#define EE_UNIONPARAMETER_4B28     0x4B28 //!< Addr: 0x4B28, Size: 0x0008, LED9_Function oder LED reagiert auf:  oder LED9_Function
#define EE_UNIONPARAMETER_4B36     0x4B36 //!< Addr: 0x4B36, Size: 0x0008,     EIN, wenn größer  oder LED9_ThresholdObjFFh
#define EE_UNIONPARAMETER_4B3B     0x4B3B //!< Addr: 0x4B3B, Size: 0x0008, LED10_Function oder LED reagiert auf:  oder LED10_Function
#define EE_UNIONPARAMETER_4B49     0x4B49 //!< Addr: 0x4B49, Size: 0x0008,     EIN, wenn größer  oder LED10_ThresholdObjFFh
#define EE_UNIONPARAMETER_4B4E     0x4B4E //!< Addr: 0x4B4E, Size: 0x0008, LED11_Function oder LED reagiert auf:  oder LED11_Function
#define EE_UNIONPARAMETER_4B5C     0x4B5C //!< Addr: 0x4B5C, Size: 0x0008,     EIN, wenn größer  oder LED11_ThresholdObjFFh
#define EE_UNIONPARAMETER_4B61     0x4B61 //!< Addr: 0x4B61, Size: 0x0008, LED12_Function oder LED reagiert auf:  oder LED12_Function
#define EE_UNIONPARAMETER_4B6F     0x4B6F //!< Addr: 0x4B6F, Size: 0x0008,     EIN, wenn größer  oder LED12_ThresholdObjFFh
#define EE_UNIONPARAMETER_4B74     0x4B74 //!< Addr: 0x4B74, Size: 0x0008, LED13_Function oder LED reagiert auf:  oder LED13_Function
#define EE_UNIONPARAMETER_4B82     0x4B82 //!< Addr: 0x4B82, Size: 0x0008,     EIN, wenn größer  oder LED13_ThresholdObjFFh
#define EE_UNIONPARAMETER_4B87     0x4B87 //!< Addr: 0x4B87, Size: 0x0008, LED14_Function oder LED reagiert auf:  oder LED14_Function
#define EE_UNIONPARAMETER_4B95     0x4B95 //!< Addr: 0x4B95, Size: 0x0008,     EIN, wenn größer  oder LED14_ThresholdObjFFh
#define EE_UNIONPARAMETER_4B9B     0x4B9B //!< Addr: 0x4B9B, Size: 0x0008,     Sendebedingung  oder LC_logicParameter_sendCondition_0
#define EE_PARAMETER_4B9C_AUSGANG_INVERTIERT     0x4B9C //!< Addr: 0x4B9C, Size: 0x0008,     Ausgang invertiert 
#define EE_PARAMETER_4B9D_TASTE_1     0x4B9D //!< Addr: 0x4B9D, Size: 0x0008,     Taste 1 
#define EE_PARAMETER_4B9E_TASTE_2     0x4B9E //!< Addr: 0x4B9E, Size: 0x0008,     Taste 2 
#define EE_PARAMETER_4B9F_TASTE_3     0x4B9F //!< Addr: 0x4B9F, Size: 0x0008,     Taste 3 
#define EE_PARAMETER_4BA0_TASTE_4     0x4BA0 //!< Addr: 0x4BA0, Size: 0x0008,     Taste 4 
#define EE_PARAMETER_4BA9_LOGIKOBJEKT_1_A_EXTERN     0x4BA9 //!< Addr: 0x4BA9, Size: 0x0008, Logikobjekt 1 A (extern) 
#define EE_PARAMETER_4BAA_LOGIKOBJEKT_1_B_EXTERN     0x4BAA //!< Addr: 0x4BAA, Size: 0x0008, Logikobjekt 1 B (extern) 
#define EE_PARAMETER_4BB2_LC_LOGICPARAMETER_LOGICOPERATION_1     0x4BB2 //!< Addr: 0x4BB2, Size: 0x0008, LC_logicParameter_logicOperation_1
#define EE_UNIONPARAMETER_4BB3     0x4BB3 //!< Addr: 0x4BB3, Size: 0x0008,     Sendebedingung  oder LC_logicParameter_sendCondition_1
#define EE_PARAMETER_4BB4_AUSGANG_INVERTIERT     0x4BB4 //!< Addr: 0x4BB4, Size: 0x0008,     Ausgang invertiert 
#define EE_PARAMETER_4BB5_TASTE_1     0x4BB5 //!< Addr: 0x4BB5, Size: 0x0008,     Taste 1 
#define EE_PARAMETER_4BB6_TASTE_2     0x4BB6 //!< Addr: 0x4BB6, Size: 0x0008,     Taste 2 
#define EE_PARAMETER_4BB7_TASTE_3     0x4BB7 //!< Addr: 0x4BB7, Size: 0x0008,     Taste 3 
#define EE_PARAMETER_4BB8_TASTE_4     0x4BB8 //!< Addr: 0x4BB8, Size: 0x0008,     Taste 4 
#define EE_PARAMETER_4BC3_LOGIKOBJEKT_2_A_EXTERN     0x4BC3 //!< Addr: 0x4BC3, Size: 0x0008, Logikobjekt 2 A (extern) 
#define EE_PARAMETER_4BC4_LOGIKOBJEKT_2_B_EXTERN     0x4BC4 //!< Addr: 0x4BC4, Size: 0x0008, Logikobjekt 2 B (extern) 
#define EE_PARAMETER_4BCA_LC_LOGICPARAMETER_LOGICOPERATION_2     0x4BCA //!< Addr: 0x4BCA, Size: 0x0008, LC_logicParameter_logicOperation_2
#define EE_UNIONPARAMETER_4BCB     0x4BCB //!< Addr: 0x4BCB, Size: 0x0008,     Sendebedingung  oder LC_logicParameter_sendCondition_2
#define EE_PARAMETER_4BCC_AUSGANG_INVERTIERT     0x4BCC //!< Addr: 0x4BCC, Size: 0x0008,     Ausgang invertiert 
#define EE_PARAMETER_4BCD_TASTE_1     0x4BCD //!< Addr: 0x4BCD, Size: 0x0008,     Taste 1 
#define EE_PARAMETER_4BCE_TASTE_2     0x4BCE //!< Addr: 0x4BCE, Size: 0x0008,     Taste 2 
#define EE_PARAMETER_4BCF_TASTE_3     0x4BCF //!< Addr: 0x4BCF, Size: 0x0008,     Taste 3 
#define EE_PARAMETER_4BD0_TASTE_4     0x4BD0 //!< Addr: 0x4BD0, Size: 0x0008,     Taste 4 
#define EE_PARAMETER_4BDD_LOGIKOBJEKT_3_A_EXTERN     0x4BDD //!< Addr: 0x4BDD, Size: 0x0008, Logikobjekt 3 A (extern) 
#define EE_PARAMETER_4BDE_LOGIKOBJEKT_3_B_EXTERN     0x4BDE //!< Addr: 0x4BDE, Size: 0x0008, Logikobjekt 3 B (extern) 
#define EE_PARAMETER_4BE2_LC_LOGICPARAMETER_LOGICOPERATION_3     0x4BE2 //!< Addr: 0x4BE2, Size: 0x0008, LC_logicParameter_logicOperation_3
#define EE_UNIONPARAMETER_4BE3     0x4BE3 //!< Addr: 0x4BE3, Size: 0x0008,     Sendebedingung  oder LC_logicParameter_sendCondition_3
#define EE_PARAMETER_4BE4_AUSGANG_INVERTIERT     0x4BE4 //!< Addr: 0x4BE4, Size: 0x0008,     Ausgang invertiert 
#define EE_PARAMETER_4BE5_TASTE_1     0x4BE5 //!< Addr: 0x4BE5, Size: 0x0008,     Taste 1 
#define EE_PARAMETER_4BE6_TASTE_2     0x4BE6 //!< Addr: 0x4BE6, Size: 0x0008,     Taste 2 
#define EE_PARAMETER_4BE7_TASTE_3     0x4BE7 //!< Addr: 0x4BE7, Size: 0x0008,     Taste 3 
#define EE_PARAMETER_4BE8_TASTE_4     0x4BE8 //!< Addr: 0x4BE8, Size: 0x0008,     Taste 4 
#define EE_PARAMETER_4BF7_LOGIKOBJEKT_4_A_EXTERN     0x4BF7 //!< Addr: 0x4BF7, Size: 0x0008, Logikobjekt 4 A (extern) 
#define EE_PARAMETER_4BF8_LOGIKOBJEKT_4_B_EXTERN     0x4BF8 //!< Addr: 0x4BF8, Size: 0x0008, Logikobjekt 4 B (extern) 
#define EE_PARAMETER_4628_ANZAHL_PRO_10_SEKUNDEN     0x4628 //!< Addr: 0x4628, Size: 0x0008, Anzahl pro 10 Sekunden 
#define EE_PARAMETER_4BFA_LC_VALUETOSEND_0_CH0     0x4BFA //!< Addr: 0x4BFA, Size: 0x0008, LC_ValueToSend_0_ch0
#define EE_UNIONPARAMETER_4BFB     0x4BFB //!< Addr: 0x4BFB, Size: 0x0008,     Szene Nummer  oder LC_ValueToSend_1_ch0 oder     1Byte Wert  oder     Zwangsführung 
#define EE_PARAMETER_4BFC_LC_VALUETOSEND_0_CH1     0x4BFC //!< Addr: 0x4BFC, Size: 0x0008, LC_ValueToSend_0_ch1
#define EE_UNIONPARAMETER_4BFD     0x4BFD //!< Addr: 0x4BFD, Size: 0x0008,     Szene Nummer  oder LC_ValueToSend_1_ch1 oder     1Byte Wert  oder     Zwangsführung 
#define EE_PARAMETER_4BFE_LC_VALUETOSEND_0_CH2     0x4BFE //!< Addr: 0x4BFE, Size: 0x0008, LC_ValueToSend_0_ch2
#define EE_UNIONPARAMETER_4BFF     0x4BFF //!< Addr: 0x4BFF, Size: 0x0008,     Szene Nummer  oder LC_ValueToSend_1_ch2 oder     1Byte Wert  oder     Szene Nummer 
#define EE_PARAMETER_4C00_LC_VALUETOSEND_0_CH3     0x4C00 //!< Addr: 0x4C00, Size: 0x0008, LC_ValueToSend_0_ch3
#define EE_UNIONPARAMETER_4C01     0x4C01 //!< Addr: 0x4C01, Size: 0x0008,     Szene Nummer  oder LC_ValueToSend_1_ch3 oder     1Byte Wert  oder     Zwangsführung 
#define EE_PARAMETER_4C07_VERHALTEN_DER_LEDS_BEI_BUSSPANNUNGSWIEDERKEHR     0x4C07 //!< Addr: 0x4C07, Size: 0x0001, Verhalten der LED's  bei Busspannungswiederkehr 
#define EE_PARAMETER_4C08_WERTE_FUER_UMSCHALTUNG     0x4C08 //!< Addr: 0x4C08, Size: 0x0001,     Werte für Umschaltung 
#define EE_PARAMETER_4620_GERAETEANLAUFZEIT_S     0x4620 //!< Addr: 0x4620, Size: 0x0010, Geräteanlaufzeit s 
#define EE_PARAMETER_4AA4_AUSWAHL_DER_OBJEKTNUMMER     0x4AA4 //!< Addr: 0x4AA4, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4AA5_BEI_TAG_WERT_EIN     0x4AA5 //!< Addr: 0x4AA5, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4ACC_BEI_NACHT_WERT_EIN     0x4ACC //!< Addr: 0x4ACC, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4AB9_BEI_NACHT_WERT_EIN     0x4AB9 //!< Addr: 0x4AB9, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4AA6_BEI_NACHT_WERT_EIN     0x4AA6 //!< Addr: 0x4AA6, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4ACD_BEI_TAG_WERT_AUS     0x4ACD //!< Addr: 0x4ACD, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4ABA_BEI_TAG_WERT_AUS     0x4ABA //!< Addr: 0x4ABA, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4AA7_BEI_TAG_WERT_AUS     0x4AA7 //!< Addr: 0x4AA7, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4ACE_BEI_NACHT_WERT_AUS     0x4ACE //!< Addr: 0x4ACE, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4ABB_BEI_NACHT_WERT_AUS     0x4ABB //!< Addr: 0x4ABB, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4AA8_BEI_NACHT_WERT_AUS     0x4AA8 //!< Addr: 0x4AA8, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4AA9_VERHALTEN_BEI_TAG_WERT_EIN     0x4AA9 //!< Addr: 0x4AA9, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4ABC_VERHALTEN_BEI_TAG_WERT_EIN     0x4ABC //!< Addr: 0x4ABC, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4ACF_VERHALTEN_BEI_TAG_WERT_EIN     0x4ACF //!< Addr: 0x4ACF, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4AD0_VERHALTEN_BEI_NACHT_WERT_EIN     0x4AD0 //!< Addr: 0x4AD0, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4ABD_VERHALTEN_BEI_NACHT_WERT_EIN     0x4ABD //!< Addr: 0x4ABD, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4AAA_VERHALTEN_BEI_NACHT_WERT_EIN     0x4AAA //!< Addr: 0x4AAA, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4AD3_BEI_NACHT     0x4AD3 //!< Addr: 0x4AD3, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4AC0_BEI_NACHT     0x4AC0 //!< Addr: 0x4AC0, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4AAD_BEI_NACHT     0x4AAD //!< Addr: 0x4AAD, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4AAE_VERHALTEN_BEI_TAG_WERT_EIN     0x4AAE //!< Addr: 0x4AAE, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4AC1_VERHALTEN_BEI_TAG_WERT_EIN     0x4AC1 //!< Addr: 0x4AC1, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4AD4_VERHALTEN_BEI_TAG_WERT_EIN     0x4AD4 //!< Addr: 0x4AD4, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4AD5_VERHALTEN_BEI_NACHT_WERT_EIN     0x4AD5 //!< Addr: 0x4AD5, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4AC2_VERHALTEN_BEI_NACHT_WERT_EIN     0x4AC2 //!< Addr: 0x4AC2, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4AAF_VERHALTEN_BEI_NACHT_WERT_EIN     0x4AAF //!< Addr: 0x4AAF, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4C03_SPERROBJEKT_FUER_LEDS     0x4C03 //!< Addr: 0x4C03, Size: 0x0008, Sperrobjekt für LEDs 
#define EE_PARAMETER_4C04_WERT_FUER_TAG_NACHT     0x4C04 //!< Addr: 0x4C04, Size: 0x0008, Wert für Tag / Nacht 
#define EE_PARAMETER_4C05_PUTZFUNKTION     0x4C05 //!< Addr: 0x4C05, Size: 0x0008, Putzfunktion 
#define EE_PARAMETER_4AB7_AUSWAHL_DER_OBJEKTNUMMER     0x4AB7 //!< Addr: 0x4AB7, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4AB8_BEI_TAG_WERT_EIN     0x4AB8 //!< Addr: 0x4AB8, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4ACA_AUSWAHL_DER_OBJEKTNUMMER     0x4ACA //!< Addr: 0x4ACA, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4ACB_BEI_TAG_WERT_EIN     0x4ACB //!< Addr: 0x4ACB, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4AD1_OBJEKT_FUER_PRIORITAET     0x4AD1 //!< Addr: 0x4AD1, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4AD2_BEI_TAG     0x4AD2 //!< Addr: 0x4AD2, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4ADE_BEI_TAG_WERT_EIN     0x4ADE //!< Addr: 0x4ADE, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4AE0_BEI_TAG_WERT_AUS     0x4AE0 //!< Addr: 0x4AE0, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4ADD_AUSWAHL_DER_OBJEKTNUMMER     0x4ADD //!< Addr: 0x4ADD, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4AE2_VERHALTEN_BEI_TAG_WERT_EIN     0x4AE2 //!< Addr: 0x4AE2, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4AE4_OBJEKT_FUER_PRIORITAET     0x4AE4 //!< Addr: 0x4AE4, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4AE5_BEI_TAG     0x4AE5 //!< Addr: 0x4AE5, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4AE7_VERHALTEN_BEI_TAG_WERT_EIN     0x4AE7 //!< Addr: 0x4AE7, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4ADF_BEI_NACHT_WERT_EIN     0x4ADF //!< Addr: 0x4ADF, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4AE1_BEI_NACHT_WERT_AUS     0x4AE1 //!< Addr: 0x4AE1, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4AE3_VERHALTEN_BEI_NACHT_WERT_EIN     0x4AE3 //!< Addr: 0x4AE3, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4AE6_BEI_NACHT     0x4AE6 //!< Addr: 0x4AE6, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4AE8_VERHALTEN_BEI_NACHT_WERT_EIN     0x4AE8 //!< Addr: 0x4AE8, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4AF1_BEI_TAG_WERT_EIN     0x4AF1 //!< Addr: 0x4AF1, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4AF3_BEI_TAG_WERT_AUS     0x4AF3 //!< Addr: 0x4AF3, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4AF0_AUSWAHL_DER_OBJEKTNUMMER     0x4AF0 //!< Addr: 0x4AF0, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4AF5_VERHALTEN_BEI_TAG_WERT_EIN     0x4AF5 //!< Addr: 0x4AF5, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4AF7_OBJEKT_FUER_PRIORITAET     0x4AF7 //!< Addr: 0x4AF7, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4AF8_BEI_TAG     0x4AF8 //!< Addr: 0x4AF8, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4AFA_VERHALTEN_BEI_TAG_WERT_EIN     0x4AFA //!< Addr: 0x4AFA, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4AF2_BEI_NACHT_WERT_EIN     0x4AF2 //!< Addr: 0x4AF2, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4AF4_BEI_NACHT_WERT_AUS     0x4AF4 //!< Addr: 0x4AF4, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4AF6_VERHALTEN_BEI_NACHT_WERT_EIN     0x4AF6 //!< Addr: 0x4AF6, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4AF9_BEI_NACHT     0x4AF9 //!< Addr: 0x4AF9, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4AFB_VERHALTEN_BEI_NACHT_WERT_EIN     0x4AFB //!< Addr: 0x4AFB, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4BA1_TASTE_5     0x4BA1 //!< Addr: 0x4BA1, Size: 0x0008,     Taste 5 
#define EE_PARAMETER_4BA2_TASTE_6     0x4BA2 //!< Addr: 0x4BA2, Size: 0x0008,     Taste 6 
#define EE_PARAMETER_4BA3_TASTE_7     0x4BA3 //!< Addr: 0x4BA3, Size: 0x0008,     Taste 7 
#define EE_PARAMETER_4BA4_TASTE_8     0x4BA4 //!< Addr: 0x4BA4, Size: 0x0008,     Taste 8 
#define EE_PARAMETER_4BE9_TASTE_5     0x4BE9 //!< Addr: 0x4BE9, Size: 0x0008,     Taste 5 
#define EE_PARAMETER_4BD1_TASTE_5     0x4BD1 //!< Addr: 0x4BD1, Size: 0x0008,     Taste 5 
#define EE_PARAMETER_4BB9_TASTE_5     0x4BB9 //!< Addr: 0x4BB9, Size: 0x0008,     Taste 5 
#define EE_PARAMETER_4BEA_TASTE_6     0x4BEA //!< Addr: 0x4BEA, Size: 0x0008,     Taste 6 
#define EE_PARAMETER_4BD2_TASTE_6     0x4BD2 //!< Addr: 0x4BD2, Size: 0x0008,     Taste 6 
#define EE_PARAMETER_4BBA_TASTE_6     0x4BBA //!< Addr: 0x4BBA, Size: 0x0008,     Taste 6 
#define EE_PARAMETER_4BEB_TASTE_7     0x4BEB //!< Addr: 0x4BEB, Size: 0x0008,     Taste 7 
#define EE_PARAMETER_4BD3_TASTE_7     0x4BD3 //!< Addr: 0x4BD3, Size: 0x0008,     Taste 7 
#define EE_PARAMETER_4BBB_TASTE_7     0x4BBB //!< Addr: 0x4BBB, Size: 0x0008,     Taste 7 
#define EE_PARAMETER_4BEC_TASTE_8     0x4BEC //!< Addr: 0x4BEC, Size: 0x0008,     Taste 8 
#define EE_PARAMETER_4BD4_TASTE_8     0x4BD4 //!< Addr: 0x4BD4, Size: 0x0008,     Taste 8 
#define EE_PARAMETER_4BBC_TASTE_8     0x4BBC //!< Addr: 0x4BBC, Size: 0x0008,     Taste 8 
#define EE_PARAMETER_4C30_TAG_NACHT_OBJEKT     0x4C30 //!< Addr: 0x4C30, Size: 0x0001,     Tag/Nacht-Objekt 
#define EE_PARAMETER_4C31_DATUM_UHRZEIT     0x4C31 //!< Addr: 0x4C31, Size: 0x0001,     Datum/Uhrzeit 
#define EE_PARAMETER_4C2B_SPRACHE     0x4C2B //!< Addr: 0x4C2B, Size: 0x0008, Sprache 
#define EE_PARAMETER_4C2D_SEITENSYMBOL     0x4C2D //!< Addr: 0x4C2D, Size: 0x0008,     Seitensymbol 
#define EE_PARAMETER_4C2E_SEITENSYMBOL     0x4C2E //!< Addr: 0x4C2E, Size: 0x0008,     Seitensymbol 
#define EE_PARAMETER_4C2F_SEITENSYMBOL     0x4C2F //!< Addr: 0x4C2F, Size: 0x0008,     Seitensymbol 
#define EE_PARAMETER_4C30_SYMBOLFARBE     0x4C30 //!< Addr: 0x4C30, Size: 0x0008,     Symbolfarbe 
#define EE_PARAMETER_4C31_SYMBOLFARBE     0x4C31 //!< Addr: 0x4C31, Size: 0x0008,     Symbolfarbe 
#define EE_PARAMETER_4C32_SYMBOLFARBE     0x4C32 //!< Addr: 0x4C32, Size: 0x0008,     Symbolfarbe 
#define EE_PARAMETER_4C33_SEITENBESCHRIFTUNG     0x4C33 //!< Addr: 0x4C33, Size: 0x0078,     Seitenbeschriftung 
#define EE_PARAMETER_4C42_SEITENBESCHRIFTUNG     0x4C42 //!< Addr: 0x4C42, Size: 0x0078,     Seitenbeschriftung 
#define EE_PARAMETER_4C51_SEITENBESCHRIFTUNG     0x4C51 //!< Addr: 0x4C51, Size: 0x0078,     Seitenbeschriftung 
#define EE_PARAMETER_4CB1_HINTERGRUNDFARBE     0x4CB1 //!< Addr: 0x4CB1, Size: 0x0008,     Hintergrundfarbe 
#define EE_PARAMETER_4CB2_SCHRIFTGROESSE_FUER_FUNKTIONSNAMEN     0x4CB2 //!< Addr: 0x4CB2, Size: 0x0008,     Schriftgröße für Funktionsnamen 
#define EE_PARAMETER_4CB3_SCHRIFTGROESSE_FUER_TASTENBESCHRIFTUNG     0x4CB3 //!< Addr: 0x4CB3, Size: 0x0008,     Schriftgröße für Tastenbeschriftung 
#define EE_PARAMETER_4CB4_VERHALTEN_WENN_TEXT_ZU_LANG     0x4CB4 //!< Addr: 0x4CB4, Size: 0x0008,     Verhalten wenn Text zu lang 
#define EE_UNIONPARAMETER_4C0D     0x4C0D //!< Addr: 0x4C0D, Size: 0x0008, Standbyanzeige wechseln nach s  oder DisplayTimeSwitchOfFF
#define EE_PARAMETER_4C0E_ZEIT_BIS_ZUM_STANDBY_S     0x4C0E //!< Addr: 0x4C0E, Size: 0x0008, Zeit bis zum Standby s 
#define EE_PARAMETER_4C0F_STANDBYANZEIGE_BEI_TAG     0x4C0F //!< Addr: 0x4C0F, Size: 0x0008, Standbyanzeige bei Tag 
#define EE_PARAMETER_4C10_ZEILE_1     0x4C10 //!< Addr: 0x4C10, Size: 0x0008,     Zeile 1 
#define EE_PARAMETER_4C11_ZEILE_2     0x4C11 //!< Addr: 0x4C11, Size: 0x0008,     Zeile 2 
#define EE_UNIONPARAMETER_4C12     0x4C12 //!< Addr: 0x4C12, Size: 0x0008,         Statuselement  oder         Statuselement links  oder         Statuselement oben 
#define EE_UNIONPARAMETER_4C13     0x4C13 //!< Addr: 0x4C13, Size: 0x0008,         Statuselement rechts  oder         Statuselement unten  oder     Statuselement 2 
#define EE_UNIONPARAMETER_4C14     0x4C14 //!< Addr: 0x4C14, Size: 0x0008,         Statuselement  oder         Statuselement links  oder         Statuselement oben  oder     Statuselement 3 
#define EE_UNIONPARAMETER_4C15     0x4C15 //!< Addr: 0x4C15, Size: 0x0008,         Statuselement rechts  oder         Statuselement unten  oder     Statuselement 4 
#define EE_PARAMETER_4C32_2_EBENE_8_FUNKTIONEN     0x4C32 //!< Addr: 0x4C32, Size: 0x0001, 2. Ebene / 8 Funktionen 
#define EE_PARAMETER_4C31_3_EBENE_12_FUNKTIONEN     0x4C31 //!< Addr: 0x4C31, Size: 0x0001, 3. Ebene / 12 Funktionen 
#define EE_PARAMETER_4C19_STANDBYANZEIGE_BEI_NACHT     0x4C19 //!< Addr: 0x4C19, Size: 0x0008, Standbyanzeige bei Nacht 
#define EE_PARAMETER_4C1A_ZEILE_1     0x4C1A //!< Addr: 0x4C1A, Size: 0x0008,     Zeile 1 
#define EE_UNIONPARAMETER_4C16     0x4C16 //!< Addr: 0x4C16, Size: 0x0008,     Angezeigte Ebene im Standby  oder     Angezeigte Ebene im Standby 
#define EE_UNIONPARAMETER_4C1C     0x4C1C //!< Addr: 0x4C1C, Size: 0x0008,         Statuselement  oder         Statuselement links  oder         Statuselement oben 
#define EE_UNIONPARAMETER_4C1D     0x4C1D //!< Addr: 0x4C1D, Size: 0x0008,         Statuselement rechts  oder         Statuselement unten  oder     Statuselement 2 
#define EE_PARAMETER_4C1B_ZEILE_2     0x4C1B //!< Addr: 0x4C1B, Size: 0x0008,     Zeile 2 
#define EE_UNIONPARAMETER_4C1E     0x4C1E //!< Addr: 0x4C1E, Size: 0x0008,         Statuselement  oder         Statuselement links  oder         Statuselement oben  oder     Statuselement 3 
#define EE_UNIONPARAMETER_4C1F     0x4C1F //!< Addr: 0x4C1F, Size: 0x0008,         Statuselement rechts  oder         Statuselement unten  oder     Statuselement 4 
#define EE_PARAMETER_4C23_NACHTABSCHALTUNG_IM_STANDBY     0x4C23 //!< Addr: 0x4C23, Size: 0x0008, Nachtabschaltung im Standby 
#define EE_PARAMETER_4C24_HELLIGKEITSENSORMAX     0x4C24 //!< Addr: 0x4C24, Size: 0x0008, HelligkeitSensorMax
#define EE_PARAMETER_4C25_MINIMALE_HELLIGKEIT_BEI_NACHT     0x4C25 //!< Addr: 0x4C25, Size: 0x0008, Minimale Helligkeit bei Nacht 
#define EE_PARAMETER_4C26_HELLIGKEITNACHTMAX     0x4C26 //!< Addr: 0x4C26, Size: 0x0008, HelligkeitNachtMax
#define EE_PARAMETER_4C27_MINIMALE_HELLIGKEIT_BEI_TAG     0x4C27 //!< Addr: 0x4C27, Size: 0x0008, Minimale Helligkeit bei Tag 
#define EE_PARAMETER_4C28_HELLIGKEITTAGMAX     0x4C28 //!< Addr: 0x4C28, Size: 0x0008, HelligkeitTagMax
#define EE_UNIONPARAMETER_4C20     0x4C20 //!< Addr: 0x4C20, Size: 0x0008,     Angezeigte Ebene im Standby  oder     Angezeigte Ebene im Standby 
#define EE_UNIONPARAMETER_4C33     0x4C33 //!< Addr: 0x4C33, Size: 0x0001, Anzeigemodus  oder Anzeigemodus 
#define EE_UNIONPARAMETER_4C60     0x4C60 //!< Addr: 0x4C60, Size: 0x0010,     Rotanteil  oder     Grünanteil  oder     Blauanteil 
#define EE_UNIONPARAMETER_4C62     0x4C62 //!< Addr: 0x4C62, Size: 0x0010,     Rotanteil  oder     Grünanteil  oder     Blauanteil 
#define EE_UNIONPARAMETER_4C64     0x4C64 //!< Addr: 0x4C64, Size: 0x0010,     Rotanteil  oder     Grünanteil  oder     Blauanteil 
#define EE_UNIONPARAMETER_4C6C     0x4C6C //!< Addr: 0x4C6C, Size: 0x0088,     Beschreibung für Messwert  oder StateValueStringOnOff_0 oder     Text für Wert = 0  oder     Text für Wert = 1 
#define EE_PARAMETER_4CB5_PRIORITAET_VON_HVAC_STATUS     0x4CB5 //!< Addr: 0x4CB5, Size: 0x0008, Priorität von HVAC-Status 
#define EE_PARAMETER_4C66_STATEVALUEDPT_0     0x4C66 //!< Addr: 0x4C66, Size: 0x0008, StateValueDPT_0
#define EE_PARAMETER_4C67_TEXT_FUER_DIE_EINHEIT     0x4C67 //!< Addr: 0x4C67, Size: 0x0028,     Text für die Einheit 
#define EE_PARAMETER_4C7F_STATEVALUEDPT_1     0x4C7F //!< Addr: 0x4C7F, Size: 0x0008, StateValueDPT_1
#define EE_PARAMETER_4C80_TEXT_FUER_DIE_EINHEIT     0x4C80 //!< Addr: 0x4C80, Size: 0x0028,     Text für die Einheit 
#define EE_UNIONPARAMETER_4C85     0x4C85 //!< Addr: 0x4C85, Size: 0x0088, StateValueStringOnOff_1 oder     Text für Wert = 0  oder     Text für Wert = 1  oder     Beschreibung für Messwert 
#define EE_PARAMETER_4C98_STATEVALUEDPT_2     0x4C98 //!< Addr: 0x4C98, Size: 0x0008, StateValueDPT_2
#define EE_PARAMETER_4C99_TEXT_FUER_DIE_EINHEIT     0x4C99 //!< Addr: 0x4C99, Size: 0x0028,     Text für die Einheit 
#define EE_UNIONPARAMETER_4C9E     0x4C9E //!< Addr: 0x4C9E, Size: 0x0088, StateValueStringOnOff_2 oder     Text für Wert = 0  oder     Text für Wert = 1  oder     Beschreibung für Messwert 
#define EE_PARAMETER_4CB6_TEXT     0x4CB6 //!< Addr: 0x4CB6, Size: 0x0078,     Text 
#define EE_PARAMETER_4CF2_ANZEIGEDAUER     0x4CF2 //!< Addr: 0x4CF2, Size: 0x0010,     Anzeigedauer 
#define EE_PARAMETER_4CC5_TEXT     0x4CC5 //!< Addr: 0x4CC5, Size: 0x0078,     Text 
#define EE_PARAMETER_4CF4_ANZEIGEDAUER     0x4CF4 //!< Addr: 0x4CF4, Size: 0x0010,     Anzeigedauer 
#define EE_PARAMETER_4CD4_TEXT     0x4CD4 //!< Addr: 0x4CD4, Size: 0x0078,     Text 
#define EE_PARAMETER_4CF6_ANZEIGEDAUER     0x4CF6 //!< Addr: 0x4CF6, Size: 0x0010,     Anzeigedauer 
#define EE_PARAMETER_4CE3_TEXT     0x4CE3 //!< Addr: 0x4CE3, Size: 0x0078,     Text 
#define EE_PARAMETER_4CF8_ANZEIGEDAUER     0x4CF8 //!< Addr: 0x4CF8, Size: 0x0010,     Anzeigedauer 
#define EE_PARAMETER_4CFA_ANZEIGEDAUER     0x4CFA //!< Addr: 0x4CFA, Size: 0x0010,     Anzeigedauer 
#define EE_PARAMETER_466D_TASTENBESCHRIFTUNG_LINKE_TASTE     0x466D //!< Addr: 0x466D, Size: 0x0030, Tastenbeschriftung linke Taste 
#define EE_PARAMETER_46C3_TASTENBESCHRIFTUNG_RECHTE_TASTE     0x46C3 //!< Addr: 0x46C3, Size: 0x0030, Tastenbeschriftung rechte Taste 
#define EE_PARAMETER_4674_SYMBOL_FUER_0PROZENT_90PROZENT     0x4674 //!< Addr: 0x4674, Size: 0x0008,     Symbol für 0% - 90% 
#define EE_PARAMETER_4675_SYMBOL_FUER_90PROZENT     0x4675 //!< Addr: 0x4675, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_4676_SYMBOL_FUER_90PROZENT     0x4676 //!< Addr: 0x4676, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_4677_FARBE_DES_SYMBOLS_FUER_0PROZENT     0x4677 //!< Addr: 0x4677, Size: 0x0008,     Farbe des Symbols für 0% 
#define EE_PARAMETER_4678_FARBE_DES_SYMBOLS_FUER_0PROZENT_90PROZENT     0x4678 //!< Addr: 0x4678, Size: 0x0008,     Farbe des Symbols für 0% - 90% 
#define EE_PARAMETER_4679_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x4679 //!< Addr: 0x4679, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_467A_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x467A //!< Addr: 0x467A, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_46CD_FARBE_DES_SYMBOLS_LAMELLE_OFFEN_55PROZENT     0x46CD //!< Addr: 0x46CD, Size: 0x0008, Farbe des Symbols Lamelle offen (<55%) 
#define EE_PARAMETER_46CA_SYMBOL_LAMELLE_MITTIG_55PROZENT_90PROZENT     0x46CA //!< Addr: 0x46CA, Size: 0x0008, Symbol Lamelle mittig (55% - 90%) 
#define EE_PARAMETER_46CE_FARBE_DES_SYMBOLS_LAMELLE_MITTIG_55PROZENT_90PROZENT     0x46CE //!< Addr: 0x46CE, Size: 0x0008, Farbe des Symbols Lamelle mittig (55% - 90%) 
#define EE_PARAMETER_46CB_SYMBOL_LAMELLE_GESCHLOSSEN_90PROZENT     0x46CB //!< Addr: 0x46CB, Size: 0x0008, Symbol Lamelle geschlossen (>90%) 
#define EE_PARAMETER_46CF_FARBE_DES_SYMBOLS_LAMELLE_GESCHLOSSEN_90PROZENT     0x46CF //!< Addr: 0x46CF, Size: 0x0008, Farbe des Symbols Lamelle geschlossen (>90%) 
#define EE_PARAMETER_4657_UMSCHALTART     0x4657 //!< Addr: 0x4657, Size: 0x0001, Umschaltart 
#define EE_PARAMETER_465E_UMSCHALTUNG_BERUECKSICHTIGT_STATUSOBJEKT     0x465E //!< Addr: 0x465E, Size: 0x0001, Umschaltung berücksichtigt Statusobjekt 
#define EE_PARAMETER_4659_FUNCTION_VALUETOICON_0     0x4659 //!< Addr: 0x4659, Size: 0x0001, Function_ValueToIcon_0
#define EE_PARAMETER_462C_ANZAHL_DER_WERTE     0x462C //!< Addr: 0x462C, Size: 0x0008, Anzahl der Werte 
#define EE_PARAMETER_462D_OM_INPUTUSAGE_SUBTYPE_0     0x462D //!< Addr: 0x462D, Size: 0x0008, OM_InputUsage_subType_0
#define EE_PARAMETER_4683_OM_INPUTUSAGE_SUBTYPE_1     0x4683 //!< Addr: 0x4683, Size: 0x0008, OM_InputUsage_subType_1
#define EE_PARAMETER_4658_DARSTELLUNG_DER_FUNKTION_KURZ_LANG     0x4658 //!< Addr: 0x4658, Size: 0x0001, Darstellung der Funktion kurz/lang 
#define EE_PARAMETER_46CC_SYMBOL_FUER_90PROZENT     0x46CC //!< Addr: 0x46CC, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_46D0_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x46D0 //!< Addr: 0x46D0, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_4682_ANZAHL_DER_WERTE     0x4682 //!< Addr: 0x4682, Size: 0x0008, Anzahl der Werte 
#define EE_PARAMETER_46AD_OM_INPUTUSAGE_VALUESWITCHMODE_1     0x46AD //!< Addr: 0x46AD, Size: 0x0001, OM_InputUsage_valueSwitchMode_1
#define EE_PARAMETER_46B4_UMSCHALTUNG_BERUECKSICHTIGT_STATUSOBJEKT     0x46B4 //!< Addr: 0x46B4, Size: 0x0001, Umschaltung berücksichtigt Statusobjekt 
#define EE_PARAMETER_46AF_FUNCTION_VALUETOICON_1     0x46AF //!< Addr: 0x46AF, Size: 0x0001, Function_ValueToIcon_1
#define EE_PARAMETER_46AE_DARSTELLUNG_DER_FUNKTION_KURZ_LANG     0x46AE //!< Addr: 0x46AE, Size: 0x0001, Darstellung der Funktion kurz/lang 
#define EE_PARAMETER_4A35_OM_INPUTUSAGE_SUBTYPE_PANIC     0x4A35 //!< Addr: 0x4A35, Size: 0x0008, OM_InputUsage_subType_Panic
#define EE_PARAMETER_4A46_OM_INPUTUSAGE_VALUETYPE_00_PANIC     0x4A46 //!< Addr: 0x4A46, Size: 0x0008, OM_InputUsage_valueType_00_Panic
#define EE_PARAMETER_4A9D_LED1_READREQUEST     0x4A9D //!< Addr: 0x4A9D, Size: 0x0008, LED1_ReadRequest
#define EE_PARAMETER_4AB0_LED2_READREQUEST     0x4AB0 //!< Addr: 0x4AB0, Size: 0x0008, LED2_ReadRequest
#define EE_PARAMETER_4AC3_LED3_READREQUEST     0x4AC3 //!< Addr: 0x4AC3, Size: 0x0008, LED3_ReadRequest
#define EE_PARAMETER_4AD6_LED4_READREQUEST     0x4AD6 //!< Addr: 0x4AD6, Size: 0x0008, LED4_ReadRequest
#define EE_PARAMETER_4AE9_LED5_READREQUEST     0x4AE9 //!< Addr: 0x4AE9, Size: 0x0008, LED5_ReadRequest
#define EE_PARAMETER_4AFC_LED6_READREQUEST     0x4AFC //!< Addr: 0x4AFC, Size: 0x0008, LED6_ReadRequest
#define EE_PARAMETER_4B0F_LED7_READREQUEST     0x4B0F //!< Addr: 0x4B0F, Size: 0x0008, LED7_ReadRequest
#define EE_PARAMETER_4B03_AUSWAHL_DER_OBJEKTNUMMER     0x4B03 //!< Addr: 0x4B03, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4B04_BEI_TAG_WERT_EIN     0x4B04 //!< Addr: 0x4B04, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4B06_BEI_TAG_WERT_AUS     0x4B06 //!< Addr: 0x4B06, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4B08_VERHALTEN_BEI_TAG_WERT_EIN     0x4B08 //!< Addr: 0x4B08, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B05_BEI_NACHT_WERT_EIN     0x4B05 //!< Addr: 0x4B05, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B07_BEI_NACHT_WERT_AUS     0x4B07 //!< Addr: 0x4B07, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4B09_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B09 //!< Addr: 0x4B09, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B0A_OBJEKT_FUER_PRIORITAET     0x4B0A //!< Addr: 0x4B0A, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4B0B_BEI_TAG     0x4B0B //!< Addr: 0x4B0B, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4B0D_VERHALTEN_BEI_TAG_WERT_EIN     0x4B0D //!< Addr: 0x4B0D, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B0C_BEI_NACHT     0x4B0C //!< Addr: 0x4B0C, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4B0E_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B0E //!< Addr: 0x4B0E, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B22_LED8_READREQUEST     0x4B22 //!< Addr: 0x4B22, Size: 0x0008, LED8_ReadRequest
#define EE_PARAMETER_4B16_AUSWAHL_DER_OBJEKTNUMMER     0x4B16 //!< Addr: 0x4B16, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4B17_BEI_TAG_WERT_EIN     0x4B17 //!< Addr: 0x4B17, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4B19_BEI_TAG_WERT_AUS     0x4B19 //!< Addr: 0x4B19, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4B1B_VERHALTEN_BEI_TAG_WERT_EIN     0x4B1B //!< Addr: 0x4B1B, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B18_BEI_NACHT_WERT_EIN     0x4B18 //!< Addr: 0x4B18, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B1A_BEI_NACHT_WERT_AUS     0x4B1A //!< Addr: 0x4B1A, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4B1C_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B1C //!< Addr: 0x4B1C, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B1D_OBJEKT_FUER_PRIORITAET     0x4B1D //!< Addr: 0x4B1D, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4B1E_BEI_TAG     0x4B1E //!< Addr: 0x4B1E, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4B20_VERHALTEN_BEI_TAG_WERT_EIN     0x4B20 //!< Addr: 0x4B20, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B1F_BEI_NACHT     0x4B1F //!< Addr: 0x4B1F, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4B21_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B21 //!< Addr: 0x4B21, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B35_LED9_READREQUEST     0x4B35 //!< Addr: 0x4B35, Size: 0x0008, LED9_ReadRequest
#define EE_PARAMETER_4B29_AUSWAHL_DER_OBJEKTNUMMER     0x4B29 //!< Addr: 0x4B29, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4B2A_BEI_TAG_WERT_EIN     0x4B2A //!< Addr: 0x4B2A, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4B2C_BEI_TAG_WERT_AUS     0x4B2C //!< Addr: 0x4B2C, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4B2E_VERHALTEN_BEI_TAG_WERT_EIN     0x4B2E //!< Addr: 0x4B2E, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B2B_BEI_NACHT_WERT_EIN     0x4B2B //!< Addr: 0x4B2B, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B2D_BEI_NACHT_WERT_AUS     0x4B2D //!< Addr: 0x4B2D, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4B2F_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B2F //!< Addr: 0x4B2F, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B30_OBJEKT_FUER_PRIORITAET     0x4B30 //!< Addr: 0x4B30, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4B31_BEI_TAG     0x4B31 //!< Addr: 0x4B31, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4B33_VERHALTEN_BEI_TAG_WERT_EIN     0x4B33 //!< Addr: 0x4B33, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B32_BEI_NACHT     0x4B32 //!< Addr: 0x4B32, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4B34_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B34 //!< Addr: 0x4B34, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B48_LED10_READREQUEST     0x4B48 //!< Addr: 0x4B48, Size: 0x0008, LED10_ReadRequest
#define EE_PARAMETER_4B3C_AUSWAHL_DER_OBJEKTNUMMER     0x4B3C //!< Addr: 0x4B3C, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4B3D_BEI_TAG_WERT_EIN     0x4B3D //!< Addr: 0x4B3D, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4B3F_BEI_TAG_WERT_AUS     0x4B3F //!< Addr: 0x4B3F, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4B41_VERHALTEN_BEI_TAG_WERT_EIN     0x4B41 //!< Addr: 0x4B41, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B3E_BEI_NACHT_WERT_EIN     0x4B3E //!< Addr: 0x4B3E, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B40_BEI_NACHT_WERT_AUS     0x4B40 //!< Addr: 0x4B40, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4B42_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B42 //!< Addr: 0x4B42, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B43_OBJEKT_FUER_PRIORITAET     0x4B43 //!< Addr: 0x4B43, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4B44_BEI_TAG     0x4B44 //!< Addr: 0x4B44, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4B46_VERHALTEN_BEI_TAG_WERT_EIN     0x4B46 //!< Addr: 0x4B46, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B45_BEI_NACHT     0x4B45 //!< Addr: 0x4B45, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4B47_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B47 //!< Addr: 0x4B47, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B5B_LED11_READREQUEST     0x4B5B //!< Addr: 0x4B5B, Size: 0x0008, LED11_ReadRequest
#define EE_PARAMETER_4B4F_AUSWAHL_DER_OBJEKTNUMMER     0x4B4F //!< Addr: 0x4B4F, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4B50_BEI_TAG_WERT_EIN     0x4B50 //!< Addr: 0x4B50, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4B52_BEI_TAG_WERT_AUS     0x4B52 //!< Addr: 0x4B52, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4B54_VERHALTEN_BEI_TAG_WERT_EIN     0x4B54 //!< Addr: 0x4B54, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B51_BEI_NACHT_WERT_EIN     0x4B51 //!< Addr: 0x4B51, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B53_BEI_NACHT_WERT_AUS     0x4B53 //!< Addr: 0x4B53, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4B55_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B55 //!< Addr: 0x4B55, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B56_OBJEKT_FUER_PRIORITAET     0x4B56 //!< Addr: 0x4B56, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4B57_BEI_TAG     0x4B57 //!< Addr: 0x4B57, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4B59_VERHALTEN_BEI_TAG_WERT_EIN     0x4B59 //!< Addr: 0x4B59, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B58_BEI_NACHT     0x4B58 //!< Addr: 0x4B58, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4B5A_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B5A //!< Addr: 0x4B5A, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B6E_LED12_READREQUEST     0x4B6E //!< Addr: 0x4B6E, Size: 0x0008, LED12_ReadRequest
#define EE_PARAMETER_4B62_AUSWAHL_DER_OBJEKTNUMMER     0x4B62 //!< Addr: 0x4B62, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4B63_BEI_TAG_WERT_EIN     0x4B63 //!< Addr: 0x4B63, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4B65_BEI_TAG_WERT_AUS     0x4B65 //!< Addr: 0x4B65, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4B67_VERHALTEN_BEI_TAG_WERT_EIN     0x4B67 //!< Addr: 0x4B67, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B64_BEI_NACHT_WERT_EIN     0x4B64 //!< Addr: 0x4B64, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B66_BEI_NACHT_WERT_AUS     0x4B66 //!< Addr: 0x4B66, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4B68_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B68 //!< Addr: 0x4B68, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B69_OBJEKT_FUER_PRIORITAET     0x4B69 //!< Addr: 0x4B69, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4B6A_BEI_TAG     0x4B6A //!< Addr: 0x4B6A, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4B6C_VERHALTEN_BEI_TAG_WERT_EIN     0x4B6C //!< Addr: 0x4B6C, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B6B_BEI_NACHT     0x4B6B //!< Addr: 0x4B6B, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4B6D_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B6D //!< Addr: 0x4B6D, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B81_LED13_READREQUEST     0x4B81 //!< Addr: 0x4B81, Size: 0x0008, LED13_ReadRequest
#define EE_PARAMETER_4B75_AUSWAHL_DER_OBJEKTNUMMER     0x4B75 //!< Addr: 0x4B75, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4B76_BEI_TAG_WERT_EIN     0x4B76 //!< Addr: 0x4B76, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4B78_BEI_TAG_WERT_AUS     0x4B78 //!< Addr: 0x4B78, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4B7A_VERHALTEN_BEI_TAG_WERT_EIN     0x4B7A //!< Addr: 0x4B7A, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B77_BEI_NACHT_WERT_EIN     0x4B77 //!< Addr: 0x4B77, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B79_BEI_NACHT_WERT_AUS     0x4B79 //!< Addr: 0x4B79, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4B7B_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B7B //!< Addr: 0x4B7B, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B7C_OBJEKT_FUER_PRIORITAET     0x4B7C //!< Addr: 0x4B7C, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4B7D_BEI_TAG     0x4B7D //!< Addr: 0x4B7D, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4B7F_VERHALTEN_BEI_TAG_WERT_EIN     0x4B7F //!< Addr: 0x4B7F, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B7E_BEI_NACHT     0x4B7E //!< Addr: 0x4B7E, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4B80_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B80 //!< Addr: 0x4B80, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B94_LED14_READREQUEST     0x4B94 //!< Addr: 0x4B94, Size: 0x0008, LED14_ReadRequest
#define EE_PARAMETER_4B88_AUSWAHL_DER_OBJEKTNUMMER     0x4B88 //!< Addr: 0x4B88, Size: 0x0008, Auswahl der Objektnummer 
#define EE_PARAMETER_4B89_BEI_TAG_WERT_EIN     0x4B89 //!< Addr: 0x4B89, Size: 0x0008,     bei Tag (Wert EIN) 
#define EE_PARAMETER_4B8B_BEI_TAG_WERT_AUS     0x4B8B //!< Addr: 0x4B8B, Size: 0x0008,     bei Tag (Wert AUS) 
#define EE_PARAMETER_4B8D_VERHALTEN_BEI_TAG_WERT_EIN     0x4B8D //!< Addr: 0x4B8D, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B8A_BEI_NACHT_WERT_EIN     0x4B8A //!< Addr: 0x4B8A, Size: 0x0008,     bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B8C_BEI_NACHT_WERT_AUS     0x4B8C //!< Addr: 0x4B8C, Size: 0x0008,     bei Nacht (Wert AUS) 
#define EE_PARAMETER_4B8E_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B8E //!< Addr: 0x4B8E, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4B8F_OBJEKT_FUER_PRIORITAET     0x4B8F //!< Addr: 0x4B8F, Size: 0x0008, Objekt für Priorität 
#define EE_PARAMETER_4B90_BEI_TAG     0x4B90 //!< Addr: 0x4B90, Size: 0x0008,     bei Tag 
#define EE_PARAMETER_4B92_VERHALTEN_BEI_TAG_WERT_EIN     0x4B92 //!< Addr: 0x4B92, Size: 0x0008,     Verhalten bei Tag (Wert EIN) 
#define EE_PARAMETER_4B91_BEI_NACHT     0x4B91 //!< Addr: 0x4B91, Size: 0x0008,     bei Nacht 
#define EE_PARAMETER_4B93_VERHALTEN_BEI_NACHT_WERT_EIN     0x4B93 //!< Addr: 0x4B93, Size: 0x0008,     Verhalten bei Nacht (Wert EIN) 
#define EE_PARAMETER_4BA5_TASTE_9     0x4BA5 //!< Addr: 0x4BA5, Size: 0x0008,     Taste 9 
#define EE_PARAMETER_4BA6_TASTE_10     0x4BA6 //!< Addr: 0x4BA6, Size: 0x0008,     Taste 10 
#define EE_PARAMETER_4BA7_TASTE_11     0x4BA7 //!< Addr: 0x4BA7, Size: 0x0008,     Taste 11 
#define EE_PARAMETER_4BA8_TASTE_12     0x4BA8 //!< Addr: 0x4BA8, Size: 0x0008,     Taste 12 
#define EE_PARAMETER_4BBD_TASTE_9     0x4BBD //!< Addr: 0x4BBD, Size: 0x0008,     Taste 9 
#define EE_PARAMETER_4BBE_TASTE_10     0x4BBE //!< Addr: 0x4BBE, Size: 0x0008,     Taste 10 
#define EE_PARAMETER_4BBF_TASTE_11     0x4BBF //!< Addr: 0x4BBF, Size: 0x0008,     Taste 11 
#define EE_PARAMETER_4BC0_TASTE_12     0x4BC0 //!< Addr: 0x4BC0, Size: 0x0008,     Taste 12 
#define EE_PARAMETER_4BD5_TASTE_9     0x4BD5 //!< Addr: 0x4BD5, Size: 0x0008,     Taste 9 
#define EE_PARAMETER_4BD6_TASTE_10     0x4BD6 //!< Addr: 0x4BD6, Size: 0x0008,     Taste 10 
#define EE_PARAMETER_4BD7_TASTE_11     0x4BD7 //!< Addr: 0x4BD7, Size: 0x0008,     Taste 11 
#define EE_PARAMETER_4BD8_TASTE_12     0x4BD8 //!< Addr: 0x4BD8, Size: 0x0008,     Taste 12 
#define EE_PARAMETER_4BED_TASTE_9     0x4BED //!< Addr: 0x4BED, Size: 0x0008,     Taste 9 
#define EE_PARAMETER_4BEE_TASTE_10     0x4BEE //!< Addr: 0x4BEE, Size: 0x0008,     Taste 10 
#define EE_PARAMETER_4BEF_TASTE_11     0x4BEF //!< Addr: 0x4BEF, Size: 0x0008,     Taste 11 
#define EE_PARAMETER_4BF0_TASTE_12     0x4BF0 //!< Addr: 0x4BF0, Size: 0x0008,     Taste 12 
#define EE_PARAMETER_462A_OM_INPUTUSAGE_MAINTYPEH_0     0x462A //!< Addr: 0x462A, Size: 0x0008, OM_inputUsage_mainTypeH_0
#define EE_PARAMETER_462B_OM_INPUTUSAGE_MAINTYPE_0     0x462B //!< Addr: 0x462B, Size: 0x0008, OM_inputUsage_mainType_0
#define EE_PARAMETER_4680_OM_INPUTUSAGE_MAINTYPEH_1     0x4680 //!< Addr: 0x4680, Size: 0x0008, OM_inputUsage_mainTypeH_1
#define EE_PARAMETER_4681_OM_INPUTUSAGE_MAINTYPE_1     0x4681 //!< Addr: 0x4681, Size: 0x0008, OM_inputUsage_mainType_1
#define EE_PARAMETER_4CFD_ANZEIGEVERHALTEN_DER_LEDS     0x4CFD //!< Addr: 0x4CFD, Size: 0x0008, Anzeigeverhalten der LED's 
#define EE_PARAMETER_4CFE_FARBE_DER_ORIENTIERUNGS_LEDS_IM_STANDBY     0x4CFE //!< Addr: 0x4CFE, Size: 0x0008, Farbe der Orientierungs-LEDs im Standby 
#define EE_PARAMETER_4CFF_LED_FARBE_BEI_TASTENBETAETIGUNG_NUR_BEI_EINSTELLUNG_OBJEKT_UND_TASTENBETAETIGUNG     0x4CFF //!< Addr: 0x4CFF, Size: 0x0008, LED-Farbe bei Tastenbetätigung, nur bei Einstellung "Objekt und Tastenbetätigung" 
#define EE_PARAMETER_4C1E_SCHRIFTGROESSE_FUER_ERSTE_STATUSZEILE     0x4C1E //!< Addr: 0x4C1E, Size: 0x0001,     Schriftgröße für erste Statuszeile 
#define EE_PARAMETER_4C1D_SCHRIFTGROESSE_FUER_ZWEITE_STATUSZEILE     0x4C1D //!< Addr: 0x4C1D, Size: 0x0001,     Schriftgröße für zweite Statuszeile 
#define EE_PARAMETER_4C27_SCHRIFTGROESSE_FUER_ZWEITE_STATUSZEILE     0x4C27 //!< Addr: 0x4C27, Size: 0x0001,     Schriftgröße für zweite Statuszeile 
#define EE_PARAMETER_4C28_SCHRIFTGROESSE_FUER_ERSTE_STATUSZEILE     0x4C28 //!< Addr: 0x4C28, Size: 0x0001,     Schriftgröße für erste Statuszeile 
#define EE_PARAMETER_46D7_OM_INPUTUSAGE_MAINTYPE_2     0x46D7 //!< Addr: 0x46D7, Size: 0x0008, OM_inputUsage_mainType_2
#define EE_PARAMETER_46D9_OM_INPUTUSAGE_SUBTYPE_2     0x46D9 //!< Addr: 0x46D9, Size: 0x0008, OM_InputUsage_subType_2
#define EE_PARAMETER_46F7_OM_INPUTUSAGE_OBJTYPE_2     0x46F7 //!< Addr: 0x46F7, Size: 0x0008, OM_inputUsage_objType_2
#define EE_PARAMETER_4723_FARBE_DES_SYMBOLS_FUER_0PROZENT     0x4723 //!< Addr: 0x4723, Size: 0x0008,     Farbe des Symbols für 0% 
#define EE_PARAMETER_4724_FARBE_DES_SYMBOLS_FUER_0PROZENT_90PROZENT     0x4724 //!< Addr: 0x4724, Size: 0x0008,     Farbe des Symbols für 0% - 90% 
#define EE_PARAMETER_4720_SYMBOL_FUER_0PROZENT_90PROZENT     0x4720 //!< Addr: 0x4720, Size: 0x0008,     Symbol für 0% - 90% 
#define EE_PARAMETER_46FC_OM_INPUTUSAGE_TIMERONACTIVE_2     0x46FC //!< Addr: 0x46FC, Size: 0x0001, OM_InputUsage_timerOnActive_2
#define EE_PARAMETER_46FD_OM_INPUTUSAGE_TIMEROFFACTIVE_2     0x46FD //!< Addr: 0x46FD, Size: 0x0001, OM_InputUsage_timerOffActive_2
#define EE_PARAMETER_46D6_OM_INPUTUSAGE_MAINTYPEH_2     0x46D6 //!< Addr: 0x46D6, Size: 0x0008, OM_inputUsage_mainTypeH_2
#define EE_PARAMETER_46EA_OM_INPUTUSAGE_VALUETYPE_00_2     0x46EA //!< Addr: 0x46EA, Size: 0x0008, OM_InputUsage_valueType_00_2
#define EE_PARAMETER_4704_DARSTELLUNG_DER_FUNKTION_KURZ_LANG     0x4704 //!< Addr: 0x4704, Size: 0x0001, Darstellung der Funktion kurz/lang 
#define EE_PARAMETER_4725_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x4725 //!< Addr: 0x4725, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_4721_SYMBOL_FUER_90PROZENT     0x4721 //!< Addr: 0x4721, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_4726_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x4726 //!< Addr: 0x4726, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_4722_SYMBOL_FUER_90PROZENT     0x4722 //!< Addr: 0x4722, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_46D8_ANZAHL_DER_WERTE     0x46D8 //!< Addr: 0x46D8, Size: 0x0008, Anzahl der Werte 
#define EE_PARAMETER_4703_UMSCHALTART     0x4703 //!< Addr: 0x4703, Size: 0x0001, Umschaltart 
#define EE_PARAMETER_470A_UMSCHALTUNG_BERUECKSICHTIGT_STATUSOBJEKT     0x470A //!< Addr: 0x470A, Size: 0x0001, Umschaltung berücksichtigt Statusobjekt 
#define EE_PARAMETER_4705_FUNCTION_VALUETOICON_2     0x4705 //!< Addr: 0x4705, Size: 0x0001, Function_ValueToIcon_2
#define EE_PARAMETER_472D_OM_INPUTUSAGE_MAINTYPE_3     0x472D //!< Addr: 0x472D, Size: 0x0008, OM_inputUsage_mainType_3
#define EE_PARAMETER_472F_OM_INPUTUSAGE_SUBTYPE_3     0x472F //!< Addr: 0x472F, Size: 0x0008, OM_InputUsage_subType_3
#define EE_PARAMETER_474D_OM_INPUTUSAGE_OBJTYPE_3     0x474D //!< Addr: 0x474D, Size: 0x0008, OM_inputUsage_objType_3
#define EE_PARAMETER_4779_FARBE_DES_SYMBOLS_LAMELLE_OFFEN_55PROZENT     0x4779 //!< Addr: 0x4779, Size: 0x0008, Farbe des Symbols Lamelle offen (<55%) 
#define EE_PARAMETER_477A_FARBE_DES_SYMBOLS_LAMELLE_MITTIG_55PROZENT_90PROZENT     0x477A //!< Addr: 0x477A, Size: 0x0008, Farbe des Symbols Lamelle mittig (55% - 90%) 
#define EE_PARAMETER_4776_SYMBOL_LAMELLE_MITTIG_55PROZENT_90PROZENT     0x4776 //!< Addr: 0x4776, Size: 0x0008, Symbol Lamelle mittig (55% - 90%) 
#define EE_PARAMETER_4752_OM_INPUTUSAGE_TIMERCONFIGON_3     0x4752 //!< Addr: 0x4752, Size: 0x0001, OM_InputUsage_timerConfigOn_3
#define EE_PARAMETER_4753_OM_INPUTUSAGE_TIMERCONFIGOFF_3     0x4753 //!< Addr: 0x4753, Size: 0x0001, OM_InputUsage_timerConfigOff_3
#define EE_PARAMETER_472C_OM_INPUTUSAGE_MAINTYPEH_3     0x472C //!< Addr: 0x472C, Size: 0x0008, OM_inputUsage_mainTypeH_3
#define EE_PARAMETER_4740_OM_INPUTUSAGE_VALUETYPE_00_3     0x4740 //!< Addr: 0x4740, Size: 0x0008, OM_InputUsage_valueType_00_3
#define EE_PARAMETER_475A_DARSTELLUNG_DER_FUNKTION_KURZ_LANG     0x475A //!< Addr: 0x475A, Size: 0x0001, Darstellung der Funktion kurz/lang 
#define EE_PARAMETER_477B_FARBE_DES_SYMBOLS_LAMELLE_GESCHLOSSEN_90PROZENT     0x477B //!< Addr: 0x477B, Size: 0x0008, Farbe des Symbols Lamelle geschlossen (>90%) 
#define EE_PARAMETER_4777_SYMBOL_LAMELLE_GESCHLOSSEN_90PROZENT     0x4777 //!< Addr: 0x4777, Size: 0x0008, Symbol Lamelle geschlossen (>90%) 
#define EE_PARAMETER_477C_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x477C //!< Addr: 0x477C, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_4778_SYMBOL_FUER_90PROZENT     0x4778 //!< Addr: 0x4778, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_472E_ANZAHL_DER_WERTE     0x472E //!< Addr: 0x472E, Size: 0x0008, Anzahl der Werte 
#define EE_PARAMETER_4759_OM_INPUTUSAGE_VALUESWITCHMODE_3     0x4759 //!< Addr: 0x4759, Size: 0x0001, OM_InputUsage_valueSwitchMode_3
#define EE_PARAMETER_4760_UMSCHALTUNG_BERUECKSICHTIGT_STATUSOBJEKT     0x4760 //!< Addr: 0x4760, Size: 0x0001, Umschaltung berücksichtigt Statusobjekt 
#define EE_PARAMETER_475B_FUNCTION_VALUETOICON_3     0x475B //!< Addr: 0x475B, Size: 0x0001, Function_ValueToIcon_3
#define EE_PARAMETER_4719_TASTENBESCHRIFTUNG_LINKE_TASTE     0x4719 //!< Addr: 0x4719, Size: 0x0030, Tastenbeschriftung linke Taste 
#define EE_PARAMETER_476F_TASTENBESCHRIFTUNG_RECHTE_TASTE     0x476F //!< Addr: 0x476F, Size: 0x0030, Tastenbeschriftung rechte Taste 
#define EE_PARAMETER_4D00_FARBE_DES_MELDUNG_TEXTES     0x4D00 //!< Addr: 0x4D00, Size: 0x0008, Farbe des Meldung-Textes 
#define EE_PARAMETER_463F_OM_INPUTUSAGE_VALUETYPE_01_0     0x463F //!< Addr: 0x463F, Size: 0x0008, OM_InputUsage_valueType_01_0
#define EE_PARAMETER_46EB_OM_INPUTUSAGE_VALUETYPE_01_2     0x46EB //!< Addr: 0x46EB, Size: 0x0008, OM_InputUsage_valueType_01_2
#define EE_UNIONPARAMETER_4D01     0x4D01 //!< Addr: 0x4D01, Size: 0x0008,     Farben der LEDs  oder Color_AlarmLedOff
#define EE_PARAMETER_4783_OM_INPUTUSAGE_MAINTYPE_4     0x4783 //!< Addr: 0x4783, Size: 0x0008, OM_inputUsage_mainType_4
#define EE_PARAMETER_4785_OM_INPUTUSAGE_SUBTYPE_4     0x4785 //!< Addr: 0x4785, Size: 0x0008, OM_InputUsage_subType_4
#define EE_PARAMETER_47A3_OM_INPUTUSAGE_OBJTYPE_4     0x47A3 //!< Addr: 0x47A3, Size: 0x0008, OM_inputUsage_objType_4
#define EE_PARAMETER_47CF_FARBE_DES_SYMBOLS_FUER_0PROZENT     0x47CF //!< Addr: 0x47CF, Size: 0x0008,     Farbe des Symbols für 0% 
#define EE_PARAMETER_47D0_FARBE_DES_SYMBOLS_FUER_0PROZENT_90PROZENT     0x47D0 //!< Addr: 0x47D0, Size: 0x0008,     Farbe des Symbols für 0% - 90% 
#define EE_PARAMETER_47CC_SYMBOL_FUER_0PROZENT_90PROZENT     0x47CC //!< Addr: 0x47CC, Size: 0x0008,     Symbol für 0% - 90% 
#define EE_PARAMETER_47A8_OM_INPUTUSAGE_TIMERONACTIVE_4     0x47A8 //!< Addr: 0x47A8, Size: 0x0001, OM_InputUsage_timerOnActive_4
#define EE_PARAMETER_47A9_OM_INPUTUSAGE_TIMEROFFACTIVE_4     0x47A9 //!< Addr: 0x47A9, Size: 0x0001, OM_InputUsage_timerOffActive_4
#define EE_PARAMETER_47D1_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x47D1 //!< Addr: 0x47D1, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_47CD_SYMBOL_FUER_90PROZENT     0x47CD //!< Addr: 0x47CD, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_47D2_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x47D2 //!< Addr: 0x47D2, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_47CE_SYMBOL_FUER_90PROZENT     0x47CE //!< Addr: 0x47CE, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_4784_ANZAHL_DER_WERTE     0x4784 //!< Addr: 0x4784, Size: 0x0008, Anzahl der Werte 
#define EE_PARAMETER_47AF_UMSCHALTART     0x47AF //!< Addr: 0x47AF, Size: 0x0001, Umschaltart 
#define EE_PARAMETER_47B6_UMSCHALTUNG_BERUECKSICHTIGT_STATUSOBJEKT     0x47B6 //!< Addr: 0x47B6, Size: 0x0001, Umschaltung berücksichtigt Statusobjekt 
#define EE_PARAMETER_47B1_FUNCTION_VALUETOICON_4     0x47B1 //!< Addr: 0x47B1, Size: 0x0001, Function_ValueToIcon_4
#define EE_PARAMETER_4782_OM_INPUTUSAGE_MAINTYPEH_4     0x4782 //!< Addr: 0x4782, Size: 0x0008, OM_inputUsage_mainTypeH_4
#define EE_PARAMETER_4796_OM_INPUTUSAGE_VALUETYPE_00_4     0x4796 //!< Addr: 0x4796, Size: 0x0008, OM_InputUsage_valueType_00_4
#define EE_PARAMETER_47B0_DARSTELLUNG_DER_FUNKTION_KURZ_LANG     0x47B0 //!< Addr: 0x47B0, Size: 0x0001, Darstellung der Funktion kurz/lang 
#define EE_PARAMETER_47D9_OM_INPUTUSAGE_MAINTYPE_5     0x47D9 //!< Addr: 0x47D9, Size: 0x0008, OM_inputUsage_mainType_5
#define EE_PARAMETER_47DB_OM_INPUTUSAGE_SUBTYPE_5     0x47DB //!< Addr: 0x47DB, Size: 0x0008, OM_InputUsage_subType_5
#define EE_PARAMETER_47F9_OM_INPUTUSAGE_OBJTYPE_5     0x47F9 //!< Addr: 0x47F9, Size: 0x0008, OM_inputUsage_objType_5
#define EE_PARAMETER_4825_FARBE_DES_SYMBOLS_LAMELLE_OFFEN_55PROZENT     0x4825 //!< Addr: 0x4825, Size: 0x0008, Farbe des Symbols Lamelle offen (<55%) 
#define EE_PARAMETER_4826_FARBE_DES_SYMBOLS_LAMELLE_MITTIG_55PROZENT_90PROZENT     0x4826 //!< Addr: 0x4826, Size: 0x0008, Farbe des Symbols Lamelle mittig (55% - 90%) 
#define EE_PARAMETER_4822_SYMBOL_LAMELLE_MITTIG_55PROZENT_90PROZENT     0x4822 //!< Addr: 0x4822, Size: 0x0008, Symbol Lamelle mittig (55% - 90%) 
#define EE_PARAMETER_47FE_OM_INPUTUSAGE_TIMERCONFIGON_5     0x47FE //!< Addr: 0x47FE, Size: 0x0001, OM_InputUsage_timerConfigOn_5
#define EE_PARAMETER_47FF_OM_INPUTUSAGE_TIMERCONFIGOFF_5     0x47FF //!< Addr: 0x47FF, Size: 0x0001, OM_InputUsage_timerConfigOff_5
#define EE_PARAMETER_4827_FARBE_DES_SYMBOLS_LAMELLE_GESCHLOSSEN_90PROZENT     0x4827 //!< Addr: 0x4827, Size: 0x0008, Farbe des Symbols Lamelle geschlossen (>90%) 
#define EE_PARAMETER_4823_SYMBOL_LAMELLE_GESCHLOSSEN_90PROZENT     0x4823 //!< Addr: 0x4823, Size: 0x0008, Symbol Lamelle geschlossen (>90%) 
#define EE_PARAMETER_4828_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x4828 //!< Addr: 0x4828, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_4824_SYMBOL_FUER_90PROZENT     0x4824 //!< Addr: 0x4824, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_47DA_ANZAHL_DER_WERTE     0x47DA //!< Addr: 0x47DA, Size: 0x0008, Anzahl der Werte 
#define EE_PARAMETER_4805_OM_INPUTUSAGE_VALUESWITCHMODE_5     0x4805 //!< Addr: 0x4805, Size: 0x0001, OM_InputUsage_valueSwitchMode_5
#define EE_PARAMETER_480C_UMSCHALTUNG_BERUECKSICHTIGT_STATUSOBJEKT     0x480C //!< Addr: 0x480C, Size: 0x0001, Umschaltung berücksichtigt Statusobjekt 
#define EE_PARAMETER_4807_FUNCTION_VALUETOICON_5     0x4807 //!< Addr: 0x4807, Size: 0x0001, Function_ValueToIcon_5
#define EE_PARAMETER_47D8_OM_INPUTUSAGE_MAINTYPEH_5     0x47D8 //!< Addr: 0x47D8, Size: 0x0008, OM_inputUsage_mainTypeH_5
#define EE_PARAMETER_47EC_OM_INPUTUSAGE_VALUETYPE_00_5     0x47EC //!< Addr: 0x47EC, Size: 0x0008, OM_InputUsage_valueType_00_5
#define EE_PARAMETER_4806_DARSTELLUNG_DER_FUNKTION_KURZ_LANG     0x4806 //!< Addr: 0x4806, Size: 0x0001, Darstellung der Funktion kurz/lang 
#define EE_PARAMETER_47C5_TASTENBESCHRIFTUNG_LINKE_TASTE     0x47C5 //!< Addr: 0x47C5, Size: 0x0030, Tastenbeschriftung linke Taste 
#define EE_PARAMETER_481B_TASTENBESCHRIFTUNG_RECHTE_TASTE     0x481B //!< Addr: 0x481B, Size: 0x0030, Tastenbeschriftung rechte Taste 
#define EE_PARAMETER_4797_OM_INPUTUSAGE_VALUETYPE_01_4     0x4797 //!< Addr: 0x4797, Size: 0x0008, OM_InputUsage_valueType_01_4
#define EE_PARAMETER_482F_OM_INPUTUSAGE_MAINTYPE_6     0x482F //!< Addr: 0x482F, Size: 0x0008, OM_inputUsage_mainType_6
#define EE_PARAMETER_4831_OM_INPUTUSAGE_SUBTYPE_6     0x4831 //!< Addr: 0x4831, Size: 0x0008, OM_InputUsage_subType_6
#define EE_PARAMETER_484F_OM_INPUTUSAGE_OBJTYPE_6     0x484F //!< Addr: 0x484F, Size: 0x0008, OM_inputUsage_objType_6
#define EE_PARAMETER_4854_OM_INPUTUSAGE_TIMERONACTIVE_6     0x4854 //!< Addr: 0x4854, Size: 0x0001, OM_InputUsage_timerOnActive_6
#define EE_PARAMETER_4855_OM_INPUTUSAGE_TIMEROFFACTIVE_6     0x4855 //!< Addr: 0x4855, Size: 0x0001, OM_InputUsage_timerOffActive_6
#define EE_PARAMETER_487B_FARBE_DES_SYMBOLS_FUER_0PROZENT     0x487B //!< Addr: 0x487B, Size: 0x0008,     Farbe des Symbols für 0% 
#define EE_PARAMETER_487C_FARBE_DES_SYMBOLS_FUER_0PROZENT_90PROZENT     0x487C //!< Addr: 0x487C, Size: 0x0008,     Farbe des Symbols für 0% - 90% 
#define EE_PARAMETER_4878_SYMBOL_FUER_0PROZENT_90PROZENT     0x4878 //!< Addr: 0x4878, Size: 0x0008,     Symbol für 0% - 90% 
#define EE_PARAMETER_487D_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x487D //!< Addr: 0x487D, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_4879_SYMBOL_FUER_90PROZENT     0x4879 //!< Addr: 0x4879, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_487E_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x487E //!< Addr: 0x487E, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_487A_SYMBOL_FUER_90PROZENT     0x487A //!< Addr: 0x487A, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_4830_ANZAHL_DER_WERTE     0x4830 //!< Addr: 0x4830, Size: 0x0008, Anzahl der Werte 
#define EE_PARAMETER_485B_UMSCHALTART     0x485B //!< Addr: 0x485B, Size: 0x0001, Umschaltart 
#define EE_PARAMETER_4862_UMSCHALTUNG_BERUECKSICHTIGT_STATUSOBJEKT     0x4862 //!< Addr: 0x4862, Size: 0x0001, Umschaltung berücksichtigt Statusobjekt 
#define EE_PARAMETER_485D_FUNCTION_VALUETOICON_6     0x485D //!< Addr: 0x485D, Size: 0x0001, Function_ValueToIcon_6
#define EE_PARAMETER_482E_OM_INPUTUSAGE_MAINTYPEH_6     0x482E //!< Addr: 0x482E, Size: 0x0008, OM_inputUsage_mainTypeH_6
#define EE_PARAMETER_4842_OM_INPUTUSAGE_VALUETYPE_00_6     0x4842 //!< Addr: 0x4842, Size: 0x0008, OM_InputUsage_valueType_00_6
#define EE_PARAMETER_485C_DARSTELLUNG_DER_FUNKTION_KURZ_LANG     0x485C //!< Addr: 0x485C, Size: 0x0001, Darstellung der Funktion kurz/lang 
#define EE_PARAMETER_4885_OM_INPUTUSAGE_MAINTYPE_7     0x4885 //!< Addr: 0x4885, Size: 0x0008, OM_inputUsage_mainType_7
#define EE_PARAMETER_4887_OM_INPUTUSAGE_SUBTYPE_7     0x4887 //!< Addr: 0x4887, Size: 0x0008, OM_InputUsage_subType_7
#define EE_PARAMETER_48A5_OM_INPUTUSAGE_OBJTYPE_7     0x48A5 //!< Addr: 0x48A5, Size: 0x0008, OM_inputUsage_objType_7
#define EE_PARAMETER_48AA_OM_INPUTUSAGE_TIMERCONFIGON_7     0x48AA //!< Addr: 0x48AA, Size: 0x0001, OM_InputUsage_timerConfigOn_7
#define EE_PARAMETER_48AB_OM_INPUTUSAGE_TIMERCONFIGOFF_7     0x48AB //!< Addr: 0x48AB, Size: 0x0001, OM_InputUsage_timerConfigOff_7
#define EE_PARAMETER_48D1_FARBE_DES_SYMBOLS_LAMELLE_OFFEN_55PROZENT     0x48D1 //!< Addr: 0x48D1, Size: 0x0008, Farbe des Symbols Lamelle offen (<55%) 
#define EE_PARAMETER_48D2_FARBE_DES_SYMBOLS_LAMELLE_MITTIG_55PROZENT_90PROZENT     0x48D2 //!< Addr: 0x48D2, Size: 0x0008, Farbe des Symbols Lamelle mittig (55% - 90%) 
#define EE_PARAMETER_48CE_SYMBOL_LAMELLE_MITTIG_55PROZENT_90PROZENT     0x48CE //!< Addr: 0x48CE, Size: 0x0008, Symbol Lamelle mittig (55% - 90%) 
#define EE_PARAMETER_48D3_FARBE_DES_SYMBOLS_LAMELLE_GESCHLOSSEN_90PROZENT     0x48D3 //!< Addr: 0x48D3, Size: 0x0008, Farbe des Symbols Lamelle geschlossen (>90%) 
#define EE_PARAMETER_48CF_SYMBOL_LAMELLE_GESCHLOSSEN_90PROZENT     0x48CF //!< Addr: 0x48CF, Size: 0x0008, Symbol Lamelle geschlossen (>90%) 
#define EE_PARAMETER_48D4_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x48D4 //!< Addr: 0x48D4, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_48D0_SYMBOL_FUER_90PROZENT     0x48D0 //!< Addr: 0x48D0, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_4886_ANZAHL_DER_WERTE     0x4886 //!< Addr: 0x4886, Size: 0x0008, Anzahl der Werte 
#define EE_PARAMETER_48B1_OM_INPUTUSAGE_VALUESWITCHMODE_7     0x48B1 //!< Addr: 0x48B1, Size: 0x0001, OM_InputUsage_valueSwitchMode_7
#define EE_PARAMETER_48B8_UMSCHALTUNG_BERUECKSICHTIGT_STATUSOBJEKT     0x48B8 //!< Addr: 0x48B8, Size: 0x0001, Umschaltung berücksichtigt Statusobjekt 
#define EE_PARAMETER_48B3_FUNCTION_VALUETOICON_7     0x48B3 //!< Addr: 0x48B3, Size: 0x0001, Function_ValueToIcon_7
#define EE_PARAMETER_4884_OM_INPUTUSAGE_MAINTYPEH_7     0x4884 //!< Addr: 0x4884, Size: 0x0008, OM_inputUsage_mainTypeH_7
#define EE_PARAMETER_4898_OM_INPUTUSAGE_VALUETYPE_00_7     0x4898 //!< Addr: 0x4898, Size: 0x0008, OM_InputUsage_valueType_00_7
#define EE_PARAMETER_48B2_DARSTELLUNG_DER_FUNKTION_KURZ_LANG     0x48B2 //!< Addr: 0x48B2, Size: 0x0001, Darstellung der Funktion kurz/lang 
#define EE_PARAMETER_4871_TASTENBESCHRIFTUNG_LINKE_TASTE     0x4871 //!< Addr: 0x4871, Size: 0x0030, Tastenbeschriftung linke Taste 
#define EE_PARAMETER_48C7_TASTENBESCHRIFTUNG_RECHTE_TASTE     0x48C7 //!< Addr: 0x48C7, Size: 0x0030, Tastenbeschriftung rechte Taste 
#define EE_PARAMETER_4843_OM_INPUTUSAGE_VALUETYPE_01_6     0x4843 //!< Addr: 0x4843, Size: 0x0008, OM_InputUsage_valueType_01_6
#define EE_PARAMETER_48DB_OM_INPUTUSAGE_MAINTYPE_8     0x48DB //!< Addr: 0x48DB, Size: 0x0008, OM_inputUsage_mainType_8
#define EE_PARAMETER_48DD_OM_INPUTUSAGE_SUBTYPE_8     0x48DD //!< Addr: 0x48DD, Size: 0x0008, OM_InputUsage_subType_8
#define EE_PARAMETER_48FB_OM_INPUTUSAGE_OBJTYPE_8     0x48FB //!< Addr: 0x48FB, Size: 0x0008, OM_inputUsage_objType_8
#define EE_PARAMETER_4900_OM_INPUTUSAGE_TIMERONACTIVE_8     0x4900 //!< Addr: 0x4900, Size: 0x0001, OM_InputUsage_timerOnActive_8
#define EE_PARAMETER_4901_OM_INPUTUSAGE_TIMEROFFACTIVE_8     0x4901 //!< Addr: 0x4901, Size: 0x0001, OM_InputUsage_timerOffActive_8
#define EE_PARAMETER_4927_FARBE_DES_SYMBOLS_FUER_0PROZENT     0x4927 //!< Addr: 0x4927, Size: 0x0008,     Farbe des Symbols für 0% 
#define EE_PARAMETER_4928_FARBE_DES_SYMBOLS_FUER_0PROZENT_90PROZENT     0x4928 //!< Addr: 0x4928, Size: 0x0008,     Farbe des Symbols für 0% - 90% 
#define EE_PARAMETER_4924_SYMBOL_FUER_0PROZENT_90PROZENT     0x4924 //!< Addr: 0x4924, Size: 0x0008,     Symbol für 0% - 90% 
#define EE_PARAMETER_4929_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x4929 //!< Addr: 0x4929, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_4925_SYMBOL_FUER_90PROZENT     0x4925 //!< Addr: 0x4925, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_492A_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x492A //!< Addr: 0x492A, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_4926_SYMBOL_FUER_90PROZENT     0x4926 //!< Addr: 0x4926, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_48DC_ANZAHL_DER_WERTE     0x48DC //!< Addr: 0x48DC, Size: 0x0008, Anzahl der Werte 
#define EE_PARAMETER_4907_UMSCHALTART     0x4907 //!< Addr: 0x4907, Size: 0x0001, Umschaltart 
#define EE_PARAMETER_490E_UMSCHALTUNG_BERUECKSICHTIGT_STATUSOBJEKT     0x490E //!< Addr: 0x490E, Size: 0x0001, Umschaltung berücksichtigt Statusobjekt 
#define EE_PARAMETER_4909_FUNCTION_VALUETOICON_8     0x4909 //!< Addr: 0x4909, Size: 0x0001, Function_ValueToIcon_8
#define EE_PARAMETER_48DA_OM_INPUTUSAGE_MAINTYPEH_8     0x48DA //!< Addr: 0x48DA, Size: 0x0008, OM_inputUsage_mainTypeH_8
#define EE_PARAMETER_48EE_OM_INPUTUSAGE_VALUETYPE_00_8     0x48EE //!< Addr: 0x48EE, Size: 0x0008, OM_InputUsage_valueType_00_8
#define EE_PARAMETER_4908_DARSTELLUNG_DER_FUNKTION_KURZ_LANG     0x4908 //!< Addr: 0x4908, Size: 0x0001, Darstellung der Funktion kurz/lang 
#define EE_PARAMETER_4931_OM_INPUTUSAGE_MAINTYPE_9     0x4931 //!< Addr: 0x4931, Size: 0x0008, OM_inputUsage_mainType_9
#define EE_PARAMETER_4933_OM_INPUTUSAGE_SUBTYPE_9     0x4933 //!< Addr: 0x4933, Size: 0x0008, OM_InputUsage_subType_9
#define EE_PARAMETER_4951_OM_INPUTUSAGE_OBJTYPE_9     0x4951 //!< Addr: 0x4951, Size: 0x0008, OM_inputUsage_objType_9
#define EE_PARAMETER_4956_OM_INPUTUSAGE_TIMERCONFIGON_9     0x4956 //!< Addr: 0x4956, Size: 0x0001, OM_InputUsage_timerConfigOn_9
#define EE_PARAMETER_4957_OM_INPUTUSAGE_TIMERCONFIGOFF_9     0x4957 //!< Addr: 0x4957, Size: 0x0001, OM_InputUsage_timerConfigOff_9
#define EE_PARAMETER_497D_FARBE_DES_SYMBOLS_LAMELLE_OFFEN_55PROZENT     0x497D //!< Addr: 0x497D, Size: 0x0008, Farbe des Symbols Lamelle offen (<55%) 
#define EE_PARAMETER_497E_FARBE_DES_SYMBOLS_LAMELLE_MITTIG_55PROZENT_90PROZENT     0x497E //!< Addr: 0x497E, Size: 0x0008, Farbe des Symbols Lamelle mittig (55% - 90%) 
#define EE_PARAMETER_497A_SYMBOL_LAMELLE_MITTIG_55PROZENT_90PROZENT     0x497A //!< Addr: 0x497A, Size: 0x0008, Symbol Lamelle mittig (55% - 90%) 
#define EE_PARAMETER_497F_FARBE_DES_SYMBOLS_LAMELLE_GESCHLOSSEN_90PROZENT     0x497F //!< Addr: 0x497F, Size: 0x0008, Farbe des Symbols Lamelle geschlossen (>90%) 
#define EE_PARAMETER_497B_SYMBOL_LAMELLE_GESCHLOSSEN_90PROZENT     0x497B //!< Addr: 0x497B, Size: 0x0008, Symbol Lamelle geschlossen (>90%) 
#define EE_PARAMETER_4980_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x4980 //!< Addr: 0x4980, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_497C_SYMBOL_FUER_90PROZENT     0x497C //!< Addr: 0x497C, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_4932_ANZAHL_DER_WERTE     0x4932 //!< Addr: 0x4932, Size: 0x0008, Anzahl der Werte 
#define EE_PARAMETER_495D_OM_INPUTUSAGE_VALUESWITCHMODE_9     0x495D //!< Addr: 0x495D, Size: 0x0001, OM_InputUsage_valueSwitchMode_9
#define EE_PARAMETER_4964_UMSCHALTUNG_BERUECKSICHTIGT_STATUSOBJEKT     0x4964 //!< Addr: 0x4964, Size: 0x0001, Umschaltung berücksichtigt Statusobjekt 
#define EE_PARAMETER_495F_FUNCTION_VALUETOICON_9     0x495F //!< Addr: 0x495F, Size: 0x0001, Function_ValueToIcon_9
#define EE_PARAMETER_4930_OM_INPUTUSAGE_MAINTYPEH_9     0x4930 //!< Addr: 0x4930, Size: 0x0008, OM_inputUsage_mainTypeH_9
#define EE_PARAMETER_4944_OM_INPUTUSAGE_VALUETYPE_00_9     0x4944 //!< Addr: 0x4944, Size: 0x0008, OM_InputUsage_valueType_00_9
#define EE_PARAMETER_495E_DARSTELLUNG_DER_FUNKTION_KURZ_LANG     0x495E //!< Addr: 0x495E, Size: 0x0001, Darstellung der Funktion kurz/lang 
#define EE_PARAMETER_491D_TASTENBESCHRIFTUNG_LINKE_TASTE     0x491D //!< Addr: 0x491D, Size: 0x0030, Tastenbeschriftung linke Taste 
#define EE_PARAMETER_4973_TASTENBESCHRIFTUNG_RECHTE_TASTE     0x4973 //!< Addr: 0x4973, Size: 0x0030, Tastenbeschriftung rechte Taste 
#define EE_PARAMETER_48EF_OM_INPUTUSAGE_VALUETYPE_01_8     0x48EF //!< Addr: 0x48EF, Size: 0x0008, OM_InputUsage_valueType_01_8
#define EE_PARAMETER_4987_OM_INPUTUSAGE_MAINTYPE_10     0x4987 //!< Addr: 0x4987, Size: 0x0008, OM_inputUsage_mainType_10
#define EE_PARAMETER_4989_OM_INPUTUSAGE_SUBTYPE_10     0x4989 //!< Addr: 0x4989, Size: 0x0008, OM_InputUsage_subType_10
#define EE_PARAMETER_49A7_OM_INPUTUSAGE_OBJTYPE_10     0x49A7 //!< Addr: 0x49A7, Size: 0x0008, OM_inputUsage_objType_10
#define EE_PARAMETER_49AC_OM_INPUTUSAGE_TIMERONACTIVE_10     0x49AC //!< Addr: 0x49AC, Size: 0x0001, OM_InputUsage_timerOnActive_10
#define EE_PARAMETER_49AD_OM_INPUTUSAGE_TIMEROFFACTIVE_10     0x49AD //!< Addr: 0x49AD, Size: 0x0001, OM_InputUsage_timerOffActive_10
#define EE_PARAMETER_49D3_FARBE_DES_SYMBOLS_FUER_0PROZENT     0x49D3 //!< Addr: 0x49D3, Size: 0x0008,     Farbe des Symbols für 0% 
#define EE_PARAMETER_49D4_FARBE_DES_SYMBOLS_FUER_0PROZENT_90PROZENT     0x49D4 //!< Addr: 0x49D4, Size: 0x0008,     Farbe des Symbols für 0% - 90% 
#define EE_PARAMETER_49D0_SYMBOL_FUER_0PROZENT_90PROZENT     0x49D0 //!< Addr: 0x49D0, Size: 0x0008,     Symbol für 0% - 90% 
#define EE_PARAMETER_49D5_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x49D5 //!< Addr: 0x49D5, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_49D1_SYMBOL_FUER_90PROZENT     0x49D1 //!< Addr: 0x49D1, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_49D6_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x49D6 //!< Addr: 0x49D6, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_49D2_SYMBOL_FUER_90PROZENT     0x49D2 //!< Addr: 0x49D2, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_4988_ANZAHL_DER_WERTE     0x4988 //!< Addr: 0x4988, Size: 0x0008, Anzahl der Werte 
#define EE_PARAMETER_49B3_UMSCHALTART     0x49B3 //!< Addr: 0x49B3, Size: 0x0001, Umschaltart 
#define EE_PARAMETER_49BA_UMSCHALTUNG_BERUECKSICHTIGT_STATUSOBJEKT     0x49BA //!< Addr: 0x49BA, Size: 0x0001, Umschaltung berücksichtigt Statusobjekt 
#define EE_PARAMETER_49B5_FUNCTION_VALUETOICON_10     0x49B5 //!< Addr: 0x49B5, Size: 0x0001, Function_ValueToIcon_10
#define EE_PARAMETER_4986_OM_INPUTUSAGE_MAINTYPEH_10     0x4986 //!< Addr: 0x4986, Size: 0x0008, OM_inputUsage_mainTypeH_10
#define EE_PARAMETER_499A_OM_INPUTUSAGE_VALUETYPE_00_10     0x499A //!< Addr: 0x499A, Size: 0x0008, OM_InputUsage_valueType_00_10
#define EE_PARAMETER_49B4_DARSTELLUNG_DER_FUNKTION_KURZ_LANG     0x49B4 //!< Addr: 0x49B4, Size: 0x0001, Darstellung der Funktion kurz/lang 
#define EE_PARAMETER_49DD_OM_INPUTUSAGE_MAINTYPE_11     0x49DD //!< Addr: 0x49DD, Size: 0x0008, OM_inputUsage_mainType_11
#define EE_PARAMETER_49DF_OM_INPUTUSAGE_SUBTYPE_11     0x49DF //!< Addr: 0x49DF, Size: 0x0008, OM_InputUsage_subType_11
#define EE_PARAMETER_49FD_OM_INPUTUSAGE_OBJTYPE_11     0x49FD //!< Addr: 0x49FD, Size: 0x0008, OM_inputUsage_objType_11
#define EE_PARAMETER_4A02_OM_INPUTUSAGE_TIMERCONFIGON_11     0x4A02 //!< Addr: 0x4A02, Size: 0x0001, OM_InputUsage_timerConfigOn_11
#define EE_PARAMETER_4A03_OM_INPUTUSAGE_TIMERCONFIGOFF_11     0x4A03 //!< Addr: 0x4A03, Size: 0x0001, OM_InputUsage_timerConfigOff_11
#define EE_PARAMETER_4A29_FARBE_DES_SYMBOLS_LAMELLE_OFFEN_55PROZENT     0x4A29 //!< Addr: 0x4A29, Size: 0x0008, Farbe des Symbols Lamelle offen (<55%) 
#define EE_PARAMETER_4A2A_FARBE_DES_SYMBOLS_LAMELLE_MITTIG_55PROZENT_90PROZENT     0x4A2A //!< Addr: 0x4A2A, Size: 0x0008, Farbe des Symbols Lamelle mittig (55% - 90%) 
#define EE_PARAMETER_4A26_SYMBOL_LAMELLE_MITTIG_55PROZENT_90PROZENT     0x4A26 //!< Addr: 0x4A26, Size: 0x0008, Symbol Lamelle mittig (55% - 90%) 
#define EE_PARAMETER_4A2B_FARBE_DES_SYMBOLS_LAMELLE_GESCHLOSSEN_90PROZENT     0x4A2B //!< Addr: 0x4A2B, Size: 0x0008, Farbe des Symbols Lamelle geschlossen (>90%) 
#define EE_PARAMETER_4A27_SYMBOL_LAMELLE_GESCHLOSSEN_90PROZENT     0x4A27 //!< Addr: 0x4A27, Size: 0x0008, Symbol Lamelle geschlossen (>90%) 
#define EE_PARAMETER_4A2C_FARBE_DES_SYMBOLS_FUER_90PROZENT     0x4A2C //!< Addr: 0x4A2C, Size: 0x0008,     Farbe des Symbols für > 90% 
#define EE_PARAMETER_4A28_SYMBOL_FUER_90PROZENT     0x4A28 //!< Addr: 0x4A28, Size: 0x0008,     Symbol für > 90% 
#define EE_PARAMETER_49DE_ANZAHL_DER_WERTE     0x49DE //!< Addr: 0x49DE, Size: 0x0008, Anzahl der Werte 
#define EE_PARAMETER_4A09_OM_INPUTUSAGE_VALUESWITCHMODE_11     0x4A09 //!< Addr: 0x4A09, Size: 0x0001, OM_InputUsage_valueSwitchMode_11
#define EE_PARAMETER_4A10_UMSCHALTUNG_BERUECKSICHTIGT_STATUSOBJEKT     0x4A10 //!< Addr: 0x4A10, Size: 0x0001, Umschaltung berücksichtigt Statusobjekt 
#define EE_PARAMETER_4A0B_FUNCTION_VALUETOICON_11     0x4A0B //!< Addr: 0x4A0B, Size: 0x0001, Function_ValueToIcon_11
#define EE_PARAMETER_49DC_OM_INPUTUSAGE_MAINTYPEH_11     0x49DC //!< Addr: 0x49DC, Size: 0x0008, OM_inputUsage_mainTypeH_11
#define EE_PARAMETER_49F0_OM_INPUTUSAGE_VALUETYPE_00_11     0x49F0 //!< Addr: 0x49F0, Size: 0x0008, OM_InputUsage_valueType_00_11
#define EE_PARAMETER_4A0A_DARSTELLUNG_DER_FUNKTION_KURZ_LANG     0x4A0A //!< Addr: 0x4A0A, Size: 0x0001, Darstellung der Funktion kurz/lang 
#define EE_PARAMETER_499B_OM_INPUTUSAGE_VALUETYPE_01_10     0x499B //!< Addr: 0x499B, Size: 0x0008, OM_InputUsage_valueType_01_10
#define EE_PARAMETER_49C9_TASTENBESCHRIFTUNG_LINKE_TASTE     0x49C9 //!< Addr: 0x49C9, Size: 0x0030, Tastenbeschriftung linke Taste 
#define EE_PARAMETER_4A1F_TASTENBESCHRIFTUNG_RECHTE_TASTE     0x4A1F //!< Addr: 0x4A1F, Size: 0x0030, Tastenbeschriftung rechte Taste 
#define EE_PARAMETER_4D03_IN_BETRIEB_ZYKLISCH_SENDEN     0x4D03 //!< Addr: 0x4D03, Size: 0x0008, "In Betrieb" zyklisch senden 
#define EE_PARAMETER_4C0D_TEMPERATURMESSUNG     0x4C0D //!< Addr: 0x4C0D, Size: 0x0001, Temperaturmessung 
#define EE_PARAMETER_4C07_SENSOR_INTERN_EXTERN     0x4C07 //!< Addr: 0x4C07, Size: 0x0008, Sensor intern/extern 
#define EE_PARAMETER_4C08_MESSWERT_ZYKLISCH_SENDEN     0x4C08 //!< Addr: 0x4C08, Size: 0x0008, Messwert zyklisch senden 
#define EE_PARAMETER_4C09_MESSWERT_SENDEN_BEI_AENDERUNG_VON     0x4C09 //!< Addr: 0x4C09, Size: 0x0008, Messwert senden bei Änderung von 
#define EE_PARAMETER_4C09_LC_READONINIT     0x4C09 //!< Addr: 0x4C09, Size: 0x0001, LC_readOnInit
#define EE_PARAMETER_4C0A_ABGLEICHWERT_FUER_INTERNE_TEMPERATUR_X0_1_K     0x4C0A //!< Addr: 0x4C0A, Size: 0x0008, Abgleichwert für interne Temperatur x0,1 K 
#define EE_PARAMETER_4C0B_TEMPERATUR_OBERER_MELDEWERT     0x4C0B //!< Addr: 0x4C0B, Size: 0x0008, Temperatur oberer Meldewert 
#define EE_PARAMETER_4C0C_TEMPERATUR_UNTERER_MELDEWERT     0x4C0C //!< Addr: 0x4C0C, Size: 0x0008, Temperatur unterer Meldewert 
#define EE_PARAMETER_465D_INTERNEN_TEMPERATURWERT_VERWENDEN     0x465D //!< Addr: 0x465D, Size: 0x0001, Internen Temperaturwert verwenden 
#define EE_PARAMETER_4709_INTERNEN_TEMPERATURWERT_VERWENDEN     0x4709 //!< Addr: 0x4709, Size: 0x0001, Internen Temperaturwert verwenden 
#define EE_PARAMETER_47B5_INTERNEN_TEMPERATURWERT_VERWENDEN     0x47B5 //!< Addr: 0x47B5, Size: 0x0001, Internen Temperaturwert verwenden 
#define EE_PARAMETER_4861_INTERNEN_TEMPERATURWERT_VERWENDEN     0x4861 //!< Addr: 0x4861, Size: 0x0001, Internen Temperaturwert verwenden 
#define EE_PARAMETER_490D_INTERNEN_TEMPERATURWERT_VERWENDEN     0x490D //!< Addr: 0x490D, Size: 0x0001, Internen Temperaturwert verwenden 
#define EE_PARAMETER_49B9_INTERNEN_TEMPERATURWERT_VERWENDEN     0x49B9 //!< Addr: 0x49B9, Size: 0x0001, Internen Temperaturwert verwenden 
#define EE_PARAMETER_465C_OM_INPUTUSAGE_VALUESWITCHMODESYMBOL_0     0x465C //!< Addr: 0x465C, Size: 0x0001, OM_InputUsage_valueSwitchModeSymbol_0
#define EE_PARAMETER_46B2_OM_INPUTUSAGE_VALUESWITCHMODESYMBOL_1     0x46B2 //!< Addr: 0x46B2, Size: 0x0001, OM_InputUsage_valueSwitchModeSymbol_1
#define EE_PARAMETER_4708_OM_INPUTUSAGE_VALUESWITCHMODESYMBOL_2     0x4708 //!< Addr: 0x4708, Size: 0x0001, OM_InputUsage_valueSwitchModeSymbol_2
#define EE_PARAMETER_475E_OM_INPUTUSAGE_VALUESWITCHMODESYMBOL_3     0x475E //!< Addr: 0x475E, Size: 0x0001, OM_InputUsage_valueSwitchModeSymbol_3
#define EE_PARAMETER_47B4_OM_INPUTUSAGE_VALUESWITCHMODESYMBOL_4     0x47B4 //!< Addr: 0x47B4, Size: 0x0001, OM_InputUsage_valueSwitchModeSymbol_4
#define EE_PARAMETER_480A_OM_INPUTUSAGE_VALUESWITCHMODESYMBOL_5     0x480A //!< Addr: 0x480A, Size: 0x0001, OM_InputUsage_valueSwitchModeSymbol_5
#define EE_PARAMETER_4860_OM_INPUTUSAGE_VALUESWITCHMODESYMBOL_6     0x4860 //!< Addr: 0x4860, Size: 0x0001, OM_InputUsage_valueSwitchModeSymbol_6
#define EE_PARAMETER_48B6_OM_INPUTUSAGE_VALUESWITCHMODESYMBOL_7     0x48B6 //!< Addr: 0x48B6, Size: 0x0001, OM_InputUsage_valueSwitchModeSymbol_7
#define EE_PARAMETER_490C_OM_INPUTUSAGE_VALUESWITCHMODESYMBOL_8     0x490C //!< Addr: 0x490C, Size: 0x0001, OM_InputUsage_valueSwitchModeSymbol_8
#define EE_PARAMETER_4962_OM_INPUTUSAGE_VALUESWITCHMODESYMBOL_9     0x4962 //!< Addr: 0x4962, Size: 0x0001, OM_InputUsage_valueSwitchModeSymbol_9
#define EE_PARAMETER_49B8_OM_INPUTUSAGE_VALUESWITCHMODESYMBOL_10     0x49B8 //!< Addr: 0x49B8, Size: 0x0001, OM_InputUsage_valueSwitchModeSymbol_10
#define EE_PARAMETER_4A0E_OM_INPUTUSAGE_VALUESWITCHMODESYMBOL_11     0x4A0E //!< Addr: 0x4A0E, Size: 0x0001, OM_InputUsage_valueSwitchModeSymbol_11
#define EE_PARAMETER_4D0B_AKTION_BEI_TASTENBETAETIGUNG_WENN_DISPLAY_AUSGESCHALTET_NUR_FUER_HARDWARE_AB_R1_5     0x4D0B //!< Addr: 0x4D0B, Size: 0x0001, Aktion bei Tastenbetätigung wenn Display ausgeschaltet (nur für Hardware ab R1.5) 
#define EE_PARAMETER_4D0A_AKTION_BEI_TASTENBETAETIGUNG_WENN_STANDBY_AKTIV_NUR_FUER_HARDWARE_AB_R1_5     0x4D0A //!< Addr: 0x4D0A, Size: 0x0001, Aktion bei Tastenbetätigung wenn Standby aktiv (nur für Hardware ab R1.5) 
#define EE_PARAMETER_4D09_RUECKNAME_DER_MELDUNG_UEBER_OBJEKT_NUR_FUER_HARDWARE_AB_R1_5     0x4D09 //!< Addr: 0x4D09, Size: 0x0001, Rückname der Meldung über Objekt (nur für Hardware ab R1.5) 
#define EE_PARAMETER_4D02_LC_DELAYTIME     0x4D02 //!< Addr: 0x4D02, Size: 0x0008, LC_DelayTime
#define EE_PARAMETER_4D05_2_EBENE_12_FUNKTIONEN     0x4D05 //!< Addr: 0x4D05, Size: 0x0001, 2. Ebene / 12 Funktionen 
#define EE_PARAMETER_4D07_PAGESWITCHBOTTOMBUTTONS     0x4D07 //!< Addr: 0x4D07, Size: 0x0001, PageSwitchBottomButtons
#define EE_PARAMETER_4D06_PAGESWITCHSCENEOBJ     0x4D06 //!< Addr: 0x4D06, Size: 0x0001, PageSwitchSceneObj
#define EE_PARAMETER_4C06_SZENEN_NUMMER_FUER_WECHSEL_ZU_EBENE_1     0x4C06 //!< Addr: 0x4C06, Size: 0x0007,         Szenen Nummer für Wechsel zu Ebene 1 
#define EE_PARAMETER_4C29_SZENEN_NUMMER_FUER_WECHSEL_ZU_EBENE_2     0x4C29 //!< Addr: 0x4C29, Size: 0x0007,         Szenen Nummer für Wechsel zu Ebene 2 
#define EE_PARAMETER_4C2A_SZENEN_NUMMER_FUER_WECHSEL_ZU_EBENE_3     0x4C2A //!< Addr: 0x4C2A, Size: 0x0007,         Szenen Nummer für Wechsel zu Ebene 3 
#define EE_PARAMETER_4D08_VERHALTEN_BEI_NAEHERUNG_PRAESENZ     0x4D08 //!< Addr: 0x4D08, Size: 0x0001,     Verhalten bei Näherung/Präsenz 
#define EE_PARAMETER_4D04_WECHSEL_ZUR_STANDBYEBENE_NACH_SEITENWECHSEL_UEBER_VERSTECKTE_TASTEN     0x4D04 //!< Addr: 0x4D04, Size: 0x0001,     Wechsel zur Standbyebene nach Seitenwechsel über versteckte Tasten 
#define EE_PARAMETER_4A33_OM_INPUTUSAGE_MAINTYPE_PANIC     0x4A33 //!< Addr: 0x4A33, Size: 0x0008, OM_inputUsage_mainType_Panic
#define EE_PARAMETER_4D0C_NAEHERUNGSSENSOR     0x4D0C //!< Addr: 0x4D0C, Size: 0x0001, Näherungssensor 


#define COMOBJ_0_TASTE_1_JALOUSIE_AUF_AB     0 //!< Com-Objekt number: 0, Description: Taste 1, Function: Jalousie Auf/Ab
#define COMOBJ_1_TASTE_1_STOP_LAMELLEN_AUF_ZU     1 //!< Com-Objekt number: 1, Description: Taste 1, Function: Stop/Lamellen Auf/Zu
#define COMOBJ_2_TASTE_1_WERT_FUER_UMSCHALTUNG     2 //!< Com-Objekt number: 2, Description: Taste 1, Function: Wert für Umschaltung
#define COMOBJ_3_TASTE_1_STATUS_FUER_ANZEIGE     3 //!< Com-Objekt number: 3, Description: Taste 1, Function: Status für Anzeige
#define COMOBJ_4_TASTE_1_SPERROBJEKT     4 //!< Com-Objekt number: 4, Description: Taste 1, Function: Sperrobjekt
#define COMOBJ_5_TASTE_2_SCHALTEN     5 //!< Com-Objekt number: 5, Description: Taste 2, Function: Schalten
#define COMOBJ_6_TASTE_2_LAMELLEN_STOP     6 //!< Com-Objekt number: 6, Description: Taste 2, Function: Lamellen/Stop
#define COMOBJ_7_TASTE_2_WERT_FUER_UMSCHALTUNG     7 //!< Com-Objekt number: 7, Description: Taste 2, Function: Wert für Umschaltung
#define COMOBJ_8_TASTE_2_STATUS_FUER_ANZEIGE     8 //!< Com-Objekt number: 8, Description: Taste 2, Function: Status für Anzeige
#define COMOBJ_9_TASTE_2_SPERROBJEKT     9 //!< Com-Objekt number: 9, Description: Taste 2, Function: Sperrobjekt
#define COMOBJ_10_TASTE_3_JALOUSIE_AUF_AB     10 //!< Com-Objekt number: 10, Description: Taste 3, Function: Jalousie Auf/Ab
#define COMOBJ_11_TASTE_3_STOP_LAMELLEN_AUF_ZU     11 //!< Com-Objekt number: 11, Description: Taste 3, Function: Stop/Lamellen Auf/Zu
#define COMOBJ_12_TASTE_3_WERT_FUER_UMSCHALTUNG     12 //!< Com-Objekt number: 12, Description: Taste 3, Function: Wert für Umschaltung
#define COMOBJ_13_TASTE_3_STATUS_FUER_ANZEIGE     13 //!< Com-Objekt number: 13, Description: Taste 3, Function: Status für Anzeige
#define COMOBJ_14_TASTE_3_SPERROBJEKT     14 //!< Com-Objekt number: 14, Description: Taste 3, Function: Sperrobjekt
#define COMOBJ_15_TASTE_4_SCHALTEN     15 //!< Com-Objekt number: 15, Description: Taste 4, Function: Schalten
#define COMOBJ_16_TASTE_4_LAMELLEN_STOP     16 //!< Com-Objekt number: 16, Description: Taste 4, Function: Lamellen/Stop
#define COMOBJ_17_TASTE_4_WERT_FUER_UMSCHALTUNG     17 //!< Com-Objekt number: 17, Description: Taste 4, Function: Wert für Umschaltung
#define COMOBJ_18_TASTE_4_STATUS_FUER_ANZEIGE     18 //!< Com-Objekt number: 18, Description: Taste 4, Function: Status für Anzeige
#define COMOBJ_19_TASTE_4_SPERROBJEKT     19 //!< Com-Objekt number: 19, Description: Taste 4, Function: Sperrobjekt
#define COMOBJ_20_TASTE_5_JALOUSIE_AUF_AB     20 //!< Com-Objekt number: 20, Description: Taste 5, Function: Jalousie Auf/Ab
#define COMOBJ_21_TASTE_5_STOP_LAMELLEN_AUF_ZU     21 //!< Com-Objekt number: 21, Description: Taste 5, Function: Stop/Lamellen Auf/Zu
#define COMOBJ_22_TASTE_5_WERT_FUER_UMSCHALTUNG     22 //!< Com-Objekt number: 22, Description: Taste 5, Function: Wert für Umschaltung
#define COMOBJ_23_TASTE_5_STATUS_FUER_ANZEIGE     23 //!< Com-Objekt number: 23, Description: Taste 5, Function: Status für Anzeige
#define COMOBJ_24_TASTE_5_SPERROBJEKT     24 //!< Com-Objekt number: 24, Description: Taste 5, Function: Sperrobjekt
#define COMOBJ_25_TASTE_6_SCHALTEN     25 //!< Com-Objekt number: 25, Description: Taste 6, Function: Schalten
#define COMOBJ_26_TASTE_6_LAMELLEN_STOP     26 //!< Com-Objekt number: 26, Description: Taste 6, Function: Lamellen/Stop
#define COMOBJ_27_TASTE_6_WERT_FUER_UMSCHALTUNG     27 //!< Com-Objekt number: 27, Description: Taste 6, Function: Wert für Umschaltung
#define COMOBJ_28_TASTE_6_STATUS_FUER_ANZEIGE     28 //!< Com-Objekt number: 28, Description: Taste 6, Function: Status für Anzeige
#define COMOBJ_29_TASTE_6_SPERROBJEKT     29 //!< Com-Objekt number: 29, Description: Taste 6, Function: Sperrobjekt
#define COMOBJ_30_TASTE_7_JALOUSIE_AUF_AB     30 //!< Com-Objekt number: 30, Description: Taste 7, Function: Jalousie Auf/Ab
#define COMOBJ_31_TASTE_7_STOP_LAMELLEN_AUF_ZU     31 //!< Com-Objekt number: 31, Description: Taste 7, Function: Stop/Lamellen Auf/Zu
#define COMOBJ_32_TASTE_7_WERT_FUER_UMSCHALTUNG     32 //!< Com-Objekt number: 32, Description: Taste 7, Function: Wert für Umschaltung
#define COMOBJ_33_TASTE_7_STATUS_FUER_ANZEIGE     33 //!< Com-Objekt number: 33, Description: Taste 7, Function: Status für Anzeige
#define COMOBJ_34_TASTE_7_SPERROBJEKT     34 //!< Com-Objekt number: 34, Description: Taste 7, Function: Sperrobjekt
#define COMOBJ_35_TASTE_8_SCHALTEN     35 //!< Com-Objekt number: 35, Description: Taste 8, Function: Schalten
#define COMOBJ_36_TASTE_8_LAMELLEN_STOP     36 //!< Com-Objekt number: 36, Description: Taste 8, Function: Lamellen/Stop
#define COMOBJ_37_TASTE_8_WERT_FUER_UMSCHALTUNG     37 //!< Com-Objekt number: 37, Description: Taste 8, Function: Wert für Umschaltung
#define COMOBJ_38_TASTE_8_STATUS_FUER_ANZEIGE     38 //!< Com-Objekt number: 38, Description: Taste 8, Function: Status für Anzeige
#define COMOBJ_39_TASTE_8_SPERROBJEKT     39 //!< Com-Objekt number: 39, Description: Taste 8, Function: Sperrobjekt
#define COMOBJ_40_TASTE_9_JALOUSIE_AUF_AB     40 //!< Com-Objekt number: 40, Description: Taste 9, Function: Jalousie Auf/Ab
#define COMOBJ_41_TASTE_9_STOP_LAMELLEN_AUF_ZU     41 //!< Com-Objekt number: 41, Description: Taste 9, Function: Stop/Lamellen Auf/Zu
#define COMOBJ_42_TASTE_9_WERT_FUER_UMSCHALTUNG     42 //!< Com-Objekt number: 42, Description: Taste 9, Function: Wert für Umschaltung
#define COMOBJ_43_TASTE_9_STATUS_FUER_ANZEIGE     43 //!< Com-Objekt number: 43, Description: Taste 9, Function: Status für Anzeige
#define COMOBJ_44_TASTE_9_SPERROBJEKT     44 //!< Com-Objekt number: 44, Description: Taste 9, Function: Sperrobjekt
#define COMOBJ_45_TASTE_10_SCHALTEN     45 //!< Com-Objekt number: 45, Description: Taste 10, Function: Schalten
#define COMOBJ_46_TASTE_10_LAMELLEN_STOP     46 //!< Com-Objekt number: 46, Description: Taste 10, Function: Lamellen/Stop
#define COMOBJ_47_TASTE_10_WERT_FUER_UMSCHALTUNG     47 //!< Com-Objekt number: 47, Description: Taste 10, Function: Wert für Umschaltung
#define COMOBJ_48_TASTE_10_STATUS_FUER_ANZEIGE     48 //!< Com-Objekt number: 48, Description: Taste 10, Function: Status für Anzeige
#define COMOBJ_49_TASTE_10_SPERROBJEKT     49 //!< Com-Objekt number: 49, Description: Taste 10, Function: Sperrobjekt
#define COMOBJ_50_TASTE_11_JALOUSIE_AUF_AB     50 //!< Com-Objekt number: 50, Description: Taste 11, Function: Jalousie Auf/Ab
#define COMOBJ_51_TASTE_11_STOP_LAMELLEN_AUF_ZU     51 //!< Com-Objekt number: 51, Description: Taste 11, Function: Stop/Lamellen Auf/Zu
#define COMOBJ_52_TASTE_11_WERT_FUER_UMSCHALTUNG     52 //!< Com-Objekt number: 52, Description: Taste 11, Function: Wert für Umschaltung
#define COMOBJ_53_TASTE_11_STATUS_FUER_ANZEIGE     53 //!< Com-Objekt number: 53, Description: Taste 11, Function: Status für Anzeige
#define COMOBJ_54_TASTE_11_SPERROBJEKT     54 //!< Com-Objekt number: 54, Description: Taste 11, Function: Sperrobjekt
#define COMOBJ_55_TASTE_12_SCHALTEN     55 //!< Com-Objekt number: 55, Description: Taste 12, Function: Schalten
#define COMOBJ_56_TASTE_12_LAMELLEN_STOP     56 //!< Com-Objekt number: 56, Description: Taste 12, Function: Lamellen/Stop
#define COMOBJ_57_TASTE_12_WERT_FUER_UMSCHALTUNG     57 //!< Com-Objekt number: 57, Description: Taste 12, Function: Wert für Umschaltung
#define COMOBJ_58_TASTE_12_STATUS_FUER_ANZEIGE     58 //!< Com-Objekt number: 58, Description: Taste 12, Function: Status für Anzeige
#define COMOBJ_59_TASTE_12_SPERROBJEKT     59 //!< Com-Objekt number: 59, Description: Taste 12, Function: Sperrobjekt
#define COMOBJ_60_PATSCH_TASTE_KURZ_SCHALTEN     60 //!< Com-Objekt number: 60, Description: Patsch-Taste kurz, Function: Schalten
#define COMOBJ_61_PATSCH_TASTE_KURZ_WERT_FUER_UMSCHALTUNG     61 //!< Com-Objekt number: 61, Description: Patsch-Taste kurz, Function: Wert für Umschaltung
#define COMOBJ_62_PATSCH_TASTE_LANG_SCHALTEN     62 //!< Com-Objekt number: 62, Description: Patsch-Taste lang, Function: Schalten
#define COMOBJ_63_PATSCH_TASTE_LANG_WERT_FUER_UMSCHALTUNG     63 //!< Com-Objekt number: 63, Description: Patsch-Taste lang, Function: Wert für Umschaltung
#define COMOBJ_64_PATSCH_TASTE_SPERROBJEKT     64 //!< Com-Objekt number: 64, Description: Patsch-Taste, Function: Sperrobjekt
#define COMOBJ_65_LOGIK_EINGANG_1_A     65 //!< Com-Objekt number: 65, Description: Logik, Function: Eingang 1 A
#define COMOBJ_66_LOGIK_EINGANG_1_B     66 //!< Com-Objekt number: 66, Description: Logik, Function: Eingang 1 B
#define COMOBJ_67_LOGIK_AUSGANG_1     67 //!< Com-Objekt number: 67, Description: Logik, Function: Ausgang 1
#define COMOBJ_68_LOGIK_EINGANG_2_A     68 //!< Com-Objekt number: 68, Description: Logik, Function: Eingang 2 A
#define COMOBJ_69_LOGIK_EINGANG_2_B     69 //!< Com-Objekt number: 69, Description: Logik, Function: Eingang 2 B
#define COMOBJ_70_LOGIK_AUSGANG_2     70 //!< Com-Objekt number: 70, Description: Logik, Function: Ausgang 2
#define COMOBJ_71_LOGIK_EINGANG_3_A     71 //!< Com-Objekt number: 71, Description: Logik, Function: Eingang 3 A
#define COMOBJ_72_LOGIK_EINGANG_3_B     72 //!< Com-Objekt number: 72, Description: Logik, Function: Eingang 3 B
#define COMOBJ_73_LOGIK_AUSGANG_3     73 //!< Com-Objekt number: 73, Description: Logik, Function: Ausgang 3
#define COMOBJ_74_LOGIK_EINGANG_4_A     74 //!< Com-Objekt number: 74, Description: Logik, Function: Eingang 4 A
#define COMOBJ_75_LOGIK_EINGANG_4_B     75 //!< Com-Objekt number: 75, Description: Logik, Function: Eingang 4 B
#define COMOBJ_76_LOGIK_AUSGANG_4     76 //!< Com-Objekt number: 76, Description: Logik, Function: Ausgang 4
#define COMOBJ_77_LED_1_SCHALTEN     77 //!< Com-Objekt number: 77, Description: LED 1, Function: Schalten
#define COMOBJ_78_LED_2_SCHALTEN     78 //!< Com-Objekt number: 78, Description: LED 2, Function: Schalten
#define COMOBJ_79_LED_3_SCHALTEN     79 //!< Com-Objekt number: 79, Description: LED 3, Function: Schalten
#define COMOBJ_80_LED_4_SCHALTEN     80 //!< Com-Objekt number: 80, Description: LED 4, Function: Schalten
#define COMOBJ_81_LED_5_SCHALTEN     81 //!< Com-Objekt number: 81, Description: LED 5, Function: Schalten
#define COMOBJ_82_LED_6_SCHALTEN     82 //!< Com-Objekt number: 82, Description: LED 6, Function: Schalten
#define COMOBJ_83_LED_7_SCHALTEN     83 //!< Com-Objekt number: 83, Description: LED 7, Function: Schalten
#define COMOBJ_84_LED_8_SCHALTEN     84 //!< Com-Objekt number: 84, Description: LED 8, Function: Schalten
#define COMOBJ_85_LED_9_SCHALTEN     85 //!< Com-Objekt number: 85, Description: LED 9, Function: Schalten
#define COMOBJ_86_LED_10_SCHALTEN     86 //!< Com-Objekt number: 86, Description: LED 10, Function: Schalten
#define COMOBJ_87_LED_11_SCHALTEN     87 //!< Com-Objekt number: 87, Description: LED 11, Function: Schalten
#define COMOBJ_88_LED_12_SCHALTEN     88 //!< Com-Objekt number: 88, Description: LED 12, Function: Schalten
#define COMOBJ_89_LED_A_SCHALTEN     89 //!< Com-Objekt number: 89, Description: LED A, Function: Schalten
#define COMOBJ_90_LED_B_SCHALTEN     90 //!< Com-Objekt number: 90, Description: LED B, Function: Schalten
#define COMOBJ_91_LED_1_PRIORITAET_SCHALTEN     91 //!< Com-Objekt number: 91, Description: LED 1 Priorität, Function: Schalten
#define COMOBJ_92_LED_2_PRIORITAET_SCHALTEN     92 //!< Com-Objekt number: 92, Description: LED 2 Priorität, Function: Schalten
#define COMOBJ_93_LED_3_PRIORITAET_SCHALTEN     93 //!< Com-Objekt number: 93, Description: LED 3 Priorität, Function: Schalten
#define COMOBJ_94_LED_4_PRIORITAET_SCHALTEN     94 //!< Com-Objekt number: 94, Description: LED 4 Priorität, Function: Schalten
#define COMOBJ_95_LED_5_PRIORITAET_SCHALTEN     95 //!< Com-Objekt number: 95, Description: LED 5 Priorität, Function: Schalten
#define COMOBJ_96_LED_6_PRIORITAET_SCHALTEN     96 //!< Com-Objekt number: 96, Description: LED 6 Priorität, Function: Schalten
#define COMOBJ_97_LED_7_PRIORITAET_SCHALTEN     97 //!< Com-Objekt number: 97, Description: LED 7 Priorität, Function: Schalten
#define COMOBJ_98_LED_8_PRIORITAET_SCHALTEN     98 //!< Com-Objekt number: 98, Description: LED 8 Priorität, Function: Schalten
#define COMOBJ_99_LED_9_PRIORITAET_SCHALTEN     99 //!< Com-Objekt number: 99, Description: LED 9 Priorität, Function: Schalten
#define COMOBJ_100_LED_10_PRIORITAET_SCHALTEN     100 //!< Com-Objekt number: 100, Description: LED 10 Priorität, Function: Schalten
#define COMOBJ_101_LED_11_PRIORITAET_SCHALTEN     101 //!< Com-Objekt number: 101, Description: LED 11 Priorität, Function: Schalten
#define COMOBJ_102_LED_12_PRIORITAET_SCHALTEN     102 //!< Com-Objekt number: 102, Description: LED 12 Priorität, Function: Schalten
#define COMOBJ_103_LED_A_PRIORITAET_SCHALTEN     103 //!< Com-Objekt number: 103, Description: LED A Priorität, Function: Schalten
#define COMOBJ_104_LED_B_PRIORITAET_SCHALTEN     104 //!< Com-Objekt number: 104, Description: LED B Priorität, Function: Schalten
#define COMOBJ_105_LED_SPERROBJEKT     105 //!< Com-Objekt number: 105, Description: LED, Function: Sperrobjekt
#define COMOBJ_106_TAG_NACHT_TAG_1_NACHT_0     106 //!< Com-Objekt number: 106, Description: Tag / Nacht, Function: Tag = 1 / Nacht = 0
#define COMOBJ_107_PRAESENZ_EINGANG     107 //!< Com-Objekt number: 107, Description: Präsenz, Function: Eingang
#define COMOBJ_108_TEMPERATURMESSWERT_AUSGANG     108 //!< Com-Objekt number: 108, Description: Temperaturmesswert, Function: Ausgang
#define COMOBJ_109_EXTERNER_TEMPERATURMESSWERT_EINGANG     109 //!< Com-Objekt number: 109, Description: Externer Temperaturmesswert, Function: Eingang
#define COMOBJ_110_MELDUNG_MAXIMALE_TEMPERATUR     110 //!< Com-Objekt number: 110, Description: Meldung, Function: Maximale Temperatur
#define COMOBJ_111_MELDUNG_MINIMALE_TEMPERATUR     111 //!< Com-Objekt number: 111, Description: Meldung, Function: Minimale Temperatur
#define COMOBJ_112_UHRZEIT_AKTUELLEN_WERT_EMPFANGEN     112 //!< Com-Objekt number: 112, Description: Uhrzeit, Function: Aktuellen Wert empfangen
#define COMOBJ_113_DATUM_AKTUELLEN_WERT_EMPFANGEN     113 //!< Com-Objekt number: 113, Description: Datum, Function: Aktuellen Wert empfangen
#define COMOBJ_114_UHRZEIT_DATUM_AKTUELLE_WERTE_EMPFANGEN     114 //!< Com-Objekt number: 114, Description: Uhrzeit/Datum, Function: Aktuelle Werte empfangen
#define COMOBJ_115_MELDUNG_1_HOECHSTE_PRIORITAET_EINGANG     115 //!< Com-Objekt number: 115, Description: Meldung 1 (höchste Priorität), Function: Eingang
#define COMOBJ_116_MELDUNG_2_EINGANG     116 //!< Com-Objekt number: 116, Description: Meldung 2, Function: Eingang
#define COMOBJ_117_MELDUNG_3_EINGANG     117 //!< Com-Objekt number: 117, Description: Meldung 3, Function: Eingang
#define COMOBJ_118_MELDUNG_4_EINGANG     118 //!< Com-Objekt number: 118, Description: Meldung 4, Function: Eingang
#define COMOBJ_119_MELDUNG_TEXT_NIEDRIGSTE_PRIORITAET_EINGANG     119 //!< Com-Objekt number: 119, Description: Meldung Text (niedrigste Priorität), Function: Eingang
#define COMOBJ_120_STATUSTEXT_1_EINGANG     120 //!< Com-Objekt number: 120, Description: Statustext 1, Function: Eingang
#define COMOBJ_121_STATUSTEXT_2_EINGANG     121 //!< Com-Objekt number: 121, Description: Statustext 2, Function: Eingang
#define COMOBJ_122_STATUSWERT_1_EINGANG     122 //!< Com-Objekt number: 122, Description: Statuswert 1, Function: Eingang
#define COMOBJ_123_STATUSWERT_2_EINGANG     123 //!< Com-Objekt number: 123, Description: Statuswert 2, Function: Eingang
#define COMOBJ_124_STATUSWERT_3_EINGANG     124 //!< Com-Objekt number: 124, Description: Statuswert 3, Function: Eingang
#define COMOBJ_125_IN_BETRIEB_AUSGANG     125 //!< Com-Objekt number: 125, Description: In Betrieb, Function: Ausgang
#define COMOBJ_126_TASTENBETAETIGUNG_AUSGANG     126 //!< Com-Objekt number: 126, Description: Tastenbetätigung, Function: Ausgang
#define COMOBJ_127_SZENE_SEITE_UMSCHALTEN     127 //!< Com-Objekt number: 127, Description: Szene, Function: Seite umschalten
#define COMOBJ_128_DISPLAY_HELLIGKEIT     128 //!< Com-Objekt number: 128, Description: Display, Function: Helligkeit
#define COMOBJ_129_LED_BLINKSTATUS     129 //!< Com-Objekt number: 129, Description: LED, Function: Blinkstatus
#define COMOBJ_130_DUMMY     130 //!< Com-Objekt number: 130, Description: Dummy, Function: 
#define COMOBJ_131_DUMMY     131 //!< Com-Objekt number: 131, Description: Dummy, Function: 
#define COMOBJ_132_DUMMY     132 //!< Com-Objekt number: 132, Description: Dummy, Function: 
#define COMOBJ_133_DUMMY     133 //!< Com-Objekt number: 133, Description: Dummy, Function: 
#define COMOBJ_134_DUMMY     134 //!< Com-Objekt number: 134, Description: Dummy, Function: 

#endif
