#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

export ASAN_OPTIONS=include_if_exists=${DIR_file}/options_asan
export LSAN_OPTIONS=include_if_exists=${DIR_file}/options_lsan
export UBSAN_OPTIONS=include_if_exists=${DIR_file}/options_ubsan
export TSAN_OPTIONS=include_if_exists=${DIR_file}/options_tsan

${DIR_file}/binary/shared_scene_1 --worldid=0 --stop=SCENE_SERVICE-51,AI_SERVICE-51 --logpath=/data/log/global/shared_scene_1
${DIR_file}/binary/shared_scene_2 --worldid=0 --stop=SCENE_SERVICE-52,AI_SERVICE-52 --logpath=/data/log/global/shared_scene_2
${DIR_file}/binary/shared_scene_3 --worldid=0 --stop=SCENE_SERVICE-53,AI_SERVICE-53 --logpath=/data/log/global/shared_scene_3
${DIR_file}/binary/shared_scene_4 --worldid=0 --stop=SCENE_SERVICE-54,AI_SERVICE-54 --logpath=/data/log/global/shared_scene_4
${DIR_file}/binary/shared_scene_5 --worldid=0 --stop=SCENE_SERVICE-55,AI_SERVICE-55 --logpath=/data/log/global/shared_scene_5