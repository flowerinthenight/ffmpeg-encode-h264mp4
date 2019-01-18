[![Build status](https://ci.appveyor.com/api/projects/status/t6wyaohtj9p5jrsg/branch/master?svg=true)](https://ci.appveyor.com/project/flowerinthenight/ffmpeg-encode-h264mp4/branch/master)

# Overview

A sample x64 application written in C/C++/C# that generates bitmaps for h264 encoding (using mp4 as container) using [ffmpeg](https://www.ffmpeg.org/). ffmpeg's [`dev`](./ffmpeg-latest-win64-dev/) and [`shared`](./ffmpeg-latest-win64-shared/) packages are included. These packages were downloaded from [this location](https://ffmpeg.zeranoe.com/builds/win64/).

You need to copy ffmpeg's [bin](./ffmpeg-latest-win64-shared/bin/) files (`*.dll`) to the output directory of the C# project. This project is tested in [Debug|x64](./H264MP4Encode/bin/x64/Debug/) configuration so the bin files are already included there.

Note that the output path of the .mp4 file is hardcoded to `c:\users\idril\downloads\encoded.mp4` so you might want to change it as well.
