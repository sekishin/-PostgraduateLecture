import matplotlib.pyplot as plt                                                      
import matplotlib.animation as animation                                             
from tqdm import tqdm                                                                
  
def makeGraph(data, save):                  
    fig = plt.figure()                                                              
    ax = fig.add_subplot(1,1,1)                                                      
    ax.grid()                                                                        
    ax.set_xlabel("t")                                                               
    ax.set_ylabel("f_best")                                                          
    ax.set_title(save)                                                               
    for d in data:
        x = [i for i in range(1,len(d)+1)]                       
        plt.plot(x, d)                                                                
    plt.savefig(save+".png") 

if __name__ == "__main__":
    data = [i for i in range(100)]
    makeGraph(data, "test")
