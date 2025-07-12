#ifndef READ_VIDEO_RGB_H
#define READ_VIDEO_RGB_H


#include <iostream>
#include <vector>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

namespace VideoProcessing {
    // 定义一个结构体来存储配置信息
    struct Config {
        std::string videoName;
        int x;
        int y;
    };

    struct RGB {
        uint8_t r, g, b;    //uint8_t 是一种8位无符号整数类型，能够表示从0到255的整数非常高效
    };

    struct FrameRGB {
        int frame_number;
        RGB rgb;
    };

    

    class VideoReader {
    public:
        VideoReader(const char* video_path);
        ~VideoReader();
        std::vector<FrameRGB> getRGBValues(int x, int y);

    private:
        void initialize();
        void cleanup();

        const char* video_path;
        AVFormatContext* format_ctx;        //用于存储视频文件的格式信息和上下文,用于打开视频文件，读取文件头，获取视频流信息等。
        AVCodecContext* codec_ctx;          //存储编解码器相关的上下文信息
        const AVCodec* codec;               //存储编解码器的信息。
        AVCodecParameters* codec_params;    //解码器参数
        AVFrame* frame;                     //帧数据
        AVFrame* frame_rgb;                 //用于存储RGB格式的帧数据
        SwsContext* sws_ctx;
        int video_stream_index;             //视频流在文件中的索引
        uint8_t* buffer;                    //存储转换后的RGB888格式的帧数据
    };

}
#endif // READ_VIDEO_RGB_H








