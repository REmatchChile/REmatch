{
	match($0, /sparql(.*OPTIONAL.*)/, arr);
	if(arr[1] != "")
		++cnt
}

END {
	print cnt 
}
