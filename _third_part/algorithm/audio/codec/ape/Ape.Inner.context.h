#pragma once
#include"Ape.Inner.basic.h"

typedef struct {
    i64 pos;
    i32 nblocks;
    //i32 size;
    i32 skip;
} ApeFrameInfo;

typedef struct {
    u8 magic[4];
    i16 fileversion;
    i16 padding;
    u32 descriptorlength;
    u32 headerlength;
    u32 seektablelength;
    u32 wavheaderlength;
    u32 audiodatalength;
    u32 audiodatalength_high;
    u32 wavtaillength;
    u8 md5[16];
} ApeDescriptor;

typedef struct {
    u16 compressiontype;
    u16 formatflags;
    u32 blocksperframe;
    u32 finalframeblocks;
    u32 totalframes;
    u16 bps;
    u16 channels;
    u32 samplerate;
} ApeHeader;

typedef struct {

    /* Info from Descriptor Block */
    ApeDescriptor descriptor;

    /* Info from Header Block */
    ApeHeader header;
} ApeContext_t;

class ApeContext
{
public:
    ApeContext() {};
    ~ApeContext() {};
public:
    const STATIC INLINE i32 GetContextSize() { return sizeof(ApeContext_t); }
    //STATIC INLINE i32 GetContextSize() { return sizeof(ApeContext_t); }
    //STATIC INLINE i32 GetContextSize() { return 1; }
public:
    INLINE u16 GetChannels() { return _context.header.channels; }
    INLINE u16 GetCompressType() { return _context.header.compressiontype; }
    INLINE i16 GetFileVersion() { return _context.descriptor.fileversion; }
    INLINE ApeHeader* GetHeader() { return &_context.header; }
    INLINE ApeDescriptor* GetDescriptor() { return &_context.descriptor; }
#if 0
    INLINE u32* GetSeekTable() { return _context.seektable; }
#endif
    INLINE ApeContext_t* GetContext() { return &_context; }

#if 0
    u32 GetStartPosFromFrame(u32 frame);
    u32 GetStartSkip(u32 frame);    
#endif

    INLINE b1 Init() {
        ALGO_MEM_SET(this, 0, sizeof(this));
        return true;
    };
public:
    void InitWithContext(MemoryManger_c* mm, ApeContext_t *context){
        _context = *context;
#if 0
        _context.seektable = (uint32_t*)mm->Malloc(_context.descriptor.seektablelength);
        ALGO_MEM_CPY(_context.seektable, context->seektable, _context.descriptor.seektablelength);
        _context.waveHeader = (uint32_t*)mm->Malloc(_context.descriptor.wavheaderlength);
        ALGO_MEM_CPY(_context.waveHeader, context->waveHeader, _context.descriptor.wavheaderlength);
#endif
    };
    i32 Parser(MemoryManger_c* mm, u8* in, i32 inByte);

//private:
public:
    ApeContext_t _context;
    /* Seektable */
    u32 seektablePos;

    /* wav header */
    u32 waveHeaderPos;
};


class ApeSeekTable_c
{
public:
    ApeSeekTable_c() {};
    ~ApeSeekTable_c() {};
public:
    INLINE b1 Init() {
        //ALGO_MEM_SET(this, 0, sizeof(this));
        _isFirst = true;
        return true;
    }
    b1 GetStartPosFromFrame(u32 frame,u32 * pos) {
        u32 posTemp;
        posTemp = _seektable[frame - 1];
        int skip = (posTemp - _seektable[0]) & 3;
        *pos = posTemp - skip;
        return true;
    }
    b1 GetStartSkip(u32 frame,u32 *skip) {
        u32 frameTmp = frame;
        *skip = (_seektable[frameTmp - 1] - _seektable[0]) & 3;
        return true;
    }
    
    b1 GetStartInfoFromPos(i32* pos, i32 *frameOff, i32* skip) {
        if (!pos)
            return false;
        //*pos
        i32 frameOffTmp = 0;
        i32 skipTmp = 0;
        for (; frameOffTmp < _seektableNum; frameOffTmp) {
            if (*pos < _seektable[frameOffTmp]){
                *pos = _seektable[frameOffTmp];
                skipTmp = (*pos - _firstFramePos) & 3;
                *pos -= skipTmp;
                if(frameOff)
                    *frameOff = frameOffTmp - 1;
                if(skip)
                    *skip = skipTmp;
                return true;
            }
        }
        return false;
    }

    void UpdataSeekTable(i32* seektable, i32 sizeInByte) {
        if(_isFirst)
            _firstFramePos = seektable[0];
        _seektable = seektable;
        _sizeInByte = sizeInByte;
        _seektableNum = _sizeInByte >> 2;
    }
private:
    i32* _seektable;
    i32 _sizeInByte;
    i32 _seektableNum;
    i32 _firstFramePos;
    b1 _isFirst;
};
