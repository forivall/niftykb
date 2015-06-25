# Nifty Keybinder

Allows mapping hard-to-detect keys to output commands over [Windows' MailSlot IPC](https://en.wikipedia.org/wiki/MailSlot) and perform other nifty tasks. <small>(run programs and faking other, more detectable keystrokes)</small>

Designed to work with an upcoming fork of the [Teamspeak 3](http://teamspeak.com)
[G-Key Plugin](http://addons.teamspeak.com/directory/addon/hardware/G-Key-plugin.html)
[(repo)](https://github.com/Armada651/g-key)

Adapted from mumble's global keybinding code. May be refactored to use [nil](https://github.com/noorus/nil) in the future.

Depends on Qt 5+, DirectInput, boost (headers only), cmake (to build).

## Build instructions

Download [Qt5](http://www.qt.io/download-open-source/),
the [DirectX June 2010 sdk](https://www.microsoft.com/en-ca/download/details.aspx?id=6812),
and [Boost](http://www.boost.org/users/download/).

<small>(Script assumes you have Qt installed in `%SYSTEMDRIVE%\Qt` and MSVS 12.0 in `%PROGRAMFILES(X86)%\Microsoft Visual Studio 12.0`</small>

Replace "[arch]" with `x86` or `x64`
```
cd build-x[arch]-debug
../usr/bin/env_x[arch].cmd cmake -DCMAKE_BUILD_TYPE=Debug -G "NMake Makefiles" ..
../usr/bin/env_x[arch].cmd jom
../usr/bin/copydebugdlls.cmd [arch]
```

You can also use qt creator:joy_cat: or :see_no_evil:visual:hear_no_evil:studio:speak_no_evil: to build, ymmv.

## TODO
- Actually do stuff!
- Create Teamspeak plugin
- Support [g-keys](http://gaming.logitech.com/developers), either with the pure lib or with nil.
