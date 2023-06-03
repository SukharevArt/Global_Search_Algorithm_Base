import matplotlib.pyplot as plt

import numpy as np



def make_data():
    x = np.arange(-5, 5, 0.01)
    y = np.arange(-5, 5, 0.01)
    x, y = np.meshgrid(x, y)

    z = (np.sin(x+0.4*y)*(y-3)+np.cos(y-0.2*x)*x)
    # z = (np.sin(x+0.4*y)+np.cos(y))
    # z = (x+1) * (x+1) + (y-2) * (y-2)
    return x, y, z


if __name__ == '__main__':
    x, y, z = make_data()
    f = open("points.txt",'r')

    tx,ty,tz = zip(*[(float(w) for w in x.split(" ")) for x in f.readlines()])
    
    o = 0.6
    
    plt.figure(figsize=(10,8))   
    for xx,yy in zip(tx,ty):
        plt.scatter(xx,yy,color=(o,o,o),s=9)
        o-=0.01
        o=max(o,0)
    cs = plt.contour(x, y, z)
    plt.clabel(cs)
    f.close()
    plt.show()