#include "header.h"

int dim;
int L;
int N;
double N_links;
int *spins;
map<int,vector<int>> mapOfNearest;
map<int,vector<int>> mapOfNext2Nearest;
int M;
double E;

double n2n = 0;
double H = 0;

unsigned int seed = 1581096802; // (unsigned)time(0);
double *T;

/*
TESTING THE PROGRAM'S CORE FUNCTIONS
To start with analysing the evolution of the average magnetisation of the system. I will do this with different Lattice sizes and a range of temperatures. Following that I will test how the average magnetisation of an evolved system changes with temperature. I will do this for different lattice sizes to show finite size effects.

PLOT 1:
For a fixed lattice size plot the evolution of the system at different temperatures, both below and above Tc.

PLOT 2:
I will choose a specific lattice size and temperature (below Tc) to demonstrate the randomness of the simulation and difference between hot and cold start

PLOT 3:
For a fixed temperature (below Tc) plot the evolution of the system at different lattice sizes. Use average magnetisation for clarity in plots.

PLOT 4:
<|magnetisaiton|> vs temperature for different lattice sizes. Shows finite size effects and how greater lattice size are closer to the real solution.
*/

void magnetisation_vs_time_data()
{
    cout << "Running for magnetisation of evolving system" << endl;
    dim = 2;
    L = 80;
    int thermalisationCycles = 0;
    int dataPoints = 10000;
    int spacingCycles = 1;
    double Temp = 2.23;
    print_all_parameters(thermalisationCycles, dataPoints, spacingCycles, 0, Temp);
    cout << "Proceed with default parameters? Enter 1 for YES, 0 for NO\n";
    int user_input = user_integer_input(0,1);
    if (user_input == 0) {
        cout << "Dimensions (2-3):\n";
        dim = user_integer_input(2,3);
        cout << "Lattice size (8-256):\n";
        L = user_integer_input(8,256);
        cout << "Thermalisation cycles (0-5000):\n";
        thermalisationCycles = user_integer_input(0,5000);
        cout << "Number of data points (100-10000):\n";
        dataPoints = user_integer_input(100,10000);
        cout << "Temperature (enter 15 for 1.5 Kelvin) (1-50):\n";
        Temp = double(user_integer_input(1,50))/10;
    }

    //initialise spins array and neighbours maps
    initialise_system_and_maps();

    // open file
    ofstream myfile;
    string folder = ".\\data\\magn_vs_time";
    string filename = "magn_vs_time_"+to_string(dim)+"D_"+to_string(L)+"_"+to_string(Temp)+".txt";
    filename_rename_if_exists(filename, folder);
    string path = folder+"\\"+filename;
    myfile.open(path);
    if (!myfile.is_open()) {
        throw "Func: magnetisation_vs_time_data(). File not opened with path: " + path + "\nPlease fix path";
    }
    cout << "Writing in file with path: " << path << endl;
    cout << "Starting computations" << endl;
    myfile << Temp << endl;
    initialise_spins_hot();
    //initialise_spins_cold();
    compute_magnetisation();
    metropolis_function(Temp,thermalisationCycles);
    myfile << (fabs(M)) << endl;
    for (int i=1; i<dataPoints; i++) {
        metropolis_function(Temp,spacingCycles);
        myfile << (fabs(M)) << endl;
    }

    // wrap up
    delete[] spins;
    if (myfile.is_open()){
       myfile.close();
    }
}

void magnetisation_vs_time_data_bulk()
{
    cout << "Running for magnetisation of evolving systems" << endl;
    dim = 2;
    int L_list[5] = {5,10,20,40,80};
    int thermalisationCycles = 0;
    int dataPoints = 5000;
    int spacingCycles = 1;
    double iniT = 1.0; double finT = 5.0; int numT = 9;
    T = linspace(iniT, finT, numT);

    ofstream myfile;
    string folder = ".\\data\\magn_vs_time";

    for (int i=0; i<numT; i++) {
        for (int j=0; j<5; j++) {
            //set L
            L = L_list[j];
            //initialise vector
            vector<double> magn;

            //initialise spins array and neighbours maps
            initialise_system_and_maps();
            print_all_parameters(thermalisationCycles, dataPoints, spacingCycles, 0, T[i]);

            //open file
            string filename = "magn_vs_time_"+to_string(dim)+"D_"+to_string(L)+"_"+to_string(T[i])+".txt";
            filename_rename_if_exists(filename, folder);
            string path = folder+"\\"+filename;
            myfile.open(path);
            if (!myfile.is_open()) {
                throw "Func: magnetisation_vs_time_data_bulk(). File not opened with path: " + path + "\nPlease fix path";
            }
            cout << "Writing in file with path: " << path << endl;
            // begin
            cout << "Starting computations" << endl;
            // compute magnetisation
            initialise_spins_hot();
            compute_magnetisation();
            metropolis_function(T[i],thermalisationCycles);
            magn.push_back(fabs(M));
            for (int k=1; k<dataPoints; k++) {
                metropolis_function(T[i],spacingCycles);
                magn.push_back(fabs(M));
            }

            // write data on opened file
            myfile << T[i] << endl;
            for(int k=0; k<dataPoints; k++) {
              myfile << magn[k] << endl;
            }

            // wrap up
            delete[] spins;
            if (myfile.is_open()){
               myfile.close();
            }
        }
    }
}

void magnetisation_vs_temp_data()
{
    cout << "Running for magnetisation at different temperatures data" << endl;
    dim = 2;
    L = 64;
    int thermalisationCycles = 10000;
    int spacingCycles = 50;
    int dataPoints = 2000;      //total data points
    double iniT = 1.0; double finT = 5.0; int numT = 41;
    T = linspace(iniT, finT, numT);
    print_all_parameters(thermalisationCycles, dataPoints, spacingCycles, numT, 0);
    cout << "Proceed with default parameters? Enter 1 for YES, 0 for NO\n";
    int user_input = user_integer_input(0,1);
    if (user_input == 0) {
        cout << "Dimensions (2-3):\n";
        dim = user_integer_input(2,3);
        cout << "Lattice size (8-256):\n";
        L = user_integer_input(8,256);
        cout << "Thermalisation cycles (0-5000):\n";
        thermalisationCycles = user_integer_input(0,5000);
        cout << "Number of data points (100-10000):\n";
        dataPoints = user_integer_input(100,10000);
        cout << "Initial Temperature (1-49) i.e enter 15 for 1.5 Kelvin:\n";
        int iniT_int = user_integer_input(1,49);
        iniT = double(iniT_int)/10;
        cout << "Final Temperature (" << iniT_int <<"-50):\n";
        int finT_int = user_integer_input(iniT_int,50);
        finT = double(finT_int)/10;
        if (iniT_int != finT_int) {
            cout << "Number of Temperature points (2-41).\n";
            numT = user_integer_input(2,41);
        }
        else {
            numT = 1;
        }
        T = linspace(iniT, finT, numT);
    }

    // initialise vectors, pointers and time variable
    vector<double> magn;
    vector<double> err_magn;
    double *arrayM;
    double *bootstrap_values;
    clock_t tStartTemp;

    //initialise spins array and neighbours maps
    initialise_system_and_maps();

    // open file
    ofstream myfile;
    string folder = ".\\data\\magn_data";
    string filename = "magn_data_"+to_string(dim)+"D_"+to_string(L)+".txt";
    filename_rename_if_exists(filename, folder);
    string path = folder+"\\"+filename;
    myfile.open(path);
    if (!myfile.is_open()) {
        throw "Func: magnetisation_vs_temp_data(). File not opened with path: "+path;
    }
    cout << "Writing in file with path: " << path << endl;
    cout << "Starting computations" << endl;
    // compute data
    for (int i = 0; i<numT; i++) {
        cout << "T = " << T[i] << endl;
        tStartTemp = clock();
        arrayM = new double[dataPoints];
        // begin
        initialise_spins_hot();
        compute_magnetisation();
        metropolis_function(T[i],thermalisationCycles);
        arrayM[0] = fabs((double)M/N);
        for (int j=1; j<dataPoints; j++) {
            metropolis_function(T[i],spacingCycles);
            arrayM[j] = fabs((double)M/N);
        }
        // average and error
        bootstrap_values = bootstrap_error(arrayM, dataPoints, 128, true);
        magn.push_back(bootstrap_values[0]);
        err_magn.push_back(bootstrap_values[1]);
        // wrap up
        delete[] arrayM;
        delete[] bootstrap_values;
        cout << "Time taken: " << (double)(clock()-tStartTemp)/CLOCKS_PER_SEC << " seconds" << endl;
    }

    // write data in myfile
   for(int i = 0; i < numT; i++) {
      myfile << T[i] << " " << magn[i] << " " << err_magn[i] << endl;
   }

   // wrap up
   delete[] spins;
   delete[] T;
   if (myfile.is_open()){
       myfile.close();
   }
}

/*
AUTOCORRELATION
Firstly, autocorrelation_initial_investigation() will investigate the autcorrelation function and it's behaviour over a wide range of temperatures and over a "long time". A single value will be recorded at each temperature where the autocorr drops by 1/e, tau_e.

Secondly I will focus my attention around the peak, T_c ~ 2.27 K, where I will attempt to get an estimate of the time lag tau_e by computing the value multiple times.

In the paper M. P. Nightingale and H. W. J. Blöte, Phys. Rev. Lett. 76 (1996) they found: "from a finite-size scaling analysis of these autocorrelation times, the dynamic critical exponent z is determined as z = 2.1665 (12)" where "tau_e ∼ L^z at the incipient critical point."

PLOT 1:
Tau_e vs temperature for range 1-5 Kelvin. I will collect data for a range of Lattice sizes. This will give rough idea of what to expect

PLOT 2,3:
For each L and T compute multiple tau_e's around critical point to get average (up to the point where computational time is too much of a cost). Fit a gaussian on each L to get the peak values. Then plot tau_e_peak vs Lattice size and fit a*L^z to get a relationship.
*/

void autocorrelation_initial_investigation()
{
    cout << "Running autocorrelation initial investigation" << endl;
    L = 16;
    dim = 2;
    int thermalisationCycles = 15000; // enough to ensure thermalisation
    int dataPoints = 5000; // "long time"
    int spacingCycles = 1;
    double iniT = 1.5; double finT = 4.5; int numT = 31;
    T = linspace(iniT, finT, numT);
    print_all_parameters(thermalisationCycles, dataPoints, spacingCycles, numT, 0);
    cout << "Proceed with default parameters? Enter 1 for YES, 0 for NO\n";
    int user_input = user_integer_input(0,1);
    if (user_input == 0) {
        cout << "Dimensions (2-3):\n";
        dim = user_integer_input(2,3);
        cout << "Lattice size (8-256):\n";
        L = user_integer_input(8,256);
        cout << "Thermalisation cycles (0-5000):\n";
        thermalisationCycles = user_integer_input(0,5000);
        cout << "Number of data points (100-10000):\n";
        dataPoints = user_integer_input(1000,20000);
        cout << "Initial Temperature i.e enter 15 for 1.5 Kelvin (1-49):\n";
        int iniT_int = user_integer_input(1,49);
        iniT = double(iniT_int)/10;
        cout << "Final Temperature (" << iniT_int <<"-50):\n";
        int finT_int = user_integer_input(iniT_int,50);
        finT = double(finT_int)/10;
        if (iniT_int != finT_int) {
            cout << "Number of Temperature points (2-41).\n";
            numT = user_integer_input(2,41);
        }
        else {
            numT = 1;
        }
        T = linspace(iniT, finT, numT);
    }

    //initialise vectors, pointers and time variable
    double *arrayM;
    double *bootstrap_values;
    double *autocorr;
    int tau_e;
    clock_t tStartTemp;
    //initialise spins array and neighbours maps
    initialise_system_and_maps();

    // open myfile -> record: T , tau_e
    ofstream myfile;
    string folder = ".\\data\\autocorrelation_data\\initial_investigation";
    string filename = "autocorr_times_"+to_string(L)+"_L.txt";
    filename_rename_if_exists(filename, folder);
    string path = folder+"\\"+filename;
    myfile.open(path);
    if (!myfile.is_open()) {
        throw "Func: autocorrelation_initial_investigation(). File not opened with path: " + path + "\nPlease fix path";
    }
    cout << "Writing in file with path: " << path << endl;

    // myfile2 -> bulk data, for each L and T record: magn, autocorr
    ofstream myfile2;
    string folder2 = ".\\data\\autocorrelation_data\\initial_investigation\\bulkdata";
    string filename2;
    string path2;

    // begin
    arrayM = new double[dataPoints];
    cout << "Starting computations" << endl;
    for (int i = 0; i<numT; i++) {
        tStartTemp = clock();
        // open myfile2
        filename2 = "autocorr_times_"+to_string(L)+"_L_"+to_string(T[i])+".txt";
        filename_rename_if_exists(filename2, folder2);
        path2 = folder2+"\\"+filename2;
        myfile2.open(path2);
        if (!myfile2.is_open()) {
            throw "Func: autocorrelation_initial_investigation(). File not opened with path: " + path2 + "\nPlease fix path";
        }

        // magnetisation array
        initialise_spins_cold();
        compute_magnetisation();
        metropolis_function(T[i],thermalisationCycles);
        arrayM[0] = fabs((double)M/N);
        for (int j=1; j<dataPoints; j++) {
            metropolis_function(T[i],spacingCycles);
            arrayM[j] = fabs((double)M/N);
        }

        // autocorrelation array
        autocorr = autocorrelation(arrayM, dataPoints);
        tau_e = dataPoints;
        for (int j = 0; j < dataPoints; j++){
           if (autocorr[j] <= exp(-1)){
               tau_e = j;
               break;
           }
        }

        // write in myfile2
        for (int j = 0; j < dataPoints; j++){
            myfile2 << arrayM[j] << " " << autocorr[j] << endl;
        }
        // close myfile2
        if (myfile2.is_open()){
            myfile2.close();
        }

        // write in myfile -> tau_e vs temp
        myfile << T[i] << " " << tau_e << endl;

        cout << "T = " << T[i] << ", tau_e = " << tau_e << ", Time: " << (double)(clock()-tStartTemp)/CLOCKS_PER_SEC << " seconds" << endl;
    }

    // wrap up
    delete[] arrayM;
    delete[] bootstrap_values;
    delete[] autocorr;
    if (myfile.is_open()){
        myfile.close();
    }
}

void autocorrelation_peak_investigation()
{
    cout << "Running autocorrelation around critical temperature. High computation time! Proceed with caution!" << endl;
    L = 20;
    dim = 2;
    int thermalisationCycles = 15000; // enough time to ensure thermalisation
    int dataPoints = 5000; //"long time", less than before but still long enough
    int spacingCycles = 1;
    double iniT = 2.23; double finT = 2.37; int numT = 8;
    T = linspace(iniT, finT, numT);
    print_all_parameters(thermalisationCycles, dataPoints, spacingCycles, numT, 0);
    cout << "Proceed with default parameters? Enter 1 for YES, 0 for NO\n";
    int user_input = user_integer_input(0,1);
    if (user_input == 0) {
        cout << "Dimensions (2-3):\n";
        dim = user_integer_input(2,3);
        cout << "Lattice size (8-256):\n";
        L = user_integer_input(8,256);
        cout << "Thermalisation cycles (0-15000):\n";
        thermalisationCycles = user_integer_input(0,15000);
        cout << "Number of data points (100-20000):\n";
        dataPoints = user_integer_input(1000,20000);
        cout << "Initial Temperature i.e enter 150 for 1.50 Kelvin (150-440):\n";
        int iniT_int = user_integer_input(150,440);
        iniT = double(iniT_int)/100;
        cout << "Final Temperature (" << iniT_int <<"-450):\n";
        int finT_int = user_integer_input(iniT_int,450);
        finT = double(finT_int)/100;
        if (iniT_int != finT_int) {
            cout << "Number of Temperature points (2-41).\n";
            numT = user_integer_input(2,41);
        }
        else {
            numT = 1;
        }
        T = linspace(iniT, finT, numT);
    }
    // number of tau_e's to compute and get average
    int numTau;
    cout << "Enter number of tau_e's to compute and average over. Beware of high computation time! (1-1000):\n";
    numTau = user_integer_input(1,1000);

    //initialise vectors, pointers and time variable
    double *arrayM;
    double *bootstrap_values;
    double *autocorr;
    double *arrayTau;
    int tau_e;
    observable O_tau;
    int t_max = dataPoints;
    clock_t tStartTemp;

    //initialise spins array and neighbours maps
    initialise_system_and_maps();
    print_all_parameters(thermalisationCycles, dataPoints, spacingCycles, numT, 0);
    cout << "Running autocorrelation peak data for L = " << L << endl;

    string folder = ".\\data\\autocorrelation_data\\peak_investigation";
    // open myfile -> average and std
    ofstream myfile;
    string filename = "autocorr_peak_"+to_string(L)+"_L.txt";
    filename_rename_if_exists(filename, folder);
    string path = folder+"\\"+filename;
    myfile.open(path);
    if (!myfile.is_open()) {
        throw "Func: autocorrelation_peak_investigation(). File not opened with path: " + path + "\nPlease fix path";
    }
    cout << "Writing in file with path: " << path << endl;

    // open myfile2 -> all computed values (to check what is going on)
    ofstream myfile2;
    string filename2 = "autocorr_peak_"+to_string(L)+"_L_allTaus.txt";
    filename_rename_if_exists(filename2,folder);
    string path2 = folder+"\\"+filename2;
    myfile2.open(path2);
    if (!myfile2.is_open()) {
        throw "Func: autocorrelation_peak_investigation(). File not opened with path: " + path2 + "\nPlease fix path";
    }
    cout << "Writing in file with path: " << path2 << endl;

    // begin
    arrayTau = new double[numTau];
    arrayM = new double[dataPoints];
    cout << "Starting computations" << endl;
    for (int i = 0; i<numT; i++) {
        tStartTemp = clock();
        for (int k=0; k<numTau; k++) {

            // compute magnetisation
            initialise_spins_cold();
            compute_magnetisation();
            metropolis_function(T[i],thermalisationCycles);
            arrayM[0] = fabs((double)M/N);
            for (int j=1; j<dataPoints; j++) {
                metropolis_function(T[i],spacingCycles);
                arrayM[j] = fabs((double)M/N);
            }

            // identify tau_e
            autocorr = autocorrelation(arrayM,t_max);
            tau_e = t_max;
            for (int j = 0; j < t_max; j++) {
               if (autocorr[j] <= exp(-1)) {
                   tau_e = j;
                   break;
               }
            }

            // print out, write in myfile2, put in array
            cout << "(" << k+1 << "/" << numTau << ") T = " << T[i] <<  ", tau = " << tau_e << endl;
            myfile2 << T[i] << " " << tau_e << endl;
            arrayTau[k] = double(tau_e);

        }
        //compute average and sigma of arrayTau
        O_tau = compute_average_and_sigma(arrayTau, numTau);

        // write in myfile
        myfile << T[i] << " " << O_tau.value << " " << O_tau.error << endl;
        cout << "\rL = " << L << ", T = " << T[i] << ", tau = " << O_tau.value << " +- " << O_tau.error << ", Time: " << (double)(clock()-tStartTemp)/CLOCKS_PER_SEC << " seconds" << endl;
    }

    // wrap up
    delete[] arrayM;
    delete[] arrayTau;
    delete[] bootstrap_values;
    delete[] autocorr;
    if (myfile.is_open()){
        myfile.close();
    }
    if (myfile2.is_open()){
        myfile2.close();
    }
}

/*
ENERGY AND SPECIFIC HEAT CAPACITY
Firstly I need create a function to measure the energy of the system. The Hamiltonian H = -J*Sum_<ij>{s_i*s_j} - mu*H*Sum_i{s_i} - n2n term. The real question here is if I want to compute the energy manually by calling the function every time I need the E value, or if I want the metropolis_function to update the energy for me every time (like it does with magnetisation). I don't tink I will need the energy after every MC sweep so I will call to compute it when I need it's value.

Before computing any data I need to manual check that the algorithm iterates over all the edges with no double counting. I will check this with a function, energy_first_check(), and some couts in my compute_energy() function (will comment out after test is passed). TEST PASSED FOR DIM 2 AND 3 :D

Now that the 1st test was passed I will continue without next to nearest neighbours for the time being. Next step: plot Energy vs Temperature. I expect to see the the average energy rising from its maximum negative value to 0 as the temperature rises. The average energy is e = E/(nearest_links + next2nearest_links*J') where the denominator is the minimum energy the system can achieve. I believe average_energy() will be necessary for the plot.

*/

void energy_first_check() {
    cout << "Running for energy of evolving system, check one" << endl;
    dim = 2;
    n2n = 1;
    L = 3;
    int thermalisationCycles = 0;
    int dataPoints = 5;
    int spacingCycles = 10;
    double Temp = 1.5;
    //initialise spins array and neighbours maps
    initialise_system_and_maps();
    //print out nearest and nexttonearest neightbours
    print_mapOfNearest(100);
    print_mapOfNext2Nearest(100);

    initialise_spins_hot();
    //initialise_spins_cold();
    compute_magnetisation();
    print_spins_2D();
    compute_energy();
    cout << "Energy = " << E << ", Mangetisation = " << M << endl;
    for (int i=1; i<dataPoints; i++) {
        metropolis_function(Temp,spacingCycles);
        print_spins_2D();
        compute_energy();
        cout << "Energy = " << E << ", Mangetisation = " << M << endl;
    }
}

void energy_second_check() {
    cout << "Running for energy of evolving system, check two" << endl;
    dim = 2;
    L = 6;
    int thermalisationCycles = 1000;
    int dataPoints = 10;
    int spacingCycles = 1;
    double Temp = 5;
    //initialise spins array and neighbours maps
    initialise_system_and_maps();
    print_all_parameters(thermalisationCycles, dataPoints, spacingCycles, 0, Temp);
    cout << N_links << endl;
    initialise_spins_hot();
    metropolis_function(Temp,thermalisationCycles);
    //initialise_spins_cold();
    print_spins_2D();
    compute_magnetisation();
    compute_energy();
    cout << "E = " << E << ", e = " << average_energy() << ", M = " << M << ", m = " << fabs((double)M/N) << endl;
    for (int i=1; i<dataPoints; i++) {
        metropolis_function(Temp,spacingCycles);
        if (i==8){
            print_spins_2D();
        }
        compute_energy();
        cout << "E = " << E << ", e = " << average_energy() << ", M = " << M << ", m = " << fabs((double)M/N) << endl;
    }
}

void energy_vs_time_data() {
    cout << "Running for energy at different temperatures data" << endl;
    dim = 2;
    L = 80;
    int thermalisationCycles = 0;
    int spacingCycles = 1;
    int dataPoints = 2000;      //total data points
    double iniT = 1.0; double finT = 5.0; int numT = 21;
    T = linspace(iniT, finT, numT);
    print_all_parameters(thermalisationCycles, dataPoints, spacingCycles, numT, 0);
    cout << "Proceed with default parameters? Enter 1 for YES, 0 for NO\n";
    int user_input = user_integer_input(0,1);
    if (user_input == 0) {
        cout << "Dimensions (2-3):\n";
        dim = user_integer_input(2,3);
        cout << "Lattice size (8-256):\n";
        L = user_integer_input(8,256);
        cout << "Thermalisation cycles (0-10000):\n";
        thermalisationCycles = user_integer_input(0,10000);
        cout << "Number of data points (100-10000):\n";
        dataPoints = user_integer_input(100,10000);
        cout << "Initial Temperature (1-49) i.e enter 15 for 1.5 Kelvin:\n";
        int iniT_int = user_integer_input(1,49);
        iniT = double(iniT_int)/10;
        cout << "Final Temperature (" << iniT_int <<"-50):\n";
        int finT_int = user_integer_input(iniT_int,50);
        finT = double(finT_int)/10;
        if (iniT_int != finT_int) {
            cout << "Number of Temperature points (2-41).\n";
            numT = user_integer_input(2,41);
        }
        else {
            numT = 1;
        }
        T = linspace(iniT, finT, numT);
    }

    clock_t tStartTemp;
    double e_ave;

    //initialise spins array and neighbours maps
    initialise_system_and_maps();

    // initiate file
    ofstream myfile;
    string folder = ".\\data\\energy_data\\vs_time";

    cout << "Starting computations" << endl;
    // compute data
    for (int i = 0; i<numT; i++) {
        tStartTemp = clock();
        //open file
        string filename= "energy_data_"+to_string(dim)+"D_"+to_string(L)+"_"+to_string(T[i])+".txt";
        filename_rename_if_exists(filename, folder);
        string path = folder+"\\"+filename;
        myfile.open(path);
        if (!myfile.is_open()) {
            throw "Func: energy_vs_temp_data(). File not opened with path: "+path;
        }
        // begin
        initialise_spins_hot();
        compute_magnetisation(); // why not
        metropolis_function(T[i],thermalisationCycles);
        compute_energy();
        myfile << average_energy() << endl;
        //arrayE[0] = average_energy();
        for (int j=1; j<dataPoints; j++) {
            metropolis_function(T[i],spacingCycles);
            compute_energy();
            //arrayE[j] = average_energy();
            myfile << average_energy() << endl;
        }

        if (myfile.is_open()){
            myfile.close();
        }
    }

   // wrap up
   //delete[] arrayE;
   delete[] spins;
   delete[] T;
}

void energy_vs_temp_data() {
    cout << "Running for energy at different temperatures data" << endl;
    dim = 2;
    L = 128;
    int thermalisationCycles = 5000;
    int spacingCycles = 50;
    int dataPoints = 1000;      //total data points
    double iniT = 1.4; double finT = 1.5; int numT = 2;
    T = linspace(iniT, finT, numT);
    print_all_parameters(thermalisationCycles, dataPoints, spacingCycles, numT, 0);
    cout << "Proceed with default parameters? Enter 1 for YES, 0 for NO\n";
    int user_input = user_integer_input(0,1);
    if (user_input == 0) {
        cout << "Dimensions (2-3):\n";
        dim = user_integer_input(2,3);
        cout << "Lattice size (8-256):\n";
        L = user_integer_input(8,256);
        cout << "Thermalisation cycles (0-10000):\n";
        thermalisationCycles = user_integer_input(0,10000);
        cout << "Number of data points (100-10000):\n";
        dataPoints = user_integer_input(100,10000);
        cout << "Initial Temperature (1-49) i.e enter 15 for 1.5 Kelvin:\n";
        int iniT_int = user_integer_input(1,49);
        iniT = double(iniT_int)/10;
        cout << "Final Temperature (" << iniT_int <<"-50):\n";
        int finT_int = user_integer_input(iniT_int,50);
        finT = double(finT_int)/10;
        if (iniT_int != finT_int) {
            cout << "Number of Temperature points (2-41).\n";
            numT = user_integer_input(2,41);
        }
        else {
            numT = 1;
        }
        T = linspace(iniT, finT, numT);
    }

    // pointers and time variable
    double *arrayE;
    double *bootstrap_values;
    clock_t tStartTemp;
    arrayE = new double[dataPoints];

    //initialise spins array and neighbours maps
    initialise_system_and_maps();

    // open file
    ofstream myfile;
    string folder = ".\\data\\energy_data\\vs_temp";
    string filename = "energy_data_"+to_string(dim)+"D_"+to_string(L)+".txt";
    filename_rename_if_exists(filename, folder);
    string path = folder+"\\"+filename;
    myfile.open(path);
    if (!myfile.is_open()) {
        throw "Func: energy_vs_temp_data(). File not opened with path: "+path;
    }
    cout << "Writing in file with path: " << path << endl;
    cout << "Starting computations" << endl;
    // compute data
    for (int i = 0; i<numT; i++) {
        tStartTemp = clock();
        // begin
        initialise_spins_auto(T[i]);
        compute_magnetisation();
        metropolis_function(T[i],thermalisationCycles);
        compute_energy();
        arrayE[0] = average_energy();
        for (int j=1; j<dataPoints; j++) {
            metropolis_function(T[i],spacingCycles);
            compute_energy();
            arrayE[j] = average_energy();
        }
        // average and error
        bootstrap_values = bootstrap_error(arrayE, dataPoints, 128, true);

        //write in file and cout things
        myfile << T[i] << " " << bootstrap_values[0] << " " << bootstrap_values[1] << endl;
        cout << "T = " << T[i] << ", E = " << bootstrap_values[0] << " +- " << bootstrap_values[1] << ", Time taken: " << (double)(clock()-tStartTemp)/CLOCKS_PER_SEC << " seconds" << endl;
    }

   // wrap up
   delete[] arrayE;
   delete[] spins;
   delete[] T;
   if (myfile.is_open()){
       myfile.close();
   }
}

int initial_menu()
{
    cout << "Please select an analysis by entering an integer:\n";
    cout << "1 for Magnetisation vs Time.\n";
    cout << "2 for Magnetisation vs Time (Bulk data, different lattice sizes and temperatures).\n";
    cout << "3 for Magnetisation vs Temperature.\n";
    cout << "4 for Tau_e vs Temperature initial investigation.\n";
    cout << "5 for Tau_e vs Temperature close to critical temperature.\n";
    cout << "6 for Energy vs Time.\n";
    cout << "7 for Energy vs Temperature\n";
    cout << "0 to Exit the program.\n";
    int choice = user_integer_input(0,7);
    return choice;
}

int main(int argc, char** argv)
{
    clock_t tStart = clock();
    srand(seed);
    cout << "Ising Model, made by Savvas Shiakas (ss2477)" << endl;
    cout << "C++ was used to produce data files ('low' level -> faster computation) which are later analysed and plotted using Python.\n\n";

    // user friendly function to run the code
    int user_choice = initial_menu();
    try {
        switch(user_choice) {
            case 1: magnetisation_vs_time_data();
            break;
            case 2: magnetisation_vs_time_data_bulk();
            break;
            case 3: magnetisation_vs_temp_data();
            break;
            case 4: autocorrelation_initial_investigation();
            break;
            case 5: autocorrelation_peak_investigation();
            break;
            case 6: energy_vs_time_data();
            break;
            case 7: energy_vs_temp_data();
            break;
        }
        cout << "\nOperation complete.";
    }
    catch(string er) {
        cerr << "ERROR. EXITING PROGRAM." << endl;
        cout << er << endl;
        return 1;
    }


    cout << "\nProgram ended in " << (double)(clock()-tStart)/CLOCKS_PER_SEC << " seconds" << endl;
    return 0;
}
