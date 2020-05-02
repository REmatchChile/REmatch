{
	match($0, /.*OPTIONAL.*/, arr);
	if(arr[1] != "")
		++cnt
}

END {
	print 1 
}
