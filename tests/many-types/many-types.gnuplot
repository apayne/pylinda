set terminal png
set output "many-types.png"

#set terminal latex
#set output "many-types.tex"

#set term postscript eps enhanced
#set output "many-types.eps"

set size 1, 1
set xrange[0:50]
set yrange[0:*]
set grid xtics
set grid ytics

set ylabel "Seconds"
set xlabel "Tuple Size (Elements)"

#set title "Time Taken To In 10,000 tuples"

plot "many-types.txt" using 1:2 title "Product Types" with lines, \
     "many-types.txt" using 1:3 title "Sum Types" with lines