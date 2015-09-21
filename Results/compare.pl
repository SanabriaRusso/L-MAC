#!/bin/perl
use warnings;
use strict;

my $file1 = $ARGV[0];
my $file2 = $ARGV[1];

my @command1 = "cp $file1 1\.txt";
my @command2 = "cp $file2 2\.txt";
system(@command1);
system(@command2);

my $cThroughput = 'cThroughput.gp';
my $cThroughputSingle = 'cThroughput-singleAC.gp';
my $cCol = 'cCol.gp';
my $cColSingle = 'cCol-singleAC.gp';
my $cDrop = 'cDropped.gp';
my $cTimeBetSxTx = 'cTimeSx.gp';

my @command3 = "gnuplot $cThroughput";
system(@command3);
@command3 = "gnuplot $cThroughputSingle";
system(@command3);
my @command4 = "gnuplot $cCol";
system(@command4);
@command4 = "gnuplot $cColSingle";
system(@command4);
my @command5 = "gnuplot $cDrop";
system(@command5);
my @command6 = "gnuplot $cTimeBetSxTx";
system(@command6);

my @eraseAll = 'rm 1.txt 2.txt';
system(@eraseAll);
