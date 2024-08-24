#include "H265Dec.h"
#include "de265.h"


b1 H265Dec_c::Init(){
    int disable_deblocking = 1;
    int disable_sao = 1;
    int verbosity = 0;

    _hd = de265_new_decoder();

    de265_decoder_context* ctx = (de265_decoder_context*)_hd;
    de265_set_parameter_bool(ctx, DE265_DECODER_PARAM_BOOL_SEI_CHECK_HASH, _check_hash);
    de265_set_parameter_bool(ctx, DE265_DECODER_PARAM_SUPPRESS_FAULTY_PICTURES, false);
    de265_set_parameter_bool(ctx, DE265_DECODER_PARAM_DISABLE_DEBLOCKING, disable_deblocking);
    de265_set_parameter_bool(ctx, DE265_DECODER_PARAM_DISABLE_SAO, disable_sao);
#if 0 
    //dump nal headers
    de265_set_parameter_int(ctx, DE265_DECODER_PARAM_DUMP_SPS_HEADERS, 1);
    de265_set_parameter_int(ctx, DE265_DECODER_PARAM_DUMP_VPS_HEADERS, 1);
    de265_set_parameter_int(ctx, DE265_DECODER_PARAM_DUMP_PPS_HEADERS, 1);
    de265_set_parameter_int(ctx, DE265_DECODER_PARAM_DUMP_SLICE_HEADERS, 1);    
#endif

#if 1
    //acceleration
    de265_set_parameter_int(ctx, DE265_DECODER_PARAM_ACCELERATION_CODE, de265_acceleration_SCALAR);
#endif

#if 0
        de265_disable_logging();
#endif
    de265_set_verbosity(verbosity);
    return true;
}


b1 H265Dec_c::Receive(u8 *in,i32 inByte){

    de265_error err = DE265_OK;
    static int pushedByte=0;
    err = de265_push_data((de265_decoder_context*)_hd, in, inByte, pushedByte, (void*)2);
    if (err != DE265_OK) {
        return false;
    }
    pushedByte+=inByte;
    return true;
}

b1 H265Dec_c::Generate(u8 *out,i32 *outByte){
    *outByte=0;
    de265_error err = DE265_OK;
    int more = 0;
    
    while (1){
        err = de265_decode((de265_decoder_context*)_hd, &more);
        if (err != DE265_OK) {
            if (_check_hash && err == DE265_ERROR_CHECKSUM_MISMATCH)
                return false;
            break;
        }
        if (!more)
            break;
        const de265_image* img = de265_get_next_picture((de265_decoder_context*)_hd);
        if (img) {
            //width = de265_get_image_width(img, 0);
            //height = de265_get_image_height(img, 0);
            _framecnt++;
            for (int c = 0; c < 3; c++) {
                int stride;
                const uint8_t* p = de265_get_image_plane(img, c, &stride);
                int width = de265_get_image_width(img, c);
                for (int y = 0; y < de265_get_image_height(img, c); y++) {
                    ALGO_MEM_CPY(out,&p[y * stride],width);
                    out+=width;
                    *outByte+=width;
                }
            }
            //ALGO_PRINT("[%d]", _framecnt);
    #if 0
            if (_framecnt >= frameNum)
            {
                stop = 1;
                break;
            }
            if ((_framecnt % 100) == 0) {
                fprintf(stderr, "frame %d\r", _framecnt);
            }
    #endif
        }
    }
    return true;
}

b1 H265Dec_c::DeInit(){
    de265_error err = DE265_OK;
    err = de265_flush_data((de265_decoder_context*)_hd); // indicate end of stream
    de265_free_decoder((de265_decoder_context*)_hd);
    return true;
}

