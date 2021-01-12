{
	match($0, /^(e.*)\[.*sparql(.*FILTER.*)/, arr);
	if(arr[1] != "")
	{
		cnt++;
	}
}

END {
	print cnt 
}
