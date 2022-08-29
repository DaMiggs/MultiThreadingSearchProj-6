#include "pa6.h"
//fresh copy
char* buffer[THREADS];
int dictionary_size;
char* dict[100000];
int numThreads = 1;
int wLen[30];
int tFlag = 0;
int vFlag = 0;
int lFlag = 0;
pthread_mutex_t mutex;
long totalBinarySearchTime;


static long get_nanos(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
	return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}
int binsearch(char* dictionaryWords[],int listSize,char* keyword){
	int mid, low = 0, high = listSize - 1;
	while (high >= low) {
		mid = (high + low) / 2;
		if (strcmp(dictionaryWords[mid],keyword) < 0)
			low = mid + 1;
		else if (strcmp(dictionaryWords[mid],keyword)>0)
			high = mid - 1;
		else
			return mid;
	}
	return -1; //not found
}
void* solverTimer(void* id){
	long startTime;
	int boo;
	char c = 0;
	int i, n, j;
	char* buf = buffer[(int)id];
	if (vFlag)
		fprintf(stderr, "Note: Thread #%d: %s\n", (int) id, "started!");
	for (i = 0; wLen[i] != 0; i++){
		n = BUFFER_SIZE/numThreads-wLen[i];
		for(j = 0; j < n; j++){
			if(c)
				buf[wLen[i] + j - 1] = c;
			c = buf[wLen[i] + j];
			buf[wLen[i] + j] = '\0';
			//wait for thread
			pthread_mutex_lock(&mutex);
			startTime = get_nanos();
			boo = binsearch(dict, dictionary_size, buf + j) + 1;
			totalBinarySearchTime += (get_nanos() - startTime);
			pthread_mutex_unlock(&mutex);
			//continue thread
			if(boo)
				printf("Thread #%d: %s\n",(int)id, buf + j);
		}
	}
}

void* word_puzzle_solver(void* id){
	char c = 0;
	int i, j, n;
	char* buf = buffer[(int)id];
	if (vFlag == 1)
		fprintf(stderr, "Note: Thread #%d: %s\n", (int)id, "started!");
	for (i = 0; wLen[i]!=0; i++){
		n = BUFFER_SIZE/numThreads-wLen[i];
		for(j = 0; j < n;j++){
			if(c)
				buf[wLen[i] + j - 1] = c;
			c = buf[wLen[i] + j];
			buf[wLen[i] + j] = '\0';
			if(binsearch(dict, dictionary_size, buf + j) + 1)//if search is successful! +1 to reach 0 if nothing is found
				printf("Thread #%d: %s\n",(int)id, buf + j);
		}
	}
}

int main(int argc, char** argv){
	int thread_number, i, j;

	//Identifying CLAs
	for (i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-len")){//has lists
			lFlag = 1;
			char* lTemp = (char*)malloc(strlen(argv[i])+1), length[20];
			strcpy(lTemp, argv[++i]);
			int x = 0;
			//printf("lTemp has: %s\n",lTemp);
			//this while loop breaks with variable decleration location
			while(sscanf(lTemp, "%[^,\n],%[^\n]\n", length, lTemp)==2){
				wLen[x++]=atoi(length);
			}
			wLen[x++] = atoi(length);
			wLen[x] = 0;
			//free(lTemp);
		}else if (!strcmp(argv[i],"-nthreads")){
			numThreads = atoi(argv[++i]);
		}else if(!strcmp(argv[i], "-time")){
			tFlag = 1;
		}else if(!strcmp(argv[i], "-verbose")){
			vFlag = 1;
		}
	}
	printf("lfl : %d and threads is %d\n", lFlag, numThreads);
	if (!lFlag){
		wLen[0] = 8;
		wLen[1] = 9;
		wLen[2] = 0;
	}

	long startTime = get_nanos();
	int size = 1 + (BUFFER_SIZE/numThreads);
	char temp[100];
	pthread_t threadID[numThreads];
	char line[1000];
	char * pointer;

	FILE* f = fopen("dict.txt", "r");
	dictionary_size = 0;
	while(fgets(line, 1000, f)){
		sscanf(line, "%s\n", temp);
		if(strlen(temp) == 0)
			continue;
		dict[dictionary_size] = (char*) malloc(sizeof(temp)+1);
		strcpy(dict[dictionary_size++], temp);
	}
	fclose(f);

	for(thread_number = 0; thread_number < numThreads; thread_number++){
		buffer[thread_number] = (char*)malloc(size);
		if(!fgets(buffer[thread_number], size, stdin)){
			fprintf(stderr, "Error: can't read the input stream!");
			break;
		}
		if (tFlag){
			if(pthread_create(threadID + thread_number, NULL, solverTimer, (void *) thread_number)){
				fprintf(stderr, "Error: Too many threads are created!\n");
				break;
			}
		}
		else{
			if(pthread_create(threadID + thread_number, NULL, word_puzzle_solver, (void *) thread_number)){
				fprintf(stderr, "Error: Too many threads are created!\n");
				break;
			}
		}
	}
	for(j = 0; j < thread_number;j++){
		pthread_join(threadID[j], NULL);
		if (vFlag)
			fprintf(stderr, "Note: Thread %d joined!\n", j);
	}
	if (vFlag)
		fprintf(stderr, "FINAL: Total time: %ld seconds using %d threads!\n", 
		(get_nanos()-startTime)/1000000000, thread_number);
	if (tFlag)
		fprintf(stderr, "FINAL: Total binary search time: %ld seconds using %d threads!\n", 
		totalBinarySearchTime/1000000000, thread_number);
}
