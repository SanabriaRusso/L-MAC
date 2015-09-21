set term aqua enhanced
set logscale y
plot "processed.txt" u 1:58 title "Total Dropped" w l ls 1 lw 2, "" u 1:60 title "dropped by Background" w l ls 2 lw 2, "" u 1:62 title "dropped by Best effort" w l ls 3 lw 2, "" u 1:64 title "dropped by Video" w l ls 4 lw 2, "" u 1:66 title "dropped by Voice" w l ls 8 lw 2
#    EOF
