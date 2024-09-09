/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights resvered.
 */

package videocompressor

public enum CompressorResponseCode {
    | SUCCESS
    | OTHER_ERROR

    static func GetTypeNum(result: CompressorResponseCode): Int64 {
        match (result) {
            case SUCCESS => return 0
            case OTHER_ERROR => return -1
        }
    }
}
