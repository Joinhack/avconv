Install
=================
- install golang
- install libopencore-amrnb-dev  libmp3lame-dev
- $ go get github.com/joinhack/avconv
- $ go install github.com/joinhack/peony/peony
- $ peony run github.com/joinhack/avconv

access:

http://localhost:8000/amrnb2mp3?url=http://127.0.0.1/ddb293aa-dede-4fc5-9957-215e942d0a7d.amr

http://localhost:8000/amrnb2wav?url=http://127.0.0.1/ddb293aa-dede-4fc5-9957-215e942d0a7d.amr

Why not ffmpeg
=================
ffmpeg API is complex for me.  Maybe support later.

