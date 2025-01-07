/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

package videocompressor

public class CompressorResponse {
    public var code: CompressorResponseCode = CompressorResponseCode.OTHER_ERROR
    public var message: String = ""
    public var outputPath: String = ""

    init(code: CompressorResponseCode, message: String, outputPath: String) {
        this.code = code
        this.message = message
        this.outputPath = outputPath
    }
}
