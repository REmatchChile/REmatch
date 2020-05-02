{
	cnt = 0;
	match($0, /sparql.*OPTIONAAL.*/, arr);
	if(arr[1] != "")
		++cnt
}

END {
	print cnt 
}
