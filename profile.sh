#!/bin/sh

EXPERIMENT="exp/benchmark/TwainBench/exp14-1"

perf stat -e cycles,instructions,cache-misses \
  build/Release/bin/rematch \
  -d $EXPERIMENT/doc.txt \
  -r $EXPERIMENT/rematch.rgx \
  -l > /dev/null


perf record \
  -e cycles \
  -g \
  --call-graph fp \
  -- \
  build/Release/bin/rematch \
  -d $EXPERIMENT/doc.txt \
  -r $EXPERIMENT/rematch.rgx \
  -l > /dev/null

perf script | c++filt | gprof2dot -f perf | dot -Tpng -o output_cycles.png

perf record \
  -e instructions \
  -g \
  --call-graph fp \
  -- \
  build/Release/bin/rematch \
  -d $EXPERIMENT/doc.txt \
  -r $EXPERIMENT/rematch.rgx \
  -l > /dev/null

perf script | c++filt | gprof2dot -f perf | dot -Tpng -o output_instructions.png

perf record \
  -e cache-misses \
  -g \
  --call-graph fp \
  -- \
  build/Release/bin/rematch \
  -d $EXPERIMENT/doc.txt \
  -r $EXPERIMENT/rematch.rgx \
  -l > /dev/null

perf script | c++filt | gprof2dot -f perf | dot -Tpng -o output_cache-misses.png



# perf report -g 'graph,0.5,caller'
