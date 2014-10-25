/*
 * BT-AMP (BlueTooth Alternate Mac and Phy) HCI (Host/Controller Interface)
 *
 * Copyright (C) 1999-2011, Broadcom Corporation
 * 
 *         Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2 (the "GPL"),
 * available at http://www.broadcom.com/licenses/GPLv2.php, with the
 * following added to such license:
 * 
 *      As a special exception, the copyright holders of this software give you
 * permission to link this software with independent modules, and to copy and
 * distribute the resulting executable under terms of your choice, provided that
 * you also meet, for each linked independent module, the terms and conditions of
 * the license of that module.  An independent module is a module which is not
 * derived from this software.  The special exception does not apply to any
 * modifications of the software.
 * 
 *      Notwithstanding the above, under no circumstances may you combine this
 * software in any way with any other Broadcom software provided under a license
 * other than the GPL, without Broadcom's express prior written consent.
 *
 * $Id: bt_amp_hci.h 277737 2011-08-16 17:54:59Z $
*/

#ifndef _bt_amp_hci_h
#define _bt_amp_hci_h


/* AMP HCI CMD packet format */
typedef __packed struct amp_hci_cmd {
	uint16 opcode;
	uint8 plen;
	uint8 parms[1];
} amp_hci_cmd_t;

#define HCI_CMD_PREAMBLE_SIZE		OFFSETOF(amp_hci_cmd_t, parms)
#define HCI_CMD_DATA_SIZE		255

/* AMP HCI CMD opcode layout */
#define HCI_CMD_OPCODE(ogf, ocf)	((((ogf) & 0x3F) << 10) | ((ocf) & 0x03FF))
#define HCI_CMD_OGF(opcode)		((uint8)(((opcode) >> 10) & 0x3F))
#define HCI_CMD_OCF(opcode)		((opcode) & 0x03FF)

/* AMP HCI command opcodes */
#define HCI_Read_Failed_Contact_Counter		HCI_CMD_OPCODE(0x05, 0x0001)
#define HCI_Reset_Failed_Contact_Counter	HCI_CMD_OPCODE(0x05, 0x0002)
#define HCI_Read_Link_Quality			HCI_CMD_OPCODE(0x05, 0x0003)
#define HCI_Read_Local_AMP_Info			HCI_CMD_OPCODE(0x05, 0x0009)
#define HCI_Read_Local_AMP_ASSOC		HCI_CMD_OPCODE(0x05, 0x000A)
#define HCI_Write_Remote_AMP_ASSOC		HCI_CMD_OPCODE(0x05, 0x000B)
#define HCI_Create_Physical_Link		HCI_CMD_OPCODE(0x01, 0x0035)
#define HCI_Accept_Physical_Link_Request	HCI_CMD_OPCODE(0x01, 0x0036)
#define HCI_Disconnect_Physical_Link		HCI_CMD_OPCODE(0x01, 0x0037)
#define HCI_Create_Logical_Link			HCI_CMD_OPCODE(0x01, 0x0038)
#define HCI_Accept_Logical_Link			HCI_CMD_OPCODE(0x01, 0x0039)
#define HCI_Disconnect_Logical_Link		HCI_CMD_OPCODE(0x01, 0x003A)
#define HCI_Logical_Link_Cancel			HCI_CMD_OPCODE(0x01, 0x003B)
#define HCI_Flow_Spec_Modify			HCI_CMD_OPCODE(0x01, 0x003C)
#define HCI_Write_Flow_Control_Mode		HCI_CMD_OPCODE(0x01, 0x0067)
#define HCI_Read_Best_Effort_Flush_Timeout	HCI_CMD_OPCODE(0x01, 0x0069)
#define HCI_Write_Best_Effort_Flush_Timeout	HCI_CMD_OPCODE(0x01, 0x006A)
#define HCI_Short_Range_Mode			HCI_CMD_OPCODE(0x01, 0x006B)
#define HCI_Reset				HCI_CMD_OPCODE(0x03, 0x0003)
#define HCI_Read_Connection_Accept_Timeout	HCI_CMD_OPCODE(0x03, 0x0015)
#define HCI_Write_Connection_Accept_Timeout	HCI_CMD_OPCODE(0x03, 0x0016)
#define HCI_Read_Link_Supervision_Timeout	HCI_CMD_OPCODE(0x03, 0x0036)
#define HCI_Write_Link_Supervision_Timeout	HCI_CMD_OPCODE(0x03, 0x0037)
#define HCI_Enhanced_Flush			HCI_CMD_OPCODE(0x03, 0x005F)
#define HCI_Read_Logical_Link_Accept_Timeout	HCI_CMD_OPCODE(0x03, 0x0061)
#define HCI_Write_Logical_Link_Accept_Timeout	HCI_CMD_OPCODE(0x03, 0x0062)
#define HCI_Set_Event_Mask_Page_2		HCI_CMD_OPCODE(0x03, 0x0063)
#define HCI_Read_Location_Data_Command		HCI_CMD_OPCODE(0x03, 0x0064)
#define HCI_Write_Location_Data_Command		HCI_CMD_OPCODE(0x03, 0x0065)
#define HCI_Read_Local_Version_Info		HCI_CMD_OPCODE(0x04, 0x0001)
#define HCI_Read_Local_Supported_Commands	HCI_CMD_OPCODE(0x04, 0x0002)
#define HCI_Read_Buffer_Size			HCI_CMD_OPCODE(0x04, 0x0005)
#define HCI_Read_Data_Block_Size		HCI_CMD_OPCODE(0x04, 0x000A)

/* AMP HCI command parameters */
typedef __packed struct read_local_cmd_parms {
	uint8 plh;
	uint8 offset[2];			/* length so far */
	uint8 max_remote[2];
} read_local_cmd_parms_t;

typedef __packed struct write_remote_cmd_parms {
	uint8 plh;
	uint8 offset[2];
	uint8 len[2];
	uint8 frag[1];
} write_remote_cmd_parms_t;

typedef __packed struct phy_link_cmd_parms {
	uint8 plh;
	uint8 key_length;
	uint8 key_type;
	uint8 key[1];
} phy_link_cmd_parms_t;

typedef __packed struct dis_phy_link_cmd_parms {
	uint8 plh;
	uint8 reason;
} dis_phy_link_cmd_parms_t;

typedef __packed struct log_link_cmd_parms {
	uint8 plh;
	uint8 txflow[16];
	uint8 rxflow[16];
} log_link_cmd_parms_t;

typedef __packed struct ext_flow_spec {
	uint8 id;
	uint8 service_type;
	uint8 max_sdu[2];
	uint8 sdu_ia_time[4];
	uint8 access_latency[4];
	uint8 flush_timeout[4];
} ext_flow_spec_t;

typedef __packed struct log_link_cancel_cmd_parms {
	uint8 plh;
	uint8 tx_fs_ID;
} log_link_cancel_cmd_parms_t;

typedef __packed struct flow_spec_mod_cmd_parms {
	uint8 llh[2];
	uint8 txflow[16];
	uint8 rxflow[16];
} flow_spec_mod_cmd_parms_t;

typedef __packed struct plh_pad {
	uint8 plh;
	uint8 pad;
} plh_pad_t;

typedef __packed union hci_handle {
	uint16 bredr;
	plh_pad_t amp;
} hci_handle_t;

typedef __packed struct ls_to_cmd_parms {
	hci_handle_t handle;
	uint8 timeout[2];
} ls_to_cmd_parms_t;

typedef __packed struct befto_cmd_parms {
	uint8 llh[2];
	uint8 befto[4];
} befto_cmd_parms_t;

typedef __packed struct srm_cmd_parms {
	uint8 plh;
	uint8 srm;
} srm_cmd_parms_t;

typedef __packed struct ld_cmd_parms {
	uint8 ld_aware;
	uint8 ld[2];
	uint8 ld_opts;
	uint8 l_opts;
} ld_cmd_parms_t;

typedef __packed struct eflush_cmd_parms {
	uint8 llh[2];
	uint8 packet_type;
} eflush_cmd_parms_t;

/* Generic AMP extended flow spec service types */
#define EFS_SVCTYPE_NO_TRAFFIC		0
#define EFS_SVCTYPE_BEST_EFFORT		1
#define EFS_SVCTYPE_GUARANTEED		2

/* AMP HCI event packet format */
typedef __packed struct amp_hci_event {
	uint8 ecode;
	uint8 plen;
	uint8 parms[1];
} amp_hci_event_t;

#define HCI_EVT_PREAMBLE_SIZE			OFFSETOF(amp_hci_event_t, parms)

/* AMP HCI event codes */
#define HCI_Command_Complete			0x0E
#define HCI_Command_Status			0x0F
#define HCI_Flush_Occurred			0x11
#define HCI_Enhanced_Flush_Complete		0x39
#define HCI_Physical_Link_Complete		0x40
#define HCI_Channel_Select			0x41
#define HCI_Disconnect_Physical_Link_Complete	0x42
#define HCI_Logical_Link_Complete		0x45
#define HCI_Disconnect_Logical_Link_Complete	0x46
#define HCI_Flow_Spec_Modify_Complete		0x47
#define HCI_Number_of_Completed_Data_Blocks	0x48
#define HCI_Short_Range_Mode_Change_Complete	0x4C
#define HCI_Status_Change_Event			0x4D
#define HCI_Vendor_Specific			0xFF

/* AMP HCI event mask bit positions */
#define HCI_Physical_Link_Complete_Event_Mask			0x0001
#define HCI_Channel_Select_Event_Mask				0x0002
#define HCI_Disconnect_Physical_Link_Complete_Event_Mask	0x0004
#define HCI_Logical_Link_Complete_Event_Mask			0x0020
#define HCI_Disconnect_Logical_Link_Complete_Event_Mask		0x0040
#define HCI_Flow_Spec_Modify_Complete_Event_Mask		0x0080
#define HCI_Number_of_Completed_Data_Blocks_Event_Mask		0x0100
#define HCI_Short_Range_Mode_Change_Complete_Event_Mask		0x1000
#define HCI_Status_Change_Event_Mask				0x2000
#define HCI_All_Event_Mask					0x31e7

/* AMP HCI event parameters */
typedef __packed struct cmd_status_parms {
	uint8 status;
	uint8 cmdpkts;
	uint16 opcode;
} cmd_status_parms_t;

typedef __packed struct cmd_complete_parms {
	uint8 cmdpkts;
	uint16 opcode;
	uint8 parms[1];
} cmd_complete_parms_t;

typedef __packed struct flush_occurred_evt_parms {
	uint16 handle;
} flush_occurred_evt_parms_t;

typedef __packed struct write_remote_evt_parms {
	uint8 status;
	uint8 plh;
} write_remote_evt_parms_t;

typedef __packed struct read_local_evt_parms {
	uint8 status;
	uint8 plh;
	uint16 len;
	uint8 frag[1];
} read_local_evt_parms_t;

typedef __packed struct read_local_info_evt_parms {
	uint8 status;
	uint8 AMP_status;
	uint32 bandwidth;
	uint32 gbandwidth;
	uint32 latency;
	uint32 PDU_size;
	uint8 ctrl_type;
	uint16 PAL_cap;
	uint16 AMP_ASSOC_len;
	uint32 max_flush_timeout;
	uint32 be_flush_timeout;
} read_local_info_evt_parms_t;

typedef __packed struct log_link_evt_parms {
	uint8 status;
	uint16 llh;
	uint8 plh;
	uint8 tx_fs_ID;
} log_link_evt_parms_t;

typedef __packed struct disc_log_link_evt_parms {
	uint8 status;
	uint16 llh;
	uint8 reason;
} disc_log_link_evt_parms_t;

typedef __packed struct log_link_cancel_evt_parms {
	uint8 status;
	uint8 plh;
	uint8 tx_fs_ID;
} log_link_cancel_evt_parms_t;

typedef __packed struct flow_spec_mod_evt_parms {
	uint8 status;
	uint16 llh;
} flow_spec_mod_evt_parms_t;

typedef __packed struct phy_link_evt_parms {
	uint8 status;
	uint8 plh;
} phy_link_evt_parms_t;

typedef __packed struct dis_phy_link_evt_parms {
	uint8 status;
	uint8 plh;
	uint8 reason;
} dis_phy_link_evt_parms_t;

typedef __packed struct read_ls_to_evt_parms {
	uint8 status;
	hci_handle_t handle;
	uint16 timeout;
} read_ls_to_evt_parms_t;

typedef __packed struct read_lla_ca_to_evt_parms {
	uint8 status;
	uint16 timeout;
} read_lla_ca_to_evt_parms_t;

typedef __packed struct read_data_block_size_evt_parms {
	uint8 status;
	uint16 ACL_pkt_len;
	uint16 data_block_len;
	uint16 data_block_num;
} read_data_block_size_evt_parms_t;

typedef __packed struct data_blocks {
	uint16 handle;
	uint16 pkts;
	uint16 blocks;
} data_blocks_t;

typedef __packed struct num_completed_data_blocks_evt_parms {
	uint16 num_blocks;
	uint8 num_handles;
	data_blocks_t completed[1];
} num_completed_data_blocks_evt_parms_t;

typedef __packed struct befto_evt_parms {
	uint8 status;
	uint32 befto;
} befto_evt_parms_t;

typedef __packed struct srm_evt_parms {
	uint8 status;
	uint8 plh;
	uint8 srm;
} srm_evt_parms_t;

typedef __packed struct contact_counter_evt_parms {
	uint8 status;
	uint8 llh[2];
	uint16 counter;
} contact_counter_evt_parms_t;

typedef __packed struct contact_counter_reset_evt_parms {
	uint8 status;
	uint8 llh[2];
} contact_counter_reset_evt_parms_t;

typedef __packed struct read_linkq_evt_parms {
	uint8 status;
	hci_handle_t handle;
	uint8 link_quality;
} read_linkq_evt_parms_t;

typedef __packed struct ld_evt_parms {
	uint8 status;
	uint8 ld_aware;
	uint8 ld[2];
	uint8 ld_opts;
	uint8 l_opts;
} ld_evt_parms_t;

typedef __packed struct eflush_complete_evt_parms {
	uint16 handle;
} eflush_complete_evt_parms_t;

typedef __packed struct vendor_specific_evt_parms {
	uint8 len;
	uint8 parms[1];
} vendor_specific_evt_parms_t;

typedef __packed struct local_version_info_evt_parms {
	uint8 status;
	uint8 hci_version;
	uint16 hci_revision;
	uint8 pal_version;
	uint16 mfg_name;
	uint16 pal_subversion;
} local_version_info_evt_parms_t;

#define MAX_SUPPORTED_CMD_BYTE	64
typedef __packed struct local_supported_cmd_evt_parms {
	uint8 status;
	uint8 cmd[MAX_SUPPORTED_CMD_BYTE];
} local_supported_cmd_evt_parms_t;

typedef __packed struct status_change_evt_parms {
	uint8 status;
	uint8 amp_status;
} status_change_evt_parms_t;

/* AMP HCI error codes */
#define HCI_SUCCESS				0x00
#define HCI_ERR_ILLEGAL_COMMAND			0x01
#define HCI_ERR_NO_CONNECTION			0x02
#define HCI_ERR_MEMORY_FULL			0x07
#define HCI_ERR_CONNECTION_TIMEOUT		0x08
#define HCI_ERR_MAX_NUM_OF_CONNECTIONS		0x09
#define HCI_ERR_CONNECTION_EXISTS		0x0B
#define HCI_ERR_CONNECTION_DISALLOWED		0x0C
#define HCI_ERR_CONNECTION_ACCEPT_TIMEOUT	0x10
#define HCI_ERR_UNSUPPORTED_VALUE		0x11
#define HCI_ERR_ILLEGAL_PARAMETER_FMT		0x12
#define HCI_ERR_CONN_TERM_BY_LOCAL_HOST		0x16
#define HCI_ERR_UNSPECIFIED			0x1F
#define HCI_ERR_UNIT_KEY_USED			0x26
#define HCI_ERR_QOS_REJECTED			0x2D
#define HCI_ERR_PARAM_OUT_OF_RANGE		0x30
#define HCI_ERR_NO_SUITABLE_CHANNEL		0x39
#define HCI_ERR_CHANNEL_MOVE			0xFF

/* AMP HCI ACL Data packet format */
typedef __packed struct amp_hci_ACL_data {
	uint16	handle;			/* 12-bit connection handle + 2-bit PB and 2-bit BC flags */
	uint16	dlen;			/* data total length */
	uint8 data[1];
} amp_hci_ACL_data_t;

#define HCI_ACL_DATA_PREAMBLE_SIZE	OFFSETOF(amp_hci_ACL_data_t, data)

#define HCI_ACL_DATA_BC_FLAGS		(0x0 << 14)
#define HCI_ACL_DATA_PB_FLAGS		(0x3 << 12)

#define HCI_ACL_DATA_HANDLE(handle)	((handle) & 0x0fff)
#define HCI_ACL_DATA_FLAGS(handle)	((handle) >> 12)

/* AMP Activity Report packet formats */
typedef __packed struct amp_hci_activity_report {
	uint8	ScheduleKnown;
	uint8	NumReports;
	uint8	data[1];
} amp_hci_activity_report_t;

typedef __packed struct amp_hci_activity_report_triple {
	uint32	StartTime;
	uint32	Duration;
	uint32	Periodicity;
} amp_hci_activity_report_triple_t;

#define HCI_AR_SCHEDULE_KNOWN		0x01

#endif /* _bt_amp_hci_h_ */
