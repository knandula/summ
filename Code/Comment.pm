     # Comment class contains different attributes and methods for a Comment
     # Author: Shafiq Joty
     # Date: Aug, 08, 2010

use warnings;
package Comment;

$cId = "";
$cTitle = "";
$cAuthor = "";
$cDate = "";
$modClass = "";
$modScore = "";
$parent = "";
$children = ();
$isProcessed = 0;
$level = 0;


$htmlsentences = [];
$tokensentences = [];
$segsentences = [];


sub new{
		my $class = shift;
		my $self = {};
		$self->{cId} = shift;
		$self->{cTitle} = shift;
		$self->{cAuthor} = shift;
		$self->{cDate} = shift;
		$self->{modClass} = shift;
		$self->{modScore} = shift;
		$self->{htmlsentences} = shift;
		$self->{tokensentences} = shift;
		$self->{segsentences} = shift;
		$self->{parent}= shift;

		bless $self, $class;
}


sub addChild{
	my $self = shift;
	my $ch = shift;
	push @{$self->{children}}, $ch;
}

sub setProcessed{
	my $self = shift;
	$self->{isProcessed} = 1;
}

sub setLevel{
	my $self = shift;
	$self->{level} = shift;
}
	
1;
