// bnoB_prefix.h
// Benennt die globalen Symbole des CEVA-Treibers (sh2, shtp) um,
// damit zwei Kopien der Bibliothek gleichzeitig verlinkt werden koennen.
#pragma once

// sh2.c: globale Variable und Funktionen
#define _sh2                       bnoB__sh2
#define sh2_open                   bnoB_sh2_open
#define sh2_close                  bnoB_sh2_close
#define sh2_service                bnoB_sh2_service
#define sh2_setSensorCallback      bnoB_sh2_setSensorCallback
#define sh2_getProdIds             bnoB_sh2_getProdIds
#define sh2_getSensorConfig        bnoB_sh2_getSensorConfig
#define sh2_setSensorConfig        bnoB_sh2_setSensorConfig
#define sh2_getMetadata            bnoB_sh2_getMetadata
#define sh2_getFrs                 bnoB_sh2_getFrs
#define sh2_setFrs                 bnoB_sh2_setFrs
#define sh2_getErrors              bnoB_sh2_getErrors
#define sh2_getCounts              bnoB_sh2_getCounts
#define sh2_clearCounts            bnoB_sh2_clearCounts
#define sh2_setTareNow             bnoB_sh2_setTareNow
#define sh2_clearTare              bnoB_sh2_clearTare
#define sh2_persistTare            bnoB_sh2_persistTare
#define sh2_setReorientation       bnoB_sh2_setReorientation
#define sh2_reinitialize           bnoB_sh2_reinitialize
#define sh2_saveDcdNow             bnoB_sh2_saveDcdNow
#define sh2_getOscType             bnoB_sh2_getOscType
#define sh2_setCalConfig           bnoB_sh2_setCalConfig
#define sh2_getCalConfig           bnoB_sh2_getCalConfig
#define sh2_setDcdAutoSave         bnoB_sh2_setDcdAutoSave
#define sh2_flush                  bnoB_sh2_flush
#define sh2_clearDcdAndReset       bnoB_sh2_clearDcdAndReset
#define sh2_startCal               bnoB_sh2_startCal
#define sh2_finishCal              bnoB_sh2_finishCal
#define sh2_setIZro                bnoB_sh2_setIZro
#define sh2_devReset               bnoB_sh2_devReset
#define sh2_devOn                  bnoB_sh2_devOn
#define sh2_devSleep               bnoB_sh2_devSleep

// sh2.c: Konstanten fuer die einzelnen Befehle
#define getProdIdOp                bnoB_getProdIdOp
#define getSensorConfigOp          bnoB_getSensorConfigOp
#define setSensorConfigOp          bnoB_setSensorConfigOp
#define getFrsOp                   bnoB_getFrsOp
#define setFrsOp                   bnoB_setFrsOp
#define getErrorsOp                bnoB_getErrorsOp
#define getCountsOp                bnoB_getCountsOp
#define reinitOp                   bnoB_reinitOp
#define saveDcdNowOp               bnoB_saveDcdNowOp
#define getOscTypeOp               bnoB_getOscTypeOp
#define setCalConfigOp             bnoB_setCalConfigOp
#define getCalConfigOp             bnoB_getCalConfigOp
#define forceFlushOp               bnoB_forceFlushOp
#define startCalOp                 bnoB_startCalOp
#define finishCalOp                bnoB_finishCalOp
#define sendCmdOp                  bnoB_sendCmdOp

// shtp.c
#define shtp_open                  bnoB_shtp_open
#define shtp_close                 bnoB_shtp_close
#define shtp_service               bnoB_shtp_service
#define shtp_send                  bnoB_shtp_send
#define shtp_listenAdvert          bnoB_shtp_listenAdvert
#define shtp_listenChan            bnoB_shtp_listenChan
#define shtp_chanNo                bnoB_shtp_chanNo
#define shtp_setEventCallback      bnoB_shtp_setEventCallback

// sh2_SensorValue.c
#define sh2_decodeSensorEvent      bnoB_sh2_decodeSensorEvent

// sh2_util.c
#define read8                      bnoB_read8
#define read16                     bnoB_read16
#define read32                     bnoB_read32
#define readu8                     bnoB_readu8
#define readu16                    bnoB_readu16
#define readu32                    bnoB_readu32
#define write8                     bnoB_write8
#define write16                    bnoB_write16
#define write32                    bnoB_write32
#define writeu8                    bnoB_writeu8
#define writeu16                   bnoB_writeu16
#define writeu32                   bnoB_writeu32
#define scaleRadToDeg              bnoB_scaleRadToDeg