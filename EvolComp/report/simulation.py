from myPSO import myPSO, sphere, rastrigin
from myDE import myDE
from myFA import myFA
from tqdm import tqdm
import numpy as np
from statistics import mean, variance
from makeAnimation import makeAnimation
from makeGraph import makeGraph

def simulation(D, func, method):
    time = []
    f_value = []
    x_log = []
    f_log = []
    pbar = tqdm(total=100)
    for i in range(100):
        t, f, x, log = method(D, func)
        time.append(t)
        f_value.append(f)
        if len(x) > len(x_log):
            x_log = x
        if len(log) > len(f_log):
            f_log = log
        pbar.update(1)
    pbar.close()
    std = np.array(f_value)
    print("{0}, {1}, {2}, {3:.3e}, {4:.3e}, {5:.3e}, {6:.2f}".format(D, method.__name__, func.__name__, mean(f_value), np.var(std,ddof=0), np.var(std,ddof=1), mean(time)))
    return x_log, f_log

def exec_simulation(D):
    methods = [myPSO, myDE]
    funcs = [sphere, rastrigin]
    result = {}
    for f in funcs:
        data = {}
        for m in methods:
            tmp = {}
            x_log, f_log = simulation(D, f, m)
            tmp['x_log'] = x_log
            tmp['f_log'] = f_log
            data[m.__name__] = tmp
        result[f.__name__] = data
    return result

if __name__ == "__main__":
    print("D, method, function, f-value mean, f-value var, f-value std-var, loop time mean")
    for d in [2, 5, 20]:
        result = exec_simulation(d)
        for f in ['sphere', 'rastrigin']:
            f_log = []
            for m in ['myPSO', 'myDE', 'myFA']:
                title = str(d)+f+m
                x_log = result[f][m]['x_log']
                makeAnimation(x_log, title)
                f_log.append(result[f][m]['f_log'])
            title = str(d)+f
            makeGraph(f_log, title)
