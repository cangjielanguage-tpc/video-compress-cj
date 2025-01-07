/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <bits/alltypes.h>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <string>
#include "muxer.h"
#include "DeMuxer.h"
#include "hilog/log.h"
#include "napi/native_api.h"
#include "cjcodec.h"
#include "video/decoder/VideoDec.h"
#include "video/encoder/VideoEnc.h"
#include "audio/decoder/AudioDec.h"
#include "audio/encoder/AudioEnc.h"
#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0x3200 // 全局domain宏，标识业务领域
#define LOG_TAG "videoCompressor" // 全局tag宏，标识模块日志tag


struct VideoCompressorBean {
    std::unique_ptr<AudioEnc> *aEncSample;
    std::unique_ptr<AudioDec> *aDecSample;
    std::unique_ptr<VideoDec> *vDecSample;
    std::unique_ptr<VideoEnc> *vEncSample;
    std::unique_ptr<DeMuxer> *demuxer;
    std::unique_ptr<Muxer> *muxer;
    AsyncCallbackInfo *asyncCallbackInfo;
    std::unique_ptr<MutexManager> *mutexManager;
};
namespace {


int32_t CreateMutex(VideoCompressorBean *bean)
{
    OH_LOG_ERROR(LOG_APP, "CreateMutex start");
    bean->muxer->get()->width = bean->demuxer->get()->width;
    bean->muxer->get()->height = bean->demuxer->get()->height;
    bean->muxer->get()->rotation = bean->demuxer->get()->rotation;
    bean->muxer->get()->videoMime = bean->demuxer->get()->videoMime;
    bean->muxer->get()->audioMime = bean->demuxer->get()->audioMime;
    bean->muxer->get()->audioChannelCount = bean->demuxer->get()->audioChannelCount;
    bean->muxer->get()->audioBitrate = bean->demuxer->get()->audioBitrate;
    bean->muxer->get()->audioSampleRate = bean->demuxer->get()->audioSampleRate;
    bean->muxer->get()->videoFrameRate = bean->demuxer->get()->frameRate;
    int ret = bean->muxer->get()->CreateMuxer(bean->asyncCallbackInfo->outFd);
    OH_LOG_ERROR(LOG_APP, "CreateMutex end");
    return ret;
}


int32_t CreateVideoEncode(VideoCompressorBean *bean)
{
    bean->vEncSample->get()->RegisterMuxerManager(bean->mutexManager->get());
    bean->vEncSample->get()->width = bean->demuxer->get()->width;
    bean->vEncSample->get()->height = bean->demuxer->get()->height;
    bean->vEncSample->get()->bitrate = bean->demuxer->get()->bitrate;
    bean->vEncSample->get()->frameRate = bean->demuxer->get()->frameRate;
    bean->vEncSample->get()->videoMime = bean->demuxer->get()->videoMime;
    bean->vEncSample->get()->isHDRVivid = bean->demuxer->get()->isHDRVivid;
    int ret = bean->vEncSample->get()->CreateVideoEncoder(bean->demuxer->get()->videoMime);
    if (ret != AV_ERR_OK) {
        OH_LOG_ERROR(LOG_APP, "CreateVideoEncoder error");
        return ret;
    }
    ret = bean->vEncSample->get()->SetVideoEncoderCallback();
    if (ret != AV_ERR_OK) {
        OH_LOG_ERROR(LOG_APP, "VideoEncoder SetVideoEncoderCallback %{public}d", ret);
        return ret;
    }
    ret = bean->vEncSample->get()->ConfigureVideoEncoder(bean->asyncCallbackInfo->quality);
    if (ret != AV_ERR_OK) {
        OH_LOG_ERROR(LOG_APP, "VideoEncoder ConfigureVideoEncoder %{public}d", ret);
        return ret;
    }
    ret = bean->vEncSample->get()->GetSurface();
    if (ret != AV_ERR_OK) {
        OH_LOG_ERROR(LOG_APP, "VideoEncoder GetSurface %{public}d", ret);
        return ret;
    }
    bean->vEncSample->get()->RegisterMuxer(bean->muxer->get());
    OH_LOG_ERROR(LOG_APP, "CreateVideoEncode end");
    return ret;
}


int32_t CreateAudioEncode(VideoCompressorBean *bean)
{
    bean->aEncSample->get()->RegisterMuxerManager(bean->mutexManager->get());
    bean->aEncSample->get()->audioChannelCount = bean->demuxer->get()->audioChannelCount;
    bean->aEncSample->get()->audioSampleRate = bean->demuxer->get()->audioSampleRate;
    bean->aEncSample->get()->audioBitrate = bean->demuxer->get()->audioBitrate;
    int ret = bean->aEncSample->get()->CreateAudioEncoder("audio/mp4a-latm");
    if (ret != AV_ERR_OK) {
        OH_LOG_ERROR(LOG_APP, "CreateAudioEncoder error");
        return ret;
    }
    ret = bean->aEncSample->get()->SetAudioEncoderCallback();
    if (ret != AV_ERR_OK) {
        OH_LOG_ERROR(LOG_APP, "AudioEncoder SetAudioEncoderCallback %{public}d", ret);
        return ret;
    }
    ret = bean->aEncSample->get()->ConfigureAudioEncoder();
    if (ret != AV_ERR_OK) {
        OH_LOG_ERROR(LOG_APP, "AudioEncoder ConfigureAudioEncoder %{public}d", ret);
        return ret;
    }
    bean->aEncSample->get()->RegisterMuxer(bean->muxer->get());
    OH_LOG_ERROR(LOG_APP, "CreateAudioEncode end");
    return ret;
}


int32_t CreateDemuxer(VideoCompressorBean *bean)
{
    bean->demuxer->get()->RegisterMuxerManager(bean->mutexManager->get());
    int32_t ret = bean->demuxer->get()->CreateDemuxer(bean->asyncCallbackInfo->inputFd,
        bean->asyncCallbackInfo->inputOffset, bean->asyncCallbackInfo->inputSize);
    if (ret != AV_ERR_OK) {
            OH_LOG_ERROR(LOG_APP, "CreateDemuxer error %{public}d", ret);
            return ret;
        }
    OH_LOG_ERROR(LOG_APP, "CreateDemuxer end");
    return ret;
}


void CreateVideoDecode(VideoCompressorBean *bean)
{
    OH_LOG_ERROR(LOG_APP, "CreateVideoDecode start");
    bean->vDecSample->get()->RegisterMuxerManager(bean->mutexManager->get());
    bean->vDecSample->get()->width = bean->demuxer->get()->width;
    bean->vDecSample->get()->height = bean->demuxer->get()->height;
    bean->vDecSample->get()->videoMime = bean->demuxer->get()->videoMime;
    bean->vDecSample->get()->RegisterDeMuxer(bean->demuxer->get());
    if (bean->demuxer->get()->frameRate > 0) {
        bean->vDecSample->get()->defaultFrameRate = bean->demuxer->get()->frameRate;
    }
    bean->vDecSample->get()->VideoEnc = bean->vEncSample->get();
    OH_LOG_ERROR(LOG_APP, "CreateVideoDecode end");
}


void CreateAudioDecode(VideoCompressorBean *bean)
{
    OH_LOG_ERROR(LOG_APP, "CreateAudioDecode start");
    bean->aDecSample->get()->RegisterMuxerManager(bean->mutexManager->get());
    bean->aDecSample->get()->audioBitrate = bean->demuxer->get()->audioBitrate;
    bean->aDecSample->get()->aacIsADTS = bean->demuxer->get()->aacIsADTS;
    bean->aDecSample->get()->audioSampleFormat = bean->demuxer->get()->audioSampleFormat;
    bean->aDecSample->get()->audioSampleRate = bean->demuxer->get()->audioSampleRate;
    bean->aDecSample->get()->audioChannelCount = bean->demuxer->get()->audioChannelCount;
    bean->aDecSample->get()->RegisterDeMuxer(bean->demuxer->get());
    OH_LOG_ERROR(LOG_APP, "CreateAudioDecode end");
}


void Release(VideoCompressorBean *bean)
{
    bean->vDecSample->get()->Release();
    bean->aDecSample->get()->Release();
    bean->vEncSample->get()->Release();
    bean->aEncSample->get()->Release();
    bean->muxer->get()->StopMuxer();
    bean->demuxer->get()->Release();
    bean->vDecSample = nullptr;
    bean->aDecSample = nullptr;
    bean->vEncSample = nullptr;
    bean->aEncSample = nullptr;
    bean->muxer = nullptr;
    bean->demuxer = nullptr;
    bean = nullptr;
}


void VideoCompressorWaitEos(VideoCompressorBean *bean, bool hasAudio)
{
    OH_LOG_ERROR(LOG_APP, "VideoCompressorDestroy");
    if (bean->vDecSample->get() != nullptr) {
        bean->vDecSample->get()->WaitForEos();
    }
    OH_LOG_ERROR(LOG_APP, "VideoCompressorDestroy1");
    if (hasAudio && bean->aDecSample->get() != nullptr) {
        bean->aDecSample->get()->WaitForEos();
    }
    OH_LOG_ERROR(LOG_APP, "VideoCompressorDestroy2");
    if (bean->vEncSample != nullptr) {
        bean->vEncSample->get()->WaitForEos();
    }
    OH_LOG_ERROR(LOG_APP, "VideoCompressorDestroy3");
    if (hasAudio && bean->aEncSample->get() != nullptr) {
        bean->aEncSample->get()->WaitForEos();
    }
    OH_LOG_ERROR(LOG_APP, "VideoCompressorDestroy4");
    if (bean->muxer != nullptr) {
        bean->muxer->get()->StopMuxer();
    }
    OH_LOG_ERROR(LOG_APP, "VideoCompressorDestroy5");
    if (bean->demuxer->get() != nullptr) {
        bean->demuxer->get()->DestroyDemuxer();
    }
    OH_LOG_ERROR(LOG_APP, "VideoCompressorDestroy6");
    Release(bean);
    OH_LOG_ERROR(LOG_APP, "VideoCompressorDestroy end");
}


void SetCallBackResult(VideoCompressorBean *bean, int32_t code, std::string str)
{
    OH_LOG_ERROR(LOG_APP, "%{public}s", str.c_str());
    bean->asyncCallbackInfo->resultCode = code;
    char* cstr = (char*)malloc(str.length()+1);
    strcpy(cstr, str.c_str());
    bean->asyncCallbackInfo->resultStr = cstr;
}


int32_t JudgeMime(VideoCompressorBean *bean)
{
    if (bean->demuxer->get()->videoMime != "video/avc" && bean->demuxer->get()->videoMime != "video/hevc") {
        SetCallBackResult(bean, -1, "not support video mime:" + bean->demuxer->get()->videoMime);
        Release(bean);
        return AV_ERR_UNKNOWN;
    }
    if (bean->demuxer->get()->audioMime != "audio/mp4a-latm" && bean->demuxer->get()->audioMime != "") {
        SetCallBackResult(bean, -1, "not support audio mime:" + bean->demuxer->get()->audioMime);
        Release(bean);
        return AV_ERR_UNKNOWN;
    }
    return AV_ERR_OK;
}


bool HasAudio(VideoCompressorBean *bean)
{
    return bean->demuxer->get()->audioMime != "";
}


AsyncCallbackInfo* NativeCompressStart(VideoCompressorBean *bean)
{
    if (CreateDemuxer(bean) != AV_ERR_OK) {
        SetCallBackResult(bean, -1, "CreateDemuxer error");
        Release(bean);
        return bean->asyncCallbackInfo;
    }
    if (JudgeMime(bean) != AV_ERR_OK) {
        return bean->asyncCallbackInfo;
    }
    bool hasAudio = HasAudio(bean);
    if (CreateMutex(bean) != AV_ERR_OK) {
        SetCallBackResult(bean, -1, "CreateMuxer error");
        Release(bean);
        return bean->asyncCallbackInfo;
    }
    
    if (CreateVideoEncode(bean) != AV_ERR_OK) {
        SetCallBackResult(bean, -1, "CreateVideoEncode error");
        Release(bean);
        return bean->asyncCallbackInfo;
    }
    
    if (hasAudio && CreateAudioEncode(bean) != AV_ERR_OK) {
        SetCallBackResult(bean, -1, "CreateAudioEncode error");
        Release(bean);
        return bean->asyncCallbackInfo;
    }
    
    // 视频编码
    if (bean->vEncSample->get()->StartVideoEncoder() != AV_ERR_OK) {
        SetCallBackResult(bean, -1, "videoEncode error");
        Release(bean);
        return bean->asyncCallbackInfo;
    }
    
    // 音频编码
    if (hasAudio && bean->aEncSample->get()->StartAudioEncoder() != AV_ERR_OK) {
        SetCallBackResult(bean, -1, "audioEncode error");
        Release(bean);
        return bean->asyncCallbackInfo;
    }

    CreateVideoDecode(bean);
    // 视频解码
    if (bean->vDecSample->get()->RunVideoDec(bean->demuxer->get()->videoMime) != AV_ERR_OK) {
        SetCallBackResult(bean, -1, "VideoDecoder decode error");
        Release(bean);
        return bean->asyncCallbackInfo;
    }

    if (hasAudio) {
        CreateAudioDecode(bean);
        // 音频解码
        if (bean->aDecSample->get()->RunAudioDec(bean->demuxer->get()->audioMime) != AV_ERR_OK) {
            SetCallBackResult(bean, -1, "AudioDecoder decode error");
            Release(bean);
            return bean->asyncCallbackInfo;
        }
    }

    SetCallBackResult(bean, 0, "videoCompressor success");
    VideoCompressorWaitEos(bean, hasAudio);
    return bean->asyncCallbackInfo;
}


// 主要功能函数
AsyncCallbackInfo* NativeCompressVideo(void *data)
{
    AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
    auto mutexManager = std::make_unique<MutexManager>();
    // 创建demuxer
    auto demuxer = std::make_unique<DeMuxer>();
    // 创建muxer
    auto muxer = std::make_unique<Muxer>();
    // 创建视频编码器
    auto vEncSample = std::make_unique<VideoEnc>();
    // 创建音频编码器
    auto aEncSample = std::make_unique<AudioEnc>();
    // 创建视频解码器
    auto vDecSample = std::make_unique<VideoDec>();
    // 创建音频解码器
    auto aDecSample = std::make_unique<AudioDec>();
    VideoCompressorBean *bean = new VideoCompressorBean();
    bean->aDecSample = &aDecSample;
    bean->aEncSample = &aEncSample;
    bean->vDecSample = &vDecSample;
    bean->vEncSample = &vEncSample;
    bean->demuxer = &demuxer;
    bean->muxer =& muxer;
    bean->asyncCallbackInfo = asyncCallbackInfo;
    bean->mutexManager = &mutexManager;
    return NativeCompressStart(bean);
}
}


// 入口方法
AsyncCallbackInfo* CompressVideo(int32_t outFd, int32_t inputFd, int32_t inputOffset, 
    int64_t inputSize, int32_t quality, char* output) 
{
    // 这些参数组成AsyncCallbackInfo
    AsyncCallbackInfo *asyncCallbackInfo = new AsyncCallbackInfo();
    asyncCallbackInfo->outFd = outFd;
    asyncCallbackInfo->inputFd = inputFd;
    asyncCallbackInfo->inputOffset = inputOffset;
    asyncCallbackInfo->inputSize = inputSize;
    asyncCallbackInfo->quality = quality;
    asyncCallbackInfo->outputPath = output;
    return NativeCompressVideo(asyncCallbackInfo);
}
