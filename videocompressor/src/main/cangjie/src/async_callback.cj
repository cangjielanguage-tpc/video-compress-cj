/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

package videocompressor

@C
struct AsyncCallbackInfoCj {
    var outFd: Int32 = 0
    var inputFd: Int32 = 0
    var inputOffset: Int32 = 0
    var inputSize: Int64 = 0
    var quality: Int32 = 0
    var resultCode: Int32 = 0
    var resultStr: CString = CString(CPointer<UInt8>())
    var outputPath: CString = CString(CPointer<UInt8>())
}
