#!/opt/local/bin/gnuplot -persist
set xlabel "Stickiness"
set ylabel "Throughput (Mbps)"
set xtics 1
plot "processed_varStickiness.txt" u 70:($2/1e6):($3/1e6) title "Overall throughput" w yerrorbars ls 1 lw 2, "" u 70:($2/1e6) notitle w l ls 1 lw 2, "" u 70:($4/1e6):($5/1e6) title "Background" w yerrorbars ls 2 lw 2, "" u 70:($4/1e6) notitle w l ls 2 lw 2, "" u 70:($6/1e6):($7/1e6) title "Best effort" w yerrorbars ls 3 lw 2,"" u 70:($6/1e6) notitle w l ls 3 lw 2, "" u 70:($8/1e6):($9/1e6) title "Video" w yerrorbars ls 4 lw 2, "" u 70:($8/1e6) notitle w l ls 4 lw 2, "" u 70:($10/1e6):($11/1e6) title "Voice" w yerrorbars ls 5 lw 2, "" u 70:($10/1e6) notitle w l ls 5 lw 2
