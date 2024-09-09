
## videocompress4cj

### 介绍
videoCompressor是一款高性能的视频压缩三方库

- 支持视频压缩。
- 支持高,中,低三种质量压缩。

### 1 videocompress4cj 视频压缩库

#### 1.1 VideoCompressor 视频压缩对象类

```cangjie
public class VideoCompressor {
    
    /**
     * 压缩视频
     * 
     * @参数 context - AbilityContext数据类型，上下文
     * @参数 inputFilePath - String数据类型，被压缩的视频地址
     * @参数 quality - CompressQuality数据类型，压缩质量高,中,低三种
     * @返回值 ?CompressorResponse 压缩结果对象
     */
    public func compressVideo(context: AbilityContext, inputFilePath: String, quality: CompressQuality):?CompressorResponse
}
```

