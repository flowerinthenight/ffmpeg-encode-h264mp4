// H264Encoder.h

#pragma once

#include "nsffmpeg.h"
#include <vcclr.h>

using namespace System;

namespace NSH264Encoder {
    // Unmanaged class that interfaces with ffmpeg.
    class FfmpegH264Encoder {
    public:
        FfmpegH264Encoder();
        ~FfmpegH264Encoder();

        void SetupVideo(char *fname, int w, int h, int fps, int gop, int bps);
        void WriteFrame(char *frame);
        void CloseVideo(void);

    private:
        int m_sws_flags;
        int m_MP4MOV_FPS;
        int m_MP4MOV_GOP;
        int m_MP4MOV_BPS;
        int m_frame_count;
        int m_MP4MOV_WIDTH;
        int m_MP4MOV_HEIGHT;
        bool m_setup;
        double m_video_time;
        char *m_filename;

        void CloseCodec(void);
        void WriteFrame(void);
        void SetupCodec(const char *fname, int codecId);

        libffmpeg::AVFrame *m_frame;
        libffmpeg::AVCodecContext *m_c;
        libffmpeg::AVStream *m_video_st;
        libffmpeg::AVOutputFormat *m_fmt;
        libffmpeg::AVFormatContext *m_oc;
        libffmpeg::AVCodec *m_video_codec;
        libffmpeg::AVPicture m_src_picture, m_dst_picture;
    };

    // Managed class for our CS code.
    public ref class H264Encoder : public IDisposable
    {
    protected:
        /// <summary>
        /// Object's finalizer.
        /// </summary>
        !H264Encoder();

    public:
        H264Encoder();

        /// <summary>
        /// Disposes the object and frees its resources.
        /// </summary>
        ~H264Encoder()
        {
            this->!H264Encoder();
            disposed = true;
        }

        void SetupEncode(String ^fileName, int w, int h, int fps);
        void WriteFrame(array<System::Byte>^ frameRGB24);
        void CloseEncode();

    private:
        bool disposed;
        FfmpegH264Encoder *enc;
    };
}
