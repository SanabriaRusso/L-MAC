set style line 1 lc rgb "red"
set style line 2 lc rgb "blue"
set style line 3 lc rgb "green"
set style line 4 lc rgb "purple"

set boxwidth 0.5
set key left
set style fill solid
set style histogram errorbars gap 1 lw 1
set yrange[*:*]
set xrange[*:16]
set xlabel "Protocol"
set ylabel "Average number of dropped packets"
set title "Thirty two nodes, 4 AC and in saturation"

plot "good/thirtyTwoNodes_dropped.dat" every ::1::3 using 1:3:xtic(2) title "Background" with boxes ls 1, "" every ::5::7 using 1:3:xtic(2) title "Best-effort" with boxes ls 2, "" every ::9::11 using 1:3:xtic(2) title "Video" with boxes ls 3, "" every ::13::15 using 1:3:4:xtic(2) title "Voice" with boxerrorbars ls 4