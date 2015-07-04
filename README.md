# Nifty Keybinder

Allows mapping hard-to-detect keys to output commands over [Windows' MailSlot IPC](https://en.wikipedia.org/wiki/MailSlot) and perform other nifty tasks. <em><small>(run programs and faking other, more detectable keystrokes)</small></em>

Works together with the [niftykb-ts3 plugin](https://github.com/forivall/niftykb-ts3) for [Teamspeak 3](http://teamspeak.com) Plugin. Let me know if you have any other uses for this!

![Screenshot](http://i.imgur.com/KuEaeQN.png)

Adapted from mumble's global keybinding code. May be refactored to use [nil](https://github.com/noorus/nil) in the future.

Depends on Qt 5+, DirectInput, boost (headers only), cmake (to build).

## Build instructions

Download [Qt5](http://www.qt.io/download-open-source/),
the [DirectX June 2010 sdk](https://www.microsoft.com/en-ca/download/details.aspx?id=6812),
and [Boost](http://www.boost.org/users/download/).

<em><small>(Script assumes you have Qt installed in `%SYSTEMDRIVE%\Qt` and MSVS 12.0 in `%PROGRAMFILES(X86)%\Microsoft Visual Studio 12.0`)</small></em>

Replace "[arch]" with `x86` or `x64`
```
cd build-x[arch]-debug
../usr/bin/env_x[arch].cmd cmake -DCMAKE_BUILD_TYPE=Debug -G "NMake Makefiles" ..
../usr/bin/env_x[arch].cmd jom
../usr/bin/copydebugdlls.cmd [arch]
```

You can also use qt creator:joy_cat: or :see_no_evil:visual:hear_no_evil:studio:speak_no_evil: to build, ymmv.

## TODO
- Release built executables
- Improve documentation
- Support multiple messages for a single keypress
  - Use `QStringList` in the qvariant
- Add right click menu for tray icon
- Automatically close to tray
- Add option to start hidden to tray
- Support [g-keys](http://gaming.logitech.com/developers), either with the pure lib or with nil.
- Detect which keyboard is being used, like nil
- Get suppress working, especially for keys that are on the default layout
- Use a usb lib to control the led on the azio levetron mech5
