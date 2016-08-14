#!/usr/bin/env python
# import sys
import os

from decimal import *

# getcontext()
os.system('clear')

class Well:
	root = "/home/cutie/Documents/git/FieldOptBenchmarks/WellIndexBenchmark/WellIndexBenchmark/wells/workflow/"
	fnameIn = "TW01.txt"
	name = ""
	date = "01/11/2016"
	xyz = Decimal()
	md = Decimal()
	incl = Decimal()
	az = Decimal()

class Completions:
	fnameOut = ""
	date = "dd/MM/yyyy"
	headers = ["[header]", "[datadef]", "[data]"]
	lbreak = "# " + "-" * 40 
	datadef = [
	"time stamp            : QDateTime",
	"top md                : double",
	"bottom md             : double",
	"event type            : int",
	"well                  : QString",
	"trajectory            : int",
	"skin factor           : double",
	"external diameter     : double"]

# make well object
well = Well();
comp = Completions();

# run RMS in batch mode
# bash /opt/roxar/rms/rms_2013.0.3_el5 -project \
# rms_wi_benchmark.pro -batch WI_CALC_BENCHMARK -readonly

# read root info from file (not working)
# fo = open("ROOT.dir")
# rootdir = fo.readline()
# fo.close()
# if not well.root:
# 	well.root = rootdir

well.fnameIn = well.root + well.fnameIn
print ("Root: %s" % well.root)

# print input file
print ("Input file (file to be read): %s" % well.fnameIn)

# read input file
fo = open(well.fnameIn)
str = fo.readlines()
fo.close()

# save well name
well.name = str[0].split()
print ("name of well: %s\n" % well.name[0])

line1 = [Decimal(i) for i in str[2].split()]
line2 = [Decimal(i) for i in str[3].split()]
print("line1: %s" % line1)
print("line2: %s\n\n" % line2)

# save well data
print ("assemble well data:")
well.xyz = line1[0:3] + line2[0:3]
well.md = [line1[3], line2[3]]
well.incl = [line1[4], line2[4]]
well.az = [line1[5], line2[5]]
print ("well xyz: %s", well.xyz)
print ("well md: %s", well.md)
print ("well incl: %s", well.incl)
print ("well incl: %s", well.az)
well.comp="PERF"
well.traj="DRILLED"
well.skin=0.00
well.diam=0.1905

# make name of output file
comp.fnameOut = well.root + "events_" + well.name[0] + ".rmscompletions"
print ("\nOutput file (file to be writen): %s\n" % comp.fnameOut)
fo = open(comp.fnameOut, 'w')

# print completion file (header)
print ("%s\n%s\n%s\n" % 
	(comp.lbreak, comp.headers[0], comp.lbreak), file=fo)
print ("%s\n" % comp.date, file=fo)

# print completion file (dataref)
print ("%s\n%s\n%s\n" % 
	(comp.lbreak, comp.headers[1], comp.lbreak), file=fo)
[print("%s" % i, file=fo) for i in comp.datadef]

# print completion file (data)
print ("\n%s\n%s\n%s" % 
	(comp.lbreak, comp.headers[2], comp.lbreak), file=fo)
print ("%s, %s, %s, %s, %s, %s, %4.2f, %6.4f\n" % 
	(well.date, well.md[0], well.md[1], 
	well.comp, well.name[0], well.traj, 
	well.skin, well.diam), file=fo)

fo.close()

