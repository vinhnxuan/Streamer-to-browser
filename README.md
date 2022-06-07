# Streaming from RTP source
## Build libdatachannel dependencies
```sh
Please follow instructions under https://github.com/paullouisageneau/libdatachannel/blob/master/BUILDING.md
```
## Build streamer
```sh
mkdir build
cd build
cmake ..
make 
```

## Start the signaling server

```sh
$ python3 apps/signaling-server-python/signaling-server.py
```

## Start a web server

```sh
$ cd apps/streamer/
$ python3 -m http.server --bind 127.0.0.1 8080
```

## Start the streamer

```sh
$ cd build/apps/streamer
$ ./streamer

Arguments:
- `-d` Signaling server IP address (default: 127.0.0.1).
- `-p` Signaling server port (default: 8000).

You can now open the streamer client at the web server URL [http://127.0.0.1:8080](http://127.0.0.1:8080).

Click button to start playing the stream

If ICE connection state is connected, then the streaming works well

Otherwise, please restart the streamer app and refresh the client page

Notes:
+ This streamer app can be run before or after running yolo4 app (pedestrian detection app) / starting rtp stream
+ This code is copied from https://github.com/paullouisageneau/libdatachannel.
```