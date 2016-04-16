#include <stdio.h>

#define NANOSWF_IMPLEMENTATION
#include "nanoswf.h"


char *tag_names[100];

void NSWF_TagNamesInit(void) {
    tag_names[0] = "End";
    tag_names[1] = "ShowFrame";
    tag_names[2] = "DefineShape";
    tag_names[4] = "PlaceObject";
    tag_names[5] = "RemoveObject";
    tag_names[6] = "DefineBits";
    tag_names[7] = "DefineButton";
    tag_names[8] = "JPEGTables";
    tag_names[9] = "SetBackgroundColor";
    tag_names[10] = "DefineFont";
    tag_names[11] = "DefineText";
    tag_names[12] = "DoAction";
    tag_names[13] = "DefineFontInfo";
    tag_names[14] = "DefineSound";
    tag_names[15] = "StartSound";
    tag_names[17] = "DefineButtonSound";
    tag_names[18] = "SoundStreamHead";
    tag_names[19] = "SoundStreamBlock";
    tag_names[20] = "DefineBitsLossless";
    tag_names[21] = "DefineBitsJPEG2";
    tag_names[22] = "DefineShape2";
    tag_names[23] = "DefineButtonCxform";
    tag_names[24] = "Protect";
    tag_names[26] = "PlaceObject2";
    tag_names[28] = "RemoveObject2";
    tag_names[32] = "DefineShape3";
    tag_names[33] = "DefineText2";
    tag_names[34] = "DefineButton2";
    tag_names[35] = "DefineBitsJPEG3";
    tag_names[36] = "DefineBitsLossless2";
    tag_names[37] = "DefineEditText";
    tag_names[39] = "DefineSprite";
    tag_names[43] = "FrameLabel";
    tag_names[45] = "SoundStreamHead2";
    tag_names[46] = "DefineMorphShape";
    tag_names[48] = "DefineFont2";
    tag_names[56] = "ExportAssets";
    tag_names[57] = "ImportAssets";
    tag_names[58] = "EnableDebugger";
    tag_names[59] = "DoInitAction";
    tag_names[60] = "DefineVideoStream";
    tag_names[61] = "VideoFrame";
    tag_names[62] = "DefineFontInfo2";
    tag_names[64] = "EnableDebugger2";
    tag_names[65] = "ScriptLimits";
    tag_names[66] = "SetTabIndex";
    tag_names[69] = "FileAttributes";
    tag_names[70] = "PlaceObject3";
    tag_names[71] = "ImportAssets2";
    tag_names[73] = "DefineFontAlignZones";
    tag_names[74] = "CSMTextSettings";
    tag_names[75] = "DefineFont3";
    tag_names[76] = "SymbolClass";
    tag_names[77] = "Metadata";
    tag_names[78] = "DefineScalingGrid";
    tag_names[82] = "DoABC";
    tag_names[83] = "DefineShape4";
    tag_names[84] = "DefineMorphShape2";
    tag_names[86] = "DefineSceneAndFrameLabelData";
    tag_names[87] = "DefineBinaryData";
    tag_names[88] = "DefineFontName";
    tag_names[89] = "StartSound2";
    tag_names[90] = "DefineBitsJPEG4";
    tag_names[91] = "DefineFont4";
    tag_names[93] = "EnableTelemetry";
};

void TagCallback(u32 TagCode, void *TagPtr) {
    printf("Tag: %s\n", tag_names[TagCode]);
    switch (TagCode) {
        case TagCode_DefineShape4: {
//                NSWF_shape *shape = NSWF_SWFDataReadTagDEFINESHAPE4(SWFData);
        } break;
        default: {
        } break;
    }
}

int main(s32 argc, char *argv[]) {
//	u32 u = 65529U;
//	s16 s = (s16)u;
//	printf("u: %d s: %d\n", u, s);
//	getchar();
//}
//
//int crap() {
    NSWF_TagNamesInit();

    NSWF_swfdata *SWFData = NSWF_SWFDataInitFromFile("Assets/test2.swf");

    NSWF_SWFDataReadTags(SWFData, TagCallback);

    NSWF_SWFDataFree(SWFData);

    getchar();
    return 0;
}