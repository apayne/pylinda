set terminal png
set output "out-memory.png"

#set terminal latex
#set output "out-memory.tex"

#set term postscript eps enhanced
#set output "out-memory.eps"

set size 1, 1
set xrange[0:50]
set yrange[0:*]
set grid xtics
set grid ytics

set ylabel "Kilobytes"
set xlabel "Tuple Size (Elements)"

#set title "Maximum Memory Used To Output 1,000 Tuples"

plot "out-memory.txt" using 1:2 title "Traditional" with lines, \
     "out-memory.txt" using 1:3 title "Registered" with lines, \
     "out-memory.txt" using 1:5 title "Registered (Compound)" with lines, \
     "out-memory.txt" using 1:4 title "Unregistered" with lines, \
     "out-memory.txt" using 1:6 title "Unregistered (Compound)" with lines
