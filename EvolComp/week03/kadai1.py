from numpy.random import *
import matplotlib.pyplot as plt

def main():
    r = randn(1000)
    plt.hist(r, bins=30)
    plt.xlabel('randn()')
    plt.ylabel('frequency')
    plt.savefig('graph1.png')
    
if __name__ =="__main__":
    main()

