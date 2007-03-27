THE OUTCAST v0.3.6
Readme
21.3.2007

Copyright 2005-07 OBJECT Networks. All rights reserved.
Reproduction of this software is free for non-commercial purposes only. Redistribution is, however, not allowed.
---

CONTENTS
1. About this release
2. History
3. Minimum Requirements
4. Reporting crashes
5. Instructions
6. Compatibility
7. Troubleshooting
8. Known problems and bugs
9. Credits & thanks
10. Change History
11. License
12. Message to CIPSoft
13. Big badda boom

---

Hey folks, this is The Outcast v0.3.51 alpha. Let's take a look at what "The Outcast" is going to be, and how to use this prerelease.

This version is a total rewrite, meaning that nothing from the old version is inside. It will take a lot of time and patience to get back to all features that were done previously, but it will progress faster. I hope you folks will stick with me. Please, TEST THIS NEW VERSION and tell me what do you think about it! It is also the right time to SUGGEST NEW STUFF and tell what you MOSTLY DISLIKED (except for graphics) in the old The Outcast, so that we can steer it properly this time.

I'd like to point out that new The Outcast supports multiple protocols in the same executable! This means, when you enter your username and password, you also pick the protocol! There's no more trouble with the Launcher (which, I am sure, has confused a lot of you).

Currently, you must enter the 3 digit code of the protocol. Multiple protocols are supported: 760, 770, 790, 792 -- more upcoming.

It is NOT allowed to connect to CipSoft servers using alternative clients! If you do connect and CipSoft notices you're not using their client, you may be banned! (Tibia Rules, Section 3d)

---

1. ABOUT THIS RELEASE

This release is intended to demonstrate current progress on the software development.
In no way it is a complete working product. There are bugs, and we are in no way responsible for any damage to your data, computer, mind, brain, arms, parents, grandparents, dog, cat, bed, house or anything else in your life, that can happen.

This is a complete rewrite of The Outcast, from scratch, started since (effectively) December 26th 2006. Folder was created on December 16th, however it was only preparation for real programming.

Currently it can log on, display character list, fetch the map from server, parse most gameworld packets in a stub manner, you can walk around, and it will show text other people say, sprite animations work for the most part, creatures are also (properly?) loaded and displayed.

1.a) Rant about this version

This release has a bit nicer display, and that's basically it.

NOTE: This release is A MEMORY HOG and WILL USE YOUR MEMORY WITHOUT MERCY. It will ALLOCATE TONS OF MEMORY WITH EVERY STEP YOU MAKE, but it will NOT DEALLOCATE ANY MEMORY UNTIL EXIT. Consider yourself warned.

1.b) Rant about the future

No significant plans.

---

2. HISTORY

This is a complete rewrite of The Outcast, from scratch, started since (effectively) December 26th 2006. Folder was created on December 16th, however it was only preparation for real programming.

Original The Outcast was being developed since ~13. 10. 2005.

I first got around OTserv when a friend showed it to me. I disliked Tibia (and up till then, never even opened it), but OTserv community was nice and seemed to open up opportunities, and I soon started hosting a server at tornado.zrs.hr (since February 12th 2005).

In summer and fall 2005 I've studied OpenGL a lot. It's taken up a lot of time, and I've been fascinated with networking for several years back from then, already. Since I've only a bit before that gained experience in programming Win32 in C++ (instead of Visual Basic), I've figured that I could make a program that would make Tibia (a visually extremely primitive game, but with a good structured networking protocol) a bit more modern and playable game. Combining my 3 great interests in computers - programming, networking and 3D - I've decided to make a modern-looking 3D Tibia-compatible client.

On October 24th 2006 I concluded what I was thinking for a few months then: that the old codebase was no longer in state of upgradeability, and I turned to some other things, and in meantime I got enough experience to turn back to The Outcast. I also saw that I was not doing anything productive in OT community and so on December 16th I've decided to write The Outcast from scratch.

---

3. MINIMUM REQUIREMENTS

Barely any, for this version.

Note: this has not been tested on large number of machines yet (I belive that I did not yet test it on anything under 1.8 GHz). So, these are just my estimates.

PROCESSOR: 500MHz
MEMORY: 64MB
GRAPHICS CARD: Probably one with at least 32MB video memory. I've got one with 2MB, I'll check up with that one later.
OS: Windows 98SE minimum (possible API incompatibility, untested)

Developed on:
PROCESSOR: Intel Celeron M 1.7GHz laptop
MEMORY: 512MB laptop
GRAPHICS CARD: Intel G915 laptop
OS: Windows XP

Recommended:
I guess I can recommend my system configuration!

---

4. REPORTING CRASHES

After every crash, "The Outcast" (using EXCHNLD.DLL) automatically generates an error report. The file ends in .RPT, and start of the file name depends on the executable you have started.
To help improving stability of "The Outcast" we would like you to send these reports. They are plain text files you can open using notepad containing a lot of non-sense information. As you don't have the source code you have no ways of understanding what that stuff means, but as we have the sources it might help us find out where "The Outcast" has crashed and eventually fix the bug.

Please, before reporting a crash, check out the "KNOWN BUGS AND PROBLEMS" section of this document.

Visit www.otfans.net - The Outcast's forum - for more information.

---

5. INSTRUCTIONS

Before playing the game, copy the "Tibia.spr" from your "Tibia version 7.6" client's folder. Place it into The Outcast's folder and name it "Tibia76.spr".
Also, please copy the "Tibia.spr" from your "Tibia version 7.9" client's folder. Place it into The Outcast's folder and name it "Tibia79.spr".
Also, please copy the "Tibia.spr" from your "Tibia version 7.92" client's folder. Place it into The Outcast's folder and name it "Tibia792.spr".
If you do not copy one of the above, you won't be able to use that protocol. Other protocols will work as usual.

On the main menu click on Login. Enter the server you want to connect to, the account number and password. Under protocol, enter either 760, 770 or 790. Click OK. Choose one character.

When the map loads, you can walk around with arrow keys. You can chat by clicking in the textbox in console, typing the message and pressing enter. Press ESC to leave the game.

---

6. COMPATIBILITY

The client given out for free by CIPSoft contains simply too much graphics, and it is too much to expect that one student would ever be able to achieve full compatibility with their software. However, as much compatibility as possible will be attempted, and as much graphics as possible recreated.

Protocols 7.6, 7.7, 7.9, 7.92 - post-gameworld stub packet support and moving around. We plan to add as much protocols as possible.


---

7. TROUBLESHOOTING

a) I'm having very low FPS!
Verify that you have valid and latest drivers for your video graphics card! Visit your video card manufacturer's web site and obtain latest drivers.
- ATI: http://www.ati.com/
- nVidia: http://www.nvidia.com/

b) Where's all the old features gone?
This version is a total rewrite, meaning that nothing from the old version is inside. It will take a lot of time and patience to get back to all features that were done previously, but it will progress faster. I hope you folks will stick with me. Please, TEST THIS NEW VERSION and tell me what do you think about it!

c) It's slowing down strangely sometimes, and lagging!
It's recommended that you close all applications before running The Outcast (or any game, in general). We have noticed that Mozilla Firefox occasionally runs at over 60% CPU usage for a couple of moments, then for a few seconds no longer uses the processor.
Thus, before trying The Outcast, please close all applications, ESPECIALLY MOZILLA FIREFOX.

d) It crashes!
Please send The Outcast.rpt file to us.
---

8. KNOWN PROBLEMS AND BUGS

* You can enter letters under the account number.
* You will have less than 3 FPS on Windows without your video card drivers. And no, you can't play Quake III either with high FPS if you don't have OpenGL drivers; id software likes OpenGL, too! Microsoft's GDI implementation, the one you have by default, is slow, don't use it! And Windows use it in case you don't have OpenGL drivers installed! (Hint: OpenGL drivers are included with your graphics card drivers.)
* Sprite animations are not in sync
* Logging out of the game and then attempting to log in fails
* There is a "grid" between sprites. It'll be fixed. Soon. Trust me!
* Creatures appear to be walking OVER 2x2 items
* Blendframes sprites are drawn in wrong order

* THIS THING SOMETIMES CRASHES. (Yes, I know, and you should know because this is an alpha version.) Still, send me the cause if you can figure it out.

---

9. CREDITS & THANKS

So far this could be called a one man project. 99% done by Ivan Vucica - Khaos. However there are some other people who have significantly contributed.
As I upgrade The Outcast with more features including some from old The Outcast, this list will be enhanced with people who contributed to those features in old The Outcast.

Credit for the name goes to Ryan Majetich a.k.a. Scotchy.
Credit for the logo goes to summvitae.

Outcast utilizes the following libraries and wishes to thank their authors:
 JPEG Library
    by Independent JPEG Group
 GLUT
    by Mark Kilgard
 Tom's Fast Math
    by Tom St Denis
 Dr.MinGW EXCHMDL.DLL
    by MinGW Project

Thanks to CIPSoft for making Tibia and creating this wonderful protocol.
Thanks to crew from #otserv (irc.quakenet.org) and from Tornadia Game Worlds for providing support when I needed it.
Thanks to my computer science teacher who kindly allowed me NOT to pay attention during her classes and develop The Outcast and study OpenGL, FMOD, OpenAL and who-knows-what-not.
Thanks to my family for ... erm, relatively good support (from time to time) and for, of course, food :)
Thanks to ZRS for supporting me during last several years in my involvement with computers.
Thanks to brave from OTservlist.com for providing a special interface to the server list.
Thanks to OTfans.net staff for opening The Outcast's forum, thus helping spread the news.
Thanks to BlackDemon for 7.81 dat reading support!
Thanks to everyone who downloaded the old versions of The Outcast!
Big thanks to OTserv team who decoded the protocol and hacked Tibia's datafiles inside out to make OTserv. It's been an unreplaceable tool for local development!

---

10. CHANGE HISTORY

Changes since 0.3.5
* Implemented cancelmove packet
* Fixed the order in which creatures are drawn

Changes since 0.3.4
* Sidescrolling
* Preemptive moving
* Minor fixes

Changes since 0.3.3
* SPR engine now properly renders 4x4 sprite alterations between identical items.
* It also knows about creature direction. Hooray!
* It properly renders "overlay sprites", too :)
* Various smaller fixes

Changes since 0.3.2
* Protocols 7.9, 7.92
* Textures are supported for 3DS
* Added face culling which should (insignificantly) increase performance
* Various internal things
* Mouse cursor nicer
* New FPS calculation mechanism
* Everything is now rendered inside GLICT subwindows, including world and chat console ;)
* You can now send messages
* Controls changed to cursor keys
* Esc now quits
* Added some spr overriding possibilities
* Now, Tom's Fast Math is used instead of GMP

Changes since 0.3.1
* Fixed some crashes
* Gameworld connection is now maintained
* Sprite loading done
* You can see sprites and their animations (the sprite rendering engine is not perfect yet, however)
* You can walk around
* You will see items that move around

Changes since 0.3
* Some crashes fixed
* Added gameworld connection! Only first gameworld message is parsed, however
* Map fetching is here, tons of packets are parsed already


Changes since 0.2.6
* Total rewrite

======REWRITE======
Changes since 0.2.5
* Fixed "fadein names wrong colors" issue
* Flythrough logon screen... let's say that with noob language: "o.O"
* Fixed a graphics glitch in tibiaperspective mode
* Distance shot implementation fixed! Actually never the correct effect that server sent to client was rendered, instead junk from memory was rendered, so you were actually lucky to see anything at all.
* Resolved several ugly rendering issues.
* Pretty lame of me, but I missed a few packets, or didn't update for a while. Stubbed 'em, at least
* Started adding icon support (battle, poison, haste etc...); you can see the icons, but they suck at the moment.
* Sprite rendering is back
* Resolved some item dragging display issues. Now it looks perfect ;)=
* Several new items, and some textures made nicer
* New distance shot effects. Oh yea, one cool thing: arrows rotate DIRECTLY towards their destination, not approximately; 3d offers so much more options for rotation ;)
* SQLite database used for storing items. No more .dat! No more items.txt! <They remain supported, however .dat will not be distributed anymore as it is copyrighted by CipSoft>
* BG pic engine fixed a bit; looks a bit more natural now ;)
* Readme rearranged a bit. It's getting pretty large a bit; maybe cleanup in next version?
* Environment lighting support
* Preliminary per-object light emission support. No creature light emission yet (utevo lux wont have effect); also it's buggy sometimes, but still, better than before ;)
* Fixed networking issue with encryption
* Additional networking issue with encryption fixed!
* Launcher: added items.db support
* Added protocol 7.81 support (thanks go out to BlackDemon for 7.81 dat reading)
* Added voiceover support
* Added "datfile" commandline argument, to allow spr loading even with SQLite database. (you must supply your own dat file in this case)
* Rearranged readme file a bit some more
* Text messages above tiles
* OMG!! Release 0.2.5 DID NOT include any subdirectories in texture/ and objects/ folders! Fixed now!
* Fix in using items
* New male model by yours truly
* Daten's stuff is gone from this release. He faked authorship.

Changes since 0.2.4
* Although Daten left the team temporarily, he appears to be back
* Added protocol 7.7 support - RSA and XTEA functions are finally there
* Added some localization support for Croatian language - our symbols are in there
* ESC while typing msgs now cancels entry.
* Disconnect check code
* 7.72
* Distance shot effects
* Item dragability (you can move the items now)
* Unexplicably, several STL Map variables (only two of them, actually) have broken. It will be fixed in next version, until then sprites loading is impossible again and game crashes on exit (not during play, as far as I've noticed)
* DzPD contains mirroring possibility and contains small patch for download correctness reporting

Changes since 0.2.3
* Magic effects preliminary support! Sounds supported only. Support for this was included in 0.2.3 too, but I forgot to upload the files ;)
* ...and also the REAL MAGIC EFFECTS SUPPORT! Explosions, spawning, blood, puffs ... it's in there!
* Added custom fonts; visible as 'skill icons', if nothing else
* Added skills
* Changed icon! Hope you like this one better
* Added spr loading and drawing support; only completely 2d scene is supported at the moment. You need to copy spr yourself as it's too big, plus copyright law forbids it (yes, I'm working on getting rid of tibia.dat too!)
* I found out why vertex arrays behaved slower! Read section on vertex arrays (under 5 - Instructions)
* Internally started adding support for different mapsizes. (You don't see it anywhere from the outside yet)
* Internally added "server pic" for orthologon view - nothing you'll see yet, orthologon screen is a compile-time option
* Added game background
* Added animated text support
* Added smiley support (compile time option which seems to slow down significantly, so turned off)
* Added support for custom fonts everywhere (one compile option switches to a 'custom' font)
* More descriptive version string, which includes some compiletime option description:
   S = Smileys
   L = Logonortho
   B = Logonortho switchable backgrounds
   H = Hide account (puts asterisks (*) instead of account number)
   F = Custom-fonts-everywhere (if off, used only where forced, e.g. UI Icons)
* Private messaging added

Changes since 0.2.2
* Optimized FPS a bit by not rendering the picking all the time
* Added more items (which are not included in this release, but will be in next one)
* Added vertex array support. Surprisingly this decreased framerate, so VA are just an option (and you need to use commandline to use it)
* Added a possibility of "simple" textures - they're just stretched and not smoothed (commandline only)
* Addressed some lag related issues
* Added "use" function as middle mouse button

Changes since 0.2.1
* Fixed a bug with loading 7.6 dat files
* Proper "too many players online" handling
* Proper "textmessage" colorization
* Added more items
* Added Negative's items
* Added & fixed Gecko Slayer's item numbers (for 7.6 borders):
  * Desert -> there's no "white.bmp" desert
  * Graything -> it's not "greything.bmp" it's "graythng.bmp"!
           (due to some stuff I've copied it to rock.bmp and put that in)
  * STILL - THANKS, YOU'VE DONE LIKE 95% OF THE WORK FOR THOSE ITEMS!
* Optimized loading a bit ;)
* Added (im)proper levelchange handling. Improper, but hey, it works
* Fixed creature-never-appearing-once-gone bug
* Unfortunately, CipSoft's server had to be removed from Launcher due to, upon inquiry, them pointing out that it is illegal to connect to their servers using alternative clients. If you do connect and CipSoft notices you're not using their client, you may be banned
* Renamed launcher back to Launcher.exe
* Preliminary selection support!! Woah, thought that'd never come, eh? :)
* Looking works!
* Attacking works!
* Even better Tibia-style perspective - several bugfixes and floor hiding (combined with attacking) makes this a pretty usable client for dungeon hunting ;) (If only the containers and item using worked... :))
* Fixed a bug with big account numbers ("Logon failed due to corrupt data.")
* Maybe some more? WHAT MORE DO YOU WANT!?! :)

Changes since 0.2 (prerelease alpha)
* Console coloring

Changes since 0.11
* Lights
* Smooth movement
* More models
* Creature displayed properly
* Camera movement
* Multilevel display
* Some chat support
* Preliminary joystick support
* Preliminary mouse support - only displays a quad
* 7.6 support
* Install system
* Launcher rewritten from scratch
* Launcher contains net module & more options
* Tibia-like perspective mode
* Secret stuff :)

---

11. LICENSE


0. TERMINOLOGY. Throughout this license, "the original The Outcast archive" or "the original archive" refers to the set of files originally distributed by OBJECT Networks as The Outcast. This license only applies to the freely distributable versions of The Outcast, OBJECT Networks may make The Outcast, or portions of it, available under different licensing conditions for selected licensees.

"Data files" refers to all files that are placed in one of the sub-folders of the original archive, or are named items.txt in the original archive.

"To destroy the software" means to destroy the software, the original archive and any other form and medium you may have The Outcast in.

"You" refers to the licensee, the person or other legal entity using The Outcast. "Using" The Outcast means executing The Outcast.

This license applies to all files distributed in the original The Outcast archive, including all source code, libraries, textures, sounds, music, binaries and documentation, unless otherwise stated in the file in its original, unmodified form as distributed in the original archive. If you are unsure whether or not a particular file is covered by this license, you must contact us to verify this.

1. REPRODUCTION. Reproduction and redistribution of this software in completely unlimited form is permitted for non-commercial purposes only. Redistribution of software outside the original archive or modified software is not permitted and results in termination of license, which means you must destroy the software.

2. TERMINATION OF LICENSE. Authors reserve the right to terminate the license and request that you destroy the software, the original archive and any other form and medium you may have The Outcast immediately.

3. MODIFICATION AND REVERSE ENGINEERING. You agree not to reverse engineer, decompile or modify the software in any way, except for modifying the data files to suit your needs.

4. OWNERSHIP. Authors reserve the ownership of the software. You are not the owner of the software; you only receive the license to use it.

5. OBLIGATIONS. You will report any problems about the software to the Authors.

6. WARRANTY. THE OUTCAST IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL ANY OF THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY DAMAGES CAUSED BY THE USE OR INABILITY TO USE, OF THE OUTCAST.

As you have not signed this license, you are not required to accept it. However, as The Outcast is copyrighted material, only this license grants you the right to use, distribute and modify it.  Therefore, by using, distributing or modifying The Outcast, you indicate that you understand and accept all the terms of this license. Hereby:

IF YOU DO NOT AGREE OR ACCEPT THE LICENSE YOU MUST IMMEDIATELY DESTROY THE SOFTWARE, THE ORIGINAL ARCHIVE AND ANY OTHER FORM AND MEDIUM YOU MAY CURRENTLY HAVE THE OUTCAST IN.

#midas

---

12. MESSAGE TO CIPSOFT

  Dear people of CIPSoft,

The Outcast is currently in no way intended as a complete replacement for your client. It is also not intended to be a cheating platform. It is simply intended to provide with a different experience and view of games using your protocol - including your own, Tibia. We will not in any way take your profits away from you, as there will surely be players of The Outcast that will buy premium accounts for your service.

We believe we have broken no patent or copyright you may have. We will cease to distribute Tibia.dat upon development of our own complete format for writing required data, and we hope that you will allow us to distribute until then.

In case we have done any infringement you are not ready to agree upon, please contact us as soon as possible with exact details where we have broken it so we can update our software, removing the copyright infringement.

Hoping you will support our project,

  Ivan Vucica
  lead programmer
  project leader

  In Zagreb, Jan 24th 2006

Update, July 19th 2006: We have ceased requiring .DAT file, and thus will stop distributing it from version 0.2.6 on.
Update, December 27th 2006: New version being written from scratch. This will NOT include ANY copyrighted data from CIPSoft, ANYWHERE.

---

13. CURRENT CODE STATISTICS

Current code statistics:
* Number of files: 89

* Code only: 68%
* Code + Comment: 6%
* Comments: 8%
* Empty: 18%

* Code only: 6293
* Empty lines: 1706
* Comment lines: 771
* Code and comments: 538
* Total: 9308

---

14. BIG BADDA BOOM

****    **  ****   ****    **   (tm) (R)  (C) (pm) (rm) (err) (argh)
*****  **** *****  *****  ****
**  *  *  * ** *** ** *** *  *
****   *  * **  ** **  ** *  *
**  *  **** **  ** **  ** ****
*****  *  * **  ** **  ** *  *
****   *  * ****** ****** *  *

**           *   *
* *          ** **
**   /\  /\  * * *                it's all about fun
* *  \/  \/  *   *
***          *   *

Wow, you really went through all that boredom? I'm really amazed. Did you really read all that? No? Then SCROLL BACK YOU LAZY BUM!
:D

