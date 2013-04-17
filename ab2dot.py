#!/usr/bin/python
import sys

def analyze(l):
	l=' '.join(l.strip().split(' ')[1:])[:-1]
	elems=[x.strip() for x in l.split('"')]
	frm=[]
	to=''
	for n in range(len(elems)):
		if elems[n]=='from=':
			frm=elems[n+1].split(' ')
		if elems[n]=='id=':
			to=elems[n+1]

	for f in frm:
		print ' "%s" -> "%s";'%(f, to)


print 'digraph d{'
for i in sys.stdin.readlines():
	if 'from' in i:
		analyze(i)
print '}'

