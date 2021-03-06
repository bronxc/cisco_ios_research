
mlpvt_mq_protocol.h,2084
#define __MLPVT_MQ_PROTOCOL_H__48,1658
#define MLPVT_YOU_ARE_MASTER	50,1691
enum mq_states mq_states53,1754
    MQ_STATE_UNKNOWN 54,1771
    MQ_STATE_QUERY_TO_PEERS,55,1797
    MQ_STATE_QUERY_DONE,56,1826
    MQ_STATE_QUERY_ABORTED,57,1851
    MQ_STATE_QUERY_FROM_PEERS58,1879
enum pb_states pb_states62,1939
    PB_STATE_CREATED 63,1956
    PB_STATE_BID_OPENED_TO_PEER,64,1982
    PB_STATE_BID_OPENED_FROM_PEER,65,2015
    PB_STATE_BID_SENT,66,2050
    PB_STATE_BID_RCVD,67,2073
    PB_STATE_BID_XCHGD_PASSIVE,68,2096
    PB_STATE_BID_XCHGD_ACTIVE,69,2128
    PB_STATE_BID_CLOSED,70,2159
    PB_STATE_DELETED71,2184
enum pb_substates pb_substates75,2238
    PB_SUBSTATE_NONE 76,2258
    PB_SUBSTATE_WE_WON,77,2284
    PB_SUBSTATE_PEER_WON78,2308
#define MLPVT_P2P_UNKNOWN	83,2353
#define MLPVT_P2P_MQO	84,2382
#define MLPVT_P2P_MB	85,2411
#define MLPVT_P2P_BA	86,2439
#define MLPVT_P2P_MQC	87,2467
#define MLPVT_P2P_MQA	88,2496
#define MLPVT_P2P_MRR	89,2525
#define MLPVT_MQO_TIMER	91,2555
#define MLPVT_MB_TIMER	92,2592
#define MLPVT_MQO_RETRIES	93,2628
#define MLPVT_MB_RETRIES	94,2657
#define MLPVT_MQ_PROCESS_WAKEUP	95,2685
#define MLPVT_MQ_MSG_MINSIZE	97,2723
#define MLPVT_MQ_MSG_MAXSIZE	98,2756
#define MLPVT_MQ_MSG_TYPE	101,2812
#define MLPVT_MQ_PEER_ID	102,2841
#define MLPVT_MQ_MEMBER_ID	103,2869
#define MLPVT_MQ_BUNDLE	104,2914
#define MLPVT_MQ_OUR_REFERENCE	105,2942
#define MLPVT_MQ_PEER_REFERENCE	106,2976
#define MLPVT_MQ_ECHO_OF_BID_SEEN	108,3012
#define MLPVT_MQ_NEW_BID	109,3049
#define MLPVT_MQ_MASTER_ID	110,3078
enum mq_result mq_result113,3165
    MLPVT_NO_MASTER 114,3182
    MLPVT_WE_ARE_MASTER,115,3207
    MLPVT_PEER_IS_MASTER116,3232
#define MLPVT_MAX_HANDLES	119,3261
#define MLPVT_REF_MASK	120,3324
typedef struct PB_state PB_state122,3357
} PB_state_t;PB_state_t134,3882
typedef struct MQ_state MQ_state136,3897
} MQ_state_t;MQ_state_t152,4652
} msgfield_t;msgfield_t158,4818
} MQmsg_t;MQmsg_t168,5141
typedef struct vpntype_t vpntype_t171,5173
} vpntype_test;vpntype_test175,5255
