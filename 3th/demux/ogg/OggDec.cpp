#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"Ogg.h"

/* Helpers for ogg_stream_encode; this keeps the structure and
   what's happening fairly clear */

#if 0
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




/* DECODING PRIMITIVES: packet streaming layer **********************/

/* This has two layers to place more of the multi-serialno and paging
   control in the application's hands.  First, we expose a data buffer
   using ogg_sync_buffer().  The app either copies into the
   buffer, or passes it directly to read(), etc.  We then call
   ogg_sync_wrote() to tell how many bytes we just added.

   Pages are returned (pointers into the buffer in ogg_sync_state)
   by ogg_sync_pageout().  The page is then submitted to
   ogg_stream_pagein() along with the appropriate
   ogg_stream_state* (ie, matching serialno).  We then get raw
   packets out calling ogg_stream_packetout() with a
   ogg_stream_state. */

   /* initialize the struct to a known state */
int ogg_sync_init(ogg_sync_state* oy) {
    if (oy) {
        oy->storage = -1; /* used as a readiness flag */
        memset(oy, 0, sizeof(*oy));
    }
    return(0);
}

/* clear non-flat storage within */
int ogg_sync_clear(ogg_sync_state* oy) {
    if (oy) { 
        memset(oy, 0, sizeof(*oy));
    }
    return(0);
}

/* clear things to an initial state.  Good to call, eg, before seeking */
int ogg_sync_reset(ogg_sync_state* oy) {
    if (ogg_sync_check(oy))return -1;

    oy->fill = 0;
    oy->returned = 0;
    oy->unsynced = 0;
    oy->headerbytes = 0;
    oy->bodybytes = 0;
    return(0);
}


int ogg_sync_check(ogg_sync_state* oy) {
    if (oy->storage < 0) return -1;
    return 0;
}

int ogg_sync_wrote(ogg_sync_state* oy, unsigned char *data,long bytes) {
    oy->data = data;
    oy->fill = bytes;
    oy->storage = oy->fill;
    oy->returned = 0;
    
    return(0);
}
/* sync the stream.  This is meant to be useful for finding page
   boundaries.

   return values for this:
  -n) skipped n bytes
   0) page not ready; more data (no bytes skipped)
   n) page synced at current location; page length n bytes

*/

long ogg_sync_pageseek(ogg_sync_state* oy, ogg_page* og) {
    unsigned char* page = oy->data + oy->returned;
    unsigned char* next;
    long bytes = oy->fill - oy->returned;

    if (ogg_sync_check(oy))return 0;

    if (oy->headerbytes == 0) {
        int headerbytes, i;
        if (bytes < 27)return(0); /* not enough for a header */

        /* verify capture pattern */
        if (memcmp(page, "OggS", 4))goto sync_fail;

        headerbytes = page[26] + 27;
        if (bytes < headerbytes)return(0); /* not enough for header + seg table */

        /* count up body length in the segment table */

        for (i = 0; i < page[26]; i++)
            oy->bodybytes += page[27 + i];
        oy->headerbytes = headerbytes;
    }

    if (oy->bodybytes + oy->headerbytes > bytes)return(0);

    /* The whole test page is buffered.  Verify the checksum */
    {
        /* Grab the checksum bytes, set the header field to zero */
        char chksum[4];
        ogg_page log;

        memcpy(chksum, page + 22, 4);
        memset(page + 22, 0, 4);

        /* set up a temp page struct and recompute the checksum */
        log.header = page;
        log.header_len = oy->headerbytes;
        log.body = page + oy->headerbytes;
        log.body_len = oy->bodybytes;
        ogg_page_checksum_set(&log);

        /* Compare */
        if (memcmp(chksum, page + 22, 4)) {
            /* D'oh.  Mismatch! Corrupt page (or miscapture and not a page
               at all) */
               /* replace the computed checksum with the one actually read in */
            memcpy(page + 22, chksum, 4);

#ifndef DISABLE_CRC
            /* Bad checksum. Lose sync */
            goto sync_fail;
#endif
        }
    }

    /* yes, have a whole page all ready to go */
    {
        if (og) {
            og->header = page;
            og->header_len = oy->headerbytes;
            og->body = page + oy->headerbytes;
            og->body_len = oy->bodybytes;
            og->body_len_valid = bytes - oy->headerbytes;
        }

        oy->unsynced = 0;
        oy->returned += (bytes = oy->headerbytes + oy->bodybytes);
        oy->headerbytes = 0;
        oy->bodybytes = 0;
        return(bytes);
    }

sync_fail:

    oy->headerbytes = 0;
    oy->bodybytes = 0;

    /* search for possible capture */
    next = (unsigned char*)memchr(page + 1, 'O', bytes - 1);
    if (!next)
        next = oy->data + oy->fill;

    oy->returned = (int)(next - oy->data);
    return((long)-(next - page));
}

/* sync the stream and get a page.  Keep trying until we find a page.
   Suppress 'sync errors' after reporting the first.

   return values:
   -1) recapture (hole in data)
    0) need more data
    1) page returned

   Returns pointers into buffered data; invalidated by next call to
   _stream, _clear, _init, or _buffer */

int ogg_sync_pageout(ogg_sync_state* oy, ogg_page* og) {

    if (ogg_sync_check(oy))return 0;
    /* all we need to do is verify a page at the head of the stream
       buffer.  If it doesn't verify, we look for the next potential
       frame */

    for (;;) {
        long ret = ogg_sync_pageseek(oy, og);
        if (ret > 0) {
            /* have a page */
            return(1);
        }
        if (ret == 0) {
            /* need more data */
            return(0);
        }

        /* head did not start a synced page... skipped some bytes */
        if (!oy->unsynced) {
            oy->unsynced = 1;
            return(-1);
        }

        /* loop. keep looking */

    }
}

int ogg_stream_pagein(ogg_stream_state* os, ogg_page* og) {
    if (!os
        || !og)
        return -1;

    unsigned char* header = og->header;
    unsigned char* body = og->body;
    long           bodysize = og->body_len;
    int            segptr = 0;

    int version = ogg_page_version(og);
    int continued = ogg_page_continued(og);
    int bos = ogg_page_bos(og);
    int eos = ogg_page_eos(og);
    ogg_int64_t granulepos = ogg_page_granulepos(og);
    int serialno = ogg_page_serialno(og);
    long pageno = ogg_page_pageno(og);
    int segments = header[26];
    //if (ogg_stream_check(os)) return -1;
    /* clean up 'returned data' */
    {
        long lr = os->lacing_returned;
        long br = os->body_returned;

        /* body data */
        os->body_returned = 0;
        if (lr) {
            /* segment table */
            if (os->lacing_fill - lr) {
                memmove(os->lacing_vals, os->lacing_vals + lr,
                    (os->lacing_fill - lr) * sizeof(*os->lacing_vals));
                memmove(os->granule_vals, os->granule_vals + lr,
                    (os->lacing_fill - lr) * sizeof(*os->granule_vals));
            }
            os->lacing_fill -= lr;
            os->lacing_packet -= lr;
            os->lacing_returned = 0;
        }
    }

    /* check the serial number */
    if (serialno != os->serialno)return(-1);
    if (version > 0)return(-1);
#if 1
    int needed = segments + 1;
    if (os->lacing_storage <= (os->lacing_fill + needed)) {
        return -1;
    }
#else
    if (_os_lacing_expand(os, segments + 1)) return -1;
#endif
    /* are we in sequence? */
    if (pageno != os->pageno) {
        int i;

        /* unroll previous partial packet (if any) */
        for (i = os->lacing_packet; i < os->lacing_fill; i++)
            os->body_fill -= os->lacing_vals[i] & 0xff;
        os->lacing_fill = os->lacing_packet;

        /* make a note of dropped data in segment table */
        if (os->pageno != -1) {
            os->lacing_vals[os->lacing_fill++] = 0x400;
            os->lacing_packet++;
        }
    }

    /* are we a 'continued packet' page?  If so, we may need to skip
       some segments */
    if (continued) {
        if (os->lacing_fill < 1 ||
            (os->lacing_vals[os->lacing_fill - 1] & 0xff) < 255 ||
            os->lacing_vals[os->lacing_fill - 1] == 0x400) {
            bos = 0;
            for (; segptr < segments; segptr++) {
                int val = header[27 + segptr];
                body += val;
                bodysize -= val;
                if (val < 255) {
                    segptr++;
                    break;
                }
            }
        }
    }

    if (bodysize) {
        os->body_data = body;
        os->body_fill = bodysize;
    }

    {
        int saved = -1;
        while (segptr < segments) {
            int val = header[27 + segptr];
            os->lacing_vals[os->lacing_fill] = val;
            os->granule_vals[os->lacing_fill] = -1;

            if (bos) {
                os->lacing_vals[os->lacing_fill] |= 0x100;
                bos = 0;
            }

            if (val < 255)saved = os->lacing_fill;

            os->lacing_fill++;
            segptr++;

            if (val < 255)os->lacing_packet = os->lacing_fill;
        }

        /* set the granulepos on the last granuleval of the last full packet */
        if (saved != -1) {
            os->granule_vals[saved] = granulepos;
        }

    }

    if (eos) {
        os->e_o_s = 1;
        if (os->lacing_fill > 0)
            os->lacing_vals[os->lacing_fill - 1] |= 0x200;
    }

    os->pageno = pageno + 1;

    return(0);
}

static int _packetout(ogg_stream_state* os, ogg_packet* op, int adv) {

    /* The last part of decode. We have the stream broken into packet
       segments.  Now we need to group them into packets (or return the
       out of sync markers) */

    int ptr = os->lacing_returned;

    if (os->lacing_packet <= ptr)return(0);

    if (os->lacing_vals[ptr] & 0x400) {
        /* we need to tell the codec there's a gap; it might need to
           handle previous packet dependencies. */
        os->lacing_returned++;
        os->packetno++;
        return(-1);
    }

    if (!op && !adv)return(1); /* just using peek as an inexpensive way
                                 to ask if there's a whole packet
                                 waiting */

                                 /* Gather the whole packet. We'll have no holes or a partial packet */
    {
        int size = os->lacing_vals[ptr] & 0xff;
        long bytes = size;
        int eos = os->lacing_vals[ptr] & 0x200; /* last packet of the stream? */
        int bos = os->lacing_vals[ptr] & 0x100; /* first packet of the stream? */

        while (size == 255) {
            int val = os->lacing_vals[++ptr];
            size = val & 0xff;
            if (val & 0x200)eos = 0x200;
            bytes += size;
        }
        if (op) {
            op->e_o_s = eos;
            op->b_o_s = bos;
            op->packet = os->body_data + os->body_returned;
            op->packetno = os->packetno;
            op->granulepos = os->granule_vals[ptr];
            op->bytes = bytes;
        }

        if (adv) {
            os->body_returned += bytes;
            os->lacing_returned = ptr + 1;
            os->packetno++;
        }
    }
    return(1);
}

int ogg_stream_packetout(ogg_stream_state* os, ogg_packet* op) {
    if (ogg_stream_check(os)) return 0;
    return _packetout(os, op, 1);
}

int ogg_stream_packetpeek(ogg_stream_state* os, ogg_packet* op) {
    if (ogg_stream_check(os)) return 0;
    return _packetout(os, op, 0);
}

