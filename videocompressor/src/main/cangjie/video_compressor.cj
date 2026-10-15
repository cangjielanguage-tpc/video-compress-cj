/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

package videocompressor

@FastNative
foreign func CompressVideo(outFd: Int32, inputFd: Int32, inputOffset: Int32, inputSize: Int64, quality: Int32,
    outputPath: CString): CPointer<AsyncCallbackInfoCj>

public class VideoCompressor {


    public func compressVideo(filedirOut: String, inputFilePath: String, quality: CompressQuality): ?CompressorResponse {
        var outPutFilePath: String = ""
        var filedir = filedirOut
        let sjc = DateTime.now().toUnixTimeStamp().toNanoseconds()
        outPutFilePath = filedir + "/" + sjc.toString() + ".mp4"
        AppLog.info("quality:" + quality.toString() + "--videoCompressor outPutFilePath:" + outPutFilePath);
        var outputFile: Option<File> = Option<File>.None
        try {
            outputFile = FileFs.open(outPutFilePath, mode: (READ_WRITE.mode | CREATE.mode))
        } catch (e: Exception) {
            //创建输出文件失败
            FileFs.unlink(outPutFilePath)
            AppLog.error("videoCompressor outputFile create error ${e.toString()}")
            return CompressorResponse(CompressorResponseCode.OTHER_ERROR,
                "videoCompressor outputFile create error ${e.toString()}", outPutFilePath)
        }
        var inputFile: Option<File> = Option<File>.None
        try {
            inputFile = FileFs.open(inputFilePath, mode: (READ_ONLY.mode))
        } catch (e: Exception) {
            //打开输入文件失败
            FileFs.unlink(outPutFilePath)
            AppLog.error("open inputFile  failed ${e.toString()}")
            return CompressorResponse(CompressorResponseCode.OTHER_ERROR, "open inputFile  failed ${e.toString()}",
                outPutFilePath)
        }
        AppLog.info("videoCompressor inputFile fd: ${inputFile.getOrThrow().fd}")
        var inputFileStat: Option<Stat> = Option<Stat>.None
        try {
            inputFileStat = FileFs.stat(inputFile.getOrThrow().fd)
        } catch (e: Exception) {
            FileFs.unlink(outPutFilePath)
            AppLog.error("videoCompressor inputFileStat is error ${e.toString()}")
            return CompressorResponse(CompressorResponseCode.OTHER_ERROR,
                "videoCompressor inputFileStat is error ${e.toString()}", outPutFilePath)
        }
        if (inputFileStat.getOrThrow().size <= 0) {
            FileFs.unlink(outPutFilePath)
            AppLog.info("videoCompressor inputFile size is 0")
            return CompressorResponse(CompressorResponseCode.OTHER_ERROR, "videoCompressor inputFile size is 0",
                outPutFilePath)
        }
        var outFd: Int32 = Int32(outputFile.getOrThrow().fd)
        var inputFd: Int32 = Int32(inputFile.getOrThrow().fd)
        var inputOffset: Int32 = 0
        var inputSize: Int64 = inputFileStat.getOrThrow().size
        var qualityres: Int32 = Int32(CompressQuality.GetTypeNum(quality))
        var response: ?CompressorResponse = None
        unsafe {
            var outputPath = LibC.mallocCString(outPutFilePath)
            var result: CPointer<AsyncCallbackInfoCj> = CompressVideo(outFd, inputFd, inputOffset, inputSize, qualityres,
                outputPath)
            var res: AsyncCallbackInfoCj = result.read()
            var cstrRes: CString = res.resultStr
            var resCode: Int32 = res.resultCode
            var strRes: String = cstrRes.toString()
            var outputPathRes: String = outputPath.toString()
            LibC.free(outputPath)
            LibC.free(cstrRes)
            AppLog.info("resCode=${resCode}   strRes=${strRes}  outputPathRes=${outputPathRes}")
            if (resCode == 0) {
                response = CompressorResponse(CompressorResponseCode.SUCCESS, strRes, outputPathRes)
            } else {
                response = CompressorResponse(CompressorResponseCode.OTHER_ERROR, strRes, outputPathRes)
            }
        }
        return response
    }
}
