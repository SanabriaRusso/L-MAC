#!/bin/perl
use warnings;
use strict;
use Data::Dumper qw(Dumper);
use Statistics::Basic qw(:all);
 
my @inputData;
my $highNodes = 0;
my $lowNodes = 10000;
my $outputFile = 'Results/processed_toPlot.txt';

####Finding the nodes limits based on the file.
my $filename = $ARGV[0];

open(my $fh, "<", $filename)
    or die "Could not open file '$filename' $!";

while (my $row = <$fh>)
{
    if($row !~ /^#/)
    {
        chomp($row);
        @inputData = split(/\s+/, $row);
        $highNodes = $inputData[0]
            if ($highNodes < $inputData[0]);
        $lowNodes = $inputData[0]
            if ($lowNodes > $inputData[0]);
    }

}

####Finding the metrics and outputting the results to file.
open(my $fw, ">", $outputFile)
    or die "Could not open write file $outputFile $!";

##Odd indexes are standard deviations if not defined
print $fw 
("#1 Nodes,                 #2 AvgThroughput,               #4 totalCol, 
  #6 fractOfCol             #8 AvgTimeBetSxTx\n");

OUTTER: foreach($lowNodes .. $highNodes)
{
    open(my $fh, "<", $filename)
        or die "Could not open file $filename $!";

    my @metrics;
    my $thereIsData = 0;

    while (my $row = <$fh>)
    {
        if($row !~ /^#/)
        {
            chomp($row);
            @inputData = split(/\s+/, $row);
            if ($inputData[0] == $_)
            {
                $thereIsData = 1;
                foreach my $i (1 .. $#inputData)
                {
                    push @{$metrics[$i]}, $inputData[$i];    
                }
                
            }
        }
    }

    next OUTTER
        if($thereIsData == 0);

    my $average;
    my $std;
    print $fw "$_ ";
    foreach my $i (1 .. $#metrics)
    {
        $average = avg(@{$metrics[$i]}) + 0; #forcing the result to a scalar instead of an Object.
        $std = stddev(@{$metrics[$i]}) + 0;
        print $fw "$average $std ";
    }
    print $fw "\n";
}