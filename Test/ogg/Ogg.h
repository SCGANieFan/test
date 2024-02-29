#pragma once

#include"OggType.h"

/* Ogg BITSTREAM PRIMITIVES: bitstream ************************/

extern void  oggpack_writeinit(oggpack_buffer* b);
extern int   oggpack_writecheck(oggpack_buffer* b);
extern void  oggpack_writetrunc(oggpack_buffer* b, long bits);
extern void  oggpack_writealign(oggpack_buffer* b);
extern void  oggpack_writecopy(oggpack_buffer* b, void* source, long bits);
extern void  oggpack_reset(oggpack_buffer* b);
extern void  oggpack_writeclear(oggpack_buffer* b);
extern void  oggpack_readinit(oggpack_buffer* b, unsigned char* buf, int bytes);
extern void  oggpack_write(oggpack_buffer* b, unsigned long value, int bits);
extern long  oggpack_look(oggpack_buffer* b, int bits);
extern long  oggpack_look1(oggpack_buffer* b);
extern void  oggpack_adv(oggpack_buffer* b, int bits);
extern void  oggpack_adv1(oggpack_buffer* b);
extern long  oggpack_read(oggpack_buffer* b, int bits);
extern long  oggpack_read1(oggpack_buffer* b);
extern long  oggpack_bytes(oggpack_buffer* b);
extern long  oggpack_bits(oggpack_buffer* b);
extern unsigned char* oggpack_get_buffer(oggpack_buffer* b);

extern void  oggpackB_writeinit(oggpack_buffer* b);
extern int   oggpackB_writecheck(oggpack_buffer* b);
extern void  oggpackB_writetrunc(oggpack_buffer* b, long bits);
extern void  oggpackB_writealign(oggpack_buffer* b);
extern void  oggpackB_writecopy(oggpack_buffer* b, void* source, long bits);
extern void  oggpackB_reset(oggpack_buffer* b);
extern void  oggpackB_writeclear(oggpack_buffer* b);
extern void  oggpackB_readinit(oggpack_buffer* b, unsigned char* buf, int bytes);
extern void  oggpackB_write(oggpack_buffer* b, unsigned long value, int bits);
extern long  oggpackB_look(oggpack_buffer* b, int bits);
extern long  oggpackB_look1(oggpack_buffer* b);
extern void  oggpackB_adv(oggpack_buffer* b, int bits);
extern void  oggpackB_adv1(oggpack_buffer* b);
extern long  oggpackB_read(oggpack_buffer* b, int bits);
extern long  oggpackB_read1(oggpack_buffer* b);
extern long  oggpackB_bytes(oggpack_buffer* b);
extern long  oggpackB_bits(oggpack_buffer* b);
extern unsigned char* oggpackB_get_buffer(oggpack_buffer* b);


/* Ogg BITSTREAM PRIMITIVES: decoding **************************/

extern int      ogg_sync_init(ogg_sync_state* oy);
extern int      ogg_sync_clear(ogg_sync_state* oy);
extern int      ogg_sync_reset(ogg_sync_state* oy);
extern int      ogg_sync_destroy(ogg_sync_state* oy);
extern int      ogg_sync_check(ogg_sync_state* oy);

extern char* ogg_sync_buffer(ogg_sync_state* oy, long size);
extern int      ogg_sync_wrote(ogg_sync_state* oy, long bytes);
extern long     ogg_sync_pageseek(ogg_sync_state* oy, ogg_page* og);
extern int      ogg_sync_pageout(ogg_sync_state* oy, ogg_page* og);
extern int      ogg_stream_pagein(ogg_stream_state* os, ogg_page* og);
extern int      ogg_stream_packetout(ogg_stream_state* os, ogg_packet* op);
extern int      ogg_stream_packetpeek(ogg_stream_state* os, ogg_packet* op);

/* Ogg BITSTREAM PRIMITIVES: general ***************************/

extern int      ogg_stream_init(ogg_stream_state* os, int serialno);
extern int      ogg_stream_clear(ogg_stream_state* os);
extern int      ogg_stream_reset(ogg_stream_state* os);
extern int      ogg_stream_reset_serialno(ogg_stream_state* os, int serialno);
extern int      ogg_stream_destroy(ogg_stream_state* os);
extern int      ogg_stream_check(ogg_stream_state* os);
extern int      ogg_stream_eos(ogg_stream_state* os);

extern void     ogg_page_checksum_set(ogg_page* og);

extern int      ogg_page_version(const ogg_page* og);
extern int      ogg_page_continued(const ogg_page* og);
extern int      ogg_page_bos(const ogg_page* og);
extern int      ogg_page_eos(const ogg_page* og);
extern ogg_int64_t  ogg_page_granulepos(const ogg_page* og);
extern int      ogg_page_serialno(const ogg_page* og);
extern long     ogg_page_pageno(const ogg_page* og);
extern int      ogg_page_packets(const ogg_page* og);

extern void     ogg_packet_clear(ogg_packet* op);

