import numpy as np
import matplotlib.pyplot as plt
import csv
import os


def plot_1():
    """
    magnetisaion vs temperature with different lattice sizes
    """
    p_files = []
    dim = 2
    folder = "magn_data"
    for file in sorted(os.listdir(folder)):
        if file.endswith(".txt") and not file.endswith(").txt"):
            p_files.append(os.path.join(folder,file))

    L_list = []
    fig, ax = plt.subplots()
    ax.axvline(2.2692, label="$T_c$", linestyle="--",color="k")
    for p_file in p_files:
        L = (os.path.splitext(os.path.basename(p_file))[0]).split('_',3)[3]
        avgM = []
        errM = []
        T = []
        if (L not in L_list):
            L_list.append(L)
            with open(p_file) as csvfile:
                lines = csv.reader(csvfile, delimiter=' ')
                for row in lines:
                    T.append(float(row[0]))
                    avgM.append(float(row[1]))
                    errM.append(float(row[2]))
            ax.errorbar(T, avgM, errM, ls='',marker='+', label="L = "+str(L))

    ax.set_title("|Magnetisation| per site vs Temperature")
    ax.set_ylabel("|M|/N")
    ax.set_xlabel("T")
    ax.legend()
    fig.savefig(folder+"\\magn_vs_temp.png")
    plt.show()

def main():
    plot_1()

if (__name__ == '__main__'):
    main()