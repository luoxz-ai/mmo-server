#!/bin/bash
cd `dirname $0`
DIR_file=`pwd`

${DIR_file}/start_global_route.sh
${DIR_file}/start_shared_zone.sh
