//Trabalho feito em dupla
//Alunos: Gabriel Aureo
//        Miguel Santos

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
    int next_ref;
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

int fifo(int nFrames, int ref, TIMED_FRAME * frames){
    //static int frames[n];
    static int faults;
    static int time = 0;
    int fault = 1;

    int oldest_time = __INT_MAX__;
    int oldest = 0;

    
    for(int i = 0; i < nFrames; i++){
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

int lru(int nFrames, int ref, TIMED_FRAME * frames){
    //static int frames[n];
    static int faults;
    static int time = 0;
    int fault = 1;

    int oldest_time = __INT_MAX__;
    int oldest = 0;

    
    for(int i = 0; i < nFrames; i++){
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

//Checa se uma pagina esta num dos frames;
//Procura num vetor de frames, a partir da posicao position,
//  para nao procurar em instrucoes ja executadas
//Se achar, retorna 1;
//Caso contrario, retorna 0;
int search_page_in_frames(int pg, TIMED_FRAME * frames, int position) 
{ 
    int size = sizeof(frames)/sizeof(frames[0]); //tamanho que devo percorrer na busca

    for (int i = position; i < size; i++) 
        if (frames[i].page == pg) 
            return 1; 
    return 0; 
}

int search_page_in_refs(){
    
}

int opt(int nFrames, int ref, TIMED_FRAME * frames){
    //static int frames[n];
    static int faults;
    static int time = 0;
    int fault = 1;

    int oldest_time = __INT_MAX__;
    int oldest = 0;

    int pos = 0;
    int size = sizeof(frames)/sizeof(frames[0]);
    int farthest = -1;
    
    for(int i = 0; i < nFrames; i++){
        pos++;
        if(ref == frames[i].page){ //page hit
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

    if(fault){ //A pagina que procuro nao esta em nenhum frame
        faults++;
        for(int i = pos; i < nFrames; i++){
            //Se nao encontrar a pagina, ela nao sera usada no futuro, e posso substitui-la
            if(!search_page_in_refs(i, pos)){
                farthest = frames[i].page;
                break;
            }
        }
        //Se farthest == -1, é porque não encontrei uma pagina que nao sera mais usada
        if(farthest == -1){
            for(int i = pos; i < size; i++){

            }
        }
        
    }
    frames[oldest].time = time;

    //tiro a pagina mais longe de ser usada, coloco a pagina usada ao chamar a funcao
    frames[farthest].page = ref;

    time++;
    return faults;

}

int main(int argc, char * argv[]){
    int page;
    int nFrames;
    RESULTS results = {0}; //inicializa membros da struct em 0
    TIMED_FRAME * fifo_frames;
    TIMED_FRAME * lru_frames;
    TIMED_FRAME * opt_frames;

    //Array de int com as referencias dadas na entrada
    int * ref_array;
    int i = 0;
    

    if(argc > 0){
        nFrames = atoi(argv[1]);
    }else{
        printf("É necessário o passar o número de quadros como argumento para o programa.");
        return 0;
    }
    results.frames = nFrames;
    
    
    fifo_frames = malloc(sizeof(TIMED_FRAME) * nFrames);
    lru_frames = malloc(sizeof(TIMED_FRAME) * nFrames);
    opt_frames = malloc(sizeof(TIMED_FRAME) * nFrames);
    for(int i = 0; i< nFrames; i++){
        fifo_frames[i].page = -1;
        lru_frames[i].page = -1;
        opt_frames[i].page = -1;
    }
    
    
    while(nextRef(&page) != -1){
        //page -= '0'; //converte código ascii para o valor inteiro
        //printf("%d ", page);
        results.references++;
        //printf("%d\n", page);
        results.fifo_faults = fifo(nFrames, page, fifo_frames);
        results.lru_faults = lru(nFrames, page, lru_frames);
    }

    ref_array = malloc(sizeof(int) * results.references);

    while (nextRef(&page) != -1){
        ref_array[i] = page;
        i++;
    }

    for (int i = 0; i < results.references; i++){
        if(ref_array[i] != 0) printf("%d\n", ref_array[i]);
    }
    

    results.opt_faults = opt(nFrames, page, lru_frames);

    output(results);
    
}





