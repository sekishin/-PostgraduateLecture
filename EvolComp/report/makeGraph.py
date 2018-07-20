import matplotlib.pyplot as plt
import matplotlib.animation as animation
from tqdm import tqdm

def makeGraph(data, save, label):
    fig = plt.figure()
    ax = fig.add_subplot(1,1,1)
    ax.grid()
    ax.set_xlabel("t")
    ax.set_ylabel("f_best")
    ax.set_title(save)
    color = ["red","green","blue"]
    for d, l, c in zip(data, label, color):
        x = [i for i in range(1,len(d)+1)]
        plt.plot(x, d, color=c, label=l)
    plt.legend(loc=2)
    plt.savefig(save+".png")
    plt.close()

if __name__ == "__main__":
    data = [[i for i in range(100)]]
    makeGraph(data, "test", "test")
