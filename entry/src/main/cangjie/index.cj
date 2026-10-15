/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

package ohos_app_cangjie_entry

import ohos.state_macro_manage.*
import ohos.concurrency.*
import ohos.file_fs.*
import ohos.resource_manager.*

@Entry
@Component
class MyView {
    let scroller = Scroller()

    @State
    var copyStatus: String = ""

    // 将包内 media 资源 test.mp4 拷贝到应用可写目录 filesDir 下
    // 说明：/data/storage/el1/bundle/ 是应用安装目录，对应用只读无法写入，
    // 因此改为拷贝到 filesDir，压缩页面的输入路径已同步修改
    func copyTestMp4ToBundle(): Unit {
        let context = globalAbilityContext.getOrThrow()
        // 读取 media 资源内容，density 传 0 表示使用系统默认像素密度
        let bytes: Array<UInt8> = context.resourceManager.getRawFileContent("test.mp4")
        // 以 读写|创建|截断 模式打开目标文件：不存在则创建，已存在则清空重写
        let mode = OpenMode.READ_WRITE.mode | OpenMode.CREATE.mode | OpenMode.TRUNC.mode
        let file = FileFs.open(context.filesDir + "/test.mp4", mode: mode)
        try {
            // Cangjie 绑定中 File 没有实例 write/close，需通过 FileFs 静态方法 + fd 操作
            FileFs.write(file.fd, bytes)
        } finally {
            FileFs.close(file)
        }
    }

    // 执行拷贝并返回结果描述（供子线程取用，避免 lambda 捕获 var 变量）
    func doCopyTestMp4(): String {
        try {
            copyTestMp4ToBundle()
            return "拷贝成功: ${globalAbilityContext.getOrThrow().filesDir}/test.mp4"
        } catch (e: Exception) {
            return "拷贝失败: ${e.message}"
        }
    }

    func build() {
        Row {
            Scroll(this.scroller) {
                Column {
                    Button("CopyTestMp4").onClick {
                        copyStatus = "拷贝中..."
                        let fut: Future<Unit> = spawn {
                            // msg 用 let 声明，lambda 只能捕获不可变局部变量
                            let msg: String = doCopyTestMp4()
                            launch {
                                copyStatus = msg
                            }
                        }
                    }.fontSize(30).height(70)
                    Text(copyStatus)
                    Button("Compressmp4High").onClick {Router.push(url: "Compressmp4High")}
                    Button("Compressmp4Medium").onClick {Router.push(url: "Compressmp4Medium")}
                    Button("Compressmp4Low").onClick {Router.push(url: "Compressmp4Low")}
                    Button("CompresstxtHigh").onClick {Router.push(url: "CompresstxtHigh")}
                    Button("CompresstxtMedium").onClick {Router.push(url: "CompresstxtMedium")}
                    Button("CompresstxtLow").onClick {Router.push(url: "CompresstxtLow")}
                    Button("Compressing").onClick {Router.push(url: "Compressing")}
                    Button("Compress3type").onClick {Router.push(url: "Compress3type")}
                }.width(100.percent)
            }
        }.height(100.percent)
    }
}
