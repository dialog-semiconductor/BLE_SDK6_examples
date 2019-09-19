/**
 ****************************************************************************************
 *
 * @file user_config_defs.h
 *
 * @brief ROM configuration table index definitions.
 *
 * Copyright (c) 2017-2018 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

#ifndef USER_CONFIG_DEFS_H
#define USER_CONFIG_DEFS_H

enum rom_func_table_pos
{
    rf_init_func_pos=0,
    rf_reinit_func_pos,
    uart_init_func_pos,
    uart_flow_on_func_pos,
    uart_flow_off_func_pos,
    uart_finish_transfers_func_pos,
    uart_read_func_pos,
    uart_write_func_pos,
    UART_Handler_func_pos,
    gtl_init_func_pos,
    gtl_eif_init_func_pos,
    gtl_eif_read_start_func_pos,
    gtl_eif_read_hdr_func_pos,
    gtl_eif_read_payl_func_pos,
    gtl_eif_tx_done_func_pos,
    gtl_eif_rx_done_func_pos,
    h4tl_init_func_pos,
    h4tl_read_start_func_pos,
    h4tl_read_hdr_func_pos,
    h4tl_read_payl_func_pos,
    h4tl_read_next_out_of_sync_func_pos,
    h4tl_out_of_sync_func_pos,
    h4tl_tx_done_func_pos,
    h4tl_rx_done_func_pos,
    ke_task_init_func_pos,
    ke_timer_init_func_pos,
    llm_encryption_done_func_pos,

    nvds_get_func_pos,
    nvds_put_func_pos,
    nvds_del_func_pos,
    nvds_init_func_pos,
    rwip_eif_get_func_pos,

    platform_reset_func_pos,
    lld_sleep_compensate_func_pos,
    lld_sleep_init_func_pos,
    lld_sleep_us_2_lpcycles_func_pos,
    lld_sleep_lpcycles_2_us_func_pos,

    lld_test_stop_func_pos,
    lld_test_mode_tx_func_pos,
    lld_test_mode_rx_func_pos,
    smpc_check_param_func_pos,
    smpc_pdu_recv_func_pos,

    prf_init_func_pos,
    prf_add_profile_func_pos,
    prf_create_func_pos,
    prf_cleanup_func_pos,
    prf_get_id_from_task_func_pos,
    prf_get_task_from_id_func_pos,

    SetSystemVars_func_pos,
    dbg_init_func_pos,
    dbg_platform_reset_complete_func_pos,

    hci_rd_local_supp_feats_cmd_handler_func_pos,

    l2cc_pdu_pack_func_pos,
    l2cc_pdu_unpack_func_pos,
    l2c_send_lecb_message_func_pos,
    l2c_process_sdu_func_pos,

    l2cc_pdu_recv_ind_handler_func_pos,
    gapc_lecb_connect_cfm_handler_func_pos,

    atts_l2cc_pdu_recv_handler_func_pos,
    attc_l2cc_pdu_recv_handler_func_pos,

//GZ 4.2
    crypto_init_func_pos,
    llm_le_adv_report_ind_func_pos,
    PK_PointMult_func_pos,
    llm_p256_start_func_pos,
    llm_create_p256_key_func_pos,
    llm_p256_req_handler_func_pos,
    llc_le_length_effective_func_pos,
    llc_le_length_conn_init_func_pos,
    lld_data_tx_prog_func_pos,
    lld_data_tx_check_func_pos,
    llc_pdu_send_func_pos,
    llc_data_notif_func_pos,
    dia_rand_func_pos,
    dia_srand_func_pos,
    ba431_get_rand_func_pos,
#if !defined (__DA14531__)      // Only for DA14585, DA14586 (uses null functions)
    modulationGainCalibration_func_pos,
    DCoffsetCalibration_func_pos,
    PllLF_IFF_MIX_CapCalibration_func_pos,
#endif
    smpc_public_key_exchange_start_func_pos,
    smpc_dhkey_calc_ind_func_pos,
    smpm_ecdh_key_create_func_pos,
    ble_init_arp_pos,

    custom_preinit_func_pos,
    custom_postinit_func_pos,
    custom_appinit_func_pos,
    custom_preloop_func_pos,
    custom_preschedule_func_pos,
    custom_postschedule_func_pos,
    custom_postschedule_async_func_pos,
    custom_presleepcheck_func_pos,
    custom_appsleepset_func_pos,
    custom_postsleepcheck_func_pos,
    custom_presleepenter_func_pos,
    custom_postsleepexit_func_pos,
    custom_prewakeup_func_pos,
    custom_postwakeup_func_pos,
    custom_preidlecheck_func_pos,
    custom_pti_set_func_pos,
};

enum rom_cfg_var_pos
{
    app_sec_task_pos=0,
    rwip_heap_non_ret_pos,
    rwip_heap_non_ret_size,
    rwip_heap_env_pos,
    rwip_heap_env_size,
    rwip_heap_db_pos,
    rwip_heap_db_size,
    rwip_heap_msg_pos,
    rwip_heap_msg_size,
    max_sleep_duration_periodic_wakeup_pos,
    max_sleep_duration_external_wakeup_pos,
    app_main_task_pos,
    man_id_pos,
    max_tl_pending_packets_adv_pos,
    max_tl_pending_packets_pos,
    ea_timer_prog_delay_pos,
    ea_clock_corr_lat_pos,
    ea_be_used_dft_pos,
    start_margin_pos,
    test_mode_margin_pos,
    bw_used_slave_dft_pos,
    bw_used_adv_dft_pos,
    hz32000_pos,
    rwble_prog_latency_dft_pos,
    rwble_asap_latency_pos,
    rwble_priority_adv_ldc_pos,
    rwble_priority_scan_pos,
    rwble_priority_mconnect_pos,
    rwble_priority_sconnect_pos,
    rwble_priority_adv_hdc_pos,
    rwble_priority_init_pos,
    rwble_priority_max_pos,
    gap_lecb_cnx_max_pos,
    gapm_scan_filter_size_pos,
    smpc_rep_attempts_timer_def_val_pos,
    smpc_rep_attempts_timer_max_val_pos,
    smpc_rep_attempts_timer_mult_pos,
    smpc_timeout_timer_duration_pos,
    att_trans_rtx_pos,
    att_sec_enc_key_size_pos,
    wakeup_boundary_var_pos,
    lld_evt_abort_cnt_duration_pos,
    ea_check_halfslot_boundary_pos,
    ea_check_slot_boundary_pos,
    lld_rx_irq_thres_pos,
    nb_links_user_pos,
    llm_adv_interval_min_noncon_disc_pos,
    sw_version_major_pos,
    sw_version_minor_pos,
    adv_int_conn_dir_pos,
    adv_int_nonconn_undir_pos,
    adv_int_con_dir_ldc_pos,
    prefetch_time_pos,
/*
 * Support for SDK symbols in ROM (only for DA14531)
 */
#if defined (__DA14531__)
    nvds_config_pos,
    chacha_config_pos,
    timer_easy_config_pos,
    custs1_prf_conf_pos,
    prfconf_config_pos,
    appconf_config_pos,
    console_config_pos,
    sleep_config_pos,
    appsuota_config_pos,
    appdiss_config_pos,
    appeasymsg_config_pos,
    system_config_pos
#endif
};

#endif //USER_CONFIG_DEFS_H
