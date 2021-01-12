{
	match($0, /\[[0-9][0-9]\/[A-Z][a-z][a-z]\/[0-9][0-9][0-9][0-9]\s+(1[2-9]|2[0-4]).*\].*"R".*"(.*(OPTIONAL|optional).*)"/, arr);
	if(arr[1] != "")
		++cnt
}

END {
	print cnt 
}
