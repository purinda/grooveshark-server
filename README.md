#Grooveshark Server
This can be run on any computer as an application or on a raspberry pi or any other single board computer as a server and let others connect to it and play songs using the [Web front-end](https://github.com/purinda/grooveshark-webui).
Server implements a TCP Server which implements very few commands just so it can Play, Pause, Stop and change Volume levels.

#Goal
This gives users ability to connect, queue and play songs from almost any device that can connect to the local network that is hosting the service.
Goal was to build an embedded streaming device using a Raspberry Pi single board computer.
Ideally you run this application on a Raspberry Pi and connect a sound system to it, then use the Web UI to control media.
So connect this at home or office and keep your favourite music going!

#How does it work?
Application is built on top of [Grooveshark music streaming service](http://www.grooveshark.com) using an unofficial API to connect and stream songs.
Unoffical API that I *had* to write (since they dont have a public streaming API for "everyone") can be found in lib/grooveshark.

## Dependancies
* Qt 5 or greater
* GStreamer (compiled or plugins with mpeg3 decoder, http streaming capabilities.)

#Steps required to get it to work
* Download the [latest codebase](https://github.com/purinda/grooveshark-server/archive/master.zip) or clone the git repo.
* Install Qt 5 development tools, [Download](http://qt-project.org) or use the package manager provided in your OS.
* Once Qt 5 development tools are installed (with GCC, included with Qt) run following commands in the project folder.
```
mkdir release && cd release
qmake ../grooveshark.pro
make
```
* You should get a compiled binary called 'grooveshark' in the relesae folder provided you have required dependancies installed and application was compiled without issues.
* That's it! just run ```./grooveshark```, it binds to port 16444 to host the service. Use your telnet client (from the same machine or different computer in the network) and connect to it (telnet [ip-of-grooveshark-box] 16444).

#Wishlist for Christmas
* Implement JSON based communication protocol.
* Add music caching so all songs that gets played at least once gets cached on a path, which can be downloaded if required. Potentially save bandwidth if you are like me who shuffle the same playlist over and over again.
* Direct music download, or a way to send the music files back to user making this application a general purpose music downloader.

#Setting up Raspberry Pi
I picked Arch Linux image for Raspberry Pi for the reasons listed below.
* Raspbian (official raspberry pi distro) is based on Debian Wheezy and didn't have Qt5 package in their official repos.
  Tried installing backport from sid but without latest libc you cannot install Qt5 (since its compiled against the latest libc).
* Crosscompiling can be done but then you need to compile libraries that Qt5 depens on for ARMv6 instruction set, which is a pain, since number of libs such as
  gstreamer, xlib, etc required to be compiled before compiling Qt.

Use the following URL to grab the Arch image for ARMv6 and follow instructions on the page to copy it to a SDCard.
http://archlinuxarm.org/platforms/armv6/raspberry-pi

##Archlinux specific setup
* Set following modules for pulseaudio in /etc/pulse/default.pa and reboot
```
### Load the integrated pulseaudio equalizer and dbus module
load-module module-equalizer-sink
load-module module-dbus-protocol
```
* start pulseaudio service
```
"pulseaudio --start"
```
* Run the following command to set sampling rate required for pulseaudio
```
"sed 's/; default-sample-rate = 44100/default-sample-rate = 96000/g' -i /etc/pulse/daemon.conf"
```


#Having problems?
These are some of the problems I came across while trying to get the app to work on rPI.
* When you try to play a song on the server side I got "Error: No URI handler imlemented for 'http'"
or
```
"Warning: "No decoder available for type 'audio/mpeg, mpegversion=(int)1, mpegaudioversion=(int)1, layer=(int)3, rate=(int)44100, channels=(int)2, parsed=(boolean)true'.
```
The issue seems to be a missing package in the system for gstreamer, http support and mpeg decoder.
So try following command to install gstreamer plugins required for QtMediaPlayer. Recompile.
```
pacman -S gst-plugins-good gstreamer0.10-good-plugins gstreamer0.10-bad-plugins gstreamer0.10-ugly-plugins
```
* Error: "Configured audiosink audiosink is not working."
Refer to https://wiki.archlinux.org/index.php/Raspberry_Pi#Audio

* No sound (god level debugging :) )
if not most probably the problem is with gstreamer library, what you can do is set debug level for gstreamer using the environment variables
try something like "export GST_DEBUG=5" in the same terminal session that you run your grooveshark app. For all DEBUG levels refer to
http://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gst-running.html
and refer to https://wiki.archlinux.org/index.php/PulseAudio
