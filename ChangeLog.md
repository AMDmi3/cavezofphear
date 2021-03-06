## CAVEZ of PHEAR 0.6.1 (2021-12-15)
- Fixed manpage category.

## CAVEZ of PHEAR 0.6.0 (2021-12-15)
- Added a manpage.
- Fixed some typos in documentation.

## CAVEZ of PHEAR 0.5.3 (2021-10-22)
- Source code cleanup.

## CAVEZ of PHEAR 0.5.2 (2021-10-22)
- Adopted by new maintainer Dmitry Marakasov.
- Switched to CMake build system.
- Updated documentation and converted to markdown.
- Fixed C warnings.

## CAVEZ of PHEAR 0.5.1 (2011-11-30)
- Updated LICENSE, added GPL3 headers to source files.

## CAVEZ of PHEAR 0.5 (2007-02-28)
- Falling stones will now kill monsters.
- Added headers inttypes.h, sys/types.h and string.h
  to src/isready.c to make (free)bsd happy.
- Replaced usleep() with nanosleep(), wrapped in
  mysleep().
- Main game loop time will now dynamically adjust
  itself, trying to keep the game steady at 50
  loops per second.
- Added three new levels.
- Merged the editor with main game executable, now
  accessible with ./phear -e.
- Now globally installable (make install).

## CAVEZ of PHEAR 0.5-pre1 (2007-02-19)
- Some dirty hacks to add bombs and monsters.
- Replaced the "level done" animation with
  fade_dissolv().
- Made a few more levels.
- Cleaned up what little documentation there is.

## CAVEZ of PHEAR 0.1 (2003-11-05)
- isready.c: added missing reference to time.h,
  thanks to Fabrice who pointed this out.
- You can now quit the game by pressing 'q' and
  disable sound by pressing 's'.
- A silly WHERE-IS feature has been added popular
  request. In game, press 'w' to highlight your
  current position.
- To suicide when stuck you will now have to press
  'k' instead of 'd'.
- A primitive form of dialogs, msgbox(), has been
  added. The game will now prompt you to play again
  when game over.
- You may now play using 4-8-6-2 as well as the arrow
  keys.
- The editor has been improved. It now counts
  the maximum possible score on your map, displays
  current object, and I've added a few colors here
  and there.
- Fixed a few minor bugs in the editor.
- There is now an upper limit of 99 lives ;)

## CAVEZ of PHEAR 0.01-beta (2003-10-31)
- First public release.
- Money specials ($) are now affected by the laws
  of gravity, just like diamonds.
- Position of player (Z) can now be specified in the
  editor.
- Lock feature added to editor.
- Stones which roll over you will now kill you. Only
  falling stones would kill you before.

## CAVEZ of PHEAR 0.01-alpha (2003-10-26)
- First release to alpha testers.
