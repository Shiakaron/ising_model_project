import numpy as np
import matplotlib.pyplot as plt
import csv
import os
import itertools
from scipy.optimize import curve_fit

folder = "C:\\Users\\savva\\Documents\\GitHub\\ising_model_2.0\\data\\chi_data"
folder2 = "C:\\Users\\savva\\Documents\\GitHub\\ising_model_2.0\\pngs\\"
texfolder = "C:\\Users\\savva\\OneDrive - University of Cambridge\\Part2\\Computational Projects\\ising_model\\Report\\texfigures\\"

def gauss(x, mean, sigma, scale):
    return (scale*np.exp(-((x-mean)/sigma)**2))

def linear(x, nu, a):
    """
    ln(T_c(N) - T_c(inf)) = (1/nu) * ln(L) + ln(a)
    y = (1/nu) * x + ln(a)
    """
    return (1/nu) * x + np.log(a)

def for_T_c_fun(L,T_c_inf,a,nu):
    return T_c_inf + a * np.power(L,-1/nu)

def plot_1():
    """
    heat vs temperature with different L's
    and
    peak vs L
    """
    p_files_dict = {}
    L_list = []
    dim = 2
    # please change file path when running on different devices
    for file in sorted(os.listdir(folder)):
        if file.endswith(".txt") and not file.endswith(").txt"):
            D = int(((os.path.splitext(os.path.basename(file))[0]).split('_',4)[3])[0])
            L = int((os.path.splitext(os.path.basename(file))[0]).split('_',4)[4])
            if (D == dim) and (L not in L_list):
                L_list.append(L)
                p_files_dict[L] = os.path.join(folder,file)

    ln_L_list = []
    y_list = []
    y_err_list = []
    T_c_N_list = []
    T_c_N_err_list = []
    T_c_inf = 2/np.log(1+np.sqrt(2))
    fig, ax = plt.subplots(figsize=(12,8))
    ax.axvline(T_c_inf, label="$T_c$", linestyle="--",color="k",alpha=0.5)
    marker = itertools.cycle(('*', '+', '.', ',', 'o'))
    limits = {
        8:[0,-1],
        12:[0,-1],
        16:[0,-1],
        20:[5,-5],
        24:[7,-7],
        28:[5,-8],
        32:[5,-1],
        36:[5,-1],
        40:[5,-1],
        44:[5,-4]
    }
    for key in limits:
        p_file = p_files_dict[key]
        avgChi = []
        errChi = []
        T = []
        with open(p_file) as csvfile:
            lines = csv.reader(csvfile, delimiter=' ')
            for row in lines:
                T.append(float(row[0]))
                avgChi.append(float(row[1]))
                errChi.append(float(row[2]))
        ax.errorbar(T, avgChi, errChi, ls='',marker = next(marker), label="L = "+str(key))
        left = limits[key][0]
        right = limits[key][1]
        T_fit = T[left:right]
        Chi_fit = avgChi[left:right]
        errChi_fit = errChi[left:right]
        popt, pcov = curve_fit(gauss, T_fit, Chi_fit, sigma=errChi_fit, absolute_sigma=True, maxfev=5000, p0=[2.3, 0.1, 1000], bounds=((2.25,-5,-np.inf),(2.6,5,np.inf)))
        x = np.linspace(T[left],T[right],100)
        ax.plot(x,gauss(x, *popt), color="c",linewidth=1)
        print(key,popt,np.sqrt(np.diag(pcov)))
        # ln_L_list.append(np.log(int(L)))
        # y_list.append(np.log(popt[0] - T_c_inf))
        # y_err_list.append(np.sqrt(np.diag(pcov)[0])/(popt[0] - T_c_inf))
        T_c_N_list.append(popt[0])
        T_c_N_err_list.append(np.sqrt(np.diag(pcov)[0]))
        # print(L, ln_L_list[-1],y_list[-1],y_err_list[-1])

    ax.set_title("Specific heat vs Temperature around critical")
    ax.set_ylabel(r"$\chi$")
    ax.set_xlabel(r"T / $J/k_B$")
    #ax.set_yscale("log")
    ax.legend()
    #fig.savefig(texfolder+"c_vs_temp_peak.pdf")
    #fig.savefig(folder2+"c_vs_temp_peak.png")
    #
    left2 = 0
    right2 = -1
    L_list_1 = L_list[left2:right2]
    T_c_N_list_1 = T_c_N_list[left2:right2]
    T_c_N_err_list_1 = T_c_N_err_list[left2:right2]
    fig3, ax3 = plt.subplots(figsize=(12,8))
    ax3.errorbar(L_list_1, T_c_N_list_1, T_c_N_err_list_1,ls="",marker='+')
    popt3, pcov3 = curve_fit(for_T_c_fun, L_list_1, T_c_N_list_1, sigma=T_c_N_err_list_1, absolute_sigma=True, maxfev=5000, p0=[2.26,1,1], bounds=((0,-np.inf,0.000001),(np.inf,np.inf,np.inf)))
    x3 = np.linspace(L_list_1[0],L_list_1[-1],100)
    ax3.plot(x3,for_T_c_fun(x3, *popt3), color="k",linewidth=1)
    ax3.set_title(r"$T_c$(L) vs L")
    ax3.set_ylabel(r"$T_c$(L) / J/$k_B$")
    ax3.set_xlabel("L")
    print(popt3,np.sqrt(np.diag(pcov3)))
    print("T_c calculated = ",popt3[0],"+-",np.sqrt(np.diag(pcov3)[0]))
    print("T_c onsager = ",T_c_inf)
    #
    # fig3.savefig(texfolder+"heat_cap_check_Onsager.pdf")
    # fig3.savefig(folder2+"heat_cap_check_Onsager.png")


def main():
    plot_1()
    plt.show()

if (__name__ == '__main__'):
    main()
