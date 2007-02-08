set terminal png
set output "latency.png"

#set terminal latex
#set output "interserver-all-three.tex"

set nokey
set xrange[0:230]
set yrange[0:*]
set grid xtics
set grid ytics

set style data boxes
set boxwidth 1
set style fill solid border -1

set ylabel "Count"
set xlabel "Milli-seconds"

set title "Latency Of Amazon's EC2 Network"

plot "latency.txt" using 1:2
