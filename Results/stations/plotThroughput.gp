#!/opt/local/bin/gnuplot
set xlabel "Station #"
set ylabel "Throughput (Mbps)"
set xrange[-1:20]
set yrange [0:2]
set xtics 0,1,19
plot "plotFile.dat" u 1:($2/1e6) title "Accum. Throughput" w p pt 1 ps 3
