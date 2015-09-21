#!/usr/bin/perl
use warnings;
use strict;

my $file1 = $ARGV[0];
my $file2 = $ARGV[1];
my $file3 = $ARGV[2];

my @command1 = "cp $file1 1\.txt";
my @command2 = "cp $file2 2\.txt";
# my @command3 = "cp $file2 3\.txt";
# my @command4 = "cp $file2 4\.txt";
system(@command1);
system(@command2);
# system(@command3);
# system(@command4);

my $cColSlots = 'cColSlots.gp';
my @command5 = "gnuplot $cColSlots";
system(@command5);


# my @eraseAll = 'rm 1.txt 2.txt 3.txt 4.txt';
my @eraseAll = 'rm 1.txt 2.txt';
system(@eraseAll);