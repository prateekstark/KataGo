#!/usr/bin/env bash
: ${WHITE:=./orig_katago.sh}
: ${BLACK:=./mmcts_katago.sh}
: ${NUM_GAMES:=64}

NOW=$(date +"%T")
gogui-twogtp -black $BLACK -white $WHITE -verbose -threads 8 -games ${NUM_GAMES} -auto -sgffile result-${NOW}
gogui-twogtp -analyze result-${NOW}.dat
