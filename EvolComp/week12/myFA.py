import random
from tqdm import tqdm
from statistics import mean, variance
import math
import numpy as np
from makeAnimation import makeAnimation
from makeGraph import makeGraph
import copy

def sphere(x):
    result = 0.0
    for elem in x:
        result += elem**2
    return result

def rastrigin(x):
    result = 0.0
    for elem in x:
        result += (elem**2 - 10 * math.cos(2 * math.pi * elem) + 10)
    return result

def calc_r(x1, x2):
    sum = 0
    for i, j in zip(x1, x2):
        sum += (i-j)*(i-j)
    return math.sqrt(sum)

def make_random_vector(D, x_max, x_min):
    return [(random.random()-0.5)*math.fabs(x_min-x_max)/2 for i in range(D)]

def calc_x_new(xi, xj, alpha, beta, e):
    x_new = []
    for i, j, k in zip(xi, xj, e):
        x_new.append(i+beta*(j-i)+alpha*k) 
    return x_new

def myFA(D, func):
    M = 30
    b_min = 0.2
    alpha0 = 0.5
    t_max = 1000
    f_end = 10**-5
    x_min = -5
    x_max = 5
    L = math.fabs(x_max - x_min)/2
    gamma = math.sqrt(L)
    x = [[random.uniform(x_max,x_min) for i in range(D)] for j in range(M)] 
    x_new = [[0 for i in range(D)] for j in range(M)]
    f = [0 for i in range(M)]
    I = [0 for i in range(M)]
    f_best = float('inf')
    x_best = [0 for i in range(D)]
    t = 0
    k = -1
    x_log = []
    x_log.append(x)
    f_log = []
    f_log.append(f_best)
    while t < t_max:
        t += 1
        alpha = alpha0*(10**-4/0.9)**(t/t_max)
        for i in range(M):
            f[i] = func(x[i])
            if f[i] == 0: I[i] = 0
            else: I[i] = 1 / f[i]
        for i in range(M):
            cnt = 0
            x_new[i] = copy.deepcopy(x[i])
            for j in range(M):
                if I[i] < I[j]:
                    cnt += 1
                    e = make_random_vector(D, x_max, x_min)
                    r = calc_r(x[i], x[j])
                    beta = (1-b_min) * math.e**(-gamma*r**2)+b_min
                    x_new[i] = calc_x_new(x[i], x[j], alpha, beta, e)
            if cnt == 0:
                k = i
                e = make_random_vector(D, x_max, x_min)
                for l in range(D):
                    x_new[k][l] += alpha * e[l]
        if f[k] < f_best:
            f_best = f[k]
            x_best = copy.deepcopy(x[k])
        for i in range(M):
            x[i] = x_new[i]
        x_log.append(copy.deepcopy(x))
        f_log.append(f_best)
        if f_best < f_end: break
    return t, f_best, x_log, f_log
            

def simulation(D,func):
    time = []
    f_value = []
    x_log = []
    f_log = []
    title = str(D)+func.__name__
    pbar = tqdm(total=100)
    for i in range(100):
        t, f, x, log = myFA(D, func)
        time.append(t)
        f_value.append(f)
        if len(x) > len(x_log):
            x_log = x
        if len(log) > len(f_log):
          f_log = log
        pbar.update(1)
    pbar.close()
    std = np.array(f_value)
    print(D, func.__name__, mean(f_value), np.var(std,ddof=0), np.var(std,ddof=1), mean(time))
    #makeAnimation(x_log, title)

if __name__ == "__main__":
    print("D, function, f-value mean, f-value var, f-value std-var, loop time mean")
    simulation(2, sphere)
    simulation(2, rastrigin)
    
    simulation(5, sphere)
    simulation(5, rastrigin)
    
    simulation(20, sphere)
    simulation(20, rastrigin)
