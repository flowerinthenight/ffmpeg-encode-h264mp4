// This is the main DLL file.

#include "stdafx.h"
#include <Windows.h>
#include <strsafe.h>
#include "H264Encoder.h"
#include <stdio.h>
using namespace std;
#include "jytrace.h"

#define MD L"FFMPEG_TEST"
#define FL __FILEW__
#define FN __FUNCTIONW__

namespace NSH264Encoder {
    // Unmanaged class that interfaces with ffmpeg.
    FfmpegH264Encoder::FfmpegH264Encoder()
    {
        m_filename = new char[1024];
        m_setup = false;
    }

    FfmpegH264Encoder::~FfmpegH264Encoder()
    {
        delete[] m_filename;
    }

    void FfmpegH264Encoder::SetupVideo(char *fname, int w, int h, int fps, int gob, int bps)
    {
        /* copy filename to local string */
        sprintf_s(m_filename, 1024, fname);

        /* set movie parameters */
        m_MP4MOV_WIDTH = w;
        m_MP4MOV_HEIGHT = h;
        m_MP4MOV_FPS = fps;
        m_MP4MOV_GOB = gob;
        m_MP4MOV_BPS = bps;

        /* pass parameters to setup codec as H264 file */
        SetupCodec(m_filename, libffmpeg::AV_CODEC_ID_H264);
    }

    void FfmpegH264Encoder::WriteFrame(char *frame)
    {
        /* frame should be RGB24 */
        
        for (int y = 0; y < m_c->height; y++) {
            for (int x = 0; x < m_c->width; x++) {
                /* save RGB frame to ffmpeg's source frame */
                m_src_picture.data[0][y * m_src_picture.linesize[0] + x * 3 + 0] = frame[(y * m_c->width + x) * 3 + 0]; /* red */
                m_src_picture.data[0][y * m_src_picture.linesize[0] + x * 3 + 1] = frame[(y * m_c->width + x) * 3 + 1]; /* green */
                m_src_picture.data[0][y * m_src_picture.linesize[0] + x * 3 + 2] = frame[(y * m_c->width + x) * 3 + 2]; /* blue */
            }
        }

        /* send frame off to ffmpeg for encoding */
        WriteFrame();
    }

    void FfmpegH264Encoder::CloseVideo(void)
    {
        CloseCodec();
    }

    void FfmpegH264Encoder::SetupCodec(const char *fname, int codecId)
    {
        if (m_setup) { return; }

        int ret;
        m_sws_flags = SWS_BICUBIC;
        m_frame_count = 0;

        libffmpeg::avcodec_register_all();
        libffmpeg::av_register_all();

        avformat_alloc_output_context2(&m_oc, NULL, NULL, fname);
        if (!m_oc) {
            /* if context failed check by specifying container */
            avformat_alloc_output_context2(&m_oc, NULL, "mp4", fname);
            EventWriteSimple(MD, FL, FN, L"Trace", L"Specify mp4.");
        }

        if (!m_oc) {
            /* if context failed check by specifying container a bit further */
            avformat_alloc_output_context2(&m_oc, NULL, "mp4", "c:\\temp.mp4");
            EventWriteSimple(MD, FL, FN, L"Trace", L"Specify mp4 and filename.");
        }

        if (!m_oc) {
            /* if you reach this point something's wrong */
            return;
        }

        /* get the format determined by the container */
        m_fmt = m_oc->oformat;

        /* add the audio and video streams using the default format codecs and initialize the codecs */
        m_video_st = NULL;
        m_fmt->video_codec = libffmpeg::AV_CODEC_ID_H264;
        m_fmt->audio_codec = libffmpeg::AV_CODEC_ID_NONE;

        /* add an output stream */
        {
            libffmpeg::AVStream *st;

            /* find the encoder */
            m_video_codec = avcodec_find_encoder(m_fmt->video_codec);
            if (!(m_video_codec)) {
                return;
            }

            /* create new video stream */
            st = avformat_new_stream(m_oc, m_video_codec);
            if (!st) {
                return;
            }

            st->id = m_oc->nb_streams - 1;

            /* set codec context */
            m_c = st->codec;

            /* setup fundumental video stream parameters */
            m_c->codec_id = m_fmt->video_codec;
            m_c->bit_rate = m_MP4MOV_BPS;
            /* resolution must be a multiple of 2! */
            m_c->width = m_MP4MOV_WIDTH;
            m_c->height = m_MP4MOV_HEIGHT;
            m_c->time_base.den = m_MP4MOV_FPS;
            m_c->time_base.num = 1;
            m_c->gop_size = m_MP4MOV_GOB;
            /* do not change this, H264 needs YUV format not RGB */
            m_c->pix_fmt = libffmpeg::AV_PIX_FMT_YUV420P;
             
            /* some formats want stream headers to be separate */
            if (m_oc->oformat->flags & AVFMT_GLOBALHEADER)
                m_c->flags |= CODEC_FLAG_GLOBAL_HEADER;

            /* set our video stream pointer */
            m_video_st = st;
        }

        /*
        Now that all the parameters are set, we can open the audio and
        video codecs and allocate the necessary encode buffers.
        */
        {
            /* allocated codec context */
            libffmpeg::AVCodecContext *c = m_video_st->codec;

            /* open the codec */
            ret = avcodec_open2(c, m_video_codec, NULL);
            if (ret < 0) {
                return;
            }

            /* allocate and init a re-usable frame */
            m_frame = libffmpeg::av_frame_alloc();
            if (!m_frame) {
                return;
            }

            /* allocate the encoded raw picture */
            ret = avpicture_alloc(&m_dst_picture, c->pix_fmt, c->width, c->height);
            if (ret < 0) {
                return;
            }

            /* allocate RGB frame that we can pass to the YUV frame */
            ret = avpicture_alloc(&m_src_picture, libffmpeg::AV_PIX_FMT_RGB24, c->width, c->height);
            if (ret < 0) {
                return;
            }

            /* copy data and linesize picture pointers to frame */
            *((libffmpeg::AVPicture *)m_frame) = m_dst_picture;
        }

        /* tell ffmpeg that we are going to write encoded frames to a file */
        av_dump_format(m_oc, 0, fname, 1);

        /* open the output file, if needed */
        if (!(m_fmt->flags & AVFMT_NOFILE)) {
            ret = avio_open(&m_oc->pb, fname, AVIO_FLAG_WRITE);
            if (ret < 0) {
                return;
            }
        }

        /* write the stream header, if any */
        ret = avformat_write_header(m_oc, NULL);

        if (ret < 0) {
            return;
        }

        /* set frame count to zero */
        if (m_frame)
            m_frame->pts = 0;

        m_setup = true;
    }

    void FfmpegH264Encoder::WriteFrame(void)
    {
        /* if video is not initalised then don't write frame */
        if (!m_setup) { return; }

        /* calculate video time */
        m_video_time = m_video_st ? m_video_st->pts.val * av_q2d(m_video_st->time_base) : 0.0;

        int ret;
        static struct libffmpeg::SwsContext *sws_ctx;
        libffmpeg::AVCodecContext *c = m_video_st->codec;

        /* if we haven't already done so init the context of the frame conversion from RGB to YUV */
        if (!sws_ctx) {
            sws_ctx = libffmpeg::sws_getContext(
                c->width,
                c->height,
                libffmpeg::AV_PIX_FMT_RGB24,
                c->width,
                c->height,
                libffmpeg::AV_PIX_FMT_YUV420P,
                m_sws_flags,
                NULL, NULL, NULL);

            if (!sws_ctx) {
                return;
            }
        }

        /* convert RGB frame (m_src_picture) to and YUV frame (m_dst_picture) */
        libffmpeg::sws_scale(
            sws_ctx,
            m_src_picture.data,
            m_src_picture.linesize,
            0,
            c->height,
            m_dst_picture.data,
            m_dst_picture.linesize);


        /* some inits for encoding the frame */
        libffmpeg::AVPacket pkt = { 0 };
        int got_packet;
        libffmpeg::av_init_packet(&pkt);

        /* encode the frame */
        ret = avcodec_encode_video2(c, &pkt, m_frame, &got_packet);
        if (ret < 0) {
            return;
        }

        /* if size of encoded frame is zero, it means the image was buffered */
        if (!ret && got_packet && pkt.size) {
            pkt.stream_index = m_video_st->index;

            /* write the compressed frame to the media file */
            ret = av_interleaved_write_frame(m_oc, &pkt);

            /* if non-zero then it means that there was something wrong writing the frame to the file */
            if (ret != 0) {
                return;
            }
        } else {
            ret = 0;
        }

        /* increment frame counter */
        m_frame_count++;
        m_frame->pts += av_rescale_q(1, m_video_st->codec->time_base, m_video_st->time_base);
    }

    void FfmpegH264Encoder::CloseCodec(void)
    {
        /* if video is not initalised then don't close frame */
        if (!m_setup) { return; }

        /* write trailing bits */
        av_write_trailer(m_oc);

        /* close video codec */
        avcodec_close(m_video_st->codec);

        /* free our frames */
        libffmpeg::av_free(m_src_picture.data[0]);
        libffmpeg::av_free(m_dst_picture.data[0]);
        av_free(m_frame);

        if (!(m_fmt->flags & AVFMT_NOFILE)) {
            avio_close(m_oc->pb);
        }

        avformat_free_context(m_oc);

        /* set open flag clear */
        m_setup = false;
    }

    // Managed class implementation for our CS code.
    H264Encoder::H264Encoder() :
        disposed(false), enc(NULL)
    {
        EventRegisterJyTrace();
    }

    H264Encoder::!H264Encoder()
    {
        EventUnregisterJyTrace();
    }

    void H264Encoder::SetupEncode(String ^fileName, int w, int h, int fps)
    {
        pin_ptr<const wchar_t> fn = PtrToStringChars(fileName);
        size_t n;
        char mbfn[1024];
        wcstombs_s(&n, mbfn, fn, 1024);

        if (!enc) {
            enc = new FfmpegH264Encoder();
            enc->SetupVideo(mbfn, w, h, fps, 10, 40000000);
            EventWriteSimple(MD, FL, FN, L"Trace", L"Setup test.mp4");
        }
    }

    void H264Encoder::WriteFrame(array<System::Byte>^ frameRGB24)
    {
        pin_ptr<System::Byte> p = &frameRGB24[0];
        unsigned char* pby = p;
        char* pch = reinterpret_cast<char*>(pby);
        enc->WriteFrame(pch);
        EventWriteSimple(MD, FL, FN, L"Trace", L"Write RGB24 frame.");
    }

    void H264Encoder::CloseEncode()
    {
        if (enc) {
            enc->CloseVideo();
            EventWriteSimple(MD, FL, FN, L"Trace", L"Close video.");
        }
    }
}