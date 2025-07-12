#include "readVideoRGB.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <tuple>

#define MAX_PATH 100
// 获取当前可执行文件所在目录的路径
std::string getExecutablePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}

// 读取配置文件并返回配置信息
 VideoProcessing::Config readConfig(const std::string& configFilePath) {
    VideoProcessing::Config config;
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        std::cerr << "无法打开配置文件: " << configFilePath << std::endl;
        return config;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        std::istringstream iss(line);
        std::string key, value;
        // 以 '=' 为分隔符分割键和值
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            // 去除键和值前后的空格
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (key == "videoName") {
                config.videoName = value;
            }
            else if (key == "X") {
                config.x = std::stoi(value);
            }
            else if (key == "Y") {
                config.y = std::stoi(value);
            }
        }
    }
    configFile.close();
    return config;
}

 std::tuple<int64_t, double, int, int>  getDurationFrameRate(const char * videoPath) {

     // 获取视频总时长（秒）和每秒帧率
     AVFormatContext* format_ctx = avformat_alloc_context();
     if (avformat_open_input(&format_ctx, videoPath, nullptr, nullptr) != 0) {
         std::cerr << "Could not open video file." << std::endl;
         return std::make_tuple(-1, 0.0, 0, 0);
     }

     if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
         std::cerr << "Could not find stream information." << std::endl;
         return std::make_tuple(-1, 0.0, 0, 0);
     }

     AVStream* video_stream = nullptr;
     for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
         if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
             video_stream = format_ctx->streams[i];
             break;
         }
     }

     if (!video_stream) {
         std::cerr << "Could not find a video stream." << std::endl;
         return std::make_tuple(-1, 0.0, 0, 0);
     }

     int64_t duration = format_ctx->duration / AV_TIME_BASE; // 视频总时长（秒）
     double frame_rate = av_q2d(video_stream->avg_frame_rate); // 每秒帧率
     int width = video_stream->codecpar->width;
     int height = video_stream->codecpar->height;

     avformat_close_input(&format_ctx);

     return std::make_tuple(duration, frame_rate, width, height);
 }


int main()
{
   /* std::cout << "Hello World!\n";
    printf("%s\n", avcodec_configuration());*/

    // 获取当前可执行文件目录
    std::string exe_path = getExecutablePath();

    std::string configFilePath = exe_path+"\\readvideoRGB.config";

    VideoProcessing::Config config = readConfig(configFilePath);
    std::string video_path = exe_path + "\\"+ config.videoName;

    std::string resultTxt = exe_path + "\\result.txt";

    std::tuple<int64_t, double, int, int>  val = getDurationFrameRate(video_path.c_str());

    VideoProcessing::VideoReader video_reader(video_path.c_str());
    std::vector<VideoProcessing::FrameRGB> rgb_values = video_reader.getRGBValues(config.x, config.y);

    // 清空result.txt内容
    std::ofstream result_file(resultTxt.c_str(), std::ios::out | std::ios::trunc);
    if (!result_file) {
        std::cerr << "Failed to open result.txt for writing." << std::endl;
        return -1;
    }

    // 将视频总时长和每秒帧率写入到result.txt文件中
    result_file << "视频总时长: " << std::get<0>(val) << " 秒" << std::endl;
    result_file << "每秒帧率: " << std::get<1>(val) << " 帧/秒" << std::endl;
    result_file << "宽: " << std::get<2>(val) << " 像素" << std::endl;
    result_file << "高: " << std::get<3>(val) << " 像素" << std::endl;

    // 将每帧的RGB值信息写入到result.txt文件中
    for (const auto& frame_rgb : rgb_values) {
        result_file << "Frame " << frame_rgb.frame_number << ": RGB=("
            << static_cast<int>(frame_rgb.rgb.r) << ", "
            << static_cast<int>(frame_rgb.rgb.g) << ", "
            << static_cast<int>(frame_rgb.rgb.b) << ")" << std::endl;
    }

    result_file.close();

    std::cout << "视频RGB读取成功!" << std::endl;

    return 0;
}


