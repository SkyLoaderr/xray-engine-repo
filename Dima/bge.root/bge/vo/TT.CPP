//////////////////////////////////////////////////////////////////////////////////
//	Module			:	transposition table										//
//	File			:	tt.cpp													//
//	Creation date	:	15.02.2001												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "board.h"
#include "game.h"
#include "misc.h"
#include "tt.h"
#include "_time.h"

#ifdef STATIC_TT
	SHashCell taHash[1 << MAX_TT_HASH_BITS];
#else
	u8 MAX_TT_HASH_BITS = 22;
	SHashCell *taHash;
#endif

const SHashValue	 taHashBoard[91*HASH_VALUES] = {
	0xaa526c4a8dcac977,0x6097cf586ca8f0bd,0xb15eabb65d1bb693,0x8ffcbfcac3375f4,0x13f30c007ce20a10,0x1b0cc7fcd0d17fe4,
	0x704ee0e40ca87879,0xb1394262277157ef,0x760e77b009879a75,0x1e019ac0b376fe40,0x18410d94b6591c4c,0x6409754052fe20c,
	0x9efadc4ecef7098b,0x181f5fbc8be252b1,0x939f657a16486767,0x52b695d4002a060c,0x5fd32ce0d89568e0,0xd65b934d8bf6eec,
	0x3ffba508714add2d,0x29f209e6260d4d83,0x265a9394b885b5e9,0xd69e35600308cbb0,0xcf3f03fccac7a374,0x19a1369cc9cf68c4,
	0x8bb935927ed5d7df,0x7c7cb760800598e5,0x5a22947e3336627b,0xff688c0c2eb3c944,0x2ef32de063507ce0,0xd19ba1ec4de3b5a4,
	0xfeb7dc6c12a78221,0xdd6f12aa86238957,0xfc430eb8beaead9d,0xf81ab300a50d1710,0xfaee61d4090438ac,0x2f4d2d4ac092fbc,
	0x27dedc16498e2873,0xa0429a4473009759,0x17dd5cc2993f5bcf,0x6ea9699477bfff4c,0x5eaae940a70e8cf0,0x300380d4d0b173bc,
	0x8140b105ea0fb55,0x656d40aecdf47f6b,0x72da2d1c0e6dd591,0xce81c2a05e199920,0xb44fe4ac0ed4b7e4,0x7ace260c50cd2ec4,
	0x26d473dac4bb2f47,0x16490c68c51b620d,0x78882246d68d4763,0xb7f7813c8e1616b4,0xe9abd0a09c207e90,0x5e5c519c12366824,
	0x64b1f4f409701cc9,0x3458b7f21c2ce3bf,0xd04f72c0a6c0c1c5,0x3d216a20276c11a0,0x89dfec1488dcccac,0xb4fe8634afb0dd0c,
	0xe3dde0deda40605b,0x57d151cc9bac4d01,0x5a22890a31ae5937,0xd4c577d4378227ec,0x6d3a1e00dc6c1e80,0xb9ff69d4ebee396c,
	0x68289e18b317fa7d,0xa090dc767039aa53,0xdf93a3a4c36b4639,0xa635d0c02ed2dfb0,0x118eed7c5eae63f4,0xb7bb3dbc707cbc44,
	0x231e47229d5d6faf,0x3de9ee70d00bec35,0x47b7750e1f69054b,0xa368346c52e2daa4,0xc7c10640d0d6b040,0x64a9322c82346ae4,
	0x4e284a7c8919e871,0xdcec523a94dc0727,0xd53cc3c8e56b76ed,0xd0e63ee0ec374210,0x4bf2b7548045dc8c,0x9b1489b46c729e9c,
	0xbdc0aa63e025143,0x1212a654d2c913a9,0xb9610a5292cfff9f,0x72450314dd86dfec,0xc0f803e0714b7130,0xb2bd00f4accdaedc,
	0x1ade7e2038697aa5,0x827cfd3ee0fd6e3b,0x63aa172cc7ada7e1,0x38e04500bad0fa00,0x41942c0c45142744,0x7974690cffc4dd44,
	0xc004cf6a0e633917,0xfa207d78571cd75d,0x3d8cacd6db963c33,0x1e9a7ebc110ad8b4,0xe3121d00c4ffdd90,0xfd8863bcd5f50524,
	0x4299fd04c3a08519,0x8f94018242c3938f,0x426821d094006d15,0x14ca8f404b43cd40,0x143b53941ce3254c,0xf1dcd457a0e80c,
	0x34f1796ececc9b2b,0xc361b7dcda9e8b51,0xef7f009a06a2ef07,0xf528185441bc578c,0x2ea661a089d223a0,0xdb8e79f4c86e742c,
	0xdcaecb2811731bcd,0x5c30648e46b23,0xacd4a7b4ec489a89,0x75e7cba0392cc270,0x59da73cc4174334,0x707a6c9cfd3b8144,
	0x316a2cb2b3b72b7f,0x5101d9805d37c385,0xdf17e99ef4e58c1b,0x57209f0c34527c44,0xb95d5a207300db20,0xee7dc52c4752a764,
	0x659a2c8c476392c1,0x62ade5ca1e3a28f7,0x9c02acd89bf5443d,0x2799e600184f5f10,0xde016654c4d989ec,0xf9988054dc96d6fc,
	0x94ea4d36da1bde13,0x31ada664dd5853f9,0x4dd68be29c69c76f,0xa54d8294dcb1a74c,0x7c7144409ac3be30,0xd93cc6d44672197c,
	0x5466a53044b67df5,0x10334dceab97410b,0xbfde753c05b3be31,0x76db97e033a4cce0,0x9d6347ec72a10f24,0xebb8d00c4105c3c4,
	0x4c247efa1e07e6e7,0x207722886c6a50ad,0xe51d4b6627ab9503,0x1c5cc6fcd641f674,0xb565f360efed8590,0xa939359c39ac73e4,
	0x74ff914d1a6b169,0xe70c1f1253da675f,0x7b9184e0a1639c65,0x45ab34e06f39c8e0,0x397549141ffce5ec,0x7cde7df470c52d0c,
	0x96c6a5fe8770b9fb,0x74f991ecef860da1,0x49b5cc2ae72b28d7,0xe47af2d4952d16ac,0x3b099b00f5768780,0xdf7369d4605b912c,
	0xcba72c3856d9411d,0xb0c1bd96fe428ff3,0x69269fc41e4ab2d9,0xd18d407d880ef0,0xa2503ebc351bfd34,0xa281b3fc4893f3c4,
	0x827de642c1480b4f,0xc6bd7890e0661ed5,0xb094f22e4940f6eb,0xaf1f25ac34a6dbe4,0x9df631c0bcae2640,0x32e9146c8808fda4,
	0x4f6829c31118111,0x9474cd5a6302eec7,0x286dc9e8fd89158d,0x652faba0030bc890,0x49b4e0d4cf935c0c,0x2c9b4b74cc98949c,
	0xbf3aa3c64ecfcee3,0x47dc9a74a29b5849,0x58dee1724c31b33f,0xc634d414732ad8ac,0x21d096a071d4a570,0xe7e442b402fe7ddc,
	0xb7658040f5250545,0x3aa1325ece16f7db,0x7d20474cb8cd1881,0x9eb7da00eb5bdc80,0x54f21d0ca6b3c144,0xca45c70c4de81dc4,
	0x30b4828a732e38b7,0x9ae5fb984100cdfd,0xf2afdf61e8251d3,0x4cf9a97cc202aff4,0x7367d600afaec690,0x3f9e7f7c6dac6964,
	0xf95ce924382fa1b9,0x862210a2cc565f2f,0x449bf0c9474fb5,0x5f50244098912340,0xa648569469f9cd4c,0xf91872d4f168ee0c,
	0xe12e668eff41bccb,0x2801dffca76fd3f1,0x1e07ebbae18c06a7,0xef309b54db63e68c,0x10191660c5ae5ce0,0xff298d341ecdba6c,
	0x106ac148b0076a6d,0x675cc2676c918c3,0xe8d28bd422de8f29,0x3791e8e0c997a430,0xcf29a27c5b4e4174,0xf8b84a9c92d9e544,
	0x397a73d279b50f1f,0x3555cba0acb3fe25,0x4fbf8ebec25045bb,0xa3dcea0cf3638844,0xd51917604886c2e0,0x76c5fd6cbbe54aa4,
	0x3d664cacbff0b361,0x874208ea503b5897,0xaf971af8f7a3eadd,0xf1e3bd003a188190,0x6312eb54724bd82c,0x92f15654485359bc,
	0xa23e0e56555323b3,0x46a8828480bf2099,0xe55b0b02dc8cc30f,0x7ac52694c1b3d04c,0x3da023c0486c30f0,0x4765055489dfe0bc,
	0x7bd40f50c5921095,0x6117aaeee91192ab,0x92588d5cd786b6d1,0x2a7988a0a31717a0,0xc3f50aacb103b1e4,0xe98c820c1214a644,
	0xf275da1a299b2e87,0x884608a8531d4f4d,0xa6e6c4869c0f72a3,0xf796633c52920e34,0xa3057da0e7065210,0x54931e9cb5945c24,
	0xb889cd34ba285609,0x6f76d632bd5c7aff,0x3a3a670080488705,0x8c7a1e20a094ce20,0xec9b4149af41f2c,0x82b3aa343a60d10c,
	0x4f39bb1e2194a39b,0x4923a20c45a8de41,0x70f65f4a904a8877,0xe0e35254486265ec,0xdf2cb600f9bc4e00,0x3fcfe454b1de2bec,
	0x43928a58fbfa97bd,0xd012eeb6012d0593,0x9616be4f1b12f79,0x452a406d1c1930,0x4ab6cbfc6757a1f4,0x4af3e1bc0a4bb8c4,
	0x60c0d56214e336ef,0xc843d2b0ff7e6175,0xe368bf4ee628788b,0xf21dc2ec84d62a24,0x71b5a8c0761d6440,0x83a86a2cf2cb4e64,
	0x81528abc540e29b1,0xcf56987a4f286667,0xd3d7a0089a02c42d,0xc5b7976048aa9190,0x9732bdd486a67c0c,0x52852ab4ce0ced9c,
	0xa58ce6ff1adc83,0xd95a5e947430ace9,0x156c0892fc1ff6df,0x1500f91415af69ec,0xc97d6016aa43b0,0x15c9847403052a5c,
	0x1eeb52608c1a9fe5,0xa627b77e6d5c117b,0x7b0476caead9921,0x78a15f005e71bb80,0x61fa4a0c7cc6bd44,0x195b150c22b706c4,
	0xc56985aac953c857,0x24b049b8b33cd49d,0x72c19f16c487f773,0x8f4779bcea3b1034,0x38ef6300e7ef2f10,0xb7a81abc0dd43f24,
	0x4ddfa54428bdce59,0xeceb6fc29c51bacf,0x886be61005444255,0xbdb626c00495cb40,0x78026594296c474c,0xc5b443542df98c0c,
	0xf39a3ae99fe6e6b,0x647d81cd3be2c91,0x264226daea2bae47,0xf93351d4a415b10c,0xd048d4a0d7c07120,0x297b857473d5c02c,
	0xcaf787681befc90d,0xe7ca254624635663,0xc19c3ff400af93c9,0xff60b5a0b94cf670,0xf40b0d3ca20cacb4,0xb6bb89c1b405ac4,
	0x79f20af21ef782bf,0x50408dc0506248c5,0xb5a183deeb1e8f5b,0x3cd7a10c26fcf444,0xf0842820d315f9a0,0xcc53892cf5e90de4,
	0xb4643ccc83b6e401,0x10337c0a154f1837,0xb4c8591866a2a17d,0x7dcd8a00ee6d8a90,0x7d61efd40b79cfec,0xac65d4e514457c,
	0xd8621f7685dbf953,0x747b2ea4679cfd39,0xb272da229dd04eaf,0x1f3f5b9467cc804c,0x752f9ec07fc737b0,0x6a10c554180bb7fc,
	0x73244970c91bb335,0x7fa2580e6c0b744b,0x690757c314ebf71,0xa0048260c0c5f8e0,0xd5b0be6c3890f4a4,0x75b43c0cf8550c44,
	0x1cd0853a169d0627,0xd97dbec8541c5ded,0xa46c8da6d460e043,0xa4279c7c1dfc8e74,0x1c9b94e0df016810,0xb8bc089cc2fde664,
	0x3ba77154b35d0aa9,0x1fa0dd5212db1e9f,0x4f121920b15781a5,0x89846b603e0cace0,0xfd3103143c0627ec,0x74b56874020a8b0c,
	0xd2bf203ea4541d3b,0x5e97822c717cbee1,0x90ec426a12347817,0x174d6f54fec8f52c,0x551e0d0048a89000,0x42536254b660652c,
	0xe9b2b8784363fe5d,0xc30c6fd66fa10b33,0x318c08049406bc19,0x2bb6c8c0662784f0,0xf38878bcb142c6b4,0xd83eb07cd7654244,
	0xdef14824856f28f,0x5344fcd0a13cb415,0xc3badc6e87c78a2b,0x49c88facbb7b0de4,0x879d474074ea7540,0xce55c8eccf9178a4,
	0x3b8462dc6b77e251,0x8c99b39a74746e07,0xff424628a1c082cd,0xaaed5aa04834df10,0x6e2b7e548283bf8c,0xc4c624f4cab7609c,
	0xf2a4c6061b8b7a23,0x65d3f2b483f11189,0xbd107fb2c8c2ca7f,0x1f52214d6b3e82c,0x4e419ba005fa5870,0x4fb4b9b4d349b05c,
	0xb037f480f7324a85,0x46988c9e6674bb1b,0xe9a2178c48b729c1,0x760c3400b343a100,0x2f99d70c0cc6c244,0x5995e30cbf856344,
	0x3b2bd8cae1fbe7f7,0x4d4767d85ab8eb3d,0x68d89036f04f2d13,0xfca958fc3777f5f4,0xaf5a100026c33f10,0x53f348fc11b4cae4,
	0x8d6a316437b30af9,0x8ff81ee2cedda66f,0x7a6003047df44f5,0x5168b1c01482d140,0xdba4809464ee9f4c,0x8acc3154706c4e0c,
	0x5e9b30cedcaab00b,0x327ba03ca4f19531,0x335b1fa27a9e5e7,0xe878bed41ed8430c,0xb5d63fe0e5db16e0,0x5dae8134fb0355ec,
	0xd01d1d88481437ad,0x28ace664a5b2403,0xb279c4140e23a869,0x69c5a560d9f8bfb0,0xba17cfc9fcf2074,0x6264d99c46379fc4,
	0x3cd8f21235a6865f,0x1c8a1fe0ce2aa365,0x4e45c8fec2f868fb,0x8bf9780c9152f844,0xf96442e0660c16e0,0x729d3aecf75eeea4,
	0xcdbfcecfe1e24a1,0xb68a3f2a2a9d67d7,0xfd5e6738a1d9681d,0x92e2f70016c43c10,0x63676cd4490370ac,0xf1859bd45fc74cbc,
	0x73de8096ba5e5ef3,0xe46daac48059e9d9,0xfd25f942685c6a4f,0xc07efc947585b04c,0x4e858540a78784f0,0x8efb79d4d20234bc,
	0x831f5390db3b65d5,0xc75b552e1e2ce5eb,0x4cce2d9c2473d811,0x52a3d7a0a7df0e20,0x9d72a9ac5897b3e4,0xcfd17e0cff48bdc4,
	0xc23c805ad8356dc7,0x73e644e86e4f7c8d,0x7836a6c6f547dde3,0xd2e64d3c068892b4,0x68ec6ba02bfa2290,0xba0a269c2d72b024,
	0xe7f0e57491accf49,0xbd923472527e523f,0x50e09b4046788c45,0x344400208986eda0,0x83547e145e52aeac,0xb7107e34d7d4430c,
	0x1aded55e0f5726db,0x539d324caa69af81,0x5e9f758a1610f7b7,0xd434e8d421f96bec,0x9075480038beba80,0x4441a0d41947d16c,
	0xabcfb698f1fd74fd,0x23640f6c446a0d3,0xe6ee74243fb358b9,0x3dee9bc06675cbb0,0x70cf597ca83be7f4,0x4d21c2bcce4e2c44,
	0x4e10a3a24fcb3e2f,0xcc88f6f0dd8916b5,0x6913498ec3c62bcb,0x7cad106c83cf0ea4,0x5baefa400fc21b40,0x2703ea2c8c0d15e4,
	0xbfd40afc54b6aaf1,0xbf461ebab20f05a7,0xa675bc481faa516d,0xc0050de030256710,0xd9a4bb547b399c8c,0x19a1b6b44b1cfb9c,
	0x6bc04f26f4c9a7c3,0x209156d4f2448629,0x91d446d21c422e1f,0x6873bf1473869bec,0x9267b6e09b0d1230,0xfa1409f4e88b89dc,
	0x1e1366a0d4540525,0xd17bb1be6508f4bb,0x1d3db7ac8a51ca61,0x1570a90072d90d00,0x165e780c2cdcc244,0x32ed10c5e05cf44,
	0x43037bea524e9797,0x9e7355f8085d11dd,0xa5f7d156487ff2b3,0xe2b22bbcdda2d0b4,0x4468100c793b590,0xe6f4aabc1a316524,
	0x99448d84eb775799,0xaf501e02c422220f,0xfebaea5035005795,0x88dc5b406ca24640,0xef223c94b2d5464c,0x67fe67d4de77000c,
	0xa2db0dee08ee81ab,0x94e5385cc4720dd1,0xa3ea8d1a652ead87,0xee391a543cdc938c,0xef089aa0511cbfa0,0x13180f46dc02c2c,
	0xb7a383a84b5cb64d,0x693fc786655881a3,0xcab31834b7a2cd09,0x7aba70a04f7a7f70,0x7aaeb3cb3840434,0x7d109b9cfcfe7b44,
	0x4037293293ea19ff,0x68fa82004ff50e05,0x8b345e1ea185d29b,0x3af8730c73f0ef44,0xf1fb0420419f2420,0xcb03772c326fcb64,
	0x9d158d0cfe8e7541,0x274e524a114e4777,0xaf21455886303ebd,0x5159b8000afe4a10,0x636d7054724001ec,0x3234c85478be4bfc,
	0x653b31b6c5825493,0x53c7f6e49d5de679,0x817c6862085915ef,0xb8d31940884c04c,0xefca6840c55f8130,0xe44759d4cddb417c,
	0x488d2db02bd92875,0xc7caa24eed1de78b,0xa959b5bc265e00b1,0xb7a5b8e056c3d4e0,0x567120ec5b44fc24,0xe1d4980c0d8728c4,
	0xcdc1cb7a258c6567,0x8b479b08c49eab2d,0x70cd0fe6a76c6b83,0x22bc3bfcfe72f374,0x9fb0ff607c92fd90,0xbd0cc49c82e00ee4,
	0xa8ae29940d7fa3e9,0x8352db92be6e15df,0x2a6ded60f593a6e5,0x700999e0d67d80e0,0xf2ca5d142e9185ec,0x82c3c4f4f8ec050c,
	0x5520da7e6645c07b,0xda7cb26c8bd7b021,0x8317f8aa09080757,0x805de5d41f5f84ac,0x566ac70070124380,0xd63722d46f4dc72c,
};

const SHashValue tBlackColor      = 0x3033181e0267749b;
const SHashValue tWhiteColor      = 0xe905b70c9fe74741;
const SHashValue tChangeColor     = tBlackColor ^ tWhiteColor;

u32  dwRandSeed        = 0;
char  cGlobalTimeStamp  = 0;
char  cBugTimeStamp     = 0;
bool  bInHash           = false;

u64  qwVariationCount  = 0;
#ifdef _DEBUG
u32  dwTTUseCount		= 0;
u32  dwTTAddCount		= 0;
u32  dwTTCollision		= 0;
u32  dwTTBADCollision	= 0;
#endif

int ifGetTTNode(SHashValue tpHashValue, u8 *ucpLevel)
{
	TIMER_START(GetTTNode)
#ifdef NO_HASH
	TIMER_STOP(GetTTNode)
	return(-1);
#endif
#ifdef _DEBUG
	dwTTUseCount++;
#endif
	u32 dwIndex = (u32)(tpHashValue & (((u32)1 << MAX_TT_HASH_BITS) - 1));
	u32 dwHashValue = (u32)((tpHashValue >> 32) & 0xffffffff);
	
	*ucpLevel = 0;

	if ((dwHashValue == taHash[dwIndex].taCells[*ucpLevel].tHash) && (taHash[dwIndex].taCells[*ucpLevel].ucMove)) {
#ifdef _DEBUG
		if ((tGlobalSearchParameters.taBoard[taHash[dwIndex].taCells[*ucpLevel].ucMove] != EMPTY) && (taHash[dwIndex].taCells[*ucpLevel].ucMove != MOVE_PASS)) {
			dwTTBADCollision++;
			vfShowBoard(BLACK,0,0);
			SHashValue tHashValue = 0;
			vfCreateHashValue(tGlobalSearchParameters.taBoard,&tHashValue,BLACK);
		}
#endif
		TIMER_STOP(GetTTNode)
		return(dwIndex);
	}
	else {
		
		*ucpLevel = 1;

		if ((dwHashValue == taHash[dwIndex].taCells[*ucpLevel].tHash) && (taHash[dwIndex].taCells[*ucpLevel].ucMove)) {
#ifdef _DEBUG
			if ((tGlobalSearchParameters.taBoard[taHash[dwIndex].taCells[*ucpLevel].ucMove] != EMPTY) && (taHash[dwIndex].taCells[*ucpLevel].ucMove != MOVE_PASS)) {
				dwTTBADCollision++;
				vfShowBoard(BLACK,0,0);
				SHashValue tHashValue = 0;
				vfCreateHashValue(tGlobalSearchParameters.taBoard,&tHashValue,BLACK);
			}
#endif
			TIMER_STOP(GetTTNode)
			return(dwIndex);
		}
		else {
			TIMER_STOP(GetTTNode)
			return(-1);
		}
	}
}

bool bfGetEndgameTTNode(SHashValue tpHashValue, STT_Structure &tHashCell)
{
	TIMER_START(GetEndgameTTNode)
	u32 dwIndex = (u32)(tpHashValue & (((u32)1 << MAX_TT_HASH_BITS) - 1));
	u32 dwHashValue = (u32)((tpHashValue >> 32) & 0xffffffff);
	tHashCell = taHash[dwIndex].taCells[0];

	if ((dwHashValue == tHashCell.tHash) && (tHashCell.ucMove)) {
		TIMER_STOP(GetEndgameTTNode)
		return(true);
	}
	else {
		tHashCell = taHash[dwIndex].taCells[1];
		if ((dwHashValue == tHashCell.tHash) && (tHashCell.ucMove)) {
			TIMER_STOP(GetEndgameTTNode)
			return(true);
		}
		else {
			TIMER_STOP(GetEndgameTTNode)
			return(false);
		}
	}
}

void vfPutTTNode(SHashValue tpHashValue, u8 ucDepth, SFeature tAlpha, SFeature tBeta, SFeature tValue, u8 ucMove) 
{
#ifdef NO_HASH
	return(0);
#endif
	bInHash = true;
#ifdef _DEBUG
	dwTTAddCount++;
#endif
	TIMER_START(PutTTNode)
	
	u32 dwIndex = (u32)(tpHashValue & (((u32)1 << MAX_TT_HASH_BITS) - 1));
	u32 dwHashValue = (u32)((tpHashValue >> 32) & 0xffffffff);
	if (tValue < tAlpha)
		tValue = tAlpha;
	if (tValue > tBeta)
		tValue = tBeta;
	
	STT_Structure *tpDeepest = taHash[dwIndex].taCells, *tpNewest = taHash[dwIndex].taCells + 1;
	if (tpDeepest->tHash == dwHashValue) {
		if (tpDeepest->ucDepth < ucDepth) {
			tpDeepest->ucDepth = ucDepth;	
			tpDeepest->ucMove  = ucMove;
			tpDeepest->tAlpha  = -INFINITY;
			tpDeepest->tBeta   = +INFINITY;
			if (tValue < tBeta)
				tpDeepest->tBeta = tValue;
			if (tValue > tAlpha)
				tpDeepest->tAlpha = tValue;
		}
		else {
			if (tpDeepest->ucDepth == ucDepth) {
				tpDeepest->ucMove  = ucMove;
				if ((tValue < tBeta) && (tValue < tpDeepest->tBeta)) 
					tpDeepest->tBeta = tValue;
				if ((tValue > tAlpha) && (tValue > tpDeepest->tAlpha)) 
					tpDeepest->tAlpha = tValue;
			}
			else {
				TIMER_STOP(PutTTNode)
				return;
			}
		}
	}
	else
		if (tpNewest->tHash == dwHashValue) {
			if (tpNewest->ucDepth < ucDepth) {
				tpNewest->ucDepth = ucDepth;	
				tpNewest->ucMove  = ucMove;
				tpNewest->tAlpha  = -INFINITY;
				tpNewest->tBeta   = +INFINITY;
				if (tValue < tBeta)
					tpNewest->tBeta = tValue;
				if (tValue > tAlpha)
					tpNewest->tAlpha = tValue;
			}
			else {
				if (tpNewest->ucDepth == ucDepth) {
					tpNewest->ucMove  = ucMove;
					if ((tValue < tBeta) && (tValue < tpNewest->tBeta)) 
						tpNewest->tBeta = tValue;
					if ((tValue > tAlpha) && (tValue > tpNewest->tAlpha)) 
						tpNewest->tAlpha = tValue;
				}
				else {
					TIMER_STOP(PutTTNode)
					return;
				}
			}
		}
		else 
			if (tpDeepest->ucDepth < ucDepth) {
				if (tpNewest->ucDepth < tpDeepest->ucDepth)
					taHash[dwIndex].taCells[1] = taHash[dwIndex].taCells[0];
				tpDeepest->tHash  = dwHashValue;	
				tpDeepest->ucDepth = ucDepth;	
				tpDeepest->ucMove  = ucMove;
				tpDeepest->tAlpha  = -INFINITY;
				tpDeepest->tBeta   = +INFINITY;
				if (tValue < tBeta)
					tpDeepest->tBeta = tValue;
				if (tValue > tAlpha)
					tpDeepest->tAlpha = tValue;
			}
			else {
				tpNewest->tHash  = dwHashValue;	
				tpNewest->ucDepth = ucDepth;	
				tpNewest->ucMove  = ucMove;
				tpNewest->tAlpha  = -INFINITY;
				tpNewest->tBeta   = +INFINITY;
				if (tValue < tBeta)
					tpNewest->tBeta = tValue;
				if (tValue > tAlpha)
					tpNewest->tAlpha = tValue;
			}
	bInHash = false;
	TIMER_STOP(PutTTNode)
}

void vfPutEndgameTTNode(SHashValue tpHashValue, u8 ucDepth, SFeature tAlpha, SFeature tBeta, SFeature tValue, u8 ucMove) 
{
	TIMER_START(PutEndgameTTNode)
	ucDepth++;

#ifdef NO_HASH
	TIMER_STOP(PutEndgameTTNode)
	return(0);
#endif
	bInHash = true;
#ifdef _DEBUG
	dwTTAddCount++;
#endif
	
	u32 dwIndex = (u32)(tpHashValue & (((u32)1 << MAX_TT_HASH_BITS) - 1));
	u32 dwHashValue = (u32)((tpHashValue >> 32) & 0xffffffff);
	
	if (tValue < tAlpha)
		tValue = tAlpha;
	if (tValue > tBeta)
		tValue = tBeta;

	STT_Structure *tpDeepest = taHash[dwIndex].taCells, *tpNewest = taHash[dwIndex].taCells + 1;
	if (tpDeepest->tHash == dwHashValue) {
		if (tpDeepest->ucDepth < ucDepth) {
			tpDeepest->ucDepth = ucDepth;	
			tpDeepest->ucMove  = ucMove;
			tpDeepest->tAlpha  = -INFINITY;
			tpDeepest->tBeta   = +INFINITY;
			if (tValue < tBeta)
				tpDeepest->tBeta = tValue;
			if (tValue > tAlpha)
				tpDeepest->tAlpha = tValue;
		}
		else {
			if (tpDeepest->ucDepth == ucDepth) {
				tpDeepest->ucMove  = ucMove;
				tpDeepest->tAlpha  = -INFINITY;
				tpDeepest->tBeta   = +INFINITY;
				if (tValue < tBeta)
					tpDeepest->tBeta = tValue;
				if (tValue > tAlpha)
					tpDeepest->tAlpha = tValue;
			}
			else {
				TIMER_STOP(PutEndgameTTNode)
				return;
			}
		}
	}
	else
		if (tpNewest->tHash == dwHashValue) {
			if (tpNewest->ucDepth < ucDepth) {
				tpNewest->ucDepth = ucDepth;	
				tpNewest->ucMove  = ucMove;
				tpNewest->tAlpha  = -INFINITY;
				tpNewest->tBeta   = +INFINITY;
				if (tValue < tBeta)
					tpNewest->tBeta = tValue;
				if (tValue > tAlpha)
					tpNewest->tAlpha = tValue;
			}
			else {
				if (tpNewest->ucDepth == ucDepth) {
					tpNewest->ucMove  = ucMove;
					tpNewest->tAlpha  = -INFINITY;
					tpNewest->tBeta   = +INFINITY;
					if (tValue < tBeta)
						tpNewest->tBeta = tValue;
					if (tValue > tAlpha)
						tpNewest->tAlpha = tValue;
				}
				else {
					TIMER_STOP(PutEndgameTTNode)
					return;
				}
			}
		}
		else 
			if (tpDeepest->ucDepth < ucDepth) {
				if (tpNewest->ucDepth < tpDeepest->ucDepth)
					taHash[dwIndex].taCells[1] = taHash[dwIndex].taCells[0];
				tpDeepest->tHash  = dwHashValue;	
				tpDeepest->ucDepth = ucDepth;	
				tpDeepest->ucMove  = ucMove;
				tpDeepest->tAlpha  = -INFINITY;
				tpDeepest->tBeta   = +INFINITY;
				if (tValue < tBeta)
					tpDeepest->tBeta = tValue;
				if (tValue > tAlpha)
					tpDeepest->tAlpha = tValue;
			}
			else {
				tpNewest->tHash  = dwHashValue;	
				tpNewest->ucDepth = ucDepth;	
				tpNewest->ucMove  = ucMove;
				tpNewest->tAlpha  = -INFINITY;
				tpNewest->tBeta   = +INFINITY;
				if (tValue < tBeta)
					tpNewest->tBeta = tValue;
				if (tValue > tAlpha)
					tpNewest->tAlpha = tValue;
			}
	bInHash = false;
	TIMER_STOP(PutEndgameTTNode)
}

void vfCreateHashValue(TBoardCell *taBoard, SHashValue *tpHashValue, TBoardCell tColor)
{
    *tpHashValue = 0;
    
	for (int i=0; i<BOARD_SIZE; i++)
        if (taBoard[i] != DUMMY)
            (*tpHashValue) ^= taHashBoard[HASH_VALUES*i + taBoard[i]];
    
	(*tpHashValue) ^= (tColor == BLACK ? tBlackColor : tWhiteColor);
}

void vfInitalizeHash(u32 dwStartSeed)
{
	vfDualPrintF("Initilazing transposition table...");

	#ifdef STATIC_TT
		memset(taHash,0,sizeof(taHash));
	#else
		taHash = (SHashCell *)calloc(1 << MAX_TT_HASH_BITS,sizeof(SHashCell));
	#endif

	vfDualPrintF("completed\n  deepest-newest 2-level x %d bits = %.1fMb (%d bytes)\n",MAX_TT_HASH_BITS,f64((1 << MAX_TT_HASH_BITS)*sizeof(SHashCell))/1048576.0,(1 << MAX_TT_HASH_BITS)*sizeof(SHashCell));
}
