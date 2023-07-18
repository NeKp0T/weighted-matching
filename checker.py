import sys

testname = sys.argv[1]

testout = open("./tests/" + testname + ".out")
correct = int(testout.readline().strip())
testout.close()

testin = open("./tests/" + testname + ".in")
lines = testin.readlines()
n = int(lines[0])

e = [set() for i in range(n + 1)]

edgeslist = list(map(lambda l: list(map(int, l.strip().split())), lines[1:]))
for [x, y] in edgeslist:
    e[x].add(y)
    e[y].add(x)


count = int(input())
if (count % 2 == 1):
    print("wtf, " + str(count) + " is odd")
    exit(1)

if (count != correct):
    print("correct: {}, but was: {}".format(correct, count // 2))
    exit(2)

used = set()
for i in range(count // 2):
    a, b = list(map(int, input().split()))
    if a == b or a in used or b in used:
        print("used {} or {}".format(a, b))
        exit(3)
    
    if a not in e[b] or b not in e[a]:
        print("Edge does not exist: {} {}".format(a, b))
        exit(4)
