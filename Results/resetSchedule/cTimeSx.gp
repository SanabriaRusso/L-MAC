set xlabel "Contenders (N)"
set yrange[*:*]
set title "pc = 0.1"
set key left
set ylabel "Average time between successful transmissions (s)"
set term postscript enhanced color solid                                                                  
set out "cTimeSx-0.1.eps"
plot "EDCA-error-0.1.dat" u 1:42 title "CSMA/CA" w l ls 1 lw 2, "ECA-hystOnly-0.1.dat" u 1:42 title "CSMA/ECA+Hyst" w l ls 3 lw 2, "ECA-hystOnly-halving-0.1.dat" u 1:42 title "CSMA/ECA+Hyst resetting" w l ls 4 lw 2, "ECA-0.1.dat" u 1:42 title "CSMA/ECA" w l ls 8 lw 2, "ECA-halving-0.1.dat" u 1:42 title "CSMA/ECA resetting" w l ls 7 lw 2