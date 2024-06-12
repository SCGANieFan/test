#pragma once
#include"Ape.Inner.basic.h"
#include"Ape.Inner.mem.manger.h"

typedef struct {
    i64 pos;
    i32 nblocks;
    //i32 size;
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

    /* Seektable */
    u32* seektable;

    /* wav header */
    void* waveHeader;
} ApeContext_t;

class ApeContext
{
public:
    ApeContext() {};
    ~ApeContext() {};
public:
    INLINE u16 GetChannels() { return _context.header.channels; }
    INLINE u16 GetCompressType() { return _context.header.compressiontype; }
    INLINE i16 GetFileVersion() { return _context.descriptor.fileversion; }
    INLINE ApeHeader* GetHeader() { return &_context.header; }
    INLINE ApeDescriptor* GetDescriptor() { return &_context.descriptor; }
    INLINE u32* GetSeekTable() { return _context.seektable; }
    INLINE ApeContext_t* GetContext() { return &_context; }
public:
    bool GetFrameInfoWithFrameNum(ApeFrameInfo* frame, u32 frameNum);
    void InitWithContext(ApeContext_t *context){_context=*context;};
    i32 InitWithBuffer(ApeMemManger* mm, u8* in, i32* inByte);
private:
    ApeContext_t _context;
};


