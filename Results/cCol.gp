set y2range[0:1]
set ytics nomirror
set key left
set y2tics auto
set ylabel "Total collisions"
set y2label "%col by AC"

plot "1.txt" u 1:12 notitle w l ls 1 lw 2, "" u 1:12 title "Total Collisions 1" w p ls 1, "" u 1:($14) notitle w l ls 3 lw 2 axes x1y2, "" u 1:($14) title "%col Background 1" w p ls 3 axes x1y2, "" u 1:($16) notitle w l ls 7 lw 2 axes x1y2, "" u 1:($16) title "%col Best effort 1" w p ls 7 axes x1y2, "" u 1:($18) notitle w l ls 9 lw 2 axes x1y2, "" u 1:($18) title "%col Video 1" w p ls 9 axes x1y2, "" u 1:($20) notitle w l ls 8 lw 2 axes x1y2, "" u 1:($20) title "%col Voice 1" w p ls 8 axes x1y2, "2.txt" u 1:12 notitle w l ls 10 lw 2, "" u 1:12 title "Total Collisions 2" w p ls 10, "" u 1:($14) notitle w l ls 30 lw 2 axes x1y2, "" u 1:($14) title "%col Background 2" w p ls 30 axes x1y2, "" u 1:($16) notitle w l ls 70 lw 2 axes x1y2, "" u 1:($16) title "%col Best effort 2" w p ls 70 axes x1y2, "" u 1:($18) notitle w l ls 90 lw 2 axes x1y2, "" u 1:($18) title "%col Video 2" w p ls 90 axes x1y2, "" u 1:($20) notitle w l ls 80 lw 2 axes x1y2, "" u 1:($20) title "%col Voice 2" w p ls 80 axes x1y2