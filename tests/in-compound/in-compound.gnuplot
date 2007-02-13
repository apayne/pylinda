set terminal png
set output "in-compound.png"

#set terminal latex
#set output "in-compund.tex"

#set term postscript eps enhanced
#set output "in-compund.eps"

set size 1, 1
set xrange[0:50]
set yrange[0:*]
set grid xtics
set grid ytics

set ylabel "Seconds"
set xlabel "Tuple Size (Elements)"

#set title "Time Taken To In 10,000 tuples"

plot "in-compound.txt" using 1:2 title "Traditional" with lines, \
     "in-compound.txt" using 1:3 title "Registered (Compound)" with lines, \
     "in-compound.txt" using 1:4 title "Unregistered (Compound)" with lines
