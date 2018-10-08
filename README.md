# ssf2fcitx
Automagically convert your favorite sogou-pinyin skin to fcitx skin

NOW ONLY support input bar skin translate, the tray and menu bar is not supported yet

## Dependencies

* Qt5core
* Qt5widgets
* openssl

## Usage

1. `./ssf2fcitx /path/to/your/ssf /path/to/save/theme`
2. copy the directory `/path/to/save/theme` to either $XDG_CONFIG_HOME/fcitx/skin or /usr/share/fcitx/skin
3. Then you can switch to the theme, enjoy it!

## Screenshots

The following skins are converted directly from ssf, no any manual modifications


![pic](https://github.com/VOID001/ssf2fcitx/raw/master/screenshot.png)

## Notice

* The converter now support many of sogou skin conversion, however there are still a lot we don't support yet.
  * we now don't support vertical layout
  * we now don't support skins with GIFs in it(I don't know whether fcitx support it)
  * We now don't support skins with overlay pictures

* The skin converted may have problem with left, right align, sometimes you need to manually adjust it (Just Change the value MarginLeft & MarginRight in skin config file


## Acknowlegements

* kimtoy for providing the ssf extraction process
* csslayer for giving hints on fcitx skin drawing process
