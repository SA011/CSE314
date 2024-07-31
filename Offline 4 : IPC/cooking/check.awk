BEGIN {
	cnt = 0;
	tot = 0;
	for(i = 0; i < MAX; i++)
		given[i] = 0;
}

{
    if ($1 == "Chef" && $2 < MAX && cnt == MAX - 1) {
		tot++;
		cnt = 0;
		for(i = 0; i < MAX; i++){
			given[i] = 0;
		}
    }
    else if ($1 == "Agent" && $5 < MAX && given[$5] == 0) {
		cnt++;
		given[$5] = 1;
    }
    
}

END {
	if (ITEM == tot && cnt == 0) {
		printf("OK. All test cases passed!\n");
	}
	else {
		printf("Please check your code. %d item remaining and %d agent extra\n", ITEM - tot, cnt);
	}
}
