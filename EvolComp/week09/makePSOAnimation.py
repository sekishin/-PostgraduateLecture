from myPSO import sphere, rastrigin, myPSO, simulation
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from tqdm import tqdm

def makeAnimation(data, save):
    fig = plt.figure()
    ax = fig.add_subplot(1,1,1)
    ims = []
    pbar = tqdm(total=len(data))
    for d in data:
        im = plt.scatter(d[0],d[1],c='red')
        ims.append([im])
        pbar.update(1)
    pbar.close()
    ax.set_title(save)
    ax.set_xlabel('x1')
    ax.set_ylabel('x2')
    anime = animation.ArtistAnimation(fig,ims,interval=100)
    anime.save(save+".gif", writer='imagemagick')

if __name__ == "__main__":
    #simulation(2, sphere)
    #simulation(5, sphere)
    makeAnimation([[i,i] for i in range(100)],"test")
