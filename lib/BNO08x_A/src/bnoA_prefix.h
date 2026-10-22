// bnoA_prefix.h
// Benennt die globalen Symbole des CEVA-Treibers (sh2, shtp) um,
// damit zwei Kopien der Bibliothek gleichzeitig verlinkt werden koennen.
#pragma once

// sh2.c: globale Variable und Funktionen
#define _sh2                       bnoA__sh2
#define sh2_open                   bnoA_sh2_open
#define sh2_close                  bnoA_sh2_close
#define sh2_service                bnoA_sh2_service
#define sh2_setSensorCallback      bnoA_sh2_setSensorCallback
#define sh2_getProdIds             bnoA_sh2_getProdIds
#define sh2_getSensorConfig        bnoA_sh2_getSensorConfig
#define sh2_setSensorConfig        bnoA_sh2_setSensorConfig
#define sh2_getMetadata            bnoA_sh2_getMetadata
#define sh2_getFrs                 bnoA_sh2_getFrs
#define sh2_setFrs                 bnoA_sh2_setFrs
#define sh2_getErrors              bnoA_sh2_getErrors
#define sh2_getCounts              bnoA_sh2_getCounts
#define sh2_clearCounts            bnoA_sh2_clearCounts
#define sh2_setTareNow             bnoA_sh2_setTareNow
#define sh2_clearTare              bnoA_sh2_clearTare
#define sh2_persistTare            bnoA_sh2_persistTare
#define sh2_setReorientation       bnoA_sh2_setReorientation
#define sh2_reinitialize           bnoA_sh2_reinitialize
#define sh2_saveDcdNow             bnoA_sh2_saveDcdNow
#define sh2_getOscType             bnoA_sh2_getOscType
#define sh2_setCalConfig           bnoA_sh2_setCalConfig
#define sh2_getCalConfig           bnoA_sh2_getCalConfig
#define sh2_setDcdAutoSave         bnoA_sh2_setDcdAutoSave
#define sh2_flush                  bnoA_sh2_flush
#define sh2_clearDcdAndReset       bnoA_sh2_clearDcdAndReset
#define sh2_startCal               bnoA_sh2_startCal
#define sh2_finishCal              bnoA_sh2_finishCal
#define sh2_setIZro                bnoA_sh2_setIZro
#define sh2_devReset               bnoA_sh2_devReset
#define sh2_devOn                  bnoA_sh2_devOn
#define sh2_devSleep               bnoA_sh2_devSleep

// sh2.c: Konstanten fuer die einzelnen Befehle
#define getProdIdOp                bnoA_getProdIdOp
#define getSensorConfigOp          bnoA_getSensorConfigOp
#define setSensorConfigOp          bnoA_setSensorConfigOp
#define getFrsOp                   bnoA_getFrsOp
#define setFrsOp                   bnoA_setFrsOp
#define getErrorsOp                bnoA_getErrorsOp
#define getCountsOp                bnoA_getCountsOp
#define reinitOp                   bnoA_reinitOp
#define saveDcdNowOp               bnoA_saveDcdNowOp
#define getOscTypeOp               bnoA_getOscTypeOp
#define setCalConfigOp             bnoA_setCalConfigOp
#define getCalConfigOp             bnoA_getCalConfigOp
#define forceFlushOp               bnoA_forceFlushOp
#define startCalOp                 bnoA_startCalOp
#define finishCalOp                bnoA_finishCalOp
#define sendCmdOp                  bnoA_sendCmdOp

// shtp.c
#define shtp_open                  bnoA_shtp_open
#define shtp_close                 bnoA_shtp_close
#define shtp_service               bnoA_shtp_service
#define shtp_send                  bnoA_shtp_send
#define shtp_listenAdvert          bnoA_shtp_listenAdvert
#define shtp_listenChan            bnoA_shtp_listenChan
#define shtp_chanNo                bnoA_shtp_chanNo
#define shtp_setEventCallback      bnoA_shtp_setEventCallback

// sh2_SensorValue.c
#define sh2_decodeSensorEvent      bnoA_sh2_decodeSensorEvent

// sh2_util.c
#define read8                      bnoA_read8
#define read16                     bnoA_read16
#define read32                     bnoA_read32
#define readu8                     bnoA_readu8
#define readu16                    bnoA_readu16
#define readu32                    bnoA_readu32
#define write8                     bnoA_write8
#define write16                    bnoA_write16
#define write32                    bnoA_write32
#define writeu8                    bnoA_writeu8
#define writeu16                   bnoA_writeu16
#define writeu32                   bnoA_writeu32
#define scaleRadToDeg              bnoA_scaleRadToDeg