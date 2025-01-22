/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

package ohos_app_cangjie_entry

import ohos.state_macro_manage.*
import videocompressor.*
import videocompressor.*
import ohos.concurrency.*

@Entry
@Component
class Compressmp4Low {
    @State
    var message: String = "开始压缩"

    @State
    var compressresult: String = ""

    @State
    var res: ?CompressorResponse = Option<CompressorResponse>.None
    func build() {
        Row {
            Column {
                Button(message).onClick {
                    let comprossor: VideoCompressor = VideoCompressor()
                    let inputFilePath: String = "/data/storage/el1/bundle/test.mp4"
                    let fut: Future<Unit> = spawn {
                        let result: ?CompressorResponse = comprossor.compressVideo(globalAbilityContext.getOrThrow(),
                            inputFilePath, CompressQuality.COMPRESS_QUALITY_LOW)
                        launch {
                            res = result
                            if (let Some(i) <- result) {
                                compressresult = i.message
                            } else {
                                compressresult = "压缩失败"
                            }
                        }
                    }
                }.fontSize(40).height(80)
                Text(compressresult)
            }.width(100.percent)
        }.height(100.percent)
    }
}
