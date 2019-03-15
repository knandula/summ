#!/usr/bin/perl

#####################################################
# Baseline segmenter: a text is divided
# into N equal-size segments.
#
# Created by Michel Galley (galley@cs.columbia.edu)
# on Tue Mar 21 11:46:06 2006
# $Id: baseline.pl 43 2006-11-14 00:22:30Z galley $
#####################################################

use strict;
use Fatal qw(open close);
use utils::opt qw(&get_args &get_opts);
use POSIX;

my %opts = get_opts(['n=s',10,'put a boundary every n sentences.']);
my $n = $opts{n};

my @lines = <STDIN>;
my $n2 = ceil(($#lines+1)/$n);
foreach my $i (0..$#lines) {
	print "==========\n" if($i % $n2 == 0);
	print $lines[$i];
}
print "==========\n";
