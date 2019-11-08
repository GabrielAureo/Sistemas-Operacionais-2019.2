//Trabalho feito em dupla
//Alunos: Gabriel Aureo
//        Miguel Santos

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <iostream>
#include <string>

using namespace std;

typedef struct{
    int frames;
    int references;
    int fifo_faults;
    int lru_faults;
    int opt_faults;
}RESULTS;

typedef struct{
    int time;
    int page; //id da página, -1 significa que o frame está vazio
    int next_ref;
}TIMED_FRAME;

typedef map<int, vector<int>> time_dictionary;

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
    //puts("bbb;");
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
int search_page_in_frames(int page, TIMED_FRAME * frames, int position) 
{ 
    int size = sizeof(frames)/sizeof(frames[0]); //tamanho que devo percorrer na busca

    for (int i = position; i < size; i++){
        if (frames[i].page == page){
            return 1;
        }
    }
    return 0; 
}

int search_page_in_refs(int pg, vector<int> vec, int position){
    for (int i = position; i < vec.size(); i++){
        if (vec[i] == pg){
            return 1;
        }
    }
    return 0;  
}

//ESSA É A VERSÃO OBSOLETA DO ALGORITMO
//A VERSÃO CORRETA ESTÁ NA FUNÇÃO OPT2()
int opt(int nFrames, int ref, TIMED_FRAME * frames, vector<int> refs_vec){
    //puts("ccc;");
    //static int frames[n];
    static int faults;
    static int time = 0;
    int fault = 1;

    int oldest_time = __INT_MAX__;
    int oldest = 0;

    int ref_pos = 0;
    int size = sizeof(frames)/sizeof(frames[0]);
    int farthest = -1;
    int farthest_pos = -1;

    //printf("ref: %d ", ref);
    //printf("%d %d %d\n", frames[0].page, frames[1].page, frames[2].page);
    for(int i = 0; i < nFrames; i++){
        if(ref == frames[i].page){ //page hit
            fault = 0;
            break;
        } 
        if(frames[i].page == -1){ //caso ache um frame vazio, escolhe ele e termina a varredura
            oldest = i;
            farthest_pos = i;
            break;
        }
        if(frames[i].time < oldest_time){
            oldest_time = frames[i].time;
            oldest = i;
        }
    }

    if(fault){ //A pagina que procuro nao esta em nenhum frame
        faults++;
        for(int i = ref_pos; i < nFrames; i++){
            //Se nao encontrar a pagina, ela nao sera usada no futuro, e posso substitui-la
            if(search_page_in_refs(ref, refs_vec, ref_pos)){
                //Guardo a proxima referencia de cada pagina contina num frame
                frames[i].next_ref = i;
            }
            else {
                //Se a pagina nao tiver referencia futura, marco next_ref como -1
                frames[i].next_ref = -1;
            }
        }
        //Olho se ainda tem algum next_ref == -1
        //Se next_ref == -1, significa que a pagina nao sera mais usada, entao posso tirar
        //Se nao tiver nenhum next_ref == -1, pego a maior de todas pra tirar
        for(int i = ref; i < nFrames; i++){
            if(frames[i].next_ref == -1){
                farthest = frames[i].page;
                farthest_pos = i;
                break;
            }
            else{
                if(frames[i].next_ref > farthest){
                    farthest = frames[i].next_ref;
                    farthest_pos = i;
                }
            }
        }
        
    }
    frames[oldest].time = time;
    //printf("pos: %d \n fault: %d\n", farthest_pos, fault);
    //tiro a pagina marcada como farthest, coloco a pagina usada ao chamar a funcao
    frames[farthest_pos].page = ref;

    ref_pos++;
    time++;
    return faults;

}

int search_frames(int ref, int cur_time, vector<int> frames, time_dictionary * timedict){
    int farthest = 0;
    int farthest_ref = -1;
    bool last = false; //flag para caso encontrar uma página que não será mais referenciada no futurpo;

    for(int i = 0; i < frames.size(); i++){
        int p = frames[i];
        vector<int> times = (*timedict)[p];
        /*printf("%d: ", p);
        for(auto t : times){
            printf("%d ", t);
        }
        puts("");*/
        for( int j = 0; j < times.size(); j++){
            last = true; //assume que a página não terá mais referencias futuras
            if(times[j] > cur_time){
                last = false; //como encontrou um tempo futuro, nega a flag
                if(times[j] > farthest){
                    farthest = times[j];
                    farthest_ref = i;
                }
                break; //termina a busca dessa referencia pois já encontrou o maior proximo tempo;
            }
        }
        if(last){
            return i;
        }

    }
    //printf("%d %d\n", farthest, farthest_ref);
    return farthest_ref;
}

int opt2(int nframes, vector<int> refs_vec, time_dictionary * timedict){
    bool fault = true;
    int faults = 0;
    int time = 0;
    int frame = -1;
    vector<int> frames;

    for(int i = 0; i < nframes; i++){
        frames.push_back(-1);
    }

    
    for(int t = 0; t < refs_vec.size(); t++){
        int ref = refs_vec[t]; //pega a proxima refencia 

        printf("Progresso do OPT: %d/%d\r",t, (int)refs_vec.size());
        fflush(stdout);

        for(int i = 0; i < nframes; i++){
            fault = true; //assume que vai ocorrer page fault
            frame = -1;
            if(ref == frames[i]){ 
                fault = false; // page hit, então nega a flag
                break;
            } 
            if(frames[i] == -1){ //caso ache um frame vazio, escolhe ele e termina a varredura
                frame = i;
                break;
            }
        }

        if(fault){
            faults++;
            if(frame == -1){ 
                int swap = search_frames(ref, time, frames, timedict);
                frames[swap] = ref;
            }else{
                frames[frame] = ref;
            }
        }
        time++;
        /*printf("%d: ", time);
        for(int k = 0; k < frames.size(); k++){
            printf("%d ", frames[k]);
        }
        printf("\n");*/
    }

    return faults;    
}


int main(int argc, char * argv[]){
    int page;
    int nFrames;
    RESULTS results = {0}; //inicializa membros da struct em 0
    TIMED_FRAME * fifo_frames;
    TIMED_FRAME * lru_frames;
    TIMED_FRAME * opt_frames;

    time_dictionary timedict;
    int time = 0;

    //Array de int com as referencias dadas na entrada
    vector<int> ref_vector;
    int i = 0;
    

    if(argc > 0){
        nFrames = atoi(argv[1]);
    }else{
        printf("É necessário o passar o número de quadros como argumento para o programa.");
        return 0;
    }
    results.frames = nFrames;

    fifo_frames = (TIMED_FRAME *) malloc(sizeof(TIMED_FRAME) * nFrames);
    lru_frames = (TIMED_FRAME *) malloc(sizeof(TIMED_FRAME) * nFrames);
    opt_frames = (TIMED_FRAME *) malloc(sizeof(TIMED_FRAME) * nFrames);

    //-1 indica que o frame esta vazio
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

        timedict[page].push_back(time);

        time++;

        ref_vector.push_back(page);
    }

    results.opt_faults = opt2(nFrames, ref_vector, &timedict);
    /*for(auto elem : timedict){
        cout << elem.first << ", " << elem.second.size() << endl;
    }

    
    //printf("%d\n", ref_vector.size());
    int j = 0;
    for(int reference : ref_vector){
        //printf("Progresso: %d/%d\r", j, ref_vector.size());
        //fflush(stdout);
        //printf("%d \n", reference);
        //results.opt_faults = opt(nFrames, reference, opt_frames, ref_vector);
        j++;
    }

    for(int i = 0; i< ref_vector.size(); i++){
        //if(ref_vector.at(i)!=0) printf("%d\n",ref_vector.at(i));
    }*/

    output(results);
    
}





