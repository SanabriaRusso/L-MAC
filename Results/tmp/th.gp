set xlabel "Contenders (N)"
set ylabel "Throughput (Mbps)"
#set term postscript enhanced color solid
#set out "th .eps"
set yrange [18:34]
set ytics 18,1,34
plot ECADynAgg u 1:($2/1e6):($3/1e6) title "CSMA/ECA dynamic aggressive" w yerrorbars ls 1 lw 2, "" u 1:($2/1e6):($3/1e6) notitle w l ls 1 lw 2, basicECA u 1:($2/1e6):($3/1e6) title "CSMA/ECA Basic" w yerrorbars ls 2 lw 2, "" u 1:($2/1e6):($3/1e6) notitle w l ls 2 lw 2, DCF u 1:($2/1e6):($3/1e6) title "DCF" w yerrorbars ls 3 lw 2, "" u 1:($2/1e6):($3/1e6) notitle w l ls 3 lw 2
#, ECA u 1:($2/1e6):($3/1e6) title "CSMA/ECA" w yerrorbars ls 2 lw 2, "" u 1:($2/1e6):($3/1e6) notitle w l ls 2 lw 2, ECAagg u 1:($2/1e6):($3/1e6) title "CSMA/ECA aggressive" w yerrorbars ls 3 lw 2, "" u 1:($2/1e6):($3/1e6) notitle w l ls 3 lw 2, ECADyn u 1:($2/1e6):($3/1e6) title "CSMA/ECA dynamic" w yerrorbars ls 4 lw 2, "" u 1:($2/1e6):($3/1e6) notitle w l ls 4 lw 2,ECADynAgg u 1:($2/1e6):($3/1e6) title "CSMA/ECA dynamic aggressive" w yerrorbars ls 5 lw 2, "" u 1:($2/1e6):($3/1e6) notitle w l ls 5 lw 2
