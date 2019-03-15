function hl=vline(in1,in2,in3)
%VLINE plots vertical line(s) 
%  The length of the line is chosen according to the current
%  axes ylim property. Let 
%     ha : handle to an axes
%     p  : vector of positions
%     s  : string of line properties (see the plot command)
%  Then
%     vline(ha,p,s)
%  adds length(p) vertical lines to the axes ha. The command
%     hl=vline(ha,p,s)
%  also returns a vector of handles. The commands
%     vline(ha,p), hl=vline(ha,p)
%  choose s as in the plot command. The commands
%     vline(p,s), hl=vline(p,s)
%  add to the current axes. The commands
%     vline(p), hl=vline(p) add to the current axes and choose s
%  as the plot command.
%  Example:
%     plot(sin(linspace(0,10))), vline(10:10:100,'k:')
%  

%  (c) 1999   Arno Linnemann.   All rights reserved. 
%  The author of this program assumes no responsibility for any  errors 
%  or omissions. In no event shall he be liable for damages  arising out of 
%  any use of the software. Redistribution of the unchanged file is allowed.
%  Distribution of changed versions is allowed provided the file is renamed
%  and the source and authorship of the original version is acknowledged in 
%  the modified file.
%
%  Sept. 99,  http://www.uni-kassel.de/~linne/

% determine ha, p, s
if nargin==1,
  ha=gca;
  p=in1;
  linestr='';
elseif nargin==2  
  if isa(in2,'char')
    ha=gca;
    p=in1;
    linestr=in2;
  else  
    ha=in1;
    p=in2;
    linestr='';
  end
elseif nargin==3
  ha=in1;
  p=in2;
  linestr=in3;
else  
  error('1,2 or 3 input arguments, please.');
end  

% get limits and number of lines
ylim=get(ha,'ylim');
nlines=length(p);

% add to axes
nextplot=get(ha,'nextplot');
set(ha,'nextplot','add');

% make axes ha current
haold=gca;
axes(ha);

% add line(s)
if isempty(linestr)
  hli=plot( [ p(:).' ; p(:).' ], [ ylim(1) , ylim(2) ] );
else
  hli=plot( [ p(:).' ; p(:).' ], [ ylim(1) , ylim(2) ], linestr );
end  

% return to original settings
axes(haold);
set(ha,'nextplot',nextplot);

% output
if nargout
  hl=hli;
end  
