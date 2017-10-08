#include <jni.h>
#include <string>
#include <android/log.h>
#include <unistd.h>



#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"mahao",FORMAT,##__VA_ARGS__)

extern "C"{

//封装格式处理
#include "libavformat/avformat.h"

//编码
#include "libavcodec/avcodec.h"

//像素处理
#include "libswscale/swscale.h"

#include <android/native_window.h>



}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_mahao_ffmpegstudy_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject jobject) {


    std::string hello = "Hello from C++";

    av_register_all();

    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_mahao_ffmpegstudy_MainActivity_open(
                JNIEnv *env,
                jobject jobject1,
                jstring  inputStr_,
                jstring outStr_){

    //注册所有组件
    av_register_all();

    const  char* inputStr = env->GetStringUTFChars(inputStr_,0);
    const char*outStr = env->GetStringUTFChars(outStr_,0);

    AVFormatContext* pContext  = avformat_alloc_context();
    if (avformat_open_input(&pContext,inputStr,NULL,NULL) < 0){

        LOGI("打开失败");
        return;
    }

    if (avformat_find_stream_info(pContext,NULL) < 0){

        LOGI("获取信息失败");
        return;
    }

    int vedio_stream_idx = -1;
    //找视频流
    for(int i = 0; i < pContext->nb_streams; i++){

       //codc 每一个流队形的编解码的上下文  codec_type 流的类型；
        if (pContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {

            vedio_stream_idx = i;
        }
    }

    //解码器的上下文
    AVCodecContext *pContextctx = pContext->streams[vedio_stream_idx]->codec;
    //获取解码器
    AVCodec* pCodex =  avcodec_find_decoder(pContextctx->codec_id);

    if (avcodec_open2(pContextctx,pCodex,NULL) < 0){

        LOGI("解码失败");
        return;
    }

    //分配内存 malloc AvPacket
    AVPacket *packet = (AVPacket*) av_malloc(sizeof(AVPacket));
    //初始化结构体
    av_init_packet(packet);

    AVFrame* frame = av_frame_alloc();
    //声明一个yuv
    AVFrame* yuvFrame = av_frame_alloc();
    //给yuv的缓冲区初始化
    uint8_t  *out_buffer = (uint8_t*) av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,pContextctx->width,pContextctx->height));

    //Setup the picture fields based on the specified image parameters
    // and the provided image data buffer.
    int requestSize = avpicture_fill((AVPicture *) yuvFrame, out_buffer, AV_PIX_FMT_YUV420P, pContextctx->width, pContextctx->height);
    LOGI("申请内存%d",requestSize);

    //pContextctx->pix_fmt :: Pixel format, see AV_PIX_FMT_xxx
    SwsContext *swsContext = sws_getContext(pContextctx->width,pContextctx->height,pContextctx->pix_fmt,
                                            pContextctx->width,pContextctx->height,AV_PIX_FMT_YUV420P
    ,SWS_BILINEAR,NULL,NULL,NULL);

    int frameCount= 0;
    FILE* fp_yuv = fopen(outStr,"wb");

    int got_frame;
    while(av_read_frame(pContext,packet) >= 0){


        // Decode the video frame of size avpkt->size from avpkt->data into picture.
        avcodec_decode_video2(pContextctx,frame,&got_frame,packet);

        if (got_frame > 0){

            sws_scale(swsContext, (const uint8_t *const *) frame->data, frame->linesize, 0, frame->height, yuvFrame->data, yuvFrame->linesize);
            int ysize = pContextctx->width * pContextctx->height;

            fwrite(yuvFrame->data[0],1,ysize,fp_yuv);
            fwrite(yuvFrame->data[1],1,ysize/4,fp_yuv);
            fwrite(yuvFrame->data[2],1,ysize/4,fp_yuv);
        }

        av_free_packet(packet);

    }
    fclose(fp_yuv);
    av_frame_free(&frame);
    av_frame_free(&yuvFrame);
    avcodec_close(pContextctx);
    avformat_free_context(pContext);
    env->ReleaseStringUTFChars(inputStr_,inputStr);
    env->ReleaseStringUTFChars(outStr_,outStr);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_mahao_ffmpegstudy_MyVideoView_render(
        JNIEnv *env,
        jobject jobject1,
        jstring  inputStr_,
        jobject surface){

    //注册所有组件
    av_register_all();

    const  char* inputStr = env->GetStringUTFChars(inputStr_,0);

    AVFormatContext* pContext  = avformat_alloc_context();
    if (avformat_open_input(&pContext,inputStr,NULL,NULL) < 0){

        LOGI("打开失败");
        return;
    }

    if (avformat_find_stream_info(pContext,NULL) < 0){

        LOGI("获取信息失败");
        return;
    }

    int vedio_stream_idx = -1;
    //找视频流
    for(int i = 0; i < pContext->nb_streams; i++){

        //codc 每一个流队形的编解码的上下文  codec_type 流的类型；
        if (pContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {

            vedio_stream_idx = i;
        }
    }

    //解码器的上下文
    AVCodecContext *pContextctx = pContext->streams[vedio_stream_idx]->codec;
    //获取解码器
    AVCodec* pCodex =  avcodec_find_decoder(pContextctx->codec_id);

    if (avcodec_open2(pContextctx,pCodex,NULL) < 0){

        LOGI("解码失败");
        return;
    }

    //分配内存 malloc AvPacket
    AVPacket *packet = (AVPacket*) av_malloc(sizeof(AVPacket));
    //初始化结构体
    av_init_packet(packet);

    AVFrame* frame = av_frame_alloc();
    //声明一个yuv
    AVFrame* rgbFrame = av_frame_alloc();
    //给yuv的缓冲区初始化
    uint8_t  *out_buffer = (uint8_t*) av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,pContextctx->width,pContextctx->height));

    //Setup the picture fields based on the specified image parameters
    // and the provided image data buffer.
    int requestSize = avpicture_fill((AVPicture *) rgbFrame, out_buffer, AV_PIX_FMT_YUV420P, pContextctx->width, pContextctx->height);
    LOGI("申请内存%d",requestSize);

    //pContextctx->pix_fmt :: Pixel format, see AV_PIX_FMT_xxx
    SwsContext *swsContext = sws_getContext(pContextctx->width,pContextctx->height,pContextctx->pix_fmt,
                                            pContextctx->width,pContextctx->height,AV_PIX_FMT_YUV420P
            ,SWS_BILINEAR,NULL,NULL,NULL);

    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env,surface);


    int got_frmae;
    int length;
    //视屏缓冲区
    ANativeWindow_Buffer outbuffer;
    while (av_read_frame(pContext,packet) >= 0) {

        if (packet->stream_index == vedio_stream_idx) {

            length = avcodec_decode_video2(pContextctx, frame, &got_frmae, packet);
            if (got_frmae) {


                // Change the format and size of the window buffers
                ANativeWindow_setBuffersGeometry(nativeWindow, pContextctx->width,
                                                 pContextctx->height,
                                                 WINDOW_FORMAT_RGBA_8888);

                //绘制
                ANativeWindow_lock(nativeWindow, &outbuffer, NULL);

                sws_scale(swsContext, (const uint8_t *const *) frame->data, frame->linesize, 0,
                          pContextctx->height, rgbFrame->data, rgbFrame->linesize);


                //真实位数
                uint8_t *dest = (uint8_t *) outbuffer.bits;
                //拿到每一行有多少个字节RGBA
                int destTride = outbuffer.stride * 4;

                //像素数据的首地址
                uint8_t *src = rgbFrame->data[0];

                //实际内存一行的数量
                int srcStride = rgbFrame->linesize[0];

                for (int i = 0; i < pContextctx->height; i++) {

                    memcpy(dest + i * destTride, src + i * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
                usleep(1000 * 16);
            }
        }
        av_free_packet(packet);
    }
    ANativeWindow_release(nativeWindow);
    av_frame_free(&frame);
    avcodec_close(pContextctx);
    avformat_free_context(pContext);
    env->ReleaseStringUTFChars(inputStr_,inputStr);
}




