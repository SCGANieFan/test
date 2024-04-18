#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"Ogg.h"
#include"OggCrcTable.h"

int ogg_page_version(const ogg_page* og) {
    return((int)(og->header[4]));
}

int ogg_page_continued(const ogg_page* og) {
    return((int)(og->header[5] & 0x01));
}

int ogg_page_bos(const ogg_page* og) {
    return((int)(og->header[5] & 0x02));
}

int ogg_page_eos(const ogg_page* og) {
    return((int)(og->header[5] & 0x04));
}

ogg_int64_t ogg_page_granulepos(const ogg_page* og) {
    unsigned char* page = og->header;
    ogg_uint64_t granulepos = page[13] & (0xff);
    granulepos = (granulepos << 8) | (page[12] & 0xff);
    granulepos = (granulepos << 8) | (page[11] & 0xff);
    granulepos = (granulepos << 8) | (page[10] & 0xff);
    granulepos = (granulepos << 8) | (page[9] & 0xff);
    granulepos = (granulepos << 8) | (page[8] & 0xff);
    granulepos = (granulepos << 8) | (page[7] & 0xff);
    granulepos = (granulepos << 8) | (page[6] & 0xff);
    return((ogg_int64_t)granulepos);
}

int ogg_page_serialno(const ogg_page* og) {
    return((int)((ogg_uint32_t)og->header[14]) |
        ((ogg_uint32_t)og->header[15] << 8) |
        ((ogg_uint32_t)og->header[16] << 16) |
        ((ogg_uint32_t)og->header[17] << 24));
}

long ogg_page_pageno(const ogg_page* og) {
    return((long)((ogg_uint32_t)og->header[18]) |
        ((ogg_uint32_t)og->header[19] << 8) |
        ((ogg_uint32_t)og->header[20] << 16) |
        ((ogg_uint32_t)og->header[21] << 24));
}



/* returns the number of packets that are completed on this page (if
   the leading packet is begun on a previous page, but ends on this
   page, it's counted */

   /* NOTE:
      If a page consists of a packet begun on a previous page, and a new
      packet begun (but not completed) on this page, the return will be:
        ogg_page_packets(page)   ==1,
        ogg_page_continued(page) !=0

      If a page happens to be a single packet that was begun on a
      previous page, and spans to the next page (in the case of a three or
      more page packet), the return will be:
        ogg_page_packets(page)   ==0,
        ogg_page_continued(page) !=0
   */

int ogg_page_packets(const ogg_page* og) {
    int i, n = og->header[26], count = 0;
    for (i = 0; i < n; i++)
        if (og->header[27 + i] < 255)count++;
    return(count);
}


/* init the encode/decode logical stream state */

int ogg_stream_init(ogg_stream_state* os, int serialno)
{
#if 0
    if (os) {
        memset(os, 0, sizeof(*os));
        os->lacing_storage = 512;
        os->malloc = malloc;
        os->realloc= realloc;
        os->free = free;
        os->lacing_vals = (int*)os->malloc(os->lacing_storage * sizeof(*os->lacing_vals));
        os->granule_vals = (ogg_int64_t*)os->malloc(os->lacing_storage * sizeof(*os->granule_vals));
        if (!os->lacing_vals || !os->granule_vals) {
            ogg_stream_clear(os);
            return -1;
        }
        os->serialno = serialno;

        return(0);
    }
    return(-1);
#else
    if (os) {
        //memset(os, 0, sizeof(*os));
        //os->lacing_storage = 512;
        //os->malloc = malloc;
        //os->realloc = realloc;
        //os->free = free;
        //os->lacing_vals = (int*)os->malloc(os->lacing_storage * sizeof(*os->lacing_vals));
        //os->granule_vals = (ogg_int64_t*)os->malloc(os->lacing_storage * sizeof(*os->granule_vals));
        ogg_stream_clear(os);
        if (!os->lacing_vals || !os->granule_vals) {
            ogg_stream_clear(os);
            return -1;
        }
        os->serialno = serialno;

        return(0);
    }
    return(-1);
#endif
}

/* async/delayed error detection for the ogg_stream_state */
int ogg_stream_check(ogg_stream_state* os) {
    if (!os) return -1;
    return 0;
}

/* _clear does not free os, only the non-flat storage within */
int ogg_stream_clear(ogg_stream_state* os) {
#if 0
    if (os) {
        if (os->lacing_vals)os->free(os->lacing_vals);
        if (os->granule_vals)os->free(os->granule_vals);

        memset(os, 0, sizeof(*os));
    }
    return(0);
#else

    if (os) {

        int* lacing_vals = os->lacing_vals;
        ogg_int64_t* granule_vals = os->granule_vals;
        long lacing_storage = os->lacing_storage;

        memset(os, 0, sizeof(*os));

        os->lacing_vals= lacing_vals;
        os->granule_vals= granule_vals;
        os->lacing_storage= lacing_storage;
    }
    return(0);

#endif
}
int ogg_stream_reset(ogg_stream_state* os) {
    if (ogg_stream_check(os)) return -1;

    os->body_fill = 0;
    os->body_returned = 0;

    os->lacing_fill = 0;
    os->lacing_packet = 0;
    os->lacing_returned = 0;

    os->header_fill = 0;

    os->e_o_s = 0;
    os->b_o_s = 0;
    os->pageno = -1;
    os->packetno = 0;
    os->granulepos = 0;

    return(0);
}

int ogg_stream_reset_serialno(ogg_stream_state* os, int serialno) {
    if (ogg_stream_check(os)) return -1;
    ogg_stream_reset(os);
    os->serialno = serialno;
    return(0);
}

#if 0
int ogg_stream_destroy(ogg_stream_state* os) {
    if (os) {
        ogg_stream_clear(os);
        os->free(os);
    }
    return(0);
}
#endif
/* Helpers for ogg_stream_encode; this keeps the structure and
   what's happening fairly clear */

#if 0
static int _os_body_expand(ogg_stream_state* os, long needed) {
    if (os->body_storage - needed <= os->body_fill) {
        long body_storage;
        void* ret;
        if (os->body_storage > LONG_MAX - needed) {
            ogg_stream_clear(os);
            return -1;
        }
        body_storage = os->body_storage + needed;
        if (body_storage < LONG_MAX - 1024)body_storage += 1024;
        ret = os->realloc(os->body_data, body_storage * sizeof(*os->body_data));
        if (!ret) {
            ogg_stream_clear(os);
            return -1;
        }
        os->body_storage = body_storage;
        os->body_data = (unsigned char*)ret;
    }
    return 0;
}
static int _os_lacing_expand(ogg_stream_state* os, long needed) {
    if (os->lacing_storage - needed <= os->lacing_fill) {
        long lacing_storage;
        void* ret;
        if (os->lacing_storage > LONG_MAX - needed) {
            ogg_stream_clear(os);
            return -1;
        }
        lacing_storage = os->lacing_storage + needed;
        if (lacing_storage < LONG_MAX - 32)lacing_storage += 32;
        ret = os->realloc(os->lacing_vals, lacing_storage * sizeof(*os->lacing_vals));
        if (!ret) {
            ogg_stream_clear(os);
            return -1;
        }
        os->lacing_vals = (int*)ret;
        ret = os->realloc(os->granule_vals, lacing_storage *
            sizeof(*os->granule_vals));
        if (!ret) {
            ogg_stream_clear(os);
            return -1;
        }
        os->granule_vals = (ogg_int64_t*)ret;
        os->lacing_storage = lacing_storage;
    }
    return 0;
}
#endif
/* checksum the page */
/* Direct table CRC; note that this will be faster in the future if we
   perform the checksum simultaneously with other copies */

static ogg_uint32_t _os_update_crc(ogg_uint32_t crc, unsigned char* buffer, int size) {
    while (size >= 8) {
        crc ^= ((ogg_uint32_t)buffer[0] << 24) | ((ogg_uint32_t)buffer[1] << 16) | ((ogg_uint32_t)buffer[2] << 8) | ((ogg_uint32_t)buffer[3]);

        crc = crc_lookup[7][crc >> 24] ^ crc_lookup[6][(crc >> 16) & 0xFF] ^
            crc_lookup[5][(crc >> 8) & 0xFF] ^ crc_lookup[4][crc & 0xFF] ^
            crc_lookup[3][buffer[4]] ^ crc_lookup[2][buffer[5]] ^
            crc_lookup[1][buffer[6]] ^ crc_lookup[0][buffer[7]];

        buffer += 8;
        size -= 8;
    }

    while (size--)
        crc = (crc << 8) ^ crc_lookup[0][((crc >> 24) & 0xff) ^ *buffer++];
    return crc;
}

void ogg_page_checksum_set(ogg_page* og) {
    if (og) {
        ogg_uint32_t crc_reg = 0;

        /* safety; needed for API behavior, but not framing code */
        og->header[22] = 0;
        og->header[23] = 0;
        og->header[24] = 0;
        og->header[25] = 0;

        crc_reg = _os_update_crc(crc_reg, og->header, og->header_len);
        crc_reg = _os_update_crc(crc_reg, og->body, og->body_len);

        og->header[22] = (unsigned char)(crc_reg & 0xff);
        og->header[23] = (unsigned char)((crc_reg >> 8) & 0xff);
        og->header[24] = (unsigned char)((crc_reg >> 16) & 0xff);
        og->header[25] = (unsigned char)((crc_reg >> 24) & 0xff);
    }
}

/* submit data to the internal buffer of the framing engine */
#if 0
int ogg_stream_iovecin(ogg_stream_state* os, ogg_iovec_t* iov, int count,
    long e_o_s, ogg_int64_t granulepos) {

    long bytes = 0, lacing_vals;
    int i;

    if (ogg_stream_check(os)) return -1;
    if (!iov) return 0;

    for (i = 0; i < count; ++i) {
        if (iov[i].iov_len > LONG_MAX) return -1;
        if (bytes > LONG_MAX - (long)iov[i].iov_len) return -1;
        bytes += (long)iov[i].iov_len;
    }
    lacing_vals = bytes / 255 + 1;

    if (os->body_returned) {
        /* advance packet data according to the body_returned pointer. We
           had to keep it around to return a pointer into the buffer last
           call */

        os->body_fill -= os->body_returned;
        if (os->body_fill)
            memmove(os->body_data, os->body_data + os->body_returned,
                os->body_fill);
        os->body_returned = 0;
    }

    /* make sure we have the buffer storage */
    if (_os_body_expand(os, bytes) || _os_lacing_expand(os, lacing_vals))
        return -1;

    /* Copy in the submitted packet.  Yes, the copy is a waste; this is
       the liability of overly clean abstraction for the time being.  It
       will actually be fairly easy to eliminate the extra copy in the
       future */

    for (i = 0; i < count; ++i) {
        memcpy(os->body_data + os->body_fill, iov[i].iov_base, iov[i].iov_len);
        os->body_fill += (int)iov[i].iov_len;
    }

    /* Store lacing vals for this packet */
    for (i = 0; i < lacing_vals - 1; i++) {
        os->lacing_vals[os->lacing_fill + i] = 255;
        os->granule_vals[os->lacing_fill + i] = os->granulepos;
    }
    os->lacing_vals[os->lacing_fill + i] = bytes % 255;
    os->granulepos = os->granule_vals[os->lacing_fill + i] = granulepos;

    /* flag the first segment as the beginning of the packet */
    os->lacing_vals[os->lacing_fill] |= 0x100;

    os->lacing_fill += lacing_vals;

    /* for the sake of completeness */
    os->packetno++;

    if (e_o_s)os->e_o_s = 1;

    return(0);
}
int ogg_stream_packetin(ogg_stream_state* os, ogg_packet* op) {
    ogg_iovec_t iov;
    iov.iov_base = op->packet;
    iov.iov_len = op->bytes;
    return ogg_stream_iovecin(os, &iov, 1, op->e_o_s, op->granulepos);
}
#endif

/* Conditionally flush a page; force==0 will only flush nominal-size
   pages, force==1 forces us to flush a page regardless of page size
   so long as there's any data available at all. */
static int ogg_stream_flush_i(ogg_stream_state* os, ogg_page* og, int force, int nfill) {
    int i;
    int vals = 0;
    int maxvals = (os->lacing_fill > 255 ? 255 : os->lacing_fill);
    int bytes = 0;
    long acc = 0;
    ogg_int64_t granule_pos = -1;

    if (ogg_stream_check(os)) return(0);
    if (maxvals == 0) return(0);

    /* construct a page */
    /* decide how many segments to include */

    /* If this is the initial header case, the first page must only include
       the initial header packet */
    if (os->b_o_s == 0) {  /* 'initial header page' case */
        granule_pos = 0;
        for (vals = 0; vals < maxvals; vals++) {
            if ((os->lacing_vals[vals] & 0x0ff) < 255) {
                vals++;
                break;
            }
        }
    }
    else {

        /* The extra packets_done, packet_just_done logic here attempts to do two things:
           1) Don't unnecessarily span pages.
           2) Unless necessary, don't flush pages if there are less than four packets on
              them; this expands page size to reduce unnecessary overhead if incoming packets
              are large.
           These are not necessary behaviors, just 'always better than naive flushing'
           without requiring an application to explicitly request a specific optimized
           behavior. We'll want an explicit behavior setup pathway eventually as well. */

        int packets_done = 0;
        int packet_just_done = 0;
        for (vals = 0; vals < maxvals; vals++) {
            if (acc > nfill && packet_just_done >= 4) {
                force = 1;
                break;
            }
            acc += os->lacing_vals[vals] & 0x0ff;
            if ((os->lacing_vals[vals] & 0xff) < 255) {
                granule_pos = os->granule_vals[vals];
                packet_just_done = ++packets_done;
            }
            else
                packet_just_done = 0;
        }
        if (vals == 255)force = 1;
    }

    if (!force) return(0);

    /* construct the header in temp storage */
    memcpy(os->header, "OggS", 4);

    /* stream structure version */
    os->header[4] = 0x00;

    /* continued packet flag? */
    os->header[5] = 0x00;
    if ((os->lacing_vals[0] & 0x100) == 0)os->header[5] |= 0x01;
    /* first page flag? */
    if (os->b_o_s == 0)os->header[5] |= 0x02;
    /* last page flag? */
    if (os->e_o_s && os->lacing_fill == vals)os->header[5] |= 0x04;
    os->b_o_s = 1;

    /* 64 bits of PCM position */
    for (i = 6; i < 14; i++) {
        os->header[i] = (unsigned char)(granule_pos & 0xff);
        granule_pos >>= 8;
    }

    /* 32 bits of stream serial number */
    {
        long serialno = os->serialno;
        for (i = 14; i < 18; i++) {
            os->header[i] = (unsigned char)(serialno & 0xff);
            serialno >>= 8;
        }
    }

    /* 32 bits of page counter (we have both counter and page header
       because this val can roll over) */
    if (os->pageno == -1)os->pageno = 0; /* because someone called
                                       stream_reset; this would be a
                                       strange thing to do in an
                                       encode stream, but it has
                                       plausible uses */
    {
        long pageno = os->pageno++;
        for (i = 18; i < 22; i++) {
            os->header[i] = (unsigned char)(pageno & 0xff);
            pageno >>= 8;
        }
    }

    /* zero for computation; filled in later */
    os->header[22] = 0;
    os->header[23] = 0;
    os->header[24] = 0;
    os->header[25] = 0;

    /* segment table */
    os->header[26] = (unsigned char)(vals & 0xff);
    for (i = 0; i < vals; i++)
        bytes += os->header[i + 27] = (unsigned char)(os->lacing_vals[i] & 0xff);

    /* set pointers in the ogg_page struct */
    og->header = os->header;
    og->header_len = os->header_fill = vals + 27;
    og->body = os->body_data + os->body_returned;
    og->body_len = bytes;

    /* advance the lacing data and set the body_returned pointer */

    os->lacing_fill -= vals;
    memmove(os->lacing_vals, os->lacing_vals + vals, os->lacing_fill * sizeof(*os->lacing_vals));
    memmove(os->granule_vals, os->granule_vals + vals, os->lacing_fill * sizeof(*os->granule_vals));
    os->body_returned += bytes;

    /* calculate the checksum */

    ogg_page_checksum_set(og);

    /* done */
    return(1);
}

/* This will flush remaining packets into a page (returning nonzero),
   even if there is not enough data to trigger a flush normally
   (undersized page). If there are no packets or partial packets to
   flush, ogg_stream_flush returns 0.  Note that ogg_stream_flush will
   try to flush a normal sized page like ogg_stream_pageout; a call to
   ogg_stream_flush does not guarantee that all packets have flushed.
   Only a return value of 0 from ogg_stream_flush indicates all packet
   data is flushed into pages.

   since ogg_stream_flush will flush the last page in a stream even if
   it's undersized, you almost certainly want to use ogg_stream_pageout
   (and *not* ogg_stream_flush) unless you specifically need to flush
   a page regardless of size in the middle of a stream. */

int ogg_stream_flush(ogg_stream_state* os, ogg_page* og) {
    return ogg_stream_flush_i(os, og, 1, 4096);
}

/* Like the above, but an argument is provided to adjust the nominal
   page size for applications which are smart enough to provide their
   own delay based flushing */

int ogg_stream_flush_fill(ogg_stream_state* os, ogg_page* og, int nfill) {
    return ogg_stream_flush_i(os, og, 1, nfill);
}

/* This constructs pages from buffered packet segments.  The pointers
returned are to static buffers; do not free. The returned buffers are
good only until the next call (using the same ogg_stream_state) */

int ogg_stream_pageout(ogg_stream_state* os, ogg_page* og) {
    int force = 0;
    if (ogg_stream_check(os)) return 0;

    if ((os->e_o_s && os->lacing_fill) ||          /* 'were done, now flush' case */
        (os->lacing_fill && !os->b_o_s))           /* 'initial header page' case */
        force = 1;

    return(ogg_stream_flush_i(os, og, force, 4096));
}

/* Like the above, but an argument is provided to adjust the nominal
page size for applications which are smart enough to provide their
own delay based flushing */

int ogg_stream_pageout_fill(ogg_stream_state* os, ogg_page* og, int nfill) {
    int force = 0;
    if (ogg_stream_check(os)) return 0;

    if ((os->e_o_s && os->lacing_fill) ||          /* 'were done, now flush' case */
        (os->lacing_fill && !os->b_o_s))           /* 'initial header page' case */
        force = 1;

    return(ogg_stream_flush_i(os, og, force, nfill));
}

int ogg_stream_eos(ogg_stream_state* os) {
    if (ogg_stream_check(os)) return 1;
    return os->e_o_s;
}













