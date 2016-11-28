# Overview

A sample application written in C#/C++ that generates bitmaps for h264 encoding (using mp4 as container) using [ffmpeg](https://www.ffmpeg.org/). ffmpeg's `dev` and `shared` packages are included. These packages were downloaded from [this location](https://ffmpeg.zeranoe.com/builds/win64/).

You need to copy ffmpeg's [bin](./ffmpeg-latest-win64-shared/bin/) files to the output directory of the C# project. This repo is tested using [Debug|x64](./H264MP4Encode/bin/x64/Debug/) so the bin files are also included there.

# License

[The MIT License](./LICENSE.md)
