/*
 * Copyright (c) Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 *   3. Neither the name of Nordic Semiconductor ASA nor the names of other
 *   contributors to this software may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 *   4. This software must only be used in a processor manufactured by Nordic
 *   Semiconductor ASA, or in a processor manufactured by a third party that
 *   is used in combination with a processor manufactured by Nordic Semiconductor.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
  @addtogroup BLE_GAP Generic Access Profile (GAP)
  @{
  @brief Definitions and prototypes for the GAP interface.
 */

#ifndef __BLE_GAP_H__
#define __BLE_GAP_H__

#include "ble_ranges.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@addtogroup BLE_GAP_ENUMERATIONS Enumerations
 * @{ */

/**@brief GAP API SVC numbers.
 */
enum BLE_GAP_SVCS
{
  SD_BLE_GAP_ADDRESS_SET  = BLE_GAP_SVC_BASE,  /**< Set own Bluetooth Address. */
  SD_BLE_GAP_ADDRESS_GET,                      /**< Get own Bluetooth Address. */
  SD_BLE_GAP_ADV_DATA_SET,                     /**< Set Advertising Data. */
  SD_BLE_GAP_ADV_START,                        /**< Start Advertising. */
  SD_BLE_GAP_ADV_STOP,                         /**< Stop Advertising. */
  SD_BLE_GAP_CONN_PARAM_UPDATE,                /**< Connection Parameter Update. */
  SD_BLE_GAP_DISCONNECT,                       /**< Disconnect. */
  SD_BLE_GAP_TX_POWER_SET,                     /**< Set TX Power. */
  SD_BLE_GAP_APPEARANCE_SET,                   /**< Set Appearance. */
  SD_BLE_GAP_APPEARANCE_GET,                   /**< Get Appearance. */
  SD_BLE_GAP_PPCP_SET,                         /**< Set PPCP. */
  SD_BLE_GAP_PPCP_GET,                         /**< Get PPCP. */
  SD_BLE_GAP_DEVICE_NAME_SET,                  /**< Set Device Name. */
  SD_BLE_GAP_DEVICE_NAME_GET,                  /**< Get Device Name. */
  SD_BLE_GAP_AUTHENTICATE,                     /**< Initiate Pairing/Bonding. */
  SD_BLE_GAP_SEC_PARAMS_REPLY,                 /**< Reply with Security Parameters. */
  SD_BLE_GAP_AUTH_KEY_REPLY,                   /**< Reply with an authentication key. */
  SD_BLE_GAP_LESC_DHKEY_REPLY,                 /**< Reply with an LE Secure Connections DHKey. */
  SD_BLE_GAP_KEYPRESS_NOTIFY,                  /**< Notify of a keypress during an authentication procedure. */
  SD_BLE_GAP_LESC_OOB_DATA_GET,                /**< Get the local LE Secure Connections OOB data. */
  SD_BLE_GAP_LESC_OOB_DATA_SET,                /**< Set the remote LE Secure Connections OOB data. */
  SD_BLE_GAP_ENCRYPT,                          /**< Initiate encryption procedure. */
  SD_BLE_GAP_SEC_INFO_REPLY,                   /**< Reply with Security Information. */
  SD_BLE_GAP_CONN_SEC_GET,                     /**< Obtain connection security level. */
  SD_BLE_GAP_RSSI_START,                       /**< Start reporting of changes in RSSI. */
  SD_BLE_GAP_RSSI_STOP,                        /**< Stop reporting of changes in RSSI. */
  SD_BLE_GAP_SCAN_START,                       /**< Start Scanning. */
  SD_BLE_GAP_SCAN_STOP,                        /**< Stop Scanning. */
  SD_BLE_GAP_CONNECT,                          /**< Connect. */
  SD_BLE_GAP_CONNECT_CANCEL,                   /**< Cancel ongoing connection procedure. */
  SD_BLE_GAP_RSSI_GET,                         /**< Get the last RSSI sample. */
};

/**@brief GAP Event IDs.
 * IDs that uniquely identify an event coming from the stack to the application.
 */
enum BLE_GAP_EVTS
{
  BLE_GAP_EVT_CONNECTED  = BLE_GAP_EVT_BASE,    /**< Connection established.                         \n See @ref ble_gap_evt_connected_t.            */
  BLE_GAP_EVT_DISCONNECTED,                     /**< Disconnected from peer.                         \n See @ref ble_gap_evt_disconnected_t.         */
  BLE_GAP_EVT_CONN_PARAM_UPDATE,                /**< Connection Parameters updated.                  \n See @ref ble_gap_evt_conn_param_update_t.    */
  BLE_GAP_EVT_SEC_PARAMS_REQUEST,               /**< Request to provide security parameters.         \n Reply with @ref sd_ble_gap_sec_params_reply.  \n See @ref ble_gap_evt_sec_params_request_t. */
  BLE_GAP_EVT_SEC_INFO_REQUEST,                 /**< Request to provide security information.        \n Reply with @ref sd_ble_gap_sec_info_reply.    \n See @ref ble_gap_evt_sec_info_request_t.   */
  BLE_GAP_EVT_PASSKEY_DISPLAY,                  /**< Request to display a passkey to the user.       \n In LESC Numeric Comparison, reply with @ref sd_ble_gap_auth_key_reply. \n See @ref ble_gap_evt_passkey_display_t. */
  BLE_GAP_EVT_KEY_PRESSED,                      /**< Notification of a keypress on the remote device.\n See @ref ble_gap_evt_key_pressed_t           */
  BLE_GAP_EVT_AUTH_KEY_REQUEST,                 /**< Request to provide an authentication key.       \n Reply with @ref sd_ble_gap_auth_key_reply.    \n See @ref ble_gap_evt_auth_key_request_t.   */
  BLE_GAP_EVT_LESC_DHKEY_REQUEST,               /**< Request to calculate an LE Secure Connections DHKey. \n Reply with @ref sd_ble_gap_lesc_dhkey_reply.  \n See @ref ble_gap_evt_lesc_dhkey_request_t */
  BLE_GAP_EVT_AUTH_STATUS,                      /**< Authentication procedure completed with status. \n See @ref ble_gap_evt_auth_status_t.          */
  BLE_GAP_EVT_CONN_SEC_UPDATE,                  /**< Connection security updated.                    \n See @ref ble_gap_evt_conn_sec_update_t.      */
  BLE_GAP_EVT_TIMEOUT,                          /**< Timeout expired.                                \n See @ref ble_gap_evt_timeout_t.              */
  BLE_GAP_EVT_RSSI_CHANGED,                     /**< RSSI report.                                    \n See @ref ble_gap_evt_rssi_changed_t.         */
  BLE_GAP_EVT_ADV_REPORT,                       /**< Advertising report.                             \n See @ref ble_gap_evt_adv_report_t.           */
  BLE_GAP_EVT_SEC_REQUEST,                      /**< Security Request.                               \n See @ref ble_gap_evt_sec_request_t.          */
  BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST,        /**< Connection Parameter Update Request.            \n Reply with @ref sd_ble_gap_conn_param_update. \n See @ref ble_gap_evt_conn_param_update_request_t. */
  BLE_GAP_EVT_SCAN_REQ_REPORT,                  /**< Scan request report.                            \n See @ref ble_gap_evt_scan_req_report_t.      */
};

/**@brief GAP Option IDs.
 * IDs that uniquely identify a GAP option.
 */
enum BLE_GAP_OPTS
{
  BLE_GAP_OPT_CH_MAP  = BLE_GAP_OPT_BASE,       /**< Channel Map. @ref ble_gap_opt_ch_map_t  */
  BLE_GAP_OPT_LOCAL_CONN_LATENCY,               /**< Local connection latency. @ref ble_gap_opt_local_conn_latency_t */
  BLE_GAP_OPT_PASSKEY,                          /**< Set passkey. @ref ble_gap_opt_passkey_t */
  BLE_GAP_OPT_PRIVACY,                          /**< Custom privacy. @ref ble_gap_opt_privacy_t */
  BLE_GAP_OPT_SCAN_REQ_REPORT,                  /**< Scan request report. @ref ble_gap_opt_scan_req_report_t */
  BLE_GAP_OPT_COMPAT_MODE                       /**< Compatibility mode. @ref ble_gap_opt_compat_mode_t */
};

/** @} */

#ifdef __cplusplus
}
#endif
#endif // BLE_GAP_H__

/**
  @}
*/
