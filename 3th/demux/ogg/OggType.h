#pragma once
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>

typedef int16_t ogg_int16_t;
typedef uint16_t ogg_uint16_t;
typedef int32_t ogg_int32_t;
typedef uint32_t ogg_uint32_t;
typedef int64_t ogg_int64_t;
typedef uint64_t ogg_uint64_t;

typedef void* (OGG_MALLOC)(int size);
typedef void* (OGG_CALLOC)(int count, int size);
typedef void* (OGG_REALLOC)(void* block, int size);
typedef void  (OGG_FREE)(void* block);

typedef struct {
	void* iov_base;
	size_t iov_len;
} ogg_iovec_t;


typedef struct {
	long endbyte;
	int  endbit;

	unsigned char* buffer;
	unsigned char* ptr;
	long storage;
} oggpack_buffer;

/* ogg_page is used to encapsulate the data in one Ogg bitstream page *****/

typedef struct {
	unsigned char* header;
	long header_len;
	unsigned char* body;
	long body_len;
	long body_len_valid;
} ogg_page;

/* ogg_stream_state contains the current encode/decode state of a logical
   Ogg bitstream **********************************************************/
typedef struct {
	unsigned char* body_data;    /* bytes from packet bodies */
	long	body_data_valid_len;
	long    body_storage;          /* storage elements allocated */
	long    body_fill;             /* elements stored; fill mark */
	long    body_returned;         /* elements of fill returned */
	int *lacing_vals;      /* The values that will go to the segment table */
	ogg_int64_t *granule_vals; /* granulepos values for headers. Not compact
								  this way, but it is simple coupled to the
								  lacing fifo */
	long    lacing_storage;
	long    lacing_fill;
	long    lacing_packet;
	long    lacing_returned;

	unsigned char    header[282];      /* working space for header encode */
	int              header_fill;

	int     e_o_s;          /* set when we have buffered the last packet in the
							   logical bitstream */
	int     b_o_s;          /* set after we've written the initial page
							   of a logical bitstream */
	long    serialno;
	long    pageno;
	ogg_int64_t  packetno;  /* sequence number for decode; the framing
							   knows where there's a hole in the data,
							   but we need coupling so that the codec
							   (which is in a separate abstraction
							   layer) also knows about the gap */
	ogg_int64_t   granulepos;
} ogg_stream_state;

/* ogg_packet is used to encapsulate the data and metadata belonging
   to a single raw Ogg/Vorbis packet *************************************/

typedef struct {
	unsigned char* packet;
	long  bytes;
	long  b_o_s;
	long  e_o_s;

	ogg_int64_t  granulepos;

	ogg_int64_t  packetno;     /* sequence number for decode; the framing
								  knows where there's a hole in the data,
								  but we need coupling so that the codec
								  (which is in a separate abstraction
								  layer) also knows about the gap */
#if 0
	OGG_MALLOC* malloc;
	OGG_REALLOC* realloc;
	OGG_FREE* free;
#endif
} ogg_packet;




typedef struct {
	unsigned char* data;
	int storage;
	int fill;
	int returned;

	int unsynced;
	int headerbytes;
	int bodybytes;

	
	//OGG_MALLOC *malloc;
	//OGG_REALLOC *realloc;
	//OGG_FREE *free;
} ogg_sync_state;