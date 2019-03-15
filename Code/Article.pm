     # Article class contains different attributes and methods for an Article
     # Author: Shafiq Joty
     # Date: Aug, 08, 2010

use warnings;
package Article;

$artId = "";
$artTitle = "";
$artAuthor = "";
$artDate = "";

$htmlsentences = [];
$tokensentences = [];
$segsentences = [];



## constructor sets the Thread id and Subject ###########################

sub new{
		my $class = shift;
		my $self = {};
		$self->{artId} = shift;
		$self->{artTitle} = shift;
		$self->{artAuthor} = shift;
		$self->{artDate} = shift;
		$self->{htmlsentences} = shift;
		$self->{tokensentences} = shift;
		$self->{segsentences} = shift;
		bless $self, $class;
}



1;
