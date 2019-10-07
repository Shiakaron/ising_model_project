#include "header.h"

double* linspace(double a, double b, int N){
    // similar to linspace of Matlab. Help to create an array of equally spaced N elements from a to b.
    // returns POINTER to that array
    double D =(b-a)/(N-1); // spacing
    double *ret;           // pointer
    ret = new double[N];   // initialise array

    //set in between
    for (int i = 0; i<N; i++) {
        ret[i] = a+D*i;
    }
    ret[N-1] = b;

    return ret;
}

bool file_exists(const string& p_file) {
    // 1. Opens the file identified by argument "name", associating it with the stream object,
    // 2. Check whether state of stream is good (public member function )
    ifstream infile(p_file);
    return infile.good();
}

void filename_rename_if_exists(string& filename, string& folder) {
    bool exists = file_exists(folder+"\\"+filename);
    // find dot index
    if (exists) {
        string name;
        string ending;
        int index = filename.length()-1;
        bool found_dot = false;
        while (!found_dot) {
            if (filename[index] == '.') {
                found_dot = true;
            }else if(index == 0) {
                cout << "no dot \".\" found" << endl;
            }else {
                index--;
            }
        }
        int dot_index = index;
        // break the string apart
        name = filename.substr(0,dot_index);
        ending = filename.substr(dot_index);
        // add the (number) in between the two parts
        int t = 1;
        while (exists) {
            string number = "("+to_string(t)+")";
            filename = name+number+ending;
            exists = file_exists(folder+"\\"+filename);
            t++;
        }
        cout << "file name changed to: " << filename << endl;
    }
}

double sumArray(double arr[], int siz) {
    // returns the sum of the elements of the array of size siz.
    double sum = 0.0;
    for (int i=0; i<siz; i++) {
        sum += arr[i];
    }
    return sum;
}

double averageArray(double arr[], int siz) {
    // returns the average of the elements of the array of size siz.
    double sum = sumArray(arr, siz);
    double average = sum/siz;
    return average;
}

void print_T(int siz) {
    cout << "Temperatures to compute are:" << endl;
    cout << "[ ";
    for (int i = 0; i<siz-1; i++) {
        cout << T[i] << ", ";
    }
    cout << T[siz-1] << " ]" << endl;
}

void print_mapOfNearest(){
    cout << "Map of Nearest:" << endl;
    vector<int> temp_vec;
    for (int i = 0; i<N; i++) {
        cout << i << ": ";
        temp_vec = mapOfNearest[i];
        for (int j = 0; j<temp_vec.size(); j++) {
            cout << temp_vec[j] << " ";
        }cout << endl;
    }
}

void print_mapOfNext2Nearest(){
    cout << "Map of Next2Nearest:" << endl;
    vector<int> temp_vec;
    for (int i = 0; i<N; i++) {
        cout << i << ": ";
        temp_vec = mapOfNext2Nearest[i];
        for (int j = 0; j<temp_vec.size(); j++) {
            cout << temp_vec[j] << " ";
        }cout << endl;
    }
}

void print_all_parameters(int thermalisationCycles, int dataPoints, int spacingCycles, int numT, double Temp) {
    // print out all the parameters
    cout << "Program Seed: " << seed << endl;
    cout << "Dimensions: " << dim << endl;
    cout << "Lattice size: " << L << endl;
    cout << "Thermalisation cycles: " << thermalisationCycles << endl;
    cout << "Number of data points: " << dataPoints << endl;
    cout << "Cycles in between data points: " << spacingCycles << endl;
    if (numT > 0) {
        print_T(numT);
    }
    else {
        cout << "Temperature of system: " << Temp << endl;
    }
    print_mapOfNearest();
    if (n2n!=0) {
        cout << "Next2Nearest interaction coefficient: " << n2n << endl;
        print_mapOfNext2Nearest();
    }

}

void print_spins() {
    cout << "Spins:" << endl;
    cout << "index" << "\t" << "spin" << endl;
    for (int i=0; i<N; i++) {
        cout << i << "\t" << spins[i] << endl;
    }
}

void print_array(double arr[], int siz) {
    cout << "[ ";
    for (int i = 0; i<siz-1; i++) {
        cout << arr[i] << ", ";
    }
    cout << arr[siz-1] << " ]" << endl;
}

void print_spins_2D() {
    // print spins with this index format (L = 3)
    // 6 7 8
    // 3 4 5
    // 0 1 2
    if (dim == 2) {
        int s;
        cout << "Spins: " << endl;
        for (int j=L-1; j>=0; j--) {
            for (int k=0; k<L; k++) {
                s = spins[j*L+k];
                if (s == 1) {
                    cout << 1 << " ";
                }
                else {
                    cout << 0 << " ";
                }
            }
            cout << endl;
        }
    }
    else {
        cout << "The function print_spins_2D() only works properly with a 2D system.\n";
        cout << "Currently dim = " << dim << endl;
    }

}