
smf_public.h,2283
#define	__SMF_PUBLIC_H__107,3837
#define	SMF_RESULT_NOT_FOUND	121,4376
#define SMF_MACTYP_MASK	123,4413
#define SMF_ACTION_MASK	124,4452
enum SMF_ACTION_BIT SMF_ACTION_BIT126,4498
    SMF_ACTION_RECEIVE_BIT	128,4521
    SMF_ACTION_FORWARD_BIT	129,4555
    SMF_ACTION_DISCARD_BIT	130,4589
    SMF_ACTION_MAXIMUM_BIT	134,4677
#define SMF_ACTION_RECEIVE	137,4714
#define SMF_ACTION_FORWARD	138,4772
#define SMF_ACTION_DISCARD	139,4830
enum SMF_MACTYP_BIT SMF_MACTYP_BIT141,4889
    SMF_MACTYP_OWN_UNICAST_BIT	143,4912
    SMF_MACTYP_PHYS_BCAST_BIT	144,4950
    SMF_MACTYP_FDDI_TRANSIT_BIT	145,4987
    SMF_MACTYP_STANDBY_UNICAST_BIT	146,5026
    SMF_MACTYP_IEEE_SPAN_BIT	148,5068
    SMF_MACTYP_DEC_SPAN_BIT	149,5104
    SMF_MACTYP_FUNC_SPAN_BIT	150,5139
    SMF_MACTYP_IEEE_IBM_SPAN_BIT	151,5175
    SMF_MACTYP_CDP_MCAST_BIT	152,5214
    SMF_MACTYP_AT_BCAST_BIT	154,5251
    SMF_MACTYP_AT_ZONE_BIT	155,5286
    SMF_MACTYP_CLNS_MCAST_BIT	156,5320
    SMF_MACTYP_DECNET_MCAST_BIT	157,5357
    SMF_MACTYP_HP_MCAST_BIT	159,5397
    SMF_MACTYP_IP_MCAST_BIT	160,5432
    SMF_MACTYP_LAT_MCAST_BIT	161,5467
    SMF_MACTYP_LAT_UNICAST_BIT 162,5503
    SMF_MACTYP_APOLLO_MCAST_BIT	164,5550
    SMF_MACTYP_MCAST_RANGE_BIT	166,5590
    SMF_MACTYP_ISL_VLAN_MCAST_BIT 167,5628
    SMF_MACTYP_VTP_MCAST_BIT 168,5674
    SMF_MACTYP_BVI_BIT 170,5721
    SMF_MACTYP_MAXIMUM_BIT	174,5808
#define SMF_MACTYP_OWN_UNICAST	177,5845
#define SMF_MACTYP_PHYS_BCAST	178,5911
#define SMF_MACTYP_FDDI_TRANSIT	179,5975
#define SMF_MACTYP_STANDBY_UNICAST	180,6043
#define SMF_MACTYP_IEEE_SPAN	182,6117
#define SMF_MACTYP_DEC_SPAN	183,6179
#define SMF_MACTYP_FUNC_SPAN	184,6239
#define SMF_MACTYP_IEEE_IBM_SPAN	185,6301
#define SMF_MACTYP_CDP_MCAST	186,6370
#define SMF_MACTYP_AT_BCAST	188,6433
#define SMF_MACTYP_AT_ZONE	189,6493
#define SMF_MACTYP_CLNS_MCAST	190,6551
#define SMF_MACTYP_DECNET_MCAST	191,6615
#define SMF_MACTYP_HP_MCAST	193,6684
#define SMF_MACTYP_IP_MCAST	194,6744
#define SMF_MACTYP_LAT_MCAST	195,6804
#define SMF_MACTYP_LAT_UNICAST 196,6866
#define SMF_MACTYP_APOLLO_MCAST	198,6941
#define SMF_MACTYP_ISL_VLAN_MCAST	200,7010
#define SMF_MACTYP_MCAST_RANGE	202,7082
#define SMF_MACTYP_BVI 204,7149
#define SMF_MACTYP_VTP_MCAST 206,7202