/*
TODO
- SWFData
- Tags:
 - FileAttributes
 - SetBackgroundColor
 - DefineSceneAndFrameLabelData
 - DefineShape4
 - DefineSprite
 - DefineButton2
 - ShowFrame
 - DefineSound
 - DefineText2
 - PlaceObject2
 - RemoveObject2
 - StartSound2
- SWFPlayer
 - Convert shapes to fast-to-render format
 - Frames
 - Play, Stop
 - MovieClips
 - Sounds
 - Buttons (with callback)

SWFPlayer does the following:
- Loads the data
- Keeps track of header info, characters, scenes, and frames
- the tag parser calls a callback for each tag which SWFPlayer uses to setup the frame
- on ShowFrame, clone the current frame and set it current
- at this point we should have a fully ready to use version of the SWF in memory and the raw data freed
- expose the capability to move through frames (play, stop, goto)
- expose the frame as a data structure that is easily rendered in nanovg

Usage:
(in pseudo-C)

FILE *f = fopen("test.swf", "rb");
void* data = freadall(f);
SWFData *swfData = NSWF_SWFDataInit(data, data.length, callback);

void callback(SWFData *data, SWFTagCode code, void* tagptr)
{
	// handle the tag
	switch (code) {
		case TagCode_DefineShape4:
			TagDefineShape4 *tag = (TagDefineShape4 *)tagptr;
			break;
	}
}

*/

#ifndef NANOSWF_H
#define NANOSWF_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define NSWF_FLOAT16_EXPONENT_BASE 15
#define MIN_VALUE(a, b) ((a < b) ? (a) : (b))
#define MAX_VALUE(a, b) ((a > b) ? (a) : (b))

#pragma region Types

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef float r32;
typedef double r64;

typedef union {
	u32 Unsigned;
	s32 Signed;
	r32 Real;
} NSWF_number;

typedef struct {
    s32		XMin;
    s32		XMax;
    s32		YMin;
    s32		YMax;
} NSWF_rect;

typedef struct {
    r32		ScaleX;
    r32		ScaleY;
    r32		RotateSkew0;
    r32		RotateSkew1;
    s32		TranslateX;
    s32		TranslateY;
} NSWF_matrix;

typedef struct {
    s32		RedMultTerm;
    s32		GreenMultTerm;
    s32		BlueMultTerm;
    s32		AlphaMultTerm;
    s32		RedAddTerm;
    s32		GreenAddTerm;
    s32		BlueAddTerm;
    s32		AlphaAddTerm;
} NSWF_cxform;

typedef struct {
    u8				Signature[3];
    u8				Version;
    u32				FileLength;
    NSWF_rect		FrameSize;
    r32				FrameRate;
    u16				FrameCount;
} NSWF_swfheader;

typedef struct {
    NSWF_swfheader  Header;
    u32             OwnsData;
	void * 		    Data;
	u8 *		    Head;
	u32			    Length;
	u32			    ByteIndex;
	u32			    BitsPending;
	void * 			TagMemory;
	u32				TagMemoryUsed;
    u32             TagMemoryTotal;
} NSWF_swfdata;

typedef struct {
	u16				TagCode;
	u32				Length;
} NSWF_tag_header;

typedef enum {
	TagCode_End = 0,
	TagCode_ShowFrame = 1,
	TagCode_DefineShape = 2,
	TagCode_PlaceObject = 4,
	TagCode_RemoveObject = 5,
	TagCode_DefineBits = 6,
	TagCode_DefineButton = 7,
	TagCode_JPEGTables = 8,
	TagCode_SetBackgroundColor = 9,
	TagCode_DefineFont = 10,
	TagCode_DefineText = 11,
	TagCode_DoAction = 12,
	TagCode_DefineFontInfo = 13,
	TagCode_DefineSound = 14,
	TagCode_StartSound = 15,
	TagCode_DefineButtonSound = 17,
	TagCode_SoundStreamHead = 18,
	TagCode_SoundStreamBlock = 19,
	TagCode_DefineBitsLossless = 20,
	TagCode_DefineBitsJPEG2 = 21,
	TagCode_DefineShape2 = 22,
	TagCode_DefineButtonCxform = 23,
	TagCode_Protect = 24,
	TagCode_PlaceObject2 = 26,
	TagCode_RemoveObject2 = 28,
	TagCode_DefineShape3 = 32,
	TagCode_DefineText2 = 33,
	TagCode_DefineButton2 = 34,
	TagCode_DefineBitsJPEG3 = 35,
	TagCode_DefineBitsLossless2 = 36,
	TagCode_DefineEditText = 37,
	TagCode_DefineSprite = 39,
	TagCode_FrameLabel = 43,
	TagCode_SoundStreamHead2 = 45,
	TagCode_DefineMorphShape = 46,
	TagCode_DefineFont2 = 48,
	TagCode_ExportAssets = 56,
	TagCode_ImportAssets = 57,
	TagCode_EnableDebugger = 58,
	TagCode_DoInitAction = 59,
	TagCode_DefineVideoStream = 60,
	TagCode_VideoFrame = 61,
	TagCode_DefineFontInfo2 = 62,
	TagCode_EnableDebugger2 = 64,
	TagCode_ScriptLimits = 65,
	TagCode_SetTabIndex = 66,
	TagCode_FileAttributes = 69,
	TagCode_PlaceObject3 = 70,
	TagCode_ImportAssets2 = 71,
	TagCode_DefineFontAlignZones = 73,
	TagCode_CSMTextSettings = 74,
	TagCode_DefineFont3 = 75,
	TagCode_SymbolClass = 76,
	TagCode_Metadata = 77,
	TagCode_DefineScalingGrid = 78,
	TagCode_DoABC = 82,
	TagCode_DefineShape4 = 83,
	TagCode_DefineMorphShape2 = 84,
	TagCode_DefineSceneAndFrameLabelData = 86,
	TagCode_DefineBinaryData = 87,
	TagCode_DefineFontName = 88,
	TagCode_StartSound2 = 89,
	TagCode_DefineBitsJPEG4 = 90,
	TagCode_DefineFont4 = 91,
	TagCode_EnableTelemetry = 93
} NSWF_tag_type;

typedef struct {
	u8								Ratio;
	u32								Color;
} NSWF_gradientrecord;

typedef struct {
	u8								SpreadMode;
	u8								InterpolationMode;
	NSWF_gradientrecord	*			GradientRecords;
	u16								GradientRecordsCount;
	r32								FocalPoint;
} NSWF_gradient;

typedef enum {
	NSWF_FillStyleType_SolidFill = 0x00,
	NSWF_FillStyleType_LinearGradient = 0x10,
	NSWF_FillStyleType_RadialGradient = 0x12,
	NSWF_FillStyleType_FocalRadialGradient = 0x13,
	NSWF_FillStyleType_RepeatingBitmap = 0x40,
	NSWF_FillStyleType_ClippedBitmap = 0x41,
	NSWF_FillStyleType_NonSmoothedRepeating = 0x42,
	NSWF_FillStyleType_NonSmoothedClipped = 0x43
} NSWF_fillstyletype;

typedef struct {
	NSWF_fillstyletype				Type;
	union {
		u32							Color;
		struct {
			NSWF_gradient			Gradient;
			NSWF_matrix 			GradientMatrix;
		};
		struct {
			u32						BitmapId;
			NSWF_matrix				BitmapMatrix;
		};
	};
} NSWF_fillstyle;

typedef enum {
	NSWF_LineStyleFlags_HasFillFlag = 0x1,
	NSWF_LineStyleFlags_NoHScaleFlag = 0x2,
	NSWF_LineStyleFlags_NoVScaleFlag = 0x4,
	NSWF_LineStyleFlags_PixelHintingFlag = 0x8,
	NSWF_LineStyleFlags_NoCloseFlag = 0x10
} NSWF_linestyleflags;

typedef struct {
	u16								Width;
	u8								StartCapStyle;
	u8								EndCapStyle;
	u8								JoinStyle;
	u8								Flags;
	u16								MiterLimitFactor;
	union {
		u32							Color;
		NSWF_fillstyle				FillStyle;
	};
} NSWF_linestyle;

typedef struct {
	NSWF_fillstyle *				FillStyles[];
	u32								FillStylesCount;
} NSWF_fillstylearray;

typedef struct {
	NSWF_linestyle *				LineStyles[];
	u32								LineStylesCount;
} NSWF_linestylearray;

typedef enum {
	NSWF_ShapeRecord_StraightEdge,
	NSWF_ShapeRecord_CurvedEdge,
	NSWF_ShapeRecord_StyleChange
} NSWF_shaperecordtype;

typedef struct {
	s32								DeltaX;
	s32								DeltaY;
} NSWF_straightedgerecord;

typedef struct {
	s32								ControlDeltaX;
	s32								ControlDeltaY;
	s32								AnchorDeltaX;
	s32								AnchorDeltaY;
} NSWF_curvededgerecord;

typedef struct {
	u32								Flags;
	s32								MoveDeltaX;
	s32								MoveDeltaY;
	u32								FillStyle0;
	u32								FillStyle1;
	NSWF_fillstylearray				FillStyles;
	NSWF_linestylearray				LineStyles;
} NSWF_stylechangerecord;

typedef struct {
	NSWF_shaperecordtype			Type;
	union {
		NSWF_straightedgerecord		StraightEdge;
		NSWF_curvededgerecord		CurvedEdge;
		NSWF_stylechangerecord		StyleChange;
	};
} NSWF_shaperecord;

typedef struct {
	NSWF_fillstylearray				FillStyles;
	NSWF_linestylearray				LineStyles;
	NSWF_shaperecord *				ShapeRecords;
	u32								ShapeRecordsCount;
} NSWF_shape;

//typedef struct {
//
//} NSWF_buttonrecord;

//typedef struct {
//
//} NSWF_symbol;

//typedef struct {
//
//} NSWF_soundinfo;
//
//typedef struct {
//
//} NSWF_soundenvelope;

#pragma endregion Types

#pragma region Tags

typedef enum {
	NSWF_DefineShapeFlags_UseFillWindingRule = 0x04,
	NSWF_DefineShapeFlags_UsesNonScalingStrokes = 0x02,
	NSWF_DefineShapeFlags_UsesScalingStrokes = 0x01
} NSWF_defineshapeflags;

typedef struct {
	u16				CharacterID;
	NSWF_rect		ShapeBounds;
	NSWF_rect		EdgeBounds;
	u8				Flags;
	NSWF_shape *	Shapes;
} NSWF_tag_defineshape;

#pragma endregion Tags

#endif

typedef void (*NSWF_tagcallback)(u32, void* );

NSWF_swfdata* NSWF_SWFDataInit(void* Data, u32 Length);
NSWF_swfdata* NSWF_SWFDataInitFromFile(const char* Filename);
void NSWF_SWFDataFree(NSWF_swfdata* SWFData);

void NSWF_SWFDataReadTags(NSWF_swfdata* SWFData, NSWF_tagcallback TagCallback);

//NSWF_swfplayer *NSWF_SWFPlayerInit(const char* Filename);
//NSWF_swfplayer *NSWF_SWFPlayerFree(NSWF_swfplayer *SWFPlayer);

#ifdef NANOSWF_IMPLEMENTATION

void NSWF_SWFDataSkip(NSWF_swfdata* SWFData, u32 bytes) {
	SWFData->Head += bytes;
	SWFData->ByteIndex += bytes;
	SWFData->BitsPending = 0;
}

u8 NSWF_SWFDataAdvance(NSWF_swfdata* SWFData) {
	u8 Result = *SWFData->Head;
	++SWFData->Head;
	++SWFData->ByteIndex;
	return(Result);
}

void NSWF_SWFDataResetBitsPending(NSWF_swfdata* SWFData) {
	SWFData->BitsPending = 0;
}

NSWF_number _NSWF_SWFDataReadBits(NSWF_swfdata* SWFData, u32 Bits, u32 BitBuffer) {
	NSWF_number Result;
	Result.Unsigned = BitBuffer;
	if (Bits == 0) {
		return Result;
	}
	u32 Partial = 0;
	u32 BitsConsumed = 0;
	if (SWFData->BitsPending > 0) {
		u8 Byte = SWFData->Head[-1] & (0xff >> (8 - SWFData->BitsPending));
		BitsConsumed = MIN_VALUE(SWFData->BitsPending, Bits);
		SWFData->BitsPending -= BitsConsumed;
		Partial = Byte >> SWFData->BitsPending;
	} else {
		BitsConsumed = MIN_VALUE(8, Bits);
		SWFData->BitsPending = 8 - BitsConsumed;
		Partial = NSWF_SWFDataAdvance(SWFData) >> SWFData->BitsPending;
	}
	Bits -= BitsConsumed;
	Result.Unsigned = (Result.Unsigned << BitsConsumed) | Partial;
	return (Bits > 0) ? _NSWF_SWFDataReadBits(SWFData, Bits, Result.Unsigned) : Result;
}

NSWF_number NSWF_SWFDataReadBits(NSWF_swfdata* SWFData, u32 Bits) {
	NSWF_number Result = _NSWF_SWFDataReadBits(SWFData, Bits, 0);
	return(Result);
}

s8 NSWF_SWFDataReadS8(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	s8 Result = *((s8 *)SWFData->Head);
	NSWF_SWFDataSkip(SWFData, 1);
	return(Result);
}

s16 NSWF_SWFDataReadS16(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	s16 Result = *((s16 *)SWFData->Head);
	NSWF_SWFDataSkip(SWFData, 2);
	return(Result);
}

s32 NSWF_SWFDataReadS32(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	s32 Result = *((s32 *)SWFData->Head);
	NSWF_SWFDataSkip(SWFData, 4);
	return(Result);
}

u8 NSWF_SWFDataReadU8(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	u8 Result = *((u8 *)SWFData->Head);
	NSWF_SWFDataSkip(SWFData, 1);
	return(Result);
}

u16 NSWF_SWFDataReadU16(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	u16 Result = *((u16 *)SWFData->Head);
	NSWF_SWFDataSkip(SWFData, 2);
	return(Result);
}

u32 NSWF_SWFDataReadU32(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	u32 Result = *((u32 *)SWFData->Head);
	NSWF_SWFDataSkip(SWFData, 4);
	return(Result);
}

u32 NSWF_SWFDataReadU24(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	u16 LowWord = NSWF_SWFDataReadU16(SWFData);
	u8 HighByte = NSWF_SWFDataReadU8(SWFData);
	u32 Result = (HighByte << 16) | LowWord;
	return(Result);
}

r32 NSWF_SWFDataReadFIXED(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	r32 Result = (r32)NSWF_SWFDataReadU32(SWFData) / 65536.0f;
	return(Result);
}

r32 NSWF_SWFDataReadFIXED8(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	r32 Result = (r32)NSWF_SWFDataReadU16(SWFData) / 256.0f;
	return(Result);
}

r32 NSWF_SWFDataReadFLOAT(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	r32 Result = *((r32 *)SWFData->Head);
	NSWF_SWFDataSkip(SWFData, 4);
	return(Result);
}

r64 NSWF_SWFDataReadDOUBLE(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	r64 Result = *((r64 *)SWFData->Head);
	NSWF_SWFDataSkip(SWFData, 8);
	return(Result);
}

r32 NSWF_SWFDataReadFLOAT16(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	s16 Word = NSWF_SWFDataReadS16(SWFData);
	s16 Sign = ((Word & 0x8000) != 0) ? -1 : 1;
	u16 Exponent = (Word >> 10) & 0x1f;
	u16 Significand = Word & 0x3ff;
	if (Exponent == 0) {
		if (Significand == 0) {
			return 0;
		} else {
			// subnormal number
			return (r32)(Sign * (1 << ((1 - NSWF_FLOAT16_EXPONENT_BASE) * (Significand / 1024))));
		}
	}
	if (Exponent == 31) { 
		if (Significand == 0) {
			return (Sign < 0) ? INFINITY : -INFINITY;
		} else {
			return NAN;
		}
	}
	// normal number
	return (r32)(Sign * (1 << (Exponent - NSWF_FLOAT16_EXPONENT_BASE) * (1 + Significand / 1024)));
}

u32 NSWF_SWFDataReadEncodedU32(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	u32 Result = NSWF_SWFDataReadU8(SWFData);
	if (Result & 0x80) {
		Result = (Result & 0x7f) | (NSWF_SWFDataReadU8(SWFData) << 7);
		if (Result & 0x4000) {
			Result = (Result & 0x3fff) | (NSWF_SWFDataReadU8(SWFData) << 14);
			if (Result & 0x200000) {
				Result = (Result & 0x1fffff) | (NSWF_SWFDataReadU8(SWFData) << 21);
				if (Result & 0x10000000) {
					Result = (Result & 0xfffffff) | (NSWF_SWFDataReadU8(SWFData) << 28);
				}
			}
		}
	}
	return(Result);
}

/////////////////////////////////////////////////////////
// Bit values
/////////////////////////////////////////////////////////

u32 NSWF_SWFDataReadUB(NSWF_swfdata* SWFData, u32 bits) {
	u32 Result = NSWF_SWFDataReadBits(SWFData, bits).Unsigned;
	return(Result);
}

s32 NSWF_SWFDataReadSB(NSWF_swfdata* SWFData, u32 bits) {
	u32 Shift = 32 - bits;
	s32 Result= (s32)(NSWF_SWFDataReadBits(SWFData, bits).Unsigned << Shift) >> Shift;
	return(Result);
}

r32 NSWF_SWFDataReadFB(NSWF_swfdata* SWFData, u32 bits) {
	r32 Result = (r32)NSWF_SWFDataReadBits(SWFData, bits).Unsigned / 65536.0f;
	return(Result);
}

/////////////////////////////////////////////////////////
// String
/////////////////////////////////////////////////////////

u32 NSWF_SWFDataReadString(NSWF_swfdata* SWFData, char Buffer[], u32 BufferLength) {
	NSWF_SWFDataResetBitsPending(SWFData);
	
	char* Walker = (char* )SWFData->Head;
	u32 Length = 0;

	while (*Walker && BufferLength) {
		*(Buffer++) = *(Walker++);
		++Length;
		--BufferLength;
	}
	*Buffer = 0;

	return Length;
}

/////////////////////////////////////////////////////////
// Labguage code
/////////////////////////////////////////////////////////

u8 NSWF_SWFDataReadLANGCODE(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	return NSWF_SWFDataReadU8(SWFData);
}

/////////////////////////////////////////////////////////
// Color records
/////////////////////////////////////////////////////////

u32 NSWF_SWFDataReadRGB(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	u8 R = NSWF_SWFDataReadU8(SWFData);
	u8 G = NSWF_SWFDataReadU8(SWFData);
	u8 B = NSWF_SWFDataReadU8(SWFData);
	return 0xff000000 | (R << 16) | (G << 8) | B;
}

u32 NSWF_SWFDataReadRGBA(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	u32 RGB = NSWF_SWFDataReadRGB(SWFData) & 0x00ffffff;
	u8 A = NSWF_SWFDataReadU8(SWFData);
	return A << 24 | RGB;
}

u32 NSWF_SWFDataReadARGB(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	u8 A = NSWF_SWFDataReadU8(SWFData);
	u32 RGB = NSWF_SWFDataReadRGB(SWFData) & 0x00ffffff;
	return A << 24 | RGB;
}
/////////////////////////////////////////////////////////
// Rectangle record
/////////////////////////////////////////////////////////

NSWF_rect NSWF_SWFDataReadRECT(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	NSWF_rect Result;
	u32 Nbits = NSWF_SWFDataReadUB(SWFData, 5);
	Result.XMin = NSWF_SWFDataReadSB(SWFData, Nbits);
	Result.XMax = NSWF_SWFDataReadSB(SWFData, Nbits);
	Result.YMin = NSWF_SWFDataReadSB(SWFData, Nbits);
	Result.YMax = NSWF_SWFDataReadSB(SWFData, Nbits);

	return(Result);
}

/////////////////////////////////////////////////////////
// Matrix record
/////////////////////////////////////////////////////////

NSWF_matrix NSWF_SWFDataReadMATRIX(NSWF_swfdata* SWFData) {
	NSWF_SWFDataResetBitsPending(SWFData);
	NSWF_matrix Result;
	u32 Nbits;

	Result.ScaleX = 1.0f;
	Result.ScaleY = 1.0f;

	if (NSWF_SWFDataReadUB(SWFData, 1)) {
		Nbits = NSWF_SWFDataReadUB(SWFData, 5);
		Result.ScaleX = NSWF_SWFDataReadFB(SWFData, Nbits);
		Result.ScaleY = NSWF_SWFDataReadFB(SWFData, Nbits);
	}
	Result.RotateSkew0 = 0.0f;
	Result.RotateSkew1 = 0.0f;
	if (NSWF_SWFDataReadUB(SWFData, 1)) {
		Nbits = NSWF_SWFDataReadUB(SWFData, 5);
		Result.RotateSkew0 = NSWF_SWFDataReadFB(SWFData, Nbits);
		Result.RotateSkew1 = NSWF_SWFDataReadFB(SWFData, Nbits);
	}

	Nbits = NSWF_SWFDataReadUB(SWFData, 5);
	Result.TranslateX = NSWF_SWFDataReadSB(SWFData, Nbits);
	Result.TranslateY = NSWF_SWFDataReadSB(SWFData, Nbits);

	return(Result);
}

/////////////////////////////////////////////////////////
// Color transform records
/////////////////////////////////////////////////////////

NSWF_cxform NSWF_SWFDataReadCXFORM(NSWF_swfdata* SWFData, u32 WithAlpha) {
	NSWF_SWFDataResetBitsPending(SWFData);
	NSWF_cxform Result;

	u32 HasAddTerms = NSWF_SWFDataReadUB(SWFData, 1);
	u32 HasMultTerms = NSWF_SWFDataReadUB(SWFData, 1);
	u32 Nbits = NSWF_SWFDataReadUB(SWFData, 4);

	if (HasMultTerms) {
		Result.RedMultTerm = NSWF_SWFDataReadSB(SWFData, Nbits);
		Result.GreenMultTerm = NSWF_SWFDataReadSB(SWFData, Nbits);
		Result.BlueMultTerm = NSWF_SWFDataReadSB(SWFData, Nbits);
		if (WithAlpha) {
			Result.AlphaMultTerm = NSWF_SWFDataReadSB(SWFData, Nbits);
		}
		else {
			Result.AlphaMultTerm = 256;
		}
	}
	else {
		Result.RedMultTerm = 256;
		Result.GreenMultTerm = 256;
		Result.BlueMultTerm = 256;
		Result.AlphaMultTerm = 256;
	}

	if (HasAddTerms) {
		Result.RedAddTerm = NSWF_SWFDataReadSB(SWFData, Nbits);
		Result.GreenAddTerm = NSWF_SWFDataReadSB(SWFData, Nbits);
		Result.BlueAddTerm = NSWF_SWFDataReadSB(SWFData, Nbits);
		if (WithAlpha) {
			Result.AlphaAddTerm = NSWF_SWFDataReadSB(SWFData, Nbits);
		}
		else {
			Result.AlphaAddTerm = 0;
		}
	}
	else {
		Result.RedAddTerm = 0;
		Result.GreenAddTerm = 0;
		Result.BlueAddTerm = 0;
		Result.AlphaAddTerm = 0;
	}

	return(Result);
}

NSWF_gradient 

//////////////////////////////
//        MEMORY
//////////////////////////////

void* NSWF_SWFDataAllocTagMemory(NSWF_swfdata* SWFData, u32 Size) {
    if (SWFData->TagMemoryUsed + Size > SWFData->TagMemoryTotal) {
        assert(!"SWFData: Out of tag memory");
    }

    void* Result = SWFData->TagMemory + SWFData->TagMemoryUsed;
    SWFData->TagMemoryUsed += Size;

    return(Result);
}

void NSWF_SWFDataFreeTagMemory(NSWF_swfdata* SWFData) {
    SWFData->TagMemoryUsed = 0;
}

//////////////////////////////
//        SHAPES
//////////////////////////////

void NSWF_SWFDataReadFILLSTYLE(NSWF_swfdata* SWFData, u32 Level, NSWF_fillstyle* Result) {
    /*
     * type = data.readUI8();
			switch(type) {
				case 0x00:
					rgb = (level <= 2) ? data.readRGB() : data.readRGBA();
					break;
				case 0x10:
				case 0x12:
				case 0x13:
					gradientMatrix = data.readMATRIX();
					gradient = (type == 0x13) ? data.readFOCALGRADIENT(level) : data.readGRADIENT(level);
					break;
				case 0x40:
				case 0x41:
				case 0x42:
				case 0x43:
					bitmapId = data.readUI16();
					bitmapMatrix = data.readMATRIX();
					break;
				default:
					throw(new Error("Unknown fill style type: 0x" + type.toString(16)));
			}
     */
    Result->Type = NSWF_SWFDataReadU8(SWFData);
    switch (Result->Type) {
        case NSWF_FillStyleType_SolidFill: {
            Result->Color = (Level <= 2) ? NSWF_SWFDataReadRGB(SWFData) : NSWF_SWFDataReadRGBA(SWFData);
        } break;
        case NSWF_FillStyleType_LinearGradient:
        case NSWF_FillStyleType_RadialGradient:
        case NSWF_FillStyleType_FocalRadialGradient: {
            Result->GradientMatrix = NSWF_SWFDataReadMATRIX(SWFData);
            Result->Gradient = NSWF_SWFDataReadGRADIENT(SWFData, Level, Result->Type == NSWF_FillStyleType_FocalRadialGradient);
        } break;
        case NSWF_FillStyleType_RepeatingBitmap:
        case NSWF_FillStyleType_ClippedBitmap:
        case NSWF_FillStyleType_NonSmoothedRepeating:
        case NSWF_FillStyleType_NonSmoothedClipped: {
            Result->BitmapId = NSWF_SWFDataReadU16(SWFData);
            Result->BitmapMatrix = NSWF_SWFDataReadMATRIX(SWFData);
        } break;
    }
}

NSWF_fillstylearray* NSWF_SWFDataReadFILLSTYLEARRAY(NSWF_swfdata* SWFData, u8 CheckExtended) {
    u32 i;
    NSWF_fillstylearray* Result = (NSWF_fillstylearray*)NSWF_SWFDataAllocTagMemory(SWFData, sizeof(NSWF_fillstylearray));
    Result->FillStylesCount = NSWF_SWFDataReadU8(SWFData);
    if (CheckExtended && Result->FillStylesCount == 0xFF) {
        Result->FillStylesCount = NSWF_SWFDataReadU16(SWFData);
    }
    Result->FillStyles = (NSWF_fillstyle*)NSWF_SWFDataAllocTagMemory(SWFData, sizeof(NSWF_fillstyle) * Result->FillStyleCount);
    for (i = 0; i < Result->FillStylesCount; ++i) {
        NSWF_SWFDataReadFILLSTYLE(SWFData, Result->FillStyles + i);
    }
    return(Result);
}

/*
 * Level 2 is LINESTYLE2
 */
NSWF_linestylearray* NSWF_SWFDataReadLINESTYLEARRAY(NSWF_swfdata* SWFData, u8 CheckExtended, u32 Level) {
    u32 i;
    NSWF_linestylearray* Result = (NSWF_linestylearray*)NSWF_SWFDataAllocTagMemory(SWFData, sizeof(NSWF_linestylearray));
    Result->LineStylesCount = NSWF_SWFDataReadU8(SWFData);
    if (CheckExtended && Result->LineStylesCount == 0xFF) {
        Result->LineStylesCount = NSWF_SWFDataReadU16(SWFData);
    }
    Result->LIneStyles = (NSWF_linestyle*)NSWF_SWFDataAllocTagMemory(SWFData, sizeof(NSWF_linestyle) * Result->LineStyleCount);
    for (i = 0; i < Result->LineStylesCount; ++i) {
        NSWF_SWFDataReadLINESTYLE(SWFData, Result->LineStyles + i, Level);
    }
    return(Result);
}


NSWF_shape* NSWF_SWFDataReadSHAPE(NSWF_swfdata* SWFData, u32 Level, u8 WithStyles) {
//    data.resetBitsPending();
//    var i:uint;
//    var fillStylesLen:uint = readStyleArrayLength(data, level);
//    for (i = 0; i < fillStylesLen; i++) {
//        initialFillStyles.push(data.readFILLSTYLE(level));
//    }
//    var lineStylesLen:uint = readStyleArrayLength(data, level);
//    for (i = 0; i < lineStylesLen; i++) {
//        initialLineStyles.push(level <= 3 ? data.readLINESTYLE(level) : data.readLINESTYLE2(level));
//    }
//    data.resetBitsPending();
//    var numFillBits:uint = data.readUB(4);
//    var numLineBits:uint = data.readUB(4);
//    readShapeRecords(data, numFillBits, numLineBits, level);
//    var len:uint = data.readUI8();
//    if (level >= 2 && len == 0xff) {
//        len = data.readUI16();
//    }
//    return len;
    NSWF_shape* Result = (NSWF_shape*)NSWF_SWFDataAllocTagMemory(SWFData, sizeof(NSWF_shape));
    NSWF_SWFDataResetBitsPending(SWFData);

    if (WithStyles) {
        u32 i;
        NSWF_SWFDataReadFILLSTYLEARRAY(SWFData);

        NSWF_SWFDataReadFILLSTYLEARRAY(SWFData, Level > 3 ? 2 : 1);
    }
    NSWF_SWFDataResetBitsPending(SWFData);
    u32 NumFillBits = NSWF_SWFDataReadUB(4);
    u32 NumLineBits = NSWF_SWFDataReadUB(4);
    //read shaperecords

    return Result;
}


//////////////////////////////
//          TAGS
//////////////////////////////

NSWF_tag_header NSWF_SWFDataReadTagHeader(NSWF_swfdata* SWFData) {
	NSWF_tag_header Result;
	u16 TagCodeAndLength = NSWF_SWFDataReadU16(SWFData);
	Result.TagCode = TagCodeAndLength >> 6;
	Result.Length = TagCodeAndLength & ((1 << 6) - 1);
	if (Result.Length == 0x3F) {
		Result.Length = NSWF_SWFDataReadU32(SWFData);
	}
	return(Result);
}

NSWF_tag_defineshape* NSWF_SWFDataReadTagDEFINESHAPE4(NSWF_swfdata* SWFData) {
	NSWF_tag_defineshape* Result = (NSWF_tag_defineshape*)NSWF_SWFDataAllocTagMemory(SWFData, sizeof(NSWF_tag_defineshape));
	Result->CharacterID = NSWF_SWFDataReadU16(SWFData);
	Result->ShapeBounds = NSWF_SWFDataReadRECT(SWFData);
	Result->EdgeBounds = NSWF_SWFDataReadRECT(SWFData);
	Result->Flags = NSWF_SWFDataReadU8(SWFData);
	Result->Shapes = NSWF_DataReadSHAPE(SWFData, 1);
	return(Result);
}

NSWF_swfdata* NSWF_SWFDataInit(void* Data, u32 Length, u32 TagMemorySize) {
    NSWF_swfdata* Result = (NSWF_swfdata*)malloc(sizeof(NSWF_swfdata));
    Result->OwnsData = 0;
    Result->Data = Data;
    Result->Head = Data;
    Result->Length = Length;
    Result->ByteIndex = 0;
    Result->BitsPending = 0;
    Result->TagMemory = malloc(TagMemorySize);
    Result->TagMemoryUsed = 0;
    Result->TagMemoryTotal = TagMemorySize;

    // Read header
    NSWF_swfheader Header = { 0 };
    Header.Signature[0] = NSWF_SWFDataReadU8(Result);
    Header.Signature[1] = NSWF_SWFDataReadU8(Result);
    Header.Signature[2] = NSWF_SWFDataReadU8(Result);
    Header.Version = NSWF_SWFDataReadU8(Result);
    Header.FileLength = NSWF_SWFDataReadU32(Result);
    Header.FrameSize = NSWF_SWFDataReadRECT(Result);
    Header.FrameRate = NSWF_SWFDataReadFIXED8(Result);
    Header.FrameCount = NSWF_SWFDataReadU16(Result);

    Result->Header = Header;

    return(Result);
}

#ifdef _MSC_VER
#define FOPEN(file, filename, mode) fopen_s(&file, filename, mode)
#else
#define FOPEN(file, filename, mode) file = fopen(filename, mode)
#endif

NSWF_swfdata* NSWF_SWFDataInitFromFile(const char* Filename, u32 TagMemorySize) {
    FILE *File;

	FOPEN(File, Filename, "rb");

    if (!File) {
        printf("File not found.\n");
        getchar();
        return(NULL);
    }

    fseek(File, 0, SEEK_END);
    long Length = ftell(File);
    fseek(File, 0, SEEK_SET);  //same as rewind(f);

    void* Data = malloc(Length);
    fread(Data, Length, 1, File);
    fclose(File);

    NSWF_swfdata* Result = NSWF_SWFDataInit(Data, Length, TagMemorySize);
    Result->OwnsData = 1;
    return(Result);
}

void NSWF_SWFDataReadTags(NSWF_swfdata* SWFData, NSWF_tagcallback TagCallback) {
    // Read tags
    while (1) {
        NSWF_tag_header TagHeader = NSWF_SWFDataReadTagHeader(SWFData);
		void* Tag = NULL;

        switch (TagHeader.TagCode) {
            case TagCode_DefineShape4: {
//                NSWF_shape *shape = NSWF_SWFDataReadTagDEFINESHAPE4(SWFData);
            }
            default: {
                NSWF_SWFDataSkip(SWFData, TagHeader.Length);
            } break;
        }

        TagCallback(TagHeader.TagCode, Tag);

        if (!TagHeader.TagCode) {
            break;
        }
    }
}

void NSWF_SWFDataFree(NSWF_swfdata* SWFData) {
    if (SWFData->OwnsData) {
        free(SWFData->Data);
    }
    free(SWFData->TagMemory);
    free(SWFData);
}

#endif