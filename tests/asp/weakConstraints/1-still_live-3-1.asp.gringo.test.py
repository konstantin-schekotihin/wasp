input = """
1 2 0 0
1 3 0 0
1 4 0 0
1 5 0 0
1 6 0 0
1 7 0 0
1 8 0 0
1 9 0 0
1 10 0 0
1 11 0 0
1 12 0 0
1 13 0 0
1 14 0 0
1 15 0 0
1 1 1 0 16
1 17 1 0 16
1 18 1 0 17
1 19 1 0 16
1 20 1 0 19
1 21 2 0 18 20
1 22 1 0 16
1 23 1 0 22
1 24 1 0 16
1 25 2 1 24 26
1 27 2 0 23 25
1 21 2 0 27 16
1 1 2 1 21 16
3 1 16 0 0
1 28 0 0
1 29 0 0
1 30 0 0
1 31 0 0
1 32 0 0
1 33 0 0
1 34 0 0
1 35 0 0
6 0 9 1 16 28 29 30 31 32 33 34 35 1 1 1 1 1 1 1 1 1
0
2 value(0)
5 value(1)
6 value(2)
4 size(1)
3 step(1)
7 step(-1)
8 diff(1,0)
9 diff(-1,0)
10 diff(0,1)
11 diff(0,-1)
12 diff(1,1)
13 diff(-1,1)
14 diff(1,-1)
15 diff(-1,-1)
16 lives(1,1)
21 reached(1,1)
27 reached(1,2)
0
B+
0
B-
1
0
1
"""
output = """
COST 9@1
"""