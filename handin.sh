HANDIN=~cs537-2/handin/sartin-t/p3
FILES="Makefile disk-array.c disk-array.h disk.h disk.c Makefile fourTest.c raidFive.c raidFive.h raidFour.c raidFour.h raid_handler.c raid_handler.h raidsim.c raidTen.c raidTen.h raidZero.c raidZero.h README.TXT zeroTest.c"

set -e

for FILE in $FILES; do
	rm -rf $HANDIN/$FILE
	cp $FILE $HANDIN
done

echo "Files handed into $HANDIN. Congrats."