#!/usr/bin/bash

INPUTFILE=$1;
OUTPUTFILE=$2;
if [ -z $2 ];
then
    OUTPUTFILE=$1;
fi;

ICTOOL=/Applications/Xcode.app/Contents/Applications/Icon\ Composer.app/Contents/Executables/ictool;


mkdir -p `pwd`/$OUTPUTFILE-temp.iconset;

## Generate auxiliary files for icon generation
set -f
ICON_FILES=""

# Generate PNG files from source
OLDIFS=$IFS;
IFS=',';
for variant in 16,1 16,2 32,1 32,2 128,1 128,2 256,1 256,2 512,1 512,2; 
do
    set -- $variant
    if [ $2 -eq 1 ]; then
        "${ICTOOL}" `pwd`/$INPUTFILE.icon --export-image --output-file `pwd`/$OUTPUTFILE-temp.iconset/icon_$1x$1.png --platform macOS --rendition Default --width $1 --height $1 --scale $2 > /dev/null
        ICON_FILES="$ICON_FILES `pwd`/$OUTPUTFILE-temp.iconset/icon_${1}x${1}.png"
    else
        "${ICTOOL}" `pwd`/$INPUTFILE.icon --export-image --output-file `pwd`/$OUTPUTFILE-temp.iconset/icon_$1x$1@$2x.png --platform macOS --rendition Default --width $1 --height $1 --scale $2 > /dev/null;
        ICON_FILES="$ICON_FILES `pwd`/$OUTPUTFILE-temp.iconset/icon_${1}x${1}@${2}x.png"
    fi
done
IFS=$OLDIFS;

"${ICTOOL}" `pwd`/$INPUTFILE.icon --export-image --output-file `pwd`/$OUTPUTFILE.png --platform macOS --rendition Default --width 1024 --height 1024 --scale 1 > /dev/null

# Legacy variant of creating icon file for macOS
#iconutil --convert icns --output $OUTPUTFILE.icns $OUTPUTFILE-temp.iconset

# Generate icon file for macOS
actool \
    --compile `pwd` \
    --app-icon $OUTPUTFILE \
    --minimum-deployment-target 12.0 \
    --output-partial-info-plist /dev/null \
    --platform macosx \
    `pwd`/$INPUTFILE.icon

# Generate icon file for windows
magick ${ICON_FILES[@]} `pwd`/$OUTPUTFILE.ico

# Cleanup the emporary files created
rm -r `pwd`/$OUTPUTFILE-temp.iconset

