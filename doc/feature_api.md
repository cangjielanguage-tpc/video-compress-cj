
## video-compress-cj

### 介绍
video-compress-cj是一款高性能的视频压缩三方库

- 支持视频压缩。
- 支持高,中,低三种质量压缩。

### 1 video-compress-cj 视频压缩库

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

#### 1.2 CompressQuality 视频压缩质量枚举

```cangjie
public enum CompressQuality {
    
    | COMPRESS_QUALITY_HIGH         //高质量压缩
    | COMPRESS_QUALITY_MEDIUM       //中质量压缩
    | COMPRESS_QUALITY_LOW          //低质量压缩
    
}
```
