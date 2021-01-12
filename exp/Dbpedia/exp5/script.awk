BEGIN {
	cnt = 0;
}
{
	match($0, /\[(.*)\+.*"R".*"(.*timeout=.*)"/, arr);
	if(arr[1] != "")
		++cnt;
}

END {
	print cnt 
}
