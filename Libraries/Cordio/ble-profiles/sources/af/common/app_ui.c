/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Application framework user interface.
 *
 *  Copyright (c) 2011-2019 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019 Packetcraft, Inc.
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

#include "wsf_types.h"
#include "wsf_os.h"
#include "wsf_trace.h"
#include "app_ui.h"
#include "dm_api.h"
#include "att_api.h"
/**************************************************************************************************
  Local Variables
**************************************************************************************************/

/*! \brief Callback struct */
static appUiCback_t appUiCbackTbl;

/*************************************************************************************************/
/*!
 *  \brief  Perform a user interface action based on the event value passed to the function.
 *
 *  \param  event   User interface event value.
 *
 *  \return None.
 */
/*************************************************************************************************/
#include <stdint.h>

void AppUiAction(uint8_t event){
    switch (event) {
        // ATT client callback events
        case ATTC_FIND_INFO_RSP:
            APP_TRACE_INFO0(">>> App Event: ATTC_FIND_INFO_RSP <<<");
            break;
        case ATTC_FIND_BY_TYPE_VALUE_RSP:
            APP_TRACE_INFO0(">>> App Event: ATTC_FIND_BY_TYPE_VALUE_RSP <<<");
            break;
        case ATTC_READ_BY_TYPE_RSP:
            APP_TRACE_INFO0(">>> App Event: ATTC_READ_BY_TYPE_RSP <<<");
            break;
        case ATTC_READ_RSP:
            APP_TRACE_INFO0(">>> App Event: ATTC_READ_RSP <<<");
            break;
        case ATTC_READ_LONG_RSP:
            APP_TRACE_INFO0(">>> App Event: ATTC_READ_LONG_RSP <<<");
            break;
        case ATTC_READ_MULTIPLE_RSP:
            APP_TRACE_INFO0(">>> App Event: ATTC_READ_MULTIPLE_RSP <<<");
            break;
        case ATTC_READ_BY_GROUP_TYPE_RSP:
            APP_TRACE_INFO0(">>> App Event: ATTC_READ_BY_GROUP_TYPE_RSP <<<");
            break;
        case ATTC_WRITE_RSP:
            APP_TRACE_INFO0(">>> App Event: ATTC_WRITE_RSP <<<");
            break;
        case ATTC_WRITE_CMD_RSP:
            APP_TRACE_INFO0(">>> App Event: ATTC_WRITE_CMD_RSP <<<");
            break;
        case ATTC_PREPARE_WRITE_RSP:
            APP_TRACE_INFO0(">>> App Event: ATTC_PREPARE_WRITE_RSP <<<");
            break;
        case ATTC_EXECUTE_WRITE_RSP:
            APP_TRACE_INFO0(">>> App Event: ATTC_EXECUTE_WRITE_RSP <<<");
            break;
        case ATTC_HANDLE_VALUE_NTF:
            APP_TRACE_INFO0(">>> App Event: ATTC_HANDLE_VALUE_NTF <<<");
            break;
        case ATTC_HANDLE_VALUE_IND:
            APP_TRACE_INFO0(">>> App Event: ATTC_HANDLE_VALUE_IND <<<");
            break;
        case ATTC_READ_MULT_VAR_RSP:
            APP_TRACE_INFO0(">>> App Event: ATTC_READ_MULT_VAR_RSP <<<");
            break;
        case ATTC_MULT_VALUE_NTF:
            APP_TRACE_INFO0(">>> App Event: ATTC_MULT_VALUE_NTF <<<");
            break;

        // ATT server callback events
        case ATTS_HANDLE_VALUE_CNF:
            APP_TRACE_INFO0(">>> App Event: ATTS_HANDLE_VALUE_CNF <<<");
            break;
        case ATTS_MULT_VALUE_CNF:
            APP_TRACE_INFO0(">>> App Event: ATTS_MULT_VALUE_CNF <<<");
            break;
        case ATTS_CCC_STATE_IND:
            APP_TRACE_INFO0(">>> App Event: ATTS_CCC_STATE_IND <<<");
            break;
        case ATTS_DB_HASH_CALC_CMPL_IND:
            APP_TRACE_INFO0(">>> App Event: ATTS_DB_HASH_CALC_CMPL_IND <<<");
            break;

        // ATT common callback events
        case ATT_MTU_UPDATE_IND:
            APP_TRACE_INFO0(">>> App Event: ATT_MTU_UPDATE_IND <<<");
            break;
        case ATT_EATT_CONN_CMPL_IND:
            APP_TRACE_INFO0(">>> App Event: ATT_EATT_CONN_CMPL_IND <<<");
            break;
        case ATT_EATT_RECONFIG_CMPL_IND:
            APP_TRACE_INFO0(">>> App Event: ATT_EATT_RECONFIG_CMPL_IND <<<");
            break;

        // DM callback events
        case DM_RESET_CMPL_IND:
            APP_TRACE_INFO0(">>> App Event: DM_RESET_CMPL_IND <<<");
            break;
        case DM_ADV_START_IND:
            APP_TRACE_INFO0(">>> App Event: DM_ADV_START_IND <<<");
            break;
        case DM_ADV_STOP_IND:
            APP_TRACE_INFO0(">>> App Event: DM_ADV_STOP_IND <<<");
            break;
        case DM_ADV_NEW_ADDR_IND:
            APP_TRACE_INFO0(">>> App Event: DM_ADV_NEW_ADDR_IND <<<");
            break;
        case DM_SCAN_START_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SCAN_START_IND <<<");
            break;
        case DM_SCAN_STOP_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SCAN_STOP_IND <<<");
            break;
        case DM_SCAN_REPORT_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SCAN_REPORT_IND <<<");
            break;
        case DM_CONN_OPEN_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_OPEN_IND <<<");
            break;
        case DM_CONN_CLOSE_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_CLOSE_IND <<<");
            break;
        case DM_CONN_UPDATE_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_UPDATE_IND <<<");
            break;
        case DM_SEC_PAIR_CMPL_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SEC_PAIR_CMPL_IND <<<");
            break;
        case DM_SEC_PAIR_FAIL_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SEC_PAIR_FAIL_IND <<<");
            break;
        case DM_SEC_ENCRYPT_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SEC_ENCRYPT_IND <<<");
            break;
        case DM_SEC_ENCRYPT_FAIL_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SEC_ENCRYPT_FAIL_IND <<<");
            break;
        case DM_SEC_AUTH_REQ_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SEC_AUTH_REQ_IND <<<");
            break;
        case DM_SEC_KEY_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SEC_KEY_IND <<<");
            break;
        case DM_SEC_LTK_REQ_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SEC_LTK_REQ_IND <<<");
            break;
        case DM_SEC_PAIR_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SEC_PAIR_IND <<<");
            break;
        case DM_SEC_SLAVE_REQ_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SEC_SLAVE_REQ_IND <<<");
            break;
        case DM_SEC_CALC_OOB_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SEC_CALC_OOB_IND <<<");
            break;
        case DM_SEC_ECC_KEY_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SEC_ECC_KEY_IND <<<");
            break;
        case DM_SEC_COMPARE_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SEC_COMPARE_IND <<<");
            break;
        case DM_SEC_KEYPRESS_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SEC_KEYPRESS_IND <<<");
            break;
        case DM_PRIV_RESOLVED_ADDR_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PRIV_RESOLVED_ADDR_IND <<<");
            break;
        case DM_PRIV_GENERATE_ADDR_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PRIV_GENERATE_ADDR_IND <<<");
            break;
        case DM_CONN_READ_RSSI_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_READ_RSSI_IND <<<");
            break;
        case DM_PRIV_ADD_DEV_TO_RES_LIST_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PRIV_ADD_DEV_TO_RES_LIST_IND <<<");
            break;
        case DM_PRIV_REM_DEV_FROM_RES_LIST_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PRIV_REM_DEV_FROM_RES_LIST_IND <<<");
            break;
        case DM_PRIV_CLEAR_RES_LIST_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PRIV_CLEAR_RES_LIST_IND <<<");
            break;
        case DM_PRIV_READ_PEER_RES_ADDR_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PRIV_READ_PEER_RES_ADDR_IND <<<");
            break;
        case DM_PRIV_READ_LOCAL_RES_ADDR_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PRIV_READ_LOCAL_RES_ADDR_IND <<<");
            break;
        case DM_PRIV_SET_ADDR_RES_ENABLE_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PRIV_SET_ADDR_RES_ENABLE_IND <<<");
            break;
        case DM_REM_CONN_PARAM_REQ_IND:
            APP_TRACE_INFO0(">>> App Event: DM_REM_CONN_PARAM_REQ_IND <<<");
            break;
        case DM_CONN_DATA_LEN_CHANGE_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_DATA_LEN_CHANGE_IND <<<");
            break;
        case DM_CONN_WRITE_AUTH_TO_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_WRITE_AUTH_TO_IND <<<");
            break;
        case DM_CONN_AUTH_TO_EXPIRED_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_AUTH_TO_EXPIRED_IND <<<");
            break;
        case DM_PHY_READ_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PHY_READ_IND <<<");
            break;
        case DM_PHY_SET_DEF_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PHY_SET_DEF_IND <<<");
            break;
        case DM_PHY_UPDATE_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PHY_UPDATE_IND <<<");
            break;
        case DM_ADV_SET_START_IND:
            APP_TRACE_INFO0(">>> App Event: DM_ADV_SET_START_IND <<<");
            break;
        case DM_ADV_SET_STOP_IND:
            APP_TRACE_INFO0(">>> App Event: DM_ADV_SET_STOP_IND <<<");
            break;
        case DM_SCAN_REQ_RCVD_IND:
            APP_TRACE_INFO0(">>> App Event: DM_SCAN_REQ_RCVD_IND <<<");
            break;
        case DM_EXT_SCAN_START_IND:
            APP_TRACE_INFO0(">>> App Event: DM_EXT_SCAN_START_IND <<<");
            break;
        case DM_EXT_SCAN_STOP_IND:
            APP_TRACE_INFO0(">>> App Event: DM_EXT_SCAN_STOP_IND <<<");
            break;
        case DM_EXT_SCAN_REPORT_IND:
            APP_TRACE_INFO0(">>> App Event: DM_EXT_SCAN_REPORT_IND <<<");
            break;
        case DM_PER_ADV_SET_START_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PER_ADV_SET_START_IND <<<");
            break;
        case DM_PER_ADV_SET_STOP_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PER_ADV_SET_STOP_IND <<<");
            break;
        case DM_PER_ADV_SYNC_EST_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PER_ADV_SYNC_EST_IND <<<");
            break;
        case DM_PER_ADV_SYNC_EST_FAIL_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PER_ADV_SYNC_EST_FAIL_IND <<<");
            break;
        case DM_PER_ADV_SYNC_LOST_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PER_ADV_SYNC_LOST_IND <<<");
            break;
        case DM_PER_ADV_SYNC_TRSF_EST_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PER_ADV_SYNC_TRSF_EST_IND <<<");
            break;
        case DM_PER_ADV_SYNC_TRSF_EST_FAIL_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PER_ADV_SYNC_TRSF_EST_FAIL_IND <<<");
            break;
        case DM_PER_ADV_SYNC_TRSF_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PER_ADV_SYNC_TRSF_IND <<<");
            break;
        case DM_PER_ADV_SET_INFO_TRSF_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PER_ADV_SET_INFO_TRSF_IND <<<");
            break;
        case DM_PER_ADV_REPORT_IND:
            APP_TRACE_INFO0(">>> App Event: DM_PER_ADV_REPORT_IND <<<");
            break;
        case DM_REMOTE_FEATURES_IND:
            APP_TRACE_INFO0(">>> App Event: DM_REMOTE_FEATURES_IND <<<");
            break;
        case DM_READ_REMOTE_VER_INFO_IND:
            APP_TRACE_INFO0(">>> App Event: DM_READ_REMOTE_VER_INFO_IND <<<");
            break;
        case DM_CONN_IQ_REPORT_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_IQ_REPORT_IND <<<");
            break;
        case DM_CTE_REQ_FAIL_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CTE_REQ_FAIL_IND <<<");
            break;
        case DM_CONN_CTE_RX_SAMPLE_START_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_CTE_RX_SAMPLE_START_IND <<<");
            break;
        case DM_CONN_CTE_RX_SAMPLE_STOP_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_CTE_RX_SAMPLE_STOP_IND <<<");
            break;
        case DM_CONN_CTE_TX_CFG_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_CTE_TX_CFG_IND <<<");
            break;
        case DM_CONN_CTE_REQ_START_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_CTE_REQ_START_IND <<<");
            break;
        case DM_CONN_CTE_REQ_STOP_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_CTE_REQ_STOP_IND <<<");
            break;
        case DM_CONN_CTE_RSP_START_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_CTE_RSP_START_IND <<<");
            break;
        case DM_CONN_CTE_RSP_STOP_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CONN_CTE_RSP_STOP_IND <<<");
            break;
        case DM_READ_ANTENNA_INFO_IND:
            APP_TRACE_INFO0(">>> App Event: DM_READ_ANTENNA_INFO_IND <<<");
            break;
        case DM_CIS_CIG_CONFIG_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CIS_CIG_CONFIG_IND <<<");
            break;
        case DM_CIS_CIG_REMOVE_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CIS_CIG_REMOVE_IND <<<");
            break;
        case DM_CIS_REQ_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CIS_REQ_IND <<<");
            break;
        case DM_CIS_OPEN_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CIS_OPEN_IND <<<");
            break;
        case DM_CIS_CLOSE_IND:
            APP_TRACE_INFO0(">>> App Event: DM_CIS_CLOSE_IND <<<");
            break;
        case DM_REQ_PEER_SCA_IND:
            APP_TRACE_INFO0(">>> App Event: DM_REQ_PEER_SCA_IND <<<");
            break;
        case DM_ISO_DATA_PATH_SETUP_IND:
            APP_TRACE_INFO0(">>> App Event: DM_ISO_DATA_PATH_SETUP_IND <<<");
            break;
        case DM_ISO_DATA_PATH_REMOVE_IND:
            APP_TRACE_INFO0(">>> App Event: DM_ISO_DATA_PATH_REMOVE_IND <<<");
            break;
        case DM_DATA_PATH_CONFIG_IND:
            APP_TRACE_INFO0(">>> App Event: DM_DATA_PATH_CONFIG_IND <<<");
            break;
        case DM_READ_LOCAL_SUP_CODECS_IND:
            APP_TRACE_INFO0(">>> App Event: DM_READ_LOCAL_SUP_CODECS_IND <<<");
            break;
        case DM_READ_LOCAL_SUP_CODEC_CAP_IND:
            APP_TRACE_INFO0(">>> App Event: DM_READ_LOCAL_SUP_CODEC_CAP_IND <<<");
            break;
        case DM_READ_LOCAL_SUP_CTR_DLY_IND:
            APP_TRACE_INFO0(">>> App Event: DM_READ_LOCAL_SUP_CTR_DLY_IND <<<");
            break;
        case DM_BIG_START_IND:
            APP_TRACE_INFO0(">>> App Event: DM_BIG_START_IND <<<");
            break;
        case DM_BIG_STOP_IND:
            APP_TRACE_INFO0(">>> App Event: DM_BIG_STOP_IND <<<");
            break;
        case DM_BIG_SYNC_EST_IND:
            APP_TRACE_INFO0(">>> App Event: DM_BIG_SYNC_EST_IND <<<");
            break;
        case DM_BIG_SYNC_EST_FAIL_IND:
            APP_TRACE_INFO0(">>> App Event: DM_BIG_SYNC_EST_FAIL_IND <<<");
            break;
        case DM_BIG_SYNC_LOST_IND:
            APP_TRACE_INFO0(">>> App Event: DM_BIG_SYNC_LOST_IND <<<");
            break;
        case DM_BIG_SYNC_STOP_IND:
            APP_TRACE_INFO0(">>> App Event: DM_BIG_SYNC_STOP_IND <<<");
            break;
        case DM_BIG_INFO_ADV_REPORT_IND:
            APP_TRACE_INFO0(">>> App Event: DM_BIG_INFO_ADV_REPORT_IND <<<");
            break;
        case DM_L2C_CMD_REJ_IND:
            APP_TRACE_INFO0(">>> App Event: DM_L2C_CMD_REJ_IND <<<");
            break;
        case DM_ERROR_IND:
            APP_TRACE_INFO0(">>> App Event: DM_ERROR_IND <<<");
            break;
        case DM_HW_ERROR_IND:
            APP_TRACE_INFO0(">>> App Event: DM_HW_ERROR_IND <<<");
            break;
        case DM_VENDOR_SPEC_IND:
            APP_TRACE_INFO0(">>> App Event: DM_VENDOR_SPEC_IND <<<");
            break;

        default:
            APP_TRACE_INFO0(">>> App Event: Unknown <<<");
    }
  if (appUiCbackTbl.actionCback)
  {
    (*appUiCbackTbl.actionCback)(event, 0);
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Display a passkey.
 *
 *  \param  passkey   Passkey to display.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppUiDisplayPasskey(uint32_t passkey)
{
  APP_TRACE_INFO1(">>> Passkey: %d <<<", passkey);

  if (appUiCbackTbl.actionCback)
  {
    (*appUiCbackTbl.actionCback)(APP_UI_DISPLAY_PASSKEY, passkey);
  }
}

/*************************************************************************************************/
/*!
*  \brief  Display a confirmation value.
*
*  \param  confirm    Confirm value to display.
*
*  \return None.
*/
/*************************************************************************************************/
void AppUiDisplayConfirmValue(uint32_t confirm)
{
  APP_TRACE_INFO1(">>> Confirm Value: %d <<<", confirm);

  if (appUiCbackTbl.actionCback)
  {
    (*appUiCbackTbl.actionCback)(APP_UI_DISPLAY_CONFIRM, confirm);
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Display an RSSI value.
 *
 *  \param  rssi   Rssi value to display.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppUiDisplayRssi(int8_t rssi)
{
  APP_TRACE_INFO1(">>> RSSI: %d dBm <<<", rssi);

  if (appUiCbackTbl.actionCback)
  {
    (*appUiCbackTbl.actionCback)(APP_UI_DISPLAY_RSSI, rssi);
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Handle a UI timer expiration event.
 *
 *  \param  pMsg    Pointer to message.
 *
 *  \return None.
 */
/*************************************************************************************************/
void appUiTimerExpired(wsfMsgHdr_t *pMsg)
{

}

/*************************************************************************************************/
/*!
 *  \brief  Perform button press polling.  This function is called to handle WSF
 *          message APP_BTN_POLL_IND.
 *
 *  \return None.
 */
/*************************************************************************************************/
void appUiBtnPoll(void)
{
  if (appUiCbackTbl.btnPollCback)
  {
    (*appUiCbackTbl.btnPollCback)();
  }
}

/*************************************************************************************************/
/*!
 *  \brief  Handle a hardware button press.  This function is called to handle WSF
 *          event APP_BTN_DOWN_EVT.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppUiBtnPressed(void)
{

}

/*************************************************************************************************/
/*!
 *  \brief  Register a callback function to receive application button press events.
 *
 *  \return None.
 *
 *  \note   Registered by application to receive button events
 */
/*************************************************************************************************/
void AppUiBtnRegister(appUiBtnCback_t btnCback)
{
  appUiCbackTbl.btnCback = btnCback;
}

/*************************************************************************************************/
/*!
 *  \brief  Register a callback function to receive action events.
 *
 *  \return None.
 *
 *  \note   Registered by platform
 */
/*************************************************************************************************/
void AppUiActionRegister(appUiActionCback_t actionCback)
{
  appUiCbackTbl.actionCback = actionCback;
}

/*************************************************************************************************/
/*!
 *  \brief  Register a callback function to receive APP_BTN_POLL_IND events.
 *
 *  \return None.
 *
 *  \note   Registered by platform
 */
/*************************************************************************************************/
void AppUiBtnPollRegister(appUiBtnPollCback_t btnPollCback)
{
  appUiCbackTbl.btnPollCback = btnPollCback;
}

/*************************************************************************************************/
/*!
 *  \brief  Play a sound.
 *
 *  \param  pSound   Pointer to sound tone/duration array.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppUiSoundPlay(const appUiSound_t *pSound)
{

}

/*************************************************************************************************/
/*!
 *  \brief  Stop the sound that is currently playing.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppUiSoundStop(void)
{

}

/*************************************************************************************************/
/*
 *  \fn     AppUiLedStart
 *
 *  \brief  Start LED blinking.
 */
/*************************************************************************************************/
void AppUiLedStart(const appUiLed_t *pLed)
{

}

/*************************************************************************************************/
/*
 *  \fn     AppUiLedStop
 *
 *  \brief  Stop LED blinking.
 */
/*************************************************************************************************/
void AppUiLedStop(void)
{

}

/*************************************************************************************************/
/*!
 *  \brief  Button test function-- for test purposes only.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppUiBtnTest(uint8_t btn)
{
  if (appUiCbackTbl.btnCback)
  {
    (*appUiCbackTbl.btnCback)(btn);
  }
}

