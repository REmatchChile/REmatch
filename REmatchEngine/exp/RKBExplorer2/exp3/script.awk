{
	cnt = 0;
	match($0, /^.................12.*sparql.*FILTER.*/, arr);
	if(arr[1] != "")
	{
		cnt++;
	}
}

END {
	print cnt 
}
