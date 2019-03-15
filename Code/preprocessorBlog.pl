				# The module extracts the features
				# Author: Shafiq Joty
				# Date: Aug 13, 2010

$| = 1;  #put autoflush on

use warnings;
use Cwd;
my $cwd = getcwd();

use Article;
use Comment;

mkdir "../Data/Preprocessed", 0777;
mkdir "Dummy", 0777;

my $quoteCount = 0;

readArticles();

print "\n avg quotation: ", $quoteCount / 20;


sub readArticles{

	#read data
	opendir DIR,"../Data/Raw" or die "Can't open ../Data/Raw";
	my @files = grep /\w+/, readdir(DIR);
	closedir DIR;
	#--------

	foreach my $file(sort @files){

		print "\n Processing article $file";
		open INFILE,"../Data/Raw/$file" or die "Can't open file: ../Data/Raw/$file : $! \n";

		my $article;
		my %comments = ();

		while(<INFILE>){
		
			# Process article
			if ($_ =~ m|<article>|i){

				my ($artId, $artTitle, $artAuthor, $artDate);
				
				my @htmlsentences = (); 
				my @tokensentences = (); 
				my @segsentences = ();

				while(1){

					$_ = <INFILE>;					
					
					if (m|</article>|i){
						last;
					}
					
					if (m|<id>http://www.semanticweb.org/ontologies/ConversationInstances.owl\#(\w+)</id>|i){
						$artId = $1;
					}
					
					if (m|<title>(.*)</title>|i){
						$artTitle = $1;
					}
					
					if (m|<author>(.*)</author>|i){
						$artAuthor = $1;
						$artAuthor =~ s/ /_/gi;
					}

					if (m|<datestamp>(.*)</datestamp>|i){
						$artDate = $1;
					}
					
					if (m|<htmltext>|i){
						my $ln = $_;
						chomp($ln);
						$ln =~ s|</?htmltext>||ig;
						push @htmlsentences, $ln if ($ln =~ m|\w+|);
						
						if (m|</htmltext>|){
							next;
						}

						while(1){
							my $ln = <INFILE>;
							chomp($ln);	
							if (!($ln =~ m|</htmltext>|i)){
								push @htmlsentences, $ln if ($ln =~ m/\w+/);
							}
							
							else{
								$ln =~ s|</htmltext>||i;
								push @htmlsentences, $ln if ($ln =~ m/\w+/);
								last;
							}
						}
					}
					
					
					if (m|<tokenext>|i){
						my $ln = $_;
						chomp($ln);
						$ln =~ s|</?tokent?ext>||i;
						push @tokensentences, $ln if ($ln =~ m|\w+|);

						if (m|</tokentext>|){
							next;
						}						
						while(1){
							my $ln = <INFILE>;
							chomp($ln);
							if (!($ln =~ m|</tokentext>|i)){
								push @tokensentences, $ln if ($ln =~ m/\w+/);
							}
							else{
								$ln =~ s|</tokentext>||ig;
								push @tokensentences, $ln if ($ln =~ m/\w+/);
								last;
							}
						}
					}
					
					if (m|<sentencetext>|i){
						my $ln = $_;
						chomp($ln);
						$ln =~ s|</?sentencetext>||i;
						push @segsentences, $ln if ($ln =~ m|\w+|);
						if (m|</sentencetext>|){
							next;
						}
						while(1){
							my $ln = <INFILE>;
							chomp($ln);
							if (!($ln =~ m|</sentencetext>|i)){								
							 push @segsentences, $ln if ($ln =~ m/\w+/);
							}
							else{
								$ln =~ s|</sentencetext>||ig;
								push @segsentences, $ln if ($ln =~ m/\w+/);
								last;
							}
						}
					}				
				}
				$article = new Article($artId, $artTitle, $artAuthor, $artDate, \@htmlsentences, \@tokensentences, \@segsentences);
			}			
				
			if ($_ =~ m|<comment>|i){

				my ($cId, $cTitle, $cAuthor, $cDate, $modClass, $modScore);
				my $parent = "";

				my @htmlsentences = (); 
				my @tokensentences = (); 
				my @segsentences = ();
				my @quotes = ();	

				while(1){

					$_ = <INFILE>;					
				
					if (m|</comment>|i){
						last;
					}
					
					if (m|<id>http://www.semanticweb.org/ontologies/ConversationInstances.owl\#\w+.(\d+)</id>|i){
						$cId = $1;
					}
					
					if (m|<title>(.*)</title>|i){
						$cTitle = $1;
					}
					
					if (m|<author>(.*)</author>|i){
						$cAuthor = $1;
						$cAuthor =~ s/ /_/gi;
					}

					if (m|<datestamp>(.*)</datestamp>|i){
						$cDate = $1;
					}
					if (m|<modclass>(.*)</modclass>|i){
						$modClass = $1;
					}
					if (m|<modscore>(.*)</modscore>|i){
						$modScore = $1;
					}
					
					if (m|<htmltext>|i){
						my $ln = $_;
						chomp($ln);
						$ln =~ s|</?htmltext>||ig;						
						@quotes= ($ln =~ m|<div class="quote"><p>\s*(.+?)\s*</p></div>|g);
						push @quotes, ($ln =~ m|<blockquote>\s*<div>\s*<p>\s*(.+?)\s*</p>\s*</div>\s*</blockquote>|g); 
						push @htmlsentences, $ln if ($ln =~ m|\w+|);
						
						if (m|</htmltext>|){
							next;
						}

						while(1){
							my $ln = <INFILE>;
							chomp($ln);	
							if (!($ln =~ m|</htmltext>|i)){
								push @quotes, ($ln =~ m|<div class="quote"><p>\s*(.+?)\s*</p></div>|g);
								push @quotes, ($ln =~ m|<blockquote>\s*<div>\s*<p>\s*(.+?)\s*</p>\s*</div>\s*</blockquote>|g); 
								push @htmlsentences, $ln if ($ln =~ m/\w+/);
							}
							else{
								$ln =~ s|</htmltext>||i;
								push @quotes, ($ln =~ m|<div class="quote"><p>\s*(.+?)\s*</p></div>|g);
								push @quotes, ($ln =~ m|<blockquote>\s*<div>\s*<p>\s*(.+?)\s*</p>\s*</div>\s*</blockquote>|g); 
								push @htmlsentences, $ln if ($ln =~ m/\w+/);
								last;
							}
						}
					}
					
					if (m|<tokenext>|i){
						my $ln = $_;
						chomp($ln);
						$ln =~ s|</?tokent?ext>||gi;
						push @tokensentences, $ln if ($ln =~ m|\w+|);

						if (m|</tokentext>|){
							next;
						}						
						while(1){
							my $ln = <INFILE>;
							chomp($ln);
							if (!($ln =~ m|</tokentext>|i)){
								push @tokensentences, $ln if ($ln =~ m/\w+/);
							}
							else{
								$ln =~ s|</tokentext>||ig;
								push @tokensentences, $ln if ($ln =~ m/\w+/);
								last;
							}
						}
					}

					if (m|<sentencetext>|i){
						my $ln = $_;
						chomp($ln);
						$ln =~ s|</?sentencetext>||gi;
						$ln =~ s|^\s*||;
						
						push @segsentences, $ln if ($ln =~ m|\w+| && isNotQuote(\$ln, @quotes));

						if (m|</sentencetext>|){
							next;
						}						

						while(1){
							my $ln = <INFILE>;
							chomp($ln);
							$ln =~ s|^\s*||;
							if (!($ln =~ m|</sentencetext>|i)){
								push @segsentences, $ln if ($ln =~ m/\w+/ && isNotQuote(\$ln, @quotes));
							}
							else{
								$ln =~ s|</sentencetext>||ig;
								push @segsentences, $ln if ($ln =~ m/\w+/ && isNotQuote(\$ln, @quotes));
								last;
							}
						}
					}					
					if (m|<parent>http://www.semanticweb.org/ontologies/ConversationInstances.owl\#\w+.(\d+)</parent>|i){
						$parent = $1;
					}					
				}
				$comments{$cId} = new Comment($cId, $cTitle, $cAuthor, $cDate, $modClass, $modScore, \@htmlsentences, \@tokensentences, \@segsentences, $parent);
			}
		}
		close(INFILE);		
		#write data to apply lexchain
		writeData($article, \%comments);
	}
}


sub isNotQuote{
	my $sen = shift;
	$$sen =~ s/[(){}\[\]]//gi;
	$$sen =~ s/<[^>]*>//gi;
	$$sen =~ s/^\s*//i;
	$$sen =~ s/\s*$//i;

	my @quotes = @_;
	
	if ($$sen =~ m/^&gt;|^&amp;gt;|^&amp;amp;gt;/){
		$quoteCount++;
		return 0;
	}
	foreach my $qt(@quotes){
	
		$qt =~ s/<[^>]*>//gi;
		$qt =~ s/[(){}\[\]]//gi;
		
		if ($qt =~ m|$$sen|i){
			$quoteCount++;
			return 0;
		}
		if ($$sen =~ m|^$qt|i){
			$quoteCount++;
			$$sen =~ s|\s*$qt\s*||i;
		}		
	}	
	return 1 if ($$sen =~ m|\w+|);	
}



#-----------------------------------------------------------------------------------------------------------------------------------------
sub writeData{

	my $art = shift;
	my $comments = shift;
	my $file = $art->{artId};

	open OUTFILE,">../Data/Preprocessed/$file" or die "Can't open file: ../Data/Preprocessed/$file: $! \n";
	my $thr = "$file"."dummy";
	open OUTFILE2,">Dummy/$thr" or die "Can't open file: Dummy/$thr : $! \n";
	

	#write article
	my $header = "_EOE_".$art->{artAuthor}."\n";
	print OUTFILE2 $header;

	foreach my $sent(@{$art->{segsentences}}){
		$sent =~ s/(&gt;)|(&amp;gt;)|(&lt;)|(&amp;lt;)|(&amp;quot;)|&amp;amp;gt;|&amp;amp;lt;//gi;
		print OUTFILE "$sent\n";	
		print OUTFILE2 "$sent\n";
	}
	
	my $aid = 0;
	#write comments
	foreach my $cId(sort {$a <=> $b}keys %{$comments}){
		my $header = "";
		if ($comments->{$cId}-> {cAuthor} eq "Anonymous"){
			$aid++;
			$header = "_EOE_".$comments->{$cId}-> {cAuthor}.$aid."\n";
		}
		else{
			$header = "_EOE_".$comments->{$cId}-> {cAuthor}."\n";
		}
		print OUTFILE2 $header;
		foreach my $sent(@{$comments->{$cId}->{segsentences}}){
			$sent =~ s/(&gt;)|(&amp;gt;)|(&lt;)|(&amp;lt;)|(&amp;quot;)|&amp;amp;gt;|&amp;amp;lt;//gi;
			print OUTFILE "$sent\n";
			print OUTFILE2 "$sent\n";
		}
	}
	close OUTFILE;	
	close OUTFILE2;
}