// bnoC_prefix.h
// Benennt die globalen Symbole des CEVA-Treibers (sh2, shtp) um,
// damit zwei Kopien der Bibliothek gleichzeitig verlinkt werden koennen.
#pragma once

// sh2.c: globale Variable und Funktionen
#define _sh2                       bnoC__sh2
#define sh2_open                   bnoC_sh2_open
#define sh2_close                  bnoC_sh2_close
#define sh2_service                bnoC_sh2_service
#define sh2_setSensorCallback      bnoC_sh2_setSensorCallback
#define sh2_getProdIds             bnoC_sh2_getProdIds
#define sh2_getSensorConfig        bnoC_sh2_getSensorConfig
#define sh2_setSensorConfig        bnoC_sh2_setSensorConfig
#define sh2_getMetadata            bnoC_sh2_getMetadata
#define sh2_getFrs                 bnoC_sh2_getFrs
#define sh2_setFrs                 bnoC_sh2_setFrs
#define sh2_getErrors              bnoC_sh2_getErrors
#define sh2_getCounts              bnoC_sh2_getCounts
#define sh2_clearCounts            bnoC_sh2_clearCounts
#define sh2_setTareNow             bnoC_sh2_setTareNow
#define sh2_clearTare              bnoC_sh2_clearTare
#define sh2_persistTare            bnoC_sh2_persistTare
#define sh2_setReorientation       bnoC_sh2_setReorientation
#define sh2_reinitialize           bnoC_sh2_reinitialize
#define sh2_saveDcdNow             bnoC_sh2_saveDcdNow
#define sh2_getOscType             bnoC_sh2_getOscType
#define sh2_setCalConfig           bnoC_sh2_setCalConfig
#define sh2_getCalConfig           bnoC_sh2_getCalConfig
#define sh2_setDcdAutoSave         bnoC_sh2_setDcdAutoSave
#define sh2_flush                  bnoC_sh2_flush
#define sh2_clearDcdAndReset       bnoC_sh2_clearDcdAndReset
#define sh2_startCal               bnoC_sh2_startCal
#define sh2_finishCal              bnoC_sh2_finishCal
#define sh2_setIZro                bnoC_sh2_setIZro
#define sh2_devReset               bnoC_sh2_devReset
#define sh2_devOn                  bnoC_sh2_devOn
#define sh2_devSleep               bnoC_sh2_devSleep

// sh2.c: Konstanten fuer die einzelnen Befehle
#define getProdIdOp                bnoC_getProdIdOp
#define getSensorConfigOp          bnoC_getSensorConfigOp
#define setSensorConfigOp          bnoC_setSensorConfigOp
#define getFrsOp                   bnoC_getFrsOp
#define setFrsOp                   bnoC_setFrsOp
#define getErrorsOp                bnoC_getErrorsOp
#define getCountsOp                bnoC_getCountsOp
#define reinitOp                   bnoC_reinitOp
#define saveDcdNowOp               bnoC_saveDcdNowOp
#define getOscTypeOp               bnoC_getOscTypeOp
#define setCalConfigOp             bnoC_setCalConfigOp
#define getCalConfigOp             bnoC_getCalConfigOp
#define forceFlushOp               bnoC_forceFlushOp
#define startCalOp                 bnoC_startCalOp
#define finishCalOp                bnoC_finishCalOp
#define sendCmdOp                  bnoC_sendCmdOp

// shtp.c
#define shtp_open                  bnoC_shtp_open
#define shtp_close                 bnoC_shtp_close
#define shtp_service               bnoC_shtp_service
#define shtp_send                  bnoC_shtp_send
#define shtp_listenAdvert          bnoC_shtp_listenAdvert
#define shtp_listenChan            bnoC_shtp_listenChan
#define shtp_chanNo                bnoC_shtp_chanNo
#define shtp_setEventCallback      bnoC_shtp_setEventCallback

// sh2_SensorValue.c
#define sh2_decodeSensorEvent      bnoC_sh2_decodeSensorEvent

// sh2_util.c
#define read8                      bnoC_read8
#define read16                     bnoC_read16
#define read32                     bnoC_read32
#define readu8                     bnoC_readu8
#define readu16                    bnoC_readu16
#define readu32                    bnoC_readu32
#define write8                     bnoC_write8
#define write16                    bnoC_write16
#define write32                    bnoC_write32
#define writeu8                    bnoC_writeu8
#define writeu16                   bnoC_writeu16
#define writeu32                   bnoC_writeu32
#define scaleRadToDeg              bnoC_scaleRadToDeg