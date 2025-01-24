/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "stdint.h"
#include <string>

#ifndef libcompressor_cjcodec_H
#define libcompressor_cjcodec_H

struct AsyncCallbackInfo {
    int32_t outFd = true;
    int32_t inputFd = true;
    int32_t inputOffset = 0;
    int64_t inputSize = 0;
    int32_t quality = 0;
    int32_t resultCode = 0;
    char* resultStr = "";
    char* outputPath = "";
};

#ifdef __cplusplus
extern "C" {
#endif

AsyncCallbackInfo* CompressVideo(int32_t outFd, int32_t inputFd,
    int32_t inputOffset, int64_t inputSize, int32_t quality, char *outputPath);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* libcompressor_cjcodec_H */
