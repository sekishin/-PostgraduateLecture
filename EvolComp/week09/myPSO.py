import random
from tqdm import tqdm
from statistics import mean, variance
import math
import numpy as np

def sphere(x):
    result = 0
    for elem in x:
        result += elem**2
    return result

def rastrigin(x):
    result = 0
    for elem in x:
        result += (elem**2 - 10 * math.cos(2 * math.pi * elem) + 10)
    return result

def next_v(v, p, x, g):
    c = 1.494
    w = 0.729
    r1 = random.random()
    r2 = random.random()
    n = w * v + c * r1 * (p-x) + c * r2 * (g-x)
    return n

def myPSO(D, func):
    M = 30
    t_max = 1000
    Cr = 10**-5
    t = 0
    x_min = -5
    x_max = 5
    x = [[random.uniform(x_max,x_min) for i in range(D)] for j in range(M)] 
    v = [[0 for i in range(D)] for j in range(M)] 
    f = [0 for i in range(M)]
    f_gbest = float('inf')
    x_gbest = [0 for i in range(D)]
    f_pbest = [float('inf') for i in range(M)] 
    x_pbest = [[0 for i in range(D)] for j in range(M)]
    while t < t_max:
        t = t + 1
        for i in range(M):
            f[i] = func(x[i])
            if f[i] < f_pbest[i]:
                f_pbest[i] = f[i]
                for d in range(D):
                    x_pbest[i][d] = x[i][d]
                if f_pbest[i] < f_gbest:
                    f_gbest = f_pbest[i]
                    for d in range(D):
                        x_gbest[d] = x_pbest[i][d]
        if f_gbest < Cr: break
        for i in range(M):
            for d in range(D):
                v[i][d] = next_v(v[i][d], x_pbest[i][d], x[i][d], x_gbest[d])
                x[i][d] += v[i][d]
    #print("loop time = {}".format(t))
    #print("best function value = {}".format(f_gbest))
    #print("answer = {}".format(x_gbest))
    return t, f_gbest, x_gbest

def simulation(D,func):
    time = []
    f_value = []
    x_position = []
    print("次元数：", D)
    pbar = tqdm(total=100)
    for i in range(100):
        t, f, x = myPSO(D, func)
        time.append(t)
        f_value.append(f)
        x_position.append(x)
        pbar.update(1)
    pbar.close()
    print("loop time : mean={0}".format(mean(time)))
    std = np.array(f_value)
    print("best value: mean={0:.2e}, std_var={1:.2e}, variance={2:.2e}".format(mean(f_value), np.var(std,ddof=1), np.var(std,ddof=0)))

if __name__ == "__main__":
    simulation(2, sphere)
    simulation(5, sphere)
    simulation(20, sphere)

    simulation(2, rastrigin)
    simulation(5, rastrigin)
    simulation(20, rastrigin)
