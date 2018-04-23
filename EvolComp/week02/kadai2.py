import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def func_1(x):
    return x[0]**2 + x[1]**2 - 1

def func_2(x):
    return x[0]**3 - x[1]

def d1_func_1(x):
    return 2*x[0]

def d2_func_1(x):
    return 2*x[1]

def d1_func_2(x):
    return 3*x[0]**2

def d2_func_2(x):
    return -1

def next_x(x):
    next_x = [0, 0] 
    det = d1_func_1(x)*d2_func_2(x) - d2_func_1(x)*d1_func_2(x)
    next_x[0] = x[0] - (d2_func_2(x)*func_1(x) + -d2_func_1(x)*func_2(x)) / det
    next_x[1] = x[1] - (-d1_func_2(x)*func_1(x) + d1_func_1(x)*func_2(x)) / det
    return next_x

def newton(x):
    e=10**-6
    t=0
    while True:
        print("==============================")
        print("x{0} = [{1:2.3f},{2:2.3f}], func1(x) = {3:4.7f}, func2(x) = {4:4.7f}".format(t,x[0],x[1],func_1(x),func_2(x)))
        if func_1(x) < e and func_2(x) < e: break
        if t > 10 : break
        x = next_x(x)
        t += 1
    print("==============================")

def draw_graph():
    x1 = np.arange(-3, 3, 0.2)
    x2 = np.arange(-3, 3, 0.2)
    x = [[a, b] for a in x1 for b in x2]
    y1 = [func_1(a) for a in x]
    y2 = [func_2(a) for a in x]
    x1 = [a for a,b in x]
    x2 = [b for a,b in x]
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter(x1, x2, y1)
    ax.scatter(x1, x2, y2)
    #plt.xlabel("X")
    #plt.ylabel("Y")
    plt.show()
    plt.savefig('graph2.png')


def main():
    newton([1,0.5])
    draw_graph()

if __name__ == "__main__":
    main()
