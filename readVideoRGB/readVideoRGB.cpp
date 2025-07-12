#include "readVideoRGB.h"
// readVideoRGB.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

VideoProcessing::VideoReader::VideoReader(const char* video_path) : video_path(video_path), format_ctx(nullptr), codec_ctx(nullptr), codec(nullptr),
codec_params(nullptr), frame(nullptr), frame_rgb(nullptr), sws_ctx(nullptr),
video_stream_index(-1), buffer(nullptr){
	initialize();
}

VideoProcessing::VideoReader::~VideoReader()
{
	cleanup();
}

std::vector<VideoProcessing::FrameRGB> VideoProcessing::VideoReader::getRGBValues(int x, int y)
{
    AVPacket packet;
    std::vector<FrameRGB> rgb_values;
    int frame_number = 0;

    while (av_read_frame(format_ctx, &packet) >= 0) {
        if (packet.stream_index == video_stream_index) {
            if (avcodec_send_packet(codec_ctx, &packet) == 0) {
                while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                    sws_scale(sws_ctx, frame->data, frame->linesize, 0, codec_ctx->height, frame_rgb->data, frame_rgb->linesize);

                    int rgb_index = y * frame_rgb->linesize[0] + x * 3;
                    RGB rgb = { frame_rgb->data[0][rgb_index], frame_rgb->data[0][rgb_index + 1], frame_rgb->data[0][rgb_index + 2] };
                    rgb_values.push_back({ frame_number, rgb });
                    frame_number++;
                }
            }
        }
        av_packet_unref(&packet);
    }

    return rgb_values;
}

void VideoProcessing::VideoReader::initialize()
{
    format_ctx = avformat_alloc_context();
    if (avformat_open_input(&format_ctx, video_path, nullptr, nullptr) != 0) {
        std::cerr << "Could not open video file." << std::endl;
        return;
    }

    if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
        std::cerr << "Could not find stream information." << std::endl;
        return;
    }

    for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            codec = avcodec_find_decoder(format_ctx->streams[i]->codecpar->codec_id);
            if (codec) {
                codec_params = format_ctx->streams[i]->codecpar;
                video_stream_index = i;
                break;
            }
        }
    }

    if (video_stream_index == -1) {
        std::cerr << "Could not find a video stream." << std::endl;
        return;
    }

    codec_ctx = avcodec_alloc_context3(codec);
    if (avcodec_parameters_to_context(codec_ctx, codec_params) < 0) {
        std::cerr << "Could not create codec context." << std::endl;
        return;
    }

    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        std::cerr << "Could not open codec." << std::endl;
        return;
    }

    frame = av_frame_alloc();
    frame_rgb = av_frame_alloc();
    if (!frame || !frame_rgb) {
        std::cerr << "Could not allocate frame." << std::endl;
        return;
    }

    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height, 1);
    buffer = (uint8_t*)av_malloc(num_bytes * sizeof(uint8_t));
    av_image_fill_arrays(frame_rgb->data, frame_rgb->linesize, buffer, AV_PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height, 1);

    sws_ctx = sws_getContext(
        codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
        codec_ctx->width, codec_ctx->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr
    );
}

void VideoProcessing::VideoReader::cleanup()
{
    if (sws_ctx) sws_freeContext(sws_ctx);
    if (buffer) av_free(buffer);
    if (frame_rgb) av_frame_free(&frame_rgb);
    if (frame) av_frame_free(&frame);
    if (codec_ctx) avcodec_free_context(&codec_ctx);
    if (format_ctx) avformat_close_input(&format_ctx);
}
