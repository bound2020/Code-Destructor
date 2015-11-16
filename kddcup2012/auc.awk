#sort by $3 asc firstly
BEGIN{
f0=-1
}
{
if($3!=f0){
	auc+=(x-x0)*(y+y0);
	f0=$3
	y0=y;
	x0=x;
}
x+=$2-$1;
y+=$1;
}
END{
	auc+=(x-x0)*(y+y0);
	auc/=2*x*y;
	print auc
}
