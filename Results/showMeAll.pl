#!/usr/bin/perl
use warnings;
use strict;

my $file1 = $ARGV[0];
my $file2 = $ARGV[1];

my @command1 = "cp $file1 processed\.txt";
system(@command1);
#optional
@command1 = "cp $file2 slots\.txt";
system(@command1);

my $throughput = 'test.gp';
my $col = 'col.gp';
my $fairness = 'fair.gp';
my $timeBST = 'timeSx.gp';
my $dropped = 'dropped.gp';
#optional
my $slots = 'slots.gp';


my @command3 = "gnuplot $throughput";
system(@command3);

my @command4 = "gnuplot $col";
system(@command4);

my @command5 = "gnuplot $fairness";
system(@command5);

my @command6 = "gnuplot $timeBST";
system(@command6);

my @command8 = "gnuplot $dropped";
system(@command8);

#optional
my @command7 = "gnuplot $slots";
system(@command7);

my @eraseAll = 'rm processed.txt slots.txt';
system(@eraseAll);