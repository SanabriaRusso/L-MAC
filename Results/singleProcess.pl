#!/usr/bin/perl
use warnings;
use strict;

sub fix ($$);
 
my %help = (
                'help'        => 1,
                '-h'        => 1,
                '--help'    => 1
            );

die ("******HELP! message\nExecute with ARGVs: 1. processSingleAmountOfNodes.txt 2. columnNumber\n\n*CheatSheet:\n\t#42 TimeBetSxTx\n") 
    if (exists $help{$ARGV[0]});

my @inputData;
my $outputFile = 'single.txt';
my $filename = shift;
my $firstAC = shift;

open(my $fh, "<", $filename)
    or die "Could not open file '$filename' $!";

while (my $row = <$fh>)
{
    if($row !~ /^#/)
    {
        chomp($row);
        @inputData = split(/\s+/, $row);
    }

}

####Finding the metrics and outputting the results to file.
open(my $fw, ">", $outputFile)
    or die "Could not open write file $outputFile $!";

print $fw "#1. BE, 2. BK, 3. VI, 4 VO\n";

my $index = 0;
$firstAC = $firstAC - 1;
my $lastAC = $firstAC + 7; #counting the spaces of the std from inpunt file

my @sequence = fix($firstAC, $lastAC);

foreach (@sequence)
{
    print $fw "$inputData[$_] ";
}
print $fw "\n";

####SUBS
sub fix($$){
    my ($first, $last) = (shift, shift);
    my @seq = ($first .. $last);

    #Here i avoid counting the std columns
    foreach my $i (0 .. $#seq)
    {
        $seq[$i] = 0
            if(($i % 2) != 0);
    }
    @seq = grep {$_} @seq; #everytime $_ is true (neq 0) it stores $seq[$_]
    return @seq;
}
