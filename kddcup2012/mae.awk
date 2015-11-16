{
	e=$2*$3-$1
	mae+=sqrt(e*e)
}
END{
	print mae/NR
}
