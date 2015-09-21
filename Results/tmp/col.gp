set yrange[0:1]
set xlabel "Contenders (N)"
set ylabel "Frac. of collision slots"
set ytics 0,0.1,1
#set term postscript enhanced color solid
#set out "col.eps"
plot ECADynAgg u 1:14:15 title "CSMA dynamic aggressive" w yerrorbars ls 1 lw 2, "" u 1:14 notitle w l ls 1 lw 2, basicECA u 1:14:15 title "CSMA/ECA Basic" w yerrorbars ls 2 lw 2, "" u 1:14 notitle w l ls 2 lw 2, DCF u 1:14:15 title "DCF" w yerrorbars ls 3 lw 2, "" u 1:14 notitle w l ls 3 lw 2
#, ECADyn u 1:($14):($15) title "CSMA/ECA dynamic" w yerrorbars ls 4 lw 2, "" u 1:($14)  notitle w l ls 4 lw 2,ECADynAgg u 1:($14):($15) title "CSMA/ECA dynamic aggressive" w yerrorbars ls 5 lw 2, "" u 1:($14) notitle w l ls 5 lw 2
