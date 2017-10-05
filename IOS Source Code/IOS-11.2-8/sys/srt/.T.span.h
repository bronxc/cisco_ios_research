
span.h,3331
#define SPAN_MAXINDEX 162,6487
#define MAX_SSE_BRIDGE_GROUPS 166,6674
#define SPAN_MAXPORTS	172,6909
#define SPAN_DEC_MAXAGE	174,6949
#define SPAN_DEC_HELLOTIME	175,7014
#define SPAN_DEC_FORWARDDELAY	176,7068
#define SPAN_DEC_DEFAULT_PRIORITY0 177,7127
#define SPAN_DEC_DEFAULT_PRIORITY1 178,7170
#define SPAN_DEC_BRIDGE_PRIORITY 179,7214
#define	SPAN_DEC_PATHDIVISOR	180,7258
#define	SPAN_DEC_DEFAULT_PATHCOST	181,7292
#define SPAN_DEFAULT_TCTIME	183,7330
#define SPAN_DEFAULT_TCNTIME	184,7362
#define SPAN_DEFAULT_HOLDTIME	185,7395
#define SPAN_PROTO_IEEE	187,7457
#define SPAN_PROTO_DEC	188,7520
#define SPAN_PROTO_IBM	190,7603
#define SPAN_IEEE_MAXAGE	192,7665
#define SPAN_IEEE_HELLOTIME	193,7712
#define SPAN_IEEE_FORWARDDELAY	194,7761
#define SPAN_IEEE_HOLDTIME	195,7814
#define SPAN_IEEE_BRIDGE_PRIORITY 196,7873
#define SPAN_IEEE_PORT_PRIORITY	197,7936
#define	SPAN_IEEE_PATHDIVISOR	198,7988
#define	SPAN_IEEE_DEFAULT_PATHCOST	199,8022
#define SPAN_IBM_MAXAGE	202,8084
#define SPAN_IBM_HELLOTIME	203,8130
#define SPAN_IBM_FORWARDDELAY	204,8161
#define SPAN_IBM_PATHDIVISOR	205,8210
#define SPAN_IBM_DEFAULT_PATHCOST	206,8248
#define SPAN_IBM_PORT_PRIORITY 207,8285
#define SPAN_IBM_MESSAGE_AGE_PASSTHRU 208,8348
#define IDBYTES	212,8589
#define MESSAGE_AGE_INCREMENT	213,8610
#define PRIORITY(PRIORITY214,8643
#define SPAN_DEFAULT_PRIORITY 215,8690
#define	CCG_MAXGROUPS	218,8789
#define	CCG_MAXCIRCUITS	219,8816
#define CCG_DEFAULT_PAUSE_INTERVAL	220,8845
typedef struct sls_ sls_234,9202
} sls;sls238,9281
#define PERMIT_PORT_SIZE 240,9289
typedef enum bte_state_t_ bte_state_t_247,9466
}bte_state_t;bte_state_t254,9600
struct btetype_ btetype_258,9618
#define BRIDGE_FORWARD	319,11898
#define BRIDGE_DISCARD	320,11923
#define BRIDGE_RECEIVE	321,11948
#define BTE_STATIC 326,12014
#define BTE_IFSPECIFIED 327,12043
#define BTE_LAT 328,12072
#define BTE_ALIQUOT	330,12128
#define BTE_MINIMUM	331,12152
#define BTE_GC	332,12175
#define BTE_MAXAGE	333,12203
typedef struct ccgtype_ ccgtype_339,12275
} ccgtype;ccgtype347,12704
struct spantype_ spantype_352,12756
#define VTP_DOMAIN_NAME_MAX_LENGTH 392,14733
#define PORT_DISABLED	436,16085
#define PORT_BLOCKING	437,16109
#define PORT_LISTENING	438,16133
#define PORT_LEARNING	439,16158
#define PORT_FORWARDING	440,16182
#define PORT_BROKEN 441,16208
#define BPDU_PROTOCOL 454,16526
#define BPDU_VERSION	461,16825
#define BPDU_TYPE_CONFIG 462,16853
#define BPDU_TYPE_TCN 463,16884
#define BPDU_TCN_SIZE	464,16915
#define BPDU_CONFIG_BYTES 465,16941
#define BPDU_TCN_BYTES	466,16971
typedef struct bpduhdrtype_ bpduhdrtype_468,16999
} bpduhdrtype;bpduhdrtype483,17537
#define bpduheadstart(bpduheadstart485,17553
#define SPAN_DEC_SHORTTIME 491,17670
#define DEC_CODE 493,17702
#define DEC_VERSION 494,17730
#define DEC_BPDU_LENGTH 496,17758
#define DEC_TYPE_HELLO 498,17887
#define DEC_TYPE_TCN 499,17915
typedef struct dechdrtype_ dechdrtype_501,17944
} dechdrtype;dechdrtype517,18605
#define decheadstart(decheadstart519,18620
typedef struct shifaQtype_ shifaQtype_529,19077
} shifaQtype;shifaQtype532,19158
#define TYPE_LAT	538,19211
#define LAT_ZEROSIZE 539,19236
#define LAT_ONEOFFSET 540,19289
#define LAT_ENTRYOVERHEAD	541,19316
#define LAT_SERVICE	547,19393