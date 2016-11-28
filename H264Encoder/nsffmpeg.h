#pragma once

namespace libffmpeg
{
    extern "C"
    {
        // disable warnings about badly formed documentation from ffmpeg, which don't need at all
#pragma warning(disable:4635)
        // disable warning about conversion int64 to int32
#pragma warning(disable:4244)

#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"

#ifdef PixelFormat
#undef PixelFormat
#endif
    }
}