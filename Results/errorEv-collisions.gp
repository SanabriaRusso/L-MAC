set xlabel "Channel errors (%)"
set ylabel "Overall collisions"
set ytics nomirror
set y2tics auto
set y2label "%collisions"
set y2range[0:1]
set yrange[*:2e5]

plot "processed_varError.txt" u 68:($12):($13) title "Overall collisions" w yerrorbars ls 1 lw 2 axis x1y1, "" u 68:($12) notitle w l ls 1 lw 2 axis x1y1, "" u 68:($14):($15) title "%col Background" w yerrorbars ls 2 lw 2 axis x1y2, "" u 68:($14) notitle w l ls 2 lw 2 axis x1y2, "" u 68:($16):($17) title "%col Best effort" w yerrorbars ls 3 lw 2 axis x1y2, "" u 68:($16) notitle w l ls 3 lw 2 axis x1y2, "" u 68:($18):($19) title "%col Video" w yerrorbars ls 4 lw 2 axis x1y2, "" u 68:($18) notitle w l ls 4 lw 2 axis x1y2, "" u 68:($20):($21) title "%col Voice" w yerrorbars ls 5 lw 2 axis x1y2, "" u 68:($20) notitle w l ls 5 lw 2 axis x1y2