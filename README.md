<div align="center">
<h1>videocompress4cj</h1>
</div>

<p align="center">
<img alt="" src="https://img.shields.io/badge/release-v0.0.1-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/build-pass-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/cjc-v0.53.10-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/project-open-brightgreen" style="display: inline-block;" />
</p>

## 介绍

videocompress4cj是一款高性能的视频压缩三方库

### 特性

- 🚀 支持视频压缩。

- 🚀 支持高,中,低三种质量压缩。


### 源码目录

```shell
├── README.md                                 #整体介绍
├── doc                                       #文档目录
│   └── feature_api.md                        #API接口文档
├── videocompressor                           #源码目录 
├── videocompress_ffi                                 #C++源码目录
├── entry                                     #示例目录
└── hvigor                                    #构建工具目录
```

### 支持的视频规格
支持的解封装格式：

| 媒体格式 | 封装格式     |
| -------- | ------------ |
| 视频     | mp4、mpeg.ts |

支持的视频解码格式：

| 视频解码类型                  |
|-------------------------|
| AVC(H.264)、 HEVC(H.265) |

支持的音频解码格式：

| 音频解码类型   |
| -------------- |
| AAC |

支持的视频编码格式：

| 视频编码类型            |
| ----------------------- |
| AVC(H.264)、 HEVC(H.265) |

支持的音频编码格式：

| 音频编码类型 |
| ------------ |
| AAC          |

支持的封装格式：

| 媒体格式 | 封装格式     |
| -------- | ------------ |
| 视频     | mp4|

### 接口说明

主要类和函数接口说明详见 [API](./doc/feature_api.md)

## 使用说明

### 编译构建

描述具体的编译过程：

```shell

    1.先通过命令把代码下载下来,
      git clone -b 具体分支名 --single-branch --recursive  https://gitcode.com/Cangjie-TPC/videocompressor4cj.git
          
    2.选择videocompressor模块,点击Build菜单,然后点击 Make Module 'videocompressor' 等待编译完成,项目则编译成功
    
```

### 功能示例

#### 1. 把videocompress作为三方库依赖引入
当前仓颉语言三方库还不支持中心仓库，可以把依赖库克隆到本地引入，具体步骤如下：

a. 目标工程把依赖库作为源码引入

```shell
> cd $工程根目录
>mkdir third-party
>cd  third-party
>git clone -b 具体分支名 --single-branch --recursive  https://gitcode.com/Cangjie-TPC/videocompressor4cj.git
然后按照上面流程把videocompress4cj库正常编译通过即可
```

b. 打开 .\entry\src\main\cangjie\cjpm.toml，添加依赖

```toml
[dependencies]
    videocompressor = {path = "../../../../videocompressor/src/main/cangjie", version = "1.0.0"}
```

#### 2. 使用videocompress压缩视频

示例代码如下：

```cangjie

package ohos_app_cangjie_entry

import ohos.state_macro_manage.*
import videocompressor.*
import ohos.concurrency.*

@Entry
@Component
class CompresstxtHigh {
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
                    let inputFilePath: String = "/data/storage/el1/bundle/test.txt"
                    let fut: Future<Unit> = spawn {
                        let result: ?CompressorResponse = comprossor.compressVideo(globalAbilityContext.getOrThrow(),
                            inputFilePath, CompressQuality.COMPRESS_QUALITY_HIGH)
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

```

执行结果如下：

```shell
视频压缩成功，并在对应的沙盒路径 /data/app/el2/100/base/(packagename)/haps/entry/files/ 中生成了一个压缩后的视频,生成视频大小比原视频小,且能正常播放
```


## 约束与限制

IDE : DevEco Studio NEXT Beta1(5.0.3.800)

Plugins (SDK) : DevEco Studio NEXT Beta1-Cangjie Plugin(5.0.3.800)


## 开源协议

本项目基于 [Apache License](./LICENSE) ，请自由的享受和参与开源。

## 参与贡献

欢迎给我们提交PR，欢迎给我们提交Issue，欢迎参与任何形式的贡献。