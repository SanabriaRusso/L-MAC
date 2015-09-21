set xlabel "Contenders (N)"
set ylabel "Throughput (Mbps)"
set yrange [20:34]
plot "processed_toPlot.txt" u 1:($2/1e6):($3/1e6) title "Acumm. Throughput" w yerrorbars ls 1 lw 2, "" u 1:($2/1e6):($3/1e6) notitle w l ls 1 lw 2