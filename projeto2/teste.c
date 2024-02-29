#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>


int main(){
	int arr[4] = {count_missed, count_one, count_two, count_three};
	int arr2[4] = {2, 2, 0, 1};
int a;
int b;
int c;
int d;
int arr0[4] = {2, 2, 0, 1};
	int arr[4] = {count_missed, count_one, count_two, count_three};
	int arr2[4] = {2, 2, 0, 1};
FILE *ft = fopen("temp.dat", "w");
fwrite(arr0, sizeof(arr0), 1, ft);
fclose(ft);
//fscanf(ft, "%d %d %d %d", &a, &b, &c, &d);
FILE *fi = fopen("temp.dat", "r");
int arr[4];
fread(arr, sizeof(arr), 1, fi);
fclose(fi);
printf("%d %d %d %d\n", arr[0], arr[1], arr[2], arr[3]);
}
