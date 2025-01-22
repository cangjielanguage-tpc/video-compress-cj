/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

package videocompressor

public enum CompressQuality {
    | COMPRESS_QUALITY_HIGH
    | COMPRESS_QUALITY_MEDIUM
    | COMPRESS_QUALITY_LOW

    static func GetTypeNum(result: CompressQuality): Int64 {
        match (result) {
            case COMPRESS_QUALITY_HIGH => return 1
            case COMPRESS_QUALITY_MEDIUM => return 2
            case COMPRESS_QUALITY_LOW => return 3
        }
    }

    func toString(): String {
        match (this) {
            case COMPRESS_QUALITY_HIGH => "COMPRESS_QUALITY_HIGH"
            case COMPRESS_QUALITY_MEDIUM => "COMPRESS_QUALITY_MEDIUM"
            case COMPRESS_QUALITY_LOW => "COMPRESS_QUALITY_LOW"
        }
    }
}
