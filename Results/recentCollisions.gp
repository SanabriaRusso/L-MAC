#set title "Collisions in the past 1000 slots. Perfect channel"
#set xrange[0:*]
#set yrange[0:*]
#set y2tics auto
set ytics nomirror
#set y2label "Errors in the last 1000 slots"
set ylabel "Collisions in the last 1000 slots"
plot "ECA_16sta_slots.dat" u 2:7 title "CSMA/ECA_{QoS}" w l ls 1 lw 2, "EDCA_16sta_slots.dat" u 2:7 title "EDCA" w l ls 2 lw 2
