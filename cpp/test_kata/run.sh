#!/usr/bin/env bash
: ${WHITE:=./orig_katago.sh}
: ${BLACK:=./mmcts_katago.sh}
: ${NUM_GAMES:=10}

NOW=$(date +"%T")
gogui-twogtp -black $BLACK -white $WHITE -verbose -threads 10 -alternate -games ${NUM_GAMES} -auto -sgffile result-${NOW}
gogui-twogtp -analyze result-${NOW}.dat
