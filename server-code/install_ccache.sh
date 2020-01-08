sudo apt-get install ccache
export CCACHE_SLOPPINESS=pch_defines,time_macros
ccache -M 20G

