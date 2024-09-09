/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights resvered.
 */

package ohos_app_cangjie_entry

import ohos.state_macro_manage.*
import videocompressor.*
import videocompressor.*
import ohos.concurrency.*

@Entry
@Component
class Compress3type {
    @State
    var message: String = "开始压缩"

    @State
    var compressresult1: String = ""
    @State
    var compressresult2: String = ""
    @State
    var compressresult3: String = ""

    @State
    var res1: ?CompressorResponse = Option<CompressorResponse>.None
    @State
    var res2: ?CompressorResponse = Option<CompressorResponse>.None
    @State
    var res3: ?CompressorResponse = Option<CompressorResponse>.None

    func build() {
        Row {
            Column {
                Button(message).onClick {
                    let comprossor: VideoCompressor = VideoCompressor()
                    let inputFilePath: String = "/data/storage/el1/bundle/test.mp4"
                    let fut1: Future<Unit> = spawn {
                        let result1: ?CompressorResponse = comprossor.compressVideo(globalAbilityContext.getOrThrow(),
                            inputFilePath, CompressQuality.COMPRESS_QUALITY_HIGH)
                        launch {
                            res1 = result1
                            compressresult1 = res1.getOrThrow().message
                        }
                    }
                    let fut2: Future<Unit> = spawn {
                        let result2: ?CompressorResponse = comprossor.compressVideo(globalAbilityContext.getOrThrow(),
                            inputFilePath, CompressQuality.COMPRESS_QUALITY_MEDIUM)
                        launch {
                            res2 = result2
                            compressresult2 = res2.getOrThrow().message
                        }
                    }
                    let fut3: Future<Unit> = spawn {
                        let result3: ?CompressorResponse = comprossor.compressVideo(globalAbilityContext.getOrThrow(),
                            inputFilePath, CompressQuality.COMPRESS_QUALITY_LOW)
                        launch {
                            res3 = result3
                            compressresult3 = res3.getOrThrow().message
                        }
                    }
                    compressresult1 = "压缩中"
                    compressresult2 = "压缩中"
                    compressresult3 = "压缩中"
                }.fontSize(40).height(80)
                Text(compressresult1)
                Text(compressresult2)
                Text(compressresult3)
            }.width(100.percent)
        }.height(100.percent)
    }
}
