start :- connect(), loop;

loop :- out(uts, tuple('ping')), in(uts, tuple('pong')), write('ping'), nl;
