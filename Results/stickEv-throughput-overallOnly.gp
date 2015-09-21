#!/opt/local/bin/gnuplot -persist
set xlabel "Stickiness"
set ylabel "Avg. Aggregated Throughput (Mbps)"
set xtics 1
#set logscale y
plot "error-0-varStick.dat" u 70:($2/1e6):($3/1e6) title "ECA 0% errors" w yerrorbars ls 1 lw 2, "" u 70:($2/1e6) notitle w l ls 1 lw 2, "error-2-varStick.dat" u 70:($2/1e6):($3/1e6) title "ECA 2% errors" w yerrorbars ls 2 lw 2, "" u 70:($2/1e6) notitle w l ls 2 lw 2, "error-5-varStick.dat" u 70:($2/1e6):($3/1e6) title "ECA 5% errors" w yerrorbars ls 3 lw 2, "" u 70:($2/1e6) notitle w l ls 3 lw 2, "error-10-varStick.dat" u 70:($2/1e6):($3/1e6) title "ECA 10% errors" w yerrorbars ls 4 lw 2, "" u 70:($2/1e6) notitle w l ls 4 lw 2, "error-20-varStick.dat" u 70:($2/1e6):($3/1e6) title "ECA 20% errors" w yerrorbars ls 5 lw 2, "" u 70:($2/1e6) notitle w l ls 5 lw 2
