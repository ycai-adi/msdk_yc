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

#include "dual_main.h"
/* LESC OOB configuration */
static dmSecLescOobCfg_t *datcOobCfg;
/* OOB Connection identifier */
dmConnId_t oobConnId;
/*************************************************************************************************/
/*!
 *  \brief  OOB RX callback.
 *
 *  \return None.
 */
/*************************************************************************************************/
// TODO: Need to think how oob pairing works in dual mode
void oobRxCback(void)
{
    if (datcOobCfg != NULL) {
        DmSecSetOob(oobConnId, datcOobCfg);
    }

    DmSecAuthRsp(oobConnId, 0, NULL);
}