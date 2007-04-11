set terminal png
set output "in-network.png"

#set terminal latex
#set output "in-network.tex"

#set term postscript eps enhanced
#set output "in-network.eps"

set size 1, 1
set xrange[0:50]
set yrange[0:*]
set grid xtics
set grid ytics

set ylabel "Kilobytes"
set xlabel "Tuple Size (Elements)"

#set title "Network Usage To Retreive 1,000 Tuples"

plot "in-network.txt" using 1:2 title "Traditional" with lines, \
     "in-network.txt" using 1:3 title "Registered" with lines, \
     "in-network.txt" using 1:4 title "Registered (Compound)" with lines, \
     "in-network.txt" using 1:5 title "Unregistered" with lines
