# Overview

A sample application written in C#/C++ that generates bitmaps for h264 encoding (using mp4 as container) using [ffmpeg](https://www.ffmpeg.org/). ffmpeg's [`dev`](./ffmpeg-latest-win64-dev/) and `shared` packages are included. These packages were downloaded from [this location](https://ffmpeg.zeranoe.com/builds/win64/).

You need to copy ffmpeg's [bin](./ffmpeg-latest-win64-shared/bin/) files to the output directory of the C# project. This project is tested using [Debug|x64](./H264MP4Encode/bin/x64/Debug/) so the bin files are already included there.

Note that the output path of the .mp4 file is hardcoded to `c:\users\idril\downloads\encoded.mp4` so you might want to change it as well.

# License

[The MIT License](./LICENSE.md)
