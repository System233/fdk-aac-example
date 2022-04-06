/**
 * Copyright (c) 2022 github.com/System233
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include <stdio.h>
#include <stdint.h>
#include <aacdecoder_lib.h>
#include "wavreader.h"
int main(int argc, char const *argv[])
{
    if(argc<3){
        fprintf(stderr,"Usage: aac-dec [in-aac-file] [out-pcm-file]");
        return 1;
    }
    char const*infile=argv[1];
    char const*outfile=argv[2];
    HANDLE_AACDECODER decoder=aacDecoder_Open(TT_MP4_ADTS,2);
    // aacDecoder_SetParam(decoder,AAC_PCM_MIN_OUTPUT_CHANNELS,2);
    // aacDecoder_SetParam(decoder,AAC_PCM_MAX_OUTPUT_CHANNELS,2);
    // aacDecoder_SetParam(decoder,AAC_PCM_OUTPUT_CHANNEL_MAPPING,1);
    FILE*fp_aac=fopen(infile,"rb");
    if(!fp_aac){
        fprintf(stderr,"open fail");
        return -1;
    }
    static UCHAR inbuf[20480];
    static INT_PCM outbuf[20480];
    UCHAR*pInbuf=inbuf;
    FILE*fp_pcm=fopen(outfile,"wb");
    int len;
    CStreamInfo*info=NULL;
    while((len=fread(inbuf,sizeof(*inbuf),sizeof(inbuf),fp_aac))){
        UINT inLen=len;
        UINT bytesValid=len;
        while(bytesValid){
            if(aacDecoder_Fill(decoder,&pInbuf,&inLen,&bytesValid)!=AAC_DEC_OK){
                fprintf(stderr,"aacDecoder_Fill error");
                return 1;
            }
            AAC_DECODER_ERROR err;
            while ((err=aacDecoder_DecodeFrame(decoder,outbuf,sizeof(outbuf)/sizeof(*outbuf),0))==AAC_DEC_OK)
            {
                if(!info){
                    info=aacDecoder_GetStreamInfo(decoder);
                    fprintf(stdout,"sampleRate:%d,bitRate:%d,frameSize:%d,channels:%d\n",
                        info->sampleRate,
                        info->bitRate,
                        info->frameSize,
                        info->numChannels
                    );
                }
                fwrite(outbuf,sizeof(INT_PCM),info->frameSize*info->numChannels,fp_pcm);
            }
            if(err!=AAC_DEC_NOT_ENOUGH_BITS){
                fprintf(stderr,"decode fail :%#x",err);
                return 1;
            }
        }
    };
exit:
    fclose(fp_pcm);
    fclose(fp_aac);
    aacDecoder_Close(decoder);
    return 0;
}
