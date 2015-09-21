#set title "Collisions in the past 1000 slots. Perfect channel"
#set xrange[0:*]
#set yrange[0:*]
set y2tics auto
set ytics nomirror
set y2label "Errors in the last 1000 slots"
set ylabel "Collisions in the last 1000 slots"
plot "slotsInTime.txt" u 2:7 title "Collisions in last 1000 slots" w l ls 1 lw 2, "" u 2:9 title "Channel errors in the last 1000 slots" w l ls 3 lw 2 axes x1y2
