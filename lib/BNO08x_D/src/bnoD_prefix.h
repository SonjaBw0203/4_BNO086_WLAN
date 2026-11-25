// bnoD_prefix.h
// Benennt die globalen Symbole des CEVA-Treibers (sh2, shtp) um,
// damit zwei Kopien der Bibliothek gleichzeitig verlinkt werden koennen.
#pragma once

// sh2.c: globale Variable und Funktionen
#define _sh2                       bnoD__sh2
#define sh2_open                   bnoD_sh2_open
#define sh2_close                  bnoD_sh2_close
#define sh2_service                bnoD_sh2_service
#define sh2_setSensorCallback      bnoD_sh2_setSensorCallback
#define sh2_getProdIds             bnoD_sh2_getProdIds
#define sh2_getSensorConfig        bnoD_sh2_getSensorConfig
#define sh2_setSensorConfig        bnoD_sh2_setSensorConfig
#define sh2_getMetadata            bnoD_sh2_getMetadata
#define sh2_getFrs                 bnoD_sh2_getFrs
#define sh2_setFrs                 bnoD_sh2_setFrs
#define sh2_getErrors              bnoD_sh2_getErrors
#define sh2_getCounts              bnoD_sh2_getCounts
#define sh2_clearCounts            bnoD_sh2_clearCounts
#define sh2_setTareNow             bnoD_sh2_setTareNow
#define sh2_clearTare              bnoD_sh2_clearTare
#define sh2_persistTare            bnoD_sh2_persistTare
#define sh2_setReorientation       bnoD_sh2_setReorientation
#define sh2_reinitialize           bnoD_sh2_reinitialize
#define sh2_saveDcdNow             bnoD_sh2_saveDcdNow
#define sh2_getOscType             bnoD_sh2_getOscType
#define sh2_setCalConfig           bnoD_sh2_setCalConfig
#define sh2_getCalConfig           bnoD_sh2_getCalConfig
#define sh2_setDcdAutoSave         bnoD_sh2_setDcdAutoSave
#define sh2_flush                  bnoD_sh2_flush
#define sh2_clearDcdAndReset       bnoD_sh2_clearDcdAndReset
#define sh2_startCal               bnoD_sh2_startCal
#define sh2_finishCal              bnoD_sh2_finishCal
#define sh2_setIZro                bnoD_sh2_setIZro
#define sh2_devReset               bnoD_sh2_devReset
#define sh2_devOn                  bnoD_sh2_devOn
#define sh2_devSleep               bnoD_sh2_devSleep

// sh2.c: Konstanten fuer die einzelnen Befehle
#define getProdIdOp                bnoD_getProdIdOp
#define getSensorConfigOp          bnoD_getSensorConfigOp
#define setSensorConfigOp          bnoD_setSensorConfigOp
#define getFrsOp                   bnoD_getFrsOp
#define setFrsOp                   bnoD_setFrsOp
#define getErrorsOp                bnoD_getErrorsOp
#define getCountsOp                bnoD_getCountsOp
#define reinitOp                   bnoD_reinitOp
#define saveDcdNowOp               bnoD_saveDcdNowOp
#define getOscTypeOp               bnoD_getOscTypeOp
#define setCalConfigOp             bnoD_setCalConfigOp
#define getCalConfigOp             bnoD_getCalConfigOp
#define forceFlushOp               bnoD_forceFlushOp
#define startCalOp                 bnoD_startCalOp
#define finishCalOp                bnoD_finishCalOp
#define sendCmdOp                  bnoD_sendCmdOp

// shtp.c
#define shtp_open                  bnoD_shtp_open
#define shtp_close                 bnoD_shtp_close
#define shtp_service               bnoD_shtp_service
#define shtp_send                  bnoD_shtp_send
#define shtp_listenAdvert          bnoD_shtp_listenAdvert
#define shtp_listenChan            bnoD_shtp_listenChan
#define shtp_chanNo                bnoD_shtp_chanNo
#define shtp_setEventCallback      bnoD_shtp_setEventCallback

// sh2_SensorValue.c
#define sh2_decodeSensorEvent      bnoD_sh2_decodeSensorEvent

// sh2_util.c
#define read8                      bnoD_read8
#define read16                     bnoD_read16
#define read32                     bnoD_read32
#define readu8                     bnoD_readu8
#define readu16                    bnoD_readu16
#define readu32                    bnoD_readu32
#define write8                     bnoD_write8
#define write16                    bnoD_write16
#define write32                    bnoD_write32
#define writeu8                    bnoD_writeu8
#define writeu16                   bnoD_writeu16
#define writeu32                   bnoD_writeu32
#define scaleRadToDeg              bnoD_scaleRadToDeg