set terminal png
set output "sudoku-traditional.png"

#set terminal latex
#set output "interserver-all-three.tex"

set nokey
set xrange[0:5]
set yrange[0:*]
set grid xtics
set grid ytics

set ylabel "Seconds"
set xlabel "Slave Processes"

set title "Time Taken To Solve Sudoku Grid"

plot "sudoku-traditional.txt" using 1:2
