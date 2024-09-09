/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "video/encoder/VideoEnc.h"
#include <multimedia/player_framework/native_avmemory.h>
#include <multimedia/player_framework/native_avcapability.h>
#include <queue>
#include "hilog/log.h"
#include "tools.h"
using namespace std;

#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0x3200 // 全局domain宏，标识业务领域
#define LOG_TAG "videoCompressor" // 全局tag宏，标识模块日志tag

VideoEnc::~VideoEnc()
{
    Release();
}

namespace {
static void VencError(OH_AVCodec *codec, int32_t errorCode, void *userData) {}

static void VencFormatChanged(OH_AVCodec *codec, OH_AVFormat *format, void *userData) {}

static void VencOutputDataReady(OH_AVCodec *codec, uint32_t index, OH_AVMemory *data, OH_AVCodecBufferAttr *attr,
                                void *userData)
{
    VEncSignal *signal = static_cast<VEncSignal *>(userData);
    unique_lock<mutex> lock(signal->outMutex_);
    signal->outIdxQueue_.push(index);
    signal->outBufferQueue_.push(data);
    if (!attr) {
        OH_LOG_ERROR(LOG_APP, "VencOutputDataReady attr is null");
    } else {
        signal->attrQueue.push(*attr);
    }
    signal->outCond_.notify_all();
}
}

int32_t VideoEnc::ConfigureVideoEncoder(int quality)
{
    OH_AVFormat *format = OH_AVFormat_Create();
    if (format == nullptr) {
        OH_LOG_ERROR(LOG_APP, "Failed to create videoEncode format");
        return AV_ERR_UNKNOWN;
    }
    bitrate = GetQualityBitrate(quality);
    OH_LOG_ERROR(LOG_APP, "get videoEncode width:%{public}d---height"
                 "---%{public}d---bitrate:%{public}d---frameRate:%{public}f",
                 width, height, bitrate, frameRate);
    int32_t rateMode = static_cast<int32_t>(OH_VideoEncodeBitrateMode::CBR);
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_WIDTH, width);
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_HEIGHT, height);
    OH_AVCapability *cap = OH_AVCodec_GetCapability(videoMime.data(), true);
    std::string codecName = OH_AVCapability_GetName(cap);
    OH_LOG_ERROR(LOG_APP, "ConfigureVideoEncoder g_codecName:%{public}s", codecName.data());
    if (codecName.find("rk") == string::npos) {
        OH_LOG_ERROR(LOG_APP, "ConfigureVideoEncoder g_codecName npos");
        OH_AVFormat_SetIntValue(format, OH_MD_KEY_PIXEL_FORMAT, AV_PIXEL_FORMAT_SURFACE_FORMAT);
    } else {
        OH_LOG_ERROR(LOG_APP, "ConfigureVideoEncoder g_codecName no npos");
        OH_AVFormat_SetIntValue(format, OH_MD_KEY_PIXEL_FORMAT, AV_PIXEL_FORMAT_RGBA);
    }
    if (frameRate <= 0) {
        frameRate = defaultFrame;
    }
    if (bitrate <= 0) {
        bitrate = defaultBitrate;
    }
    OH_LOG_ERROR(LOG_APP, "final videoEncode width:%{public}d---height"
                 "---%{public}d---bitrate:%{public}d---frameRate:%{public}f",
                 width, height, bitrate, frameRate);
    OH_AVFormat_SetDoubleValue(format, OH_MD_KEY_FRAME_RATE, frameRate);
    OH_AVFormat_SetLongValue(format, OH_MD_KEY_BITRATE, bitrate);
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_VIDEO_ENCODE_BITRATE_MODE, rateMode);
    if (isHDRVivid == 1) {
        OH_AVFormat_SetIntValue(format, OH_MD_KEY_PROFILE, HEVC_PROFILE_MAIN_10);
    }
    int ret = OH_VideoEncoder_Configure(venc_, format);
    OH_AVFormat_Destroy(format);
    return ret;
}

uint32_t VideoEnc::GetQualityBitrate(int quality)
{
    const uint32_t highCompressor = 1;
    const uint32_t mediumCompressor = 2;
    const uint32_t lowCompressor = 3;
    double_t highProportions = 0.8;
    double_t mediumProportions = 0.5;
    double_t lowProportions = 0.3;
    switch (quality) {
        default:
        case highCompressor:
            bitrate = bitrate * highProportions;
            break;
        case mediumCompressor:
            bitrate = bitrate * mediumProportions;
            break;
        case lowCompressor:
            bitrate = bitrate * lowProportions;
            break;
    }
    bitrate = std::max(10000u, std::min(bitrate, 100000000u));
    return bitrate;
}

int32_t VideoEnc::SetVideoEncoderCallback()
{
    signal_ = make_unique<VEncSignal>();
    if (signal_ == nullptr) {
        OH_LOG_ERROR(LOG_APP, "Failed to new VencSignal");
        return AV_ERR_UNKNOWN;
    }
    
    cb_.onError = VencError;
    cb_.onStreamChanged = VencFormatChanged;
    cb_.onNeedOutputData = VencOutputDataReady;
    return OH_VideoEncoder_SetCallback(venc_, cb_, static_cast<void *>(signal_.get()));
}

void VideoEnc::RegisterMuxerManager(MutexManager *mutex)
{
    mutexManager = mutex;
}

int32_t VideoEnc::StartVideoEncoder()
{
    isRunning_.store(true);
    int ret = OH_VideoEncoder_Start(venc_);
    if (ret != AV_ERR_OK) {
        OH_LOG_ERROR(LOG_APP, "Failed to start video codec");
        isRunning_.store(false);
        signal_->outCond_.notify_all();
        Release();
        return ret;
    }
    
    outputLoop_ = make_unique<thread>(&VideoEnc::OutputFunc, this);
    if (outputLoop_ == nullptr) {
        OH_LOG_ERROR(LOG_APP, "Failed to cteate output video outputLoop");
        isRunning_.store(false);
        Release();
        return AV_ERR_UNKNOWN;
    }
    return AV_ERR_OK;
}

int32_t VideoEnc::CreateVideoEncoder(std::string codeName)
{
    OH_LOG_ERROR(LOG_APP, "CreateVideoEncode start CreateVideoEncoder");
    venc_ = OH_VideoEncoder_CreateByMime(codeName.data());
    return venc_ == nullptr ? AV_ERR_UNKNOWN : AV_ERR_OK;
}

void VideoEnc::WaitForEos()
{
    if (outputLoop_ && outputLoop_->joinable()) {
        outputLoop_->join();
    }
    outputLoop_ = nullptr;
    OH_LOG_ERROR(LOG_APP, "VideoEnc::WaitForEos END");
}

int32_t VideoEnc::GetSurface()
{
    return OH_VideoEncoder_GetSurface(venc_, &mutexManager->nativeWindow);
}

void VideoEnc::SendEncEos()
{
    if (venc_ == nullptr) {
        return;
    }
    
    int32_t ret = OH_VideoEncoder_NotifyEndOfStream(venc_);
    if (ret == 0) {
        OH_LOG_ERROR(LOG_APP, "ENC IN: input Eos notifyEndOfStream");
    } else {
        OH_LOG_ERROR(LOG_APP, "ENC IN: input Eos notifyEndOfStream error");
    }
}

void VideoEnc::OutputFunc()
{
    uint32_t errCount = 0;
    int64_t enCount = 0;
    while (true) {
        if (!isRunning_.load()) { break; }
        unique_lock<mutex> lock(signal_->outMutex_);
        signal_->outCond_.wait(lock, [this]() {
            return (signal_->outIdxQueue_.size() > 0 || !isRunning_.load());
        });
        if (!isRunning_.load()) { break; }
        uint32_t index = signal_->outIdxQueue_.front();
        OH_AVCodecBufferAttr attr = signal_->attrQueue.front();
        if (attr.flags == AVCODEC_BUFFER_FLAGS_EOS) {
            isRunning_.store(false);
            signal_->outCond_.notify_all();
            OH_LOG_ERROR(LOG_APP, "ENCODE EOS %{public}lld", enCount);
            break;
        }
        OH_AVMemory *buffer = signal_->outBufferQueue_.front();
        if (OH_AVMuxer_WriteSample(muxer->muxer, muxer->vTrackId, buffer, attr) != AV_ERR_OK) {
            OH_LOG_ERROR(LOG_APP, "input video track data failed");
        }
        
        if (OH_VideoEncoder_FreeOutputData(venc_, index) != AV_ERR_OK) {
            OH_LOG_ERROR(LOG_APP, "videoEncode FreeOutputDat error");
            errCount = errCount + 1;
        }
        
        if (errCount > 0) {
            OH_LOG_ERROR(LOG_APP, "videoEncode errCount > 0");
            isRunning_.store(false);
            signal_->outCond_.notify_all();
            Release();
            break;
        }
        signal_->outIdxQueue_.pop();
        signal_->attrQueue.pop();
        signal_->outBufferQueue_.pop();
        enCount++;
    }
}

void VideoEnc::RegisterMuxer(Muxer *m)
{
    muxer = m;
}

int32_t VideoEnc::Release()
{
    StopOutLoop();
    if (venc_ != nullptr) {
        OH_VideoEncoder_Flush(venc_);
        OH_VideoEncoder_Stop(venc_);
        int ret = OH_VideoEncoder_Destroy(venc_);
        venc_ = nullptr;
        if (signal_ != nullptr) {
            signal_ = nullptr;
        }
        return ret;
    }
    return AV_ERR_OK;
}

void VideoEnc::StopOutLoop()
{
    if (outputLoop_ != nullptr && outputLoop_->joinable()) {
        unique_lock<mutex> lock(signal_->outMutex_);
        Tools::ClearIntQueue(signal_->outIdxQueue_);
        Tools::ClearBufferQueue(signal_->attrQueue);
        Tools::ClearMemoryBufferQueue(signal_->outBufferQueue_);
        signal_->outCond_.notify_all();
        lock.unlock();
        outputLoop_->join();
    }
}