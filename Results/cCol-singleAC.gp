set ytics nomirror
set key left
set ylabel "Total collisions"

plot "1.txt" u 1:12 title "Total Collisions 1" w l ls 1 lw 2, "" u 1:12 notitle w p ls 1, "2.txt" u 1:12 title "Total Collisions 2" w l ls 10 lw 2, "" u 1:12 notitle w p ls 10