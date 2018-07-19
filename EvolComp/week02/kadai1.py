import numpy as np
import matplotlib.pyplot as plt

def func(x):
    return x**3+2*x**2-5*x+6

def d_func(x):
    return 3*x**2+4*x-5

def draw_graph():
    x = np.arange(-5, 3, 0.01)
    y = func(x)
    plt.plot(x, y)
    plt.title("Y = X^3 + 2X^2 - 5X + 6")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.savefig('graph1.png')

def newton(x):
    e = 10**-4
    t = 0
    print("初期値 : x0 = ", x)
    while True:
        print("===============================")
        print("x{0:d} = {1:2.3f}, func(x) = {2:3.6f}".format(t, x, abs(func(x))))
        if abs(func(x)) < e or t > 10: break
        x = x - func(x) / d_func(x)
        t += 1
    print("===============================")


def main():
    draw_graph()
    newton(-3)
    newton(1)

if __name__ == '__main__':
    main()
