/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Data transmitter sample application.
 *
 *  Copyright (c) 2012-2019 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019-2020 Packetcraft, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/

#include <string.h>
#include "wsf_types.h"
#include "util/bstream.h"
#include "hci_api.h"
#include "util/calc128.h"
#include "gatt/gatt_api.h"
#include "dats_api.h"
#include "wut.h"
#include "trimsir_regs.h"
#include "pal_btn.h"
#include "pal_uart.h"
#include "tmr.h"
#include "wsf_msg.h"
#include "wsf_trace.h"
#include "wsf_buf.h"
#include "wsf_nvm.h"
#include "wsf_timer.h"
#include "wsf_assert.h"
#include "hci_api.h"
#include "dm_api.h"
#include "dm_priv.h"
#include "gap/gap_api.h"
#include "att_api.h"
#include "smp_api.h"
#include "app_api.h"
#include "app_main.h"
#include "app_db.h"
#include "app_ui.h"
#include "app_cfg.h"
#include "svc_core.h"
#include "wpc/wpc_api.h"
#include "datc_api.h"
#include "sdsc_api.h"
#include "svc_sds.h"
#include "svc_ch.h"
#include "sec_api.h"
#include "svc_wp.h"

#include "datc_api.h"
#include "dats_api.h"
#include "dual_main.h"


/* LESC OOB configuration */
static dmSecLescOobCfg_t *datcOobCfg;

extern void setAdvTxPower(void);

/* OOB Connection identifier */
dmConnId_t oobConnId;
bool_t isClient = TRUE;
extern datcCb_t datcCb;
extern datsCb_t datsCb;

/* Timer for trimming of the 32 kHz crystal */
wsfTimer_t trimTimer;


/*************************************************************************************************/
/*!
 *  \brief  OOB RX callback.
 *
 *  \return None.
 */
/*************************************************************************************************/
// TODO: Need to consider how oob pairing works in dual mode
void oobRxCback(void)
{
    if (datcOobCfg != NULL) {
        DmSecSetOob(oobConnId, datcOobCfg);
    }

    DmSecAuthRsp(oobConnId, 0, NULL);
}

static void dualDmCback(dmEvt_t *pDmEvt, bool isClient)
{
    dmEvt_t *pMsg;
    uint16_t len;
    uint16_t reportLen;

    if (pDmEvt->hdr.event == DM_SEC_ECC_KEY_IND) {
        DmSecSetEccKey(&pDmEvt->eccMsg.data.key);
        APP_TRQACE_INFO0("DM_SEC_ECC_KEY_IND");
        /* If the local device sends OOB data. */
       // if (isClient ? datcSendOobData : datsSendOobData) {
            // uint8_t oobLocalRandom[SMP_RAND_LEN];
            // SecRand(oobLocalRandom, SMP_RAND_LEN);
            // DmSecCalcOobReq(oobLocalRandom, pDmEvt->eccMsg.data.key.pubKey_x);

            // /* Setup HCI UART for OOB */
            // PalUartConfig_t hciUartCfg;
            // hciUartCfg.rdCback = oobRxCback;
            // hciUartCfg.wrCback = NULL;
            // hciUartCfg.baud = OOB_BAUD;
            // hciUartCfg.hwFlow = OOB_FLOW;

            // PalUartInit(PAL_UART_ID_CHCI, &hciUartCfg);
      //  }
    } else if (pDmEvt->hdr.event == DM_SEC_CALC_OOB_IND) {
        APP_TRQACE_INFO0("DM_SEC_CALC_OOB_IND");
        // TODO: OOB
        // this needs to happen in dual_main.c
       
    } else {
        len = DmSizeOfEvt(pDmEvt);

        if (isClient && pDmEvt->hdr.event == DM_SCAN_REPORT_IND) {
            reportLen = pDmEvt->scanReport.len;
        } else {
            reportLen = 0;
        }

        if ((pMsg = WsfMsgAlloc(len + reportLen)) != NULL) {
            memcpy(pMsg, pDmEvt, len);
            if (isClient && pDmEvt->hdr.event == DM_SCAN_REPORT_IND) {
                pMsg->scanReport.pData = (uint8_t *)((uint8_t *)pMsg + len);
                memcpy(pMsg->scanReport.pData, pDmEvt->scanReport.pData, reportLen);
            }
            WsfMsgSend(isClient ? datcCb.handlerId : datsCb.handlerId, pMsg);
        }
    }
}

static void dmCback(dmEvt_t *pDmEvt)
{
    //send event to propper handler
    if(isClient)
    {
        datcDmCback(pDmEvt);
    }
    else{
        datsDmCback(pDmEvt);
    }
}

static void attCback(attEvt_t *pEvt)
{
    if(isClient)
    {
        datcAttCback(pEvt);
    }
    else{
        datsAttCback(pEvt);
    }
}

void ServerClientStart(void)
{
    // Register common
    DmRegister((dmCback));
    AttRegister(attCback);
    DmConnRegister(DM_CLIENT_ID_APP, dmCback);
    // Start server
    DatsStart();


    // Start client
    DatcStart(); 

}