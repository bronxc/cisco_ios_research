
frame_relay.h,9484
#define __FRAME_RELAY_H__227,8822
#define _FR_SVC_230,8886
#define FR_MAXLIST 243,9133
enum FR_LIST_TYPE FR_LIST_TYPE244,9156
    FR_DE_LIST,245,9176
    FR_DLCI_LIST 246,9192
typedef struct fr_plisttype_ fr_plisttype_248,9217
} fr_listtype;fr_listtype259,9479
# define LINK_IP_INHERIT_COMPR 262,9541
# define LINK_IP_INHERIT_ACTIVE_COMPR 263,9600
# define LINK_IP_INHERIT_PASSIVE_COMPR 264,9659
# define LINK_COMPR_TCP_AND_PASSIVE 265,9718
# define LINK_COMPR_TCP_AND_ACTIVE 266,9777
# define FR_LINK_ILLEGAL_SVC	267,9836
# define COMPRESS_OVER_IP_LINK(COMPRESS_OVER_IP_LINK269,9897
# define COMPRESS_OVER_COMPR_TCP_LINK(COMPRESS_OVER_COMPR_TCP_LINK273,10031
# define COMPRESS_OVER_P2P_LINK(COMPRESS_OVER_P2P_LINK276,10117
										# define NO_OPTIONS 280,10259
# define LINK_LEARN_OK(LINK_LEARN_OK281,10301
# define GET_PVC_DLCI(GET_PVC_DLCI284,10482
# define MAXDLCI 286,10524
# define MAXDLCI_VALUES	287,10557
# define FR_MIN_PKT_SIZE 288,10590
# define FRAME_RELAY_EA_BITS 289,10617
# define FR_INVALID_EA_BITS 290,10653
# define DLCI_PRESENT 291,10689
# define DLCI_ADDED 292,10713
# define DLCI_DELETED 293,10737
# define DLCI_STATUS_MASK 294,10761
# define DLCI_CHG_MASK 295,10792
# define DLCI_ACTIVE(DLCI_ACTIVE296,10823
# define IS_DLCI_DELETED(IS_DLCI_DELETED297,10855
# define DLCI_CONGESTED(DLCI_CONGESTED298,10891
# define FR_SIGNALING_DLCI 300,10927
# define DLCI0_MASK 301,10998
# define NULL_DLCI 302,11035
# define IGNORE_DLCI 303,11056
# define FR_N391_COUNTER_MAX	304,11093
# define FR_PVC_BW_NOTSET	305,11126
# define FRAME_RELAY_DEFAULT_MULTICAST_DLCI 306,11154
# define FRAME_RELAY_DEFAULT_LOCAL_DLCI 307,11200
# define FR_UNDEFINED_DLCI 308,11242
# define FRAME_RELAY_DEFAULT_DLCI_BITS 309,11276
# define FRAME_RELAY_SNAPENCAPBYTES 310,11318
# define FRAME_RELAY_Q933_NLPID_SIZE 311,11357
# define FRAME_RELAY_ENCAPBYTES 312,11401
# define FR_TRANSIT_ENCSIZE(FR_TRANSIT_ENCSIZE313,11435
# define FRAME_RELAY_SNAPENCAPWORDS	316,11570
# define FRAME_RELAY_ENCAPWORDS	317,11640
# define FRAME_RELAY_LMI_ENCAPBYTES 318,11703
# define FRAME_REALY_LMI_ENCAPWORDS	319,11741
# define FR_SNAP_NLPID 320,11807
# define FR_SNAP_CONTROL 321,11849
# define FR_Q933_NLPID 322,11889
# define L2_8022 323,11931
# define L3_SNA 324,11971
# define FR_L3_SNA 325,12011
# define L3_SNA_PERIPHERAL 326,12053
# define FR_L3_SNA_PERIPHERAL 327,12093
# define L3_APPN 328,12135
# define FR_L3_APPN 329,12175
# define L3_NETBIOS 330,12217
# define FR_L3_NETBIOS 331,12257
typedef struct fr_q933_hdr_ fr_q933_hdr_333,12300
} fr_q933_hdr;fr_q933_hdr339,12442
typedef struct frame_relay_hdr_ frame_relay_hdr_342,12459
} frame_relay_hdr;frame_relay_hdr348,12656
#define FR_COMPRESSED	355,12751
typedef struct fr_payload_compressed_hdr_ fr_payload_compressed_hdr_357,12778
} fr_payload_compressed_hdr;fr_payload_compressed_hdr360,12955
typedef struct fr_ietf_payload_compressed_hdr_ fr_ietf_payload_compressed_hdr_362,12985
} fr_ietf_payload_compressed_hdr;fr_ietf_payload_compressed_hdr368,13205
#define	FR_IETF_COMP_DCP_EXT	371,13281
# define FR_SNAP_HDRSIZE 374,13317
# define FR1490_ETHER_FCS	378,13401
# define FR1490_ETHER	379,13434
# define FR1490_802_4_FCS	380,13464
# define FR1490_802_4	381,13497
# define FR1490_802_5_FCS	382,13527
# define FR1490_802_5	383,13560
# define FR1490_FDDI_FCS	384,13590
# define FR1490_FDDI	385,13622
# define FR1490_802_6	386,13651
# define FR1490_SPANNING	387,13681
# define FR1490_FDDI_PADBYTES	389,13714
# define RELAYHASH 391,13747
# define DEF_BQSIZE 392,13769
# define DEF_BQBYTES 393,13793
# define DEF_BQPKTS 394,13821
# define MIN_DLCI 395,13846
# define MAX_DLCI 396,13867
# define MAX_FR_MGD_TIMER_COUNT 397,13890
# define FR_ALL_MAPPED 399,13925
# define FR_NOT_MAPPED 405,14113
# define FR_IP_MAPPED 406,14140
# define FR_IPX_MAPPED 407,14170
# define FR_AT_MAPPED 408,14200
# define FR_XNS_MAPPED 409,14230
# define FR_VINES_MAPPED 410,14260
# define FR_DNET_MAPPED 411,14290
# define FR_CLNS_MAPPED 412,14320
# define FR_BR_MAPPED 413,14350
# define FR_RSRB_MAPPED 414,14380
# define FR_STUN_MAPPED 415,14411
# define FR_LLC2_MAPPED 416,14442
# define FR_NOT_DYNAMIC 418,14474
# define FR_IP_DYNAMIC 419,14501
# define FR_IPX_DYNAMIC 420,14531
# define FR_AT_DYNAMIC 421,14561
# define FR_XNS_DYNAMIC 422,14591
# define FR_VINES_DYNAMIC 423,14621
# define FR_DNET_DYNAMIC 424,14653
#define FR_INTF_DTE	427,14687
#define FR_INTF_DCE	428,14709
enum FR_MC_TYPE FR_MC_TYPE429,14731
    FR_MULTICAST_NOTSET,430,14749
    FR_MULTICAST_STATIC,431,14774
    FR_MULTICAST_DYNAMIC 432,14799
enum PVC_USAGE_TYPE PVC_USAGE_TYPE434,14832
    PVC_LOCAL,435,14854
    PVC_SWITCHED_SERIAL,436,14869
    PVC_SWITCHED_IP_TUNNEL,437,14894
    PVC_SVC,438,14922
    PVC_NOTSET 439,14935
enum PVC_CONGESTION_INFO PVC_CONGESTION_INFO441,14954
    NOT_CONGESTED,442,14981
    FORWARD_CONGESTED,443,15000
    BACKWARD_CONGESTED 444,15023
enum FR_SHAPE_CONF FR_SHAPE_CONF446,15050
    SHAPE_CREATE,447,15071
    SHAPE_DELETE,448,15089
    SHAPE_UPDATE 449,15107
enum MAP_TYPE MAP_TYPE451,15128
    FR_MAP_STATIC,452,15144
    FR_MAP_DYNAMIC,453,15163
    FR_MAP_INT_DLCI,454,15183
    FR_MAP_INT_DLCI_SVC,455,15204
    FR_MAP_INARP,456,15229
    FR_MAP_AUTOINSTALL,457,15248
     FR_MAP_SVC_DYNAMIC}458,15272
enum PVC_TYPE PVC_TYPE460,15300
    PVC_STATIC,461,15316
    PVC_DYNAMIC,462,15332
    PVC_AUTOINSTALL,463,15350
    SVC_DYNAMIC}464,15371
enum FR_INTF_TYPE FR_INTF_TYPE470,15518
    FR_DTE,471,15538
    FR_DCE,472,15550
    FR_NNI 473,15562
enum FR_TIMER_TYPE FR_TIMER_TYPE475,15577
    FR_INARP_TIMER,476,15598
    FR_LMI_TIMER,477,15618
    FR_SWITCH_TIMER478,15636
#define FR_FORCE_STATE	485,15702
#define FR_ASSIGN_STATE	486,15727
#define FR_PVC_STATE	487,15753
#define FR_HWIF_STATE	488,15776
typedef struct fr_traffic_t_ fr_traffic_t_493,15842
} fr_traffic_t;fr_traffic_t502,16061
typedef struct fr_pvc_stats_type_ fr_pvc_stats_type_510,16257
} fr_pvc_stats_type;fr_pvc_stats_type532,17635
struct fr_idb_class fr_idb_class535,17658
static inline ulong fr_pvc_input_pkts 542,17782
static inline ulong fr_pvc_output_pkts 547,17916
static inline ulong fr_pvc_input_bytes 552,18053
static inline ulong fr_pvc_output_bytes 557,18186
static inline ulong fr_pvc_input_fecn_pkts 562,18322
static inline ulong fr_pvc_input_becn_pkts 567,18469
static inline ulong fr_pvc_input_de_pkts 572,18616
static inline void clear_fr_pvc_stats 577,18757
typedef struct fr_route_type_ fr_route_type_583,18872
} fr_route_type;fr_route_type588,19099
enum FR_ENCAPS_FORM FR_ENCAPS_FORM589,19116
    FR_ENCAPS_NULL 590,19138
    FR_ENCAPS_NOTSET 591,19164
    FR_ENCAPS_CISCO 592,19190
    FR_ENCAPS_IETF 593,19216
    FR_ENCAPS_CISCO2 594,19242
enum FR_CLEAR FR_CLEAR596,19274
    FR_PVC 597,19290
    FR_PVC = 0x1,597,19290
    FR_MAP 598,19308
    FR_MAP = 0x2,598,19308
    FR_BOTH 599,19326
    FR_BOTH = 0x3,599,19326
    FR_LMI_CLEAR 600,19345
    FR_LMI_CLEAR = 0x4600,19345
typedef struct frame_relay_map_ frame_relay_map_603,19378
} frmaptype;frmaptype619,19874
struct fr_svc_info_type_ fr_svc_info_type_624,19927
struct fr_svc_nli_type_ fr_svc_nli_type_674,23032
struct fr_pvc_info_type_ fr_pvc_info_type_695,24069
#define this_dlci 722,25435
struct fr_bootp_stuff_ fr_bootp_stuff_741,26180
struct fr_idb_struct_type_ fr_idb_struct_type_746,26384
#define FR_BQ_INT 752,26655
# define FALSE_BUT_CONTINUE 812,28904
struct frame_relay_multicast_struct frame_relay_multicast_struct814,28940
struct frame_relay_pvc_struct frame_relay_pvc_struct823,29175
# define MULTICAST_SOURCE_DLCI_INDEX 831,29333
# define MULTICAST_DLCI_INDEX 832,29372
# define MULTICAST_DLCI_STATUS_INDEX 833,29404
# define PVC_DLCI_INDEX 835,29444
# define PVC_STATUS_INDEX 836,29470
# define PVC_BANDWIDTH_INDEX 837,29498
#define NUM_TO_DLCI(NUM_TO_DLCI846,29637
#define DLCI_TO_NUM(DLCI_TO_NUM849,29824
#define BR_NUM_TO_DLCI(BR_NUM_TO_DLCI854,30010
#define BR_DLCI_TO_NUM(BR_DLCI_TO_NUM857,30182
#define NUM_TO_ANSI_DLCI(NUM_TO_ANSI_DLCI861,30352
#define ANSI_DLCI_TO_NUM(ANSI_DLCI_TO_NUM863,30433
#define HWIDB_TO_FR_PVC(HWIDB_TO_FR_PVC864,30504
#define FR_ANSI_VALID_PVC_MASK 870,30669
#define FR_ANSI_EA_BIT 871,30707
#define FR_ANSI_VALID_STATUS_MASK 875,30761
#define FR_SNAP1PAD 877,30801
#define FR_SNAP0PAD 878,30831
#define FR_OUI23_BRIDGE 879,30861
#define NLPID_DEF(NLPID_DEF881,30894
#define ISFR_FECN(ISFR_FECN884,30964
#define ISFR_BECN(ISFR_BECN885,30993
#define ISFR_DE(ISFR_DE886,31022
#define FR_FECN_BIT 888,31050
#define FR_BECN_BIT 889,31077
#define FR_DE_BIT 890,31104
#define FR_CR_BIT 891,31131
#define Q922_TEST 892,31158
#define Q921_DLCI 897,31222
#define Q922MAR90_DLCI 898,31267
#define Q922NOV90_DLCI 899,31312
#define GET_FR_IDB(GET_FR_IDB902,31418
inline static boolean fr_pak_is_compressed(906,31557
inline static boolean fr_valid_linktype_change(911,31682
inline static boolean fr_valid_encaps_change(916,31805
inline static boolean fr_valid_link_linktype_combo(923,32000
static inline void INC_PVC_SWITCHED 929,32178
static inline void INC_PVC_FAST_INPUT 934,32283
static inline void INC_PVC_FAST_OUTPUT 953,32895
static inline ushort link2nlpid 973,33455
static inline ulong nlpid2link 998,34133
static inline uint fr_svc_get_exponential(1023,34730