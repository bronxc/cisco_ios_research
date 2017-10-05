
vsvc_nsm.h,4255
#define VNSM_ERR_NOTSUPPORTED	36,1130
#define VNSM_ERR_NOTIMPLEMENTED	37,1165
#define VNSM_ERR_INVPARM	38,1202
#define VNSM_ERR_NOMEM	39,1232
#define VNSM_ERR_NOTSERIAL	40,1261
#define VNSM_ERR_NOTLAN	41,1293
#define VNSM_ERR_NOTCONFIG	42,1323
#define VNSM_ERR_UNSUPREV	43,1355
#define VNSM_ERR_ALREADYREG	44,1386
#define VNSM_ERR_NOTREG	45,1419
#define VNSM_ERR_BCASTPORT	46,1449
#define VNSM_VERSION_1	52,1565
#define VNSM_VERSION_2	53,1591
#define VNSM_TAGS	55,1618
#define VNSM_DATA	56,1639
#define VNSM_RUNTIME	58,1661
#define VNSM_MAX_MACLENGTH	64,1821
#define VNSM_MAX_X25LENGTH	65,1850
#define VNSM_LEN_STACKNAME	72,2010
#define VNSM_LEN_FAMILYNAME	73,2069
#define VNSM_LEN_PROTOCOLNAME	74,2100
#define VNSM_LEN_TABLENAME	75,2163
#define VNSM_LEN_PRODUCTNAME	76,2223
#define VNSM_LEN_OSREVNAME	77,2272
#define VNSM_LEN_OSTYPENAME	78,2319
#define VNSM_LEN_DISKNAME	79,2367
#define VNSM_LEN_IFNAME	80,2413
#define VNSM_LEN_HELPSECTION	81,2455
#define VNSM_LEN_HELPFILE	82,2487
#define VNSM_LEN_LANGUAGE	83,2516
#define VNSM_LEN_LABELMAX	85,2546
#define VNSM_NMA_COUNTER16	92,2744
#define VNSM_NMA_COUNTER32	93,2773
#define VNSM_NMA_RATE	94,2802
#define VNSM_NMA_GAGUE16	95,2827
#define VNSM_NMA_GAGUE32	96,2854
#define VNSM_NMA_TICKS16	97,2881
#define VNSM_NMA_TICKS32	98,2908
#define VNSM_NMA_BOOLEAN	99,2935
#define VNSM_NMA_BYTE	100,2962
#define VNSM_NMA_CSTRING	101,2987
#define VNSM_NMA_XSTRING	102,3015
#define VNSM_NMA_BITSTRING	103,3043
#define VNSM_NMA_STATUS16	104,3073
#define VNSM_NMA_LOWTIDE16	105,3102
#define VNSM_NMA_LOWTIDE32	106,3132
#define VNSM_NMA_HIGHTIDE16	107,3162
#define VNSM_NMA_HIGHTIDE32	108,3193
#define VNSM_NMA_LLADDRESS	109,3224
#define VNSM_NMA_CARDSEQ	110,3254
#define VNSM_NMA_BYTESTRING	111,3282
#define VNSM_NMA_IPADDRESS	112,3313
#define VNSM_NMA_NSAP	113,3343
#define VNSM_NMA_SNPA	114,3369
#define VNSM_NMA_SockAddrISO	115,3395
#define VNSM_NMA_INT16	116,3427
#define VNSM_NMA_INT32	117,3454
#define VNSM_NMA_ISTRING	118,3481
#define VNSM_NMA_EMBED_INT_TBL 119,3509
#define VNSM_NMA_EMBED_INT_STAT 120,3544
#define VNSM_NMA_TIME_TICKS	121,3579
#define VNSM_NMA_STAUS32	122,3610
#define VNSM_FLAG_USABLE	127,3708
#define VNSM_FLAG_SERVER	128,3740
#define VNSM_FLAG_NEIGHBOR	129,3772
#define VNSM_FLAG_DELETE_ME	130,3840
#define VNSM_FLAG_FOR_DOS	131,3875
#define VNSM_FLAG_WHO_KNOWS	132,3908
#define VNSM_FLAG_MODIFIED	133,3969
#define VNSM_FLAG_WHO_KNOWS2	134,4003
#define VNSM_FLAG_NON_VINES	135,4065
#define VNSM_FLAG_REDIRECT_C	136,4100
#define VNSM_FLAG_SUPPRESSION1	137,4162
#define VNSM_FLAG_SUPPRESSION2	138,4200
#define VNSM_FLAG_WAN	139,4238
#define VNSM_FLAG_LAN	140,4268
#define VNSM_FLAG_REDIRECT_M	141,4298
#define VNSM_FLAG_PRE_550	142,4361
#define VNSM_SVR_OSINFO	156,4654
#define VNSM_SVR_FSINFO	157,4702
#define VNSM_SVR_COMMINFO	158,4750
#define VNSM_SVR_CPUINFO	159,4799
#define VNSM_SVR_OS_UNIX	161,4848
#define VNSM_NUM_STACKS	163,4893
#define VNSM_NUM_PROTOCOLS	164,4921
#define VNSM_SVR_SUMMSTAT	171,5007
#define VNSM_SVR_FSSTAT	172,5056
#define VNSM_SVR_DISKSTAT	173,5104
#define VNSM_MAX_DISKS	175,5154
#define VNSM_LIF_SUPP_VINES	182,5266
#define VNSM_LIF_SUPP_IP	183,5313
#define VNSM_LIF_SUPP_ATALK	184,5357
#define VNSM_LIF_SUPP_ISO	185,5404
#define VNSM_LANIF_COMMSTATS	192,5513
#define VNSM_LANIF_IFSTATS	193,5565
#define VNSM_NUM_INTERFACES	195,5616
#define VNSM_FAMILY_VINES	204,5747
#define VNSM_FAMILY_DARPA	205,5775
#define VNSM_FAMILY_ATALK	206,5803
#define VNSM_FAMILY_ISO	207,5831
#define VNSM_FAMILY_IPX	208,5858
#define VNSM_PROT_VINESARP	211,5906
#define VNSM_PROT_VINESIP	212,5935
#define VNSM_PROT_VINESRTP	213,5963
#define VNSM_PROT_VINESSPP	214,5992
#define VNSM_TBL_VRTP_NBRS	217,6042
#define VNSM_TBL_VRTP_ROUTES	218,6087
typedef struct vnsm_pstk_pass_info_ vnsm_pstk_pass_info_231,6405
} vnsm_pstk_pass_info;vnsm_pstk_pass_info241,6620
#define VNSM_RTP_MAXTTL	243,6644
#define VNSM_RTP_NOTAGED 244,6713
#define VNSM_DATA_LIMIT	245,6792
#define VNSM_PERIOD	252,6912
typedef struct vnsm_send_addr_ vnsm_send_addr_258,7101
} vnsm_send_addr;vnsm_send_addr261,7184
#define VNSM_TRACE_GNET_VALID	268,7248
#define VNSM_TRACE_PNET_VALID	269,7305