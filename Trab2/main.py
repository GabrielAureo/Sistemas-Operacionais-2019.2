from tabulate import tabulate
from typing import List
from sys import maxsize as maxInt

class Task:
    def __init__(self, id, arrival, duration, priority):
        self.id = id
        self.arrival = arrival
        self.duration = duration
        self.priority = priority
    
    def __str__(self):
        return "(t" + str(self.id) + ", " + str(self.arrival) + ", " + str(self.duration) + ", " + str(self.priority) + ")"

    def __repr__(self):
        return self.__str__()
 
class TaskSet:
    def __init__(self, tasks = []):
        self.tasks = tasks

    def addTask(self, task):
        self.tasks.append(task)

    def __str__(self):
        
        return self.createTable()
    
    def __len__(self):
        return len(self.tasks)
    
    def createTable(self):
        header = ["Tarefas"]
        for i in range(0, len(self.tasks)):
            header.append("t" + str(i+1))
        
        data = [["Ingresso"], ["Duração"], ["Prioridade"]]

        for task in self.tasks:
            data[0].append(task.arrival)
            data[1].append(task.duration)
            data[2].append(task.priority)

        return tabulate(data, headers=header)
    
class SchedulingTable:
    def __init__(self):
        self.data = dict()
    
    def appendResult(self, key, result):
        self.data[key] = result

    def __str__(self):
        header = ["Algoritmo"]
        table = [["Tempo Médio de Execução Tt"], ["Tempo médio de Espera Tw"], ["Número de trocas de contexto"], ["Tempo total de processamento"]]
        for k,v in self.data.items():
            header.append(k)
            table[0].append(v.avgTt)
            table[1].append(v.avgTw)
            table[2].append(v.switchs)
            table[3].append(v.total)

        return tabulate(table, headers=header, floatfmt=".1f")
            
class SchedulingResult:
    def __init__(self, avgTt = 0, avgTw = 0, switchs = 0, total = 0):
        self.avgTt = avgTt
        self.avgTw = avgTw
        self.switchs = switchs
        self.total = total


def readFile():
    f = open("entrada.txt","r")

    taskSet = TaskSet()
    
    tasks = int(f.readline())

    buff = f.readlines()
    buff = [item.strip().split(" ") for item in buff]
    buff = [[int(item) for item in itemList] for itemList in buff]

    f.close()

    for i in range (0, tasks):
        taskSet.addTask(Task(i, buff[0][i], buff[1][i], buff[2][i]))
    
    return taskSet

def fcfs(tasks):
    tw = 0
    tt = 0
    time = 0
    queue = sorted(tasks,key=lambda x:x.arrival)

    for i in range(0, len(queue)):
        tt += time - queue[i].arrival
        time += queue[i].duration
        tw += time - queue[i].arrival
    tw /= len(queue)
    tt /= len(queue)

    result = SchedulingResult(tt,tw, len(queue) - 1, time)
    return result

def srtf(tasks):
    rt = [0] * len(tasks)
    wt = [0] * len(tasks)
    tt = [0] * len(tasks)
    switchs = 0
    time = 0
    complete = 0

    shortest = 0
    minR = maxInt

    check = False

    for i in range(len(tasks)):
        rt[i] = tasks[i].duration

    while complete != len(tasks):
        for i in range(len(tasks)):
            if ((tasks[i].arrival <= time) and (rt[i] < minR)
            and (rt[i]>0)):
                minR = rt[i]
                shortest = i
                check = True

        if(check == False):
            time += 1
            continue

        rt[shortest] -= 1

        minR = rt[shortest]

        if minR == 0:
            minR = maxInt
            switchs += 1

        if rt[shortest] == 0:
            complete += 1
            check = False
            
            finish = time + 1 
            tt[shortest] = (finish - tasks[shortest].arrival)
            wt[shortest] = (finish - tasks[shortest].duration - tasks[shortest].arrival)

            if(wt[shortest] < 0):
                wt[shortest] = 0

        time += 1

    
    avgWait = sum(wt) / len(tasks)
    avgTime = sum(tt) / len(tasks)

    result = SchedulingResult(avgTime,avgWait,switchs,time)
    return result
    
def sjf(tasks):
    rt = [0] * len(tasks)
    wt = [0] * len(tasks)
    tt = [0] * len(tasks)
    switchs = 0
    time = 0
    complete = 0
    shortest = 0

    minD = maxInt

    for i in range(len(tasks)):
        rt[i] = tasks[i].duration

    while complete != len(tasks):
        for i in range(len(tasks)):
            if (tasks[i].arrival <= time) and (rt[i] < minD) and rt[i] > 0:
                minD = rt[i]
                shortest = i
                
        
        time += minD
        rt[shortest] = 0
        complete += 1
        switchs += 1
        minD = maxInt
        for i in range(len(tasks)):
            if(tasks[i].arrival < time) and rt[i] != 0:
                wt[i] = time - tasks[i].arrival
    for i in range(len(tasks)):
        tt[i] = wt[i] + tasks[i].duration
    
    avgWait = sum(wt)/len(tasks)
    avgTime = sum(tt)/ len(tasks)

    return SchedulingResult(avgTime, avgWait, switchs - 1, time)

def rr(tasks):
    rt = [0] * len(tasks)
    wt = [0] * len(tasks)
    t = 0
    quantum = 2
    completed = 0 

    for i in range(len(tasks)):
        rt[i] = tasks[i].duration

    while(True):
        done = True

        for i in range(len(tasks)):
            if(tasks[i].arrival <= t):
                if(rt[i] > 0):
                    done = False
                    if(rt[i] > quantum):
                        t += quantum
                        rt[i] -= quantum
                    else:
                        t += rt[i]
                        wt[i] = t - tasks[i].duration - tasks[i].arrival
                        rt[i] = 0
                
        if done:
            break
    return SchedulingResult()

def prioc(tasks):
    tw = 0
    tt = 0
    time = 0
    queue = tasks.copy()
    for i in range(len(tasks)):
        p = 0
        for j in range(len(queue)):
            if queue[j].arrival <= time and queue[j].priority > queue[p].priority:
                p = j      
        time += queue[p].duration
        tt += time - queue[p].arrival
        tw += time - queue[p].duration - queue[p].arrival
        del queue[p]
    
    tw /= len(tasks)
    tt /= len(tasks)
    
    return SchedulingResult(tt,tw, len(tasks) - 1, time)

def priop(tasks):
    n = len(tasks)
    rt = [0] * n
    wt = [0] * n
    p = 0
    


def main():
    table = SchedulingTable()
    taskSet = readFile()
    table.appendResult("FCFS", fcfs(taskSet.tasks))
    table.appendResult("SRTF", srtf(taskSet.tasks))
    table.appendResult("SJF", sjf(taskSet.tasks))
    table.appendResult("RR", rr(taskSet.tasks))
    table.appendResult("PRIOc", prioc(taskSet.tasks))
    print(table)


main()




