# GNU/Linux compilation makefile
# matches my machine, prolly won't compile on any other :P
HEADERS = assert.h buffer.h charlist.h colors.h console.h creature.h creatures.h database.h debugprinth. defines.h flythrough.h gamemode.h glutfont.h glutwin.h gm_gameworld.h gm_logo.h gm_mainmenu.h gwlogon.h imgfmts.h item.h items.h map.h networkdirect.h networkmessage.h obj3ds.h object.h objspr.h player.h protocol.h protocol75.h protocol76.h protocol77.h protocol79.h protocol792.h simple_effects.h socketstrings.h sound.h sprfmts.h texmgmt.h thing.h threads.h tile.h types.h 

SOURCES = buffer.cpp charlist.cpp colors.cpp console.cpp creature.cpp creatures.cpp database.cpp debugprint.cpp  flythrough.cpp gamemode.cpp glutfont.cpp glutwin.cpp gm_gameworld.cpp gm_logo.cpp gm_mainmenu.cpp gwlogon.cpp imgfmts.cpp item.cpp items.cpp main.cpp map.cpp networkdirect.cpp networkmessage.cpp obj3ds.cpp object.cpp objspr.cpp player.cpp protocol.cpp protocol75.cpp protocol76.cpp protocol77.cpp protocol79.cpp simple_effects.cpp socketstrings.cpp sound.cpp sprfmts.cpp texmgmt.cpp thing.cpp threads.cpp tile.cpp 
OBJECTS = buffer.o charlist.o colors.o console.o creature.o creatures.o database.o debugprint.o flythrough.o gamemode.o glutfont.o glutwin.o gm_gameworld.o gm_logo.o gm_mainmenu.o gwlogon.o imgfmts.o item.o items.o main.o map.o networkdirect.o networkmessage.o obj3ds.o object.o objspr.o player.o protocol.o protocol75.o protocol76.o protocol77.o protocol79.o simple_effects.o socketstrings.o sound.o sprfmts.o texmgmt.o thing.o threads.o tile.o 

CPPFLAGS = -D POSIX_THREADS -D SQLITE_OLD -D DEBUGLEVEL_BUILDTIME=3 -I /home/ivucica/glict/\[CPP\]\ GLICT

theoutcast: ${OBJECTS}
	g++ -o theoutcast -lsqlite  -lGL  -lglut /home/ivucica/lib3ds-1.2.0/lib3ds/*.o /home/ivucica/glict/\[CPP\]\ GLICT/GLICT/*.o ${OBJECTS}
	
all: theoutcast
