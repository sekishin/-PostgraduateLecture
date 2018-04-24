from numpy.random import *

def func(x):
    return x**3+2*x**2-5*x+6

def next_x(x):
    p = rand()
    r = rand()
    if r < p:
        n = randn()
        #print(n)
        return x + n
    else:
        return x 

def markov():
    #x = rand()
    x = -3
    e = 10*-4
    t = 0
    min_f = abs(func(x))
    min_t = 0
    min_x = x
    while True:
        print("="*30)
        print("x{2} = {0:.4f}, func(x) = {1:.5f}".format(x, func(x),t))
        if abs(func(x)) < e or t > 10000: break
        x = next_x(x)
        t += 1
        if min_f > abs(func(x)): 
            min_f = abs(func(x))
            min_x = x
            min_t = t
    print("t = {0}, x = {1}, min = {2}".format(min_t,min_x,min_f))

def main():
    markov()

if __name__ == "__main__":
    main()
        
