out:
	cc -ggdb general.c pp.c ppexpr.c keywords.c main.c -std=c11 -o mcc
	#cc -O3 general.c pp.c ppexpr.c keywords.c main.c -std=c11 -o mcc
