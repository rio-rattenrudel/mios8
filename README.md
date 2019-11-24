MIOS8 - MBSID v2 - MOD
======================
## Preamble

This fork contains some modifications to the MBSID v2 Master Branch. The customizations are largely compatible with the original version, but include the following changes. The master branch version on which the modifications are based lies in the branch 'original'. All changes in the source are uniformly marked with beginning and ending comment and described in detail in an explicite info file (in German).

## Modifications

1. [Added: Configuration flags & preparation](https://github.com/rio-rattenrudel/mios8/commit/e3d58de8cf004d3b170aafb38c5f38645b8d5a75)

* New hardware-config files has been added: setup_6581_rio.asm/setup_8580_rio.asm, which contains a few own adjustments of my hardware. There are now two new configuration flags, where only the new config files have been adjusted.

2. [Changed: Initial Lead Patch Name](https://github.com/rio-rattenrudel/mios8/commit/b15ca106b951550995c208fb17f39697c776b11f)

* Patch name was changed in more lucrative "Commodore 64" name as it always appears first after boot

3. [Feature: Patch-independent "Mono Drum Engine"](https://github.com/rio-rattenrudel/mios8/commit/e8d1399be1e0e440a953fab928314a28acac47a3)

* Skip all stereo stuff

4. [Feature: Use special LCD_MOP_AL202C characters](https://github.com/rio-rattenrudel/mios8/commit/b5a26850143e2ad2757866419bcf5ed19f4bc712)

* Use of more eye-friendly characters (but you need that display type for that!)

5. [Feature: Triggering SID refresh to avoid hanging VCA](https://github.com/rio-rattenrudel/mios8/commit/323101d8fdf683136688ee70eabc316e7e72c9dd)

* Added "Reload Patch CC" via CC#120. Why? You can read more about it in this thread: http://midibox.org/forums/topic/20714-triggering-sid-refresh-by-an-event/#comment-180392