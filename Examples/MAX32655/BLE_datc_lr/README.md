# BLE_datc_lr
This project is based on the BLE_datc project. The Dev-Kit is used as a central device. It
scans in the CODED PHY (s=8) mode.

# Log
==========================================␍␊
Long distance scanner demo (CODED PHY S=8)␍␊
BT_VER=9␍␊
==========================================␍␊
32kHz trimmed to 0x16␍␊
DatcHandlerInit␍␊
MAC Addr: 00:18:80:3C:6B:11␍␊
>>> Reset complete <<<␍␊
Database hash updated␍␊
dmDevPassEvtToDevPriv: event: 12, param: 74, advHandle: 0␍␊
␍␊
00:18:80:ca:a2:22 60 02 03 ... 30 31 60␍␊
00:18:80:ca:a2:22 61 02 03 ... 30 31 61␍␊
00:18:80:ca:a2:22 62 02 03 ... 30 31 62␍␊
00:18:80:ca:a2:22 63 02 03 ... 30 31 63␍␊
00:18:80:ca:a2:22 64 02 03 ... 30 31 64␍␊
00:18:80:ca:a2:22 66 02 03 ... 30 31 66␍␊
00:18:80:ca:a2:22 67 02 03 ... 30 31 67␍␊
00:18:80:ca:a2:22 68 02 03 ... 30 31 68␍␊
00:18:80:ca:a2:22 69 02 03 ... 30 31 69␍␊
00:18:80:ca:a2:22 6A 02 03 ... 30 31 6A␍␊
00:18:80:ca:a2:22 6B 02 03 ... 30 31 6B␍␊
00:18:80:ca:a2:22 6C 02 03 ... 30 31 6C␍␊
00:18:80:ca:a2:22 6D 02 03 ... 30 31 6D␍␊

# Handler, Message and Event Lists
HandlerId   Handler
0           Terminalhandler
1           SchHandler
2           LlHandler
3           HciHandler
4           DmHandler
5           AttHandler
6           SmpHandler
7           AppHandler
8           DatcHandler

LlHandler
search "LCTR_EVENT_TOTAL"

DmHandler
dmDevHciHandler
dmDevMsgHandler
dmExtScanHciHandler
12  DM_DEV_PRIV_MSG_RPA_START
44  HCI_LE_EXT_ADV_REPORT_CBACK_EVT
54  HCI_LE_EXT_SCAN_ENABLE_CMD_CMPL_CBACK_EVT

DatcHandler
74  DM_EXT_SCAN_START_IND
76  DM_EXT_SCAN_REPORT_IND
