	# This script is to help analyzing the output of LCSeg
	# Author: Shafiq Joty
    # Date: 24-09-2009	


use warnings;
use Cwd;
my $cwd = getcwd();

mkdir "SysLCSegTopic", 0777;
	
writeOutput();

rmdir ("Dummy");

	
sub writeOutput{

	opendir DIR,"LCOutput" or die "Can't open ..LCOutput";
	my @threads = grep /\w+/, readdir(DIR);
	closedir DIR;

	foreach my $thread(sort @threads){

		print "\n Processing thread $thread";
		#open the files to read and write
		my $thr = "$thread"."dummy";
		open INFILE1,"Dummy/$thr" or die "Can't open file: Dummy/$thr : $! \n";
		open INFILE2,"LCOutput/$thread" or die "Can't open file: ../LCOutput/$thread : $! \n";
		open OUTFILE,">SYSLCSegTopic/$thread" or die "Can't open file: LCSegTopic/$thread : $! \n";

		my $topId = 0;
		my $senId = 0;
		my $emailId = 1;
		my @name = ();

		while(<INFILE2>){
			my $line1 = $_;

			if ($line1 =~ m/=========/){
				$topId++;
				next;
			}
		
			#read dummy sen
			my $dSen = "";			

			while (<INFILE1>){
				$dSen = $_;			
				if($dSen =~ m/_EOE_/){
					@name = ($dSen =~ m/_EOE_([^\s]+)\s/);
					$emailId++;
				}
				if ($dSen eq $line1){
					last;
				}	
			}
			chomp($line1);
			$senId++ ;
		  	print OUTFILE "T", $topId, " ",$senId," ", $name[0], " : ", $line1, "\n";  
		}
		close (INFILE1);
		close (INFILE2);
		close (OUTFILE);
		unlink ("Dummy/$thr");
	}
}
