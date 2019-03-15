	# This script runs LCSeg on the blogs
	# Author: Shafiq Joty
    # Date: 16-10-2010	
	# Run LCSeg with its default stopword list (used for meetings)

use warnings;
use Cwd;
my $cwd = getcwd();


mkdir "LCOutput", 0777;
	
my %th_topicnb = (1 => 14, 
		  2 => 10, 
		  3 => 10, 
		  4 => 15, 
		  5 => 14, 
		  6 => 8, 
		  7 => 9, 
		  8 => 14, 
		  9 => 10, 
		  10 => 7, 
		  11 => 11, 
		  12 => 8, 
		  13 => 9, 
		  14 => 10, 
		  15 => 9, 
		  16 => 12, 
		  17 => 9, 
		  18 => 10, 
		  19 => 13, 
		  20 => 11);

runLCSeg();

sub runLCSeg{

	opendir DIR,"../Data/Preprocessed" or die "Can't open ../Data/Preprocessed";
	my @threads = grep /\w+/, readdir(DIR);
	my $tid = 0;
	
	foreach my $thread(sort @threads){
		$tid++;
		my $topicNo = $th_topicnb{$tid};
		print "\n running LCSeg on thread", $tid, " with tNo $topicNo";
		my $output = `perl $cwd/../lcseg/bin/segment -n $topicNo $cwd/../Data/Preprocessed/$thread >LCOutput/$thread`;	
#		my $output = `perl $cwd/../lcseg/bin/segment $cwd/../Data/Preprocessed/$thread >LCOutput/$thread`;	
		print $output;
	}
	closedir DIR;	
}
