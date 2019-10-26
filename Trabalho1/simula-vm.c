#include <stdlib.h>
#include <stdio.h>

typedef struct{
    int frames;
    int references;
    int fifo_faults;
    int lru_faults;
    int opt_faults;
}RESULTS;

typedef struct{
    int time;
    int page;
}TIMED_FRAME;

//Pega o próximo valor da entrada padrão, até chegar ao final da entrada
//Retorna 0 caso chegue ao EOF ou 1 caso tenha uma referência
int nextRef(int *ref){
    char str[100];
    // do{
    //     *ref = fgets(stdin);
    // }while(*ref == '\n');
    
    if(!fgets(str, 100, stdin)){
        return -1;
    }
    //printf("%s", str);
    sscanf(str, "%d", ref);
    return 1;
       
}

void output(RESULTS r){
    printf ("%5d quadros, %7d refs: FIFO: %5d PFs, LRU: %5d PFs, OPT: %5d PFs\n", r.frames, r.references, r.fifo_faults, r.lru_faults, r.opt_faults);
}

int fifo(int n, int ref, TIMED_FRAME * frames){
    //static int frames[n];
    static int faults;
    static int time = 0;
    int fault = 1;

    int oldest_time = __INT_MAX__;
    int oldest = 0;

    
    for(int i = 0; i < n; i++){
        if(ref == frames[i].page){
            fault = 0;
            break;
        } 
        if(frames[i].page == -1){ //caso ache um frame vazio, escolhe ele e termina a varredura
            oldest = i;
            break;
        }
        if(frames[i].time < oldest_time){
            oldest_time = frames[i].time;
            oldest = i;
        }
    }

    if(fault){
        faults++;
        frames[oldest].page = ref;
        frames[oldest].time = time;
    }


    //printf("%d %d %d\n", frames[0].page, frames[1].page, frames[2].page);
    //printf("%d %d %d\n", frames[0].time, frames[1].time, frames[2].time);
    time++;
    return faults;

}

int lru(int n, int ref, TIMED_FRAME * frames){
    //static int frames[n];
    static int faults;
    static int time = 0;
    int fault = 1;

    int oldest_time = __INT_MAX__;
    int oldest = 0;

    
    for(int i = 0; i < n; i++){
        if(ref == frames[i].page){
            fault = 0;
            oldest = i; 
            break;
        } 
        if(frames[i].page == -1){ //caso ache um frame vazio, escolhe ele e termina a varredura
            oldest = i;
            break;
        }
        if(frames[i].time < oldest_time){
            oldest_time = frames[i].time;
            oldest = i;
        }
    }

    if(fault){
        faults++;
        frames[oldest].page = ref;
        
    }
    frames[oldest].time = time;
    //printf("%d %d %d\n", frames[0].page, frames[1].page, frames[2].page);

    time++;
    return faults;

}

int main(int argc, char * argv[]){
    int page;
    int n;
    RESULTS results = {0}; //inicializa membros da struct em 0
    TIMED_FRAME * fifo_frames;
    TIMED_FRAME * lru_frames;
    

    if(argc > 0){
        n = atoi(argv[1]);
    }else{
        printf("É necessário o passar o número de quadros como argumento para o programa.");
        return 0;
    }
    results.frames = n;
    
    
    fifo_frames = malloc(sizeof(TIMED_FRAME) * n);
    lru_frames = malloc(sizeof(TIMED_FRAME) * n);
    for(int i = 0; i< n; i++){
        fifo_frames[i].page = -1;
        lru_frames[i].page = -1;
    }
    
    
    while(nextRef(&page) != -1){
        //page -= '0'; //converte código ascii para o valor inteiro
        //printf("%d ", page);
        results.references++;
        //printf("%d\n", page);
        results.fifo_faults = fifo(n, page, fifo_frames);
        results.lru_faults = lru(n, page, lru_frames);
    }

    output(results);
    
}





