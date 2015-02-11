#*****************************************************************
# Make file for Conquest
# created 12/11/1997 by James D. Bennett
#
# Change History:
#
#*****************************************************************/

#CFLAGS = -std -taso -unsigned -O3 -verbose
CFLAGS = -unsigned -O3 -Wall
LIBS = -lc -lm

TARGET = conqd
TARGETSRC = conqd.c
TARGETOBJ = conqd.o

AOUT = $(TARGET)

# Define below all of the .c's to be compiled and linked into target

SRC = $(TARGETOBJ) conquest.o barb.o battle.o emp.o help.o house.o \
items.o mage.o msclib.o parser.o quest.o continent.o vampire.o cleric.o \
ranger.o heroes.o weather.o feudalism.o mercs.o message.o wsaerrors.o \
bounty.o steward.o xml.o

$(AOUT): $(SRC) 
	gcc $(CFLAGS) $(SRC) $(LIBS) -o $(AOUT)
	strip $(AOUT)
	@echo Compile complete

.c.o:
	@echo "Compiling $@"
	gcc -c $(CFLAGS) $<

# Define below all of the dependencies for all of the compilable .c's above

$(TARGETOBJ)	: $(TARGETSRC) 
conquest.o	: conquest.c
barb.o		: barb.c
battle.o	: battle.c
emp.o		: emp.c
help.o		: help.c
house.o		: house.c
items.o		: items.c
mage.o		: mage.c
msclib.o	: msclib.c
parser.o	: parser.c
quest.o		: quest.c
continent.o	: continent.c
vampire.o	: vampire.c
cleric.o	: cleric.c
ranger.o	: ranger.c
heroes.o	: heroes.c
weather.o	: weather.c
feudalism.o	: feudalism.c
mercs.o		: mercs.c
message.o	: message.c
wsaerrors.o	: wsaerrors.c
bounty.o	: bounty.c
steward.o	: steward.c
xml.o		: xml.c
