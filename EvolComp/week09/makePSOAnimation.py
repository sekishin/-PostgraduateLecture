import matplotlib.pyplot as plt
import matplotlib.animation as animation
from tqdm import tqdm

def makeAnimation(data, save):
    fig = plt.figure()
    ax = fig.add_subplot(1,1,1)
    ims = []
    pbar = tqdm(total=len(data))
    t = 1
    for d in data:
        x1 = [i[0] for i in d]
        x2 = [i[1] for i in d]
        print(len(x1),len(x2))
        ax.set_title(save+" t = "+str(t))
        t += 1
        im = ax.scatter(x1,x2, c='red', s=20, marker='o')
        ims.append([im])
        pbar.update(1)
    pbar.close()
    ax.set_xlim(-5, 5)
    ax.set_ylim(-5, 5)
    ax.grid()
    ax.set_xlabel('x1')
    ax.set_ylabel('x2')
    anime = animation.ArtistAnimation(fig,ims,interval=100)
    anime.save(save+".gif", writer='imagemagick')

if __name__ == "__main__":
    data = [[[i,d] for i in range(10)] for d in range(100)]
    makeAnimation(data,"test")
