set yrange[*:75]
set xlabel "Contenders (n)"
set ylabel "Average throughput (Mbps)"
plot "1.txt" u 1:($2/1e6) title "Overall throughput 1" w l ls 1 lw 2, "" u 1:($2/1e6) notitle w p ls 1, "2.txt" u 1:($2/1e6) title "Overall throughput 2" w l ls 10 lw 2, "" u 1:($2/1e6) notitle w p ls 10