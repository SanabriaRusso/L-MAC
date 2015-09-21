set style line 1 lc rgb "red"
set style line 2 lc rgb "blue"
set style line 3 lc rgb "green"
set style line 4 lc rgb "purple"

set boxwidth 0.5
set style fill solid
set yrange[*:*]
set xlabel "Protocol"
set ylabel "Average time between successful transmissions (s)"
set title "Sixteen nodes, 4 AC and in saturation"

plot "good/sixteenNodes.dat" every ::0::2 using 1:3:xtic(2) title "Background" with boxes ls 1, "" every ::3::5 using 1:3:xtic(2) title "Best-effort" with boxes ls 2, "" every ::6::8 using 1:3:xtic(2) title "Video" with boxes ls 3, "" every ::9::11 using 1:3:xtic(2) title "Voice" with boxes ls 4