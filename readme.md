Goal
-----


Setting up Raspberry Pi
--------------------------
I picked Arch Linux image for Raspberry Pi for the reasons listed below.
    1. Raspbian (official raspberry pi distro) is based on Debian Wheezy and didn't have Qt5 package in their official repos.
    Tried installing backport from sid but without latest libc you cannot install Qt5 (since its compiled against the latest libc).
    2. Crosscompiling can be done but then you need to compile libraries that Qt5 depens on for ARMv6 instruction set, which is a pain, since number of libs such as
    gstreamer, xlib, etc required to be compiled before compiling Qt.

Use the following URL to grab the Arch image for ARMv6 and follow instructions on the page to copy it to a SDCard.
http://archlinuxarm.org/platforms/armv6/raspberry-pi

Arch specific setup
    1. Set following modules for pulseaudio in /etc/pulse/default.pa
    ### Load the integrated pulseaudio equalizer and dbus module
    load-module module-equalizer-sink
    load-module module-dbus-protocol

    2.


Common Issues
---------------
These are some of the problems I came across while trying to get the app to work on rPI.
    1. When you try to play a song on the server side I got "Error: No URI handler imlemented for 'http'"
    or "Warning: "No decoder available for type 'audio/mpeg, mpegversion=(int)1, mpegaudioversion=(int)1, layer=(int)3, rate=(int)44100, channels=(int)2, parsed=(boolean)true'.
    The issue seems to be a missing package in the system for gstreamer, http support and mpeg decoder.
    So try "pacman -S gst-plugins-good gstreamer0.10-good-plugins gstreamer0.10-bad-plugins gstreamer0.10-ugly-plugins"
    to install gstreamer plugins required for QtMediaPlayer. Recompile.

    2. Error: "Configured audiosink audiosink is not working."
    Refer to https://wiki.archlinux.org/index.php/Raspberry_Pi#Audio

    4. No sound (try setting GST_DEBUG as per below to WARN)
    first check whether you started pulseaudio. try "pulseaudio --start"
    Refer to https://wiki.archlinux.org/index.php/PulseAudio
    if you see a warning such as " WARN      playsinkconvertbin gstplaysinkconvertbin.c:481:gst_play_sink_convert_bin_cache_converter_caps:<GstPlaySinkAudioConvert@0x1ab030> No conversion elements"
    run the following command which will set sampling rate for pulseaudio output device.
    "sed 's/; default-sample-rate = 44100/default-sample-rate = 96000/g' -i /etc/pulse/daemon.conf"

    5. No sound (god level debugging :) )
    if not most probably the problem is with gstreamer library, what you can do is set debug level for gstreamer using the environment variables
    try something like "export GST_DEBUG=5" in the same terminal session that you run your grooveshark app. For all DEBUG levels refer to
    http://gstreamer.freedesktop.org/data/doc/gstreamer/head/gstreamer/html/gst-running.html
