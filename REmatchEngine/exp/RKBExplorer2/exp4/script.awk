{
	match($0, /^[^+]*\+.*sparql.*OPTIONAL.*OPTIONAL.*/, arr);
	if(arr[1] != "")
		++cnt
}

END {
	print "1" 
}
