#!/bin/bash

DTA_EXECUTABLE="../build/bin/dta"
MAFIA_LOCATION="../mafia"

cd "$(dirname "$0")"  # cd to this script's directory
. general.sh          # include helpers

print_h1 "TESTING DTA UTIL"

AB_LINES=`$DTA_EXECUTABLE "$MAFIA_LOCATION/ab.dta" | wc -l`
A2_LINES=`$DTA_EXECUTABLE "$MAFIA_LOCATION/a2.dta" | wc -l`
A2_AB_LINE_DIFFERENCE=`expr $A2_LINES - $AB_LINES`

assert_equal $A2_AB_LINE_DIFFERENCE 3182 "file list check"

# try extracting uncompressed file:
print_info "extracting SOUNDS\\MUSIC\\city_music_01.ogg from ab.dta"
$DTA_EXECUTABLE "${MAFIA_LOCATION}/ab.dta" -e "SOUNDS\\MUSIC\\city_music_01.ogg" > /dev/null
assert_command "diff '$MAFIA_LOCATION/sounds/music/city_music_01.ogg' ./sounds/music/city_music_01.ogg"
rm -rf ./sounds

# try extracting LZSS compressed file:
print_info "extracting DIFF\\dif01b.chg from a5.dta"
$DTA_EXECUTABLE "${MAFIA_LOCATION}/a5.dta" -e "DIFF\\dif01b.chg" > /dev/null
assert_command "diff '$MAFIA_LOCATION/diff/dif01b.chg' ./diff/dif01b.chg"
rm -rf ./diff

# try extracting DPCM compressed file:
print_info "extracting DIFF\\sounds\\10b_nasedani.wav"
$DTA_EXECUTABLE "${MAFIA_LOCATION}/a0.dta" -e "sounds\\10b_nasedani.wav" > /dev/null
assert_command "diff '$MAFIA_LOCATION/sounds/10b_nasedani.wav' ./sounds/10b_nasedani.wav"
rm -rf ./sounds

print_summary
