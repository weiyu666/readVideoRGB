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
    // ����һ���ṹ�����洢������Ϣ
    struct Config {
        std::string videoName;
        int x;
        int y;
    };

    struct RGB {
        uint8_t r, g, b;    //uint8_t ��һ��8λ�޷����������ͣ��ܹ���ʾ��0��255�������ǳ���Ч
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
        AVFormatContext* format_ctx;        //���ڴ洢��Ƶ�ļ��ĸ�ʽ��Ϣ��������,���ڴ���Ƶ�ļ�����ȡ�ļ�ͷ����ȡ��Ƶ����Ϣ�ȡ�
        AVCodecContext* codec_ctx;          //�洢���������ص���������Ϣ
        const AVCodec* codec;               //�洢�����������Ϣ��
        AVCodecParameters* codec_params;    //����������
        AVFrame* frame;                     //֡����
        AVFrame* frame_rgb;                 //���ڴ洢RGB��ʽ��֡����
        SwsContext* sws_ctx;
        int video_stream_index;             //��Ƶ�����ļ��е�����
        uint8_t* buffer;                    //�洢ת�����RGB888��ʽ��֡����
    };

}
#endif // READ_VIDEO_RGB_H








