BEGIN {
	cnt = 0;
}
{
	match($0, /\[(.*)\+.*"R".*"(.*(OPTIONAL|optional).*)"/, arr);
	if(arr[1] != "")
		++cnt;
}

END {
	print cnt 
}
