from myDE import sphere, rastrigin, myDE, simulation
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from tqdm import tqdm

def makeAnimation(data, save):
    fig = plt.figure()
    ax = fig.add_subplot(1,1,1)
    ims = []
    pbar = tqdm(total=len(data))
    for d in data:
        for i in d:
          im = plt.scatter(i[0],i[1],c='red')
        ims.append([im])
        pbar.update(1)
    pbar.close()
    ax.set_title(save)
    ax.set_xlabel('x1')
    ax.set_ylabel('x2')
    anime = animation.ArtistAnimation(fig,ims,interval=100)
    anime.save(save+".gif", writer='imagemagick')

if __name__ == "__main__":
    data = [[[i,d] for i in range(10)] for d in range(100)]
    makeAnimation(data,"test")
