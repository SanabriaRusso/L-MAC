set yrange[*:75]
set xlabel "Contenders (n)"
set ylabel "Average throughput (Mbps)"
plot "1.txt" u 1:($2/1e6) notitle w l ls 1 lw 2, "" u 1:($2/1e6) title "Overall throughput 1" w p ls 1, "" u 1:($4/1e6) notitle w l ls 3 lw 2, "" u 1:($4/1e6) title "Background 1" w p ls 3, "" u 1:($6/1e6) notitle w l ls 7 lw 2, "" u 1:($6/1e6) title "Best effort 1" w p ls 7, "" u 1:($8/1e6) notitle w l ls 9 lw 2, "" u 1:($8/1e6) title "Video 1" w p ls 9, "" u 1:($10/1e6) notitle w l ls 8 lw 2, "" u 1:($10/1e6) title "Voice 1" w p ls 8, "2.txt" u 1:($2/1e6) notitle w l ls 10 lw 2, "" u 1:($2/1e6) title "Overall throughput 2" w p ls 10, "" u 1:($4/1e6) notitle w l ls 30 lw 2, "" u 1:($4/1e6) title "Background 2" w p ls 30, "" u 1:($6/1e6) notitle w l ls 70 lw 2, "" u 1:($6/1e6) title "Best effort 2" w p ls 70, "" u 1:($8/1e6) notitle w l ls 90 lw 2, "" u 1:($8/1e6) title "Video 2" w p ls 90, "" u 1:($10/1e6) notitle w l ls 80 lw 2, "" u 1:($10/1e6) title "Voice 2" w p ls 80