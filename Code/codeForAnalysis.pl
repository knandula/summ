	# This script is to help analyzing the output of LCSeg
	# Author: Shafiq Joty
        # Date: 24-09-2009	

use warnings;
use Cwd;
my $cwd = getcwd();

mkdir "../OutputForAnalysis", 0777;
	
#tag the output with the issue id annotated by me
tagOutput();
	
sub tagOutput{

	opendir DIR,"../Data/Raw/BC3" or die "Can't open ../Data/Raw/BC3";
	my @threads = grep /\w+/, readdir(DIR);

	foreach my $thread(sort @threads){

		print "\n Processing thread $thread";

		#open the files to read and write
		open INFILE1,"../Data/Raw/BC3/$thread" or die "Can't open file: /Data/Raw/BC3/$thread : $! \n";

		open INFILE2,"../Output/$thread" or die "Can't open file: ../Output/$thread : $! \n";

		open OUTFILE,">../OutputForAnalysis/$thread" or die "Can't open file: ../OutputForAnalysis/$thread : $! \n";

		#Process
		while(<INFILE1>){
			if ($_ =~ m[<Text>]){
				while(<INFILE1>){
					my $line1 = $_;
						
					if($line1 =~ m{</Text>}){
						last ;
			   		}

					$line1 =~ s/\s*<Sent id="[\d.]+">\s*//g;
					my $issueId = 0;
					if ($line1 =~ m/\s*<Issue id\s*=\s*([\d]+)>\s*/){
						$issueId = $1;
						$line1 =~ s/\s*<Issue id\s*=\s*[\d]+>\s*//gi;				
#						print "\nIssue id: $issueId";
					}

					$line1 =~ s|\s*</Sent>\s*||gi;
					$line1 =~ s/\s*$//;
					
					if ($line1 =~ m/\w+/ && !($line1 =~ m/^&gt;|^&amp;gt;/)){
						$line1 =~ s/&amp;quot;//gi;
						my $line2 = <INFILE2>;
						chomp($line2);
						if ($line2 =~ m/=========/){
					  		print OUTFILE $line2, "\n";
							$line2 = <INFILE2>;
							chomp($line2);
						}

						print "\n line1 read: $line1";
						print "\n line2 read: $line2";
						#my $wt = <STDIN>;

				  		print OUTFILE $line2,"<Issue id = $issueId>", "\n";

					}
					
				} #end of while
			} #end of if

		} #end of while

		close (INFILE1);
		close (INFILE2);
		close (OUTFILE);
	}
	closedir DIR;
}
