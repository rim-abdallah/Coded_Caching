#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "config.h"

using namespace std;

void readProbabilitiesCSV(
    const string& filename,
    double **probs,
    unsigned int n_users,
    unsigned int m_files
) {
    ifstream file(filename.c_str());

    if (!file.is_open()) {
        cout << "Error: Cannot open probability CSV file: " << filename << endl;
        exit(0);
    }

    string line;
    getline(file, line); // skip header

    unsigned int user = 0;

    while (getline(file, line) && user < n_users) {
        stringstream ss(line);
        string value;

        getline(ss, value, ','); // skip first column User_x

        double sum = 0.0;

        for (unsigned int f = 0; f < m_files; f++) {
            if (!getline(ss, value, ',')) {
                cout << "Error: Missing probability at user "
                     << user << ", file " << f << endl;
                exit(0);
            }

            probs[user][f] = stod(value);

            if (probs[user][f] < 0) {
                cout << "Error: negative probability at user "
                     << user << ", file " << f << endl;
                exit(0);
            }

            sum += probs[user][f];
        }

        if (sum <= 0) {
            cout << "Error: probability sum is zero for user " << user << endl;
            exit(0);
        }

        // normalize probabilities
        for (unsigned int f = 0; f < m_files; f++) {
            probs[user][f] /= sum;
        }

        user++;
    }

    if (user < n_users) {
        cout << "Error: CSV has only " << user
             << " users, but N_USERS = " << n_users << endl;
        exit(0);
    }

    file.close();
}

int main()
{
    const unsigned int n_utenti = N_USERS;
    const unsigned int m_files = N_FILES;
    const int N_RUNS = 50;


    double **probs_request =
        (double **) malloc(n_utenti * sizeof(double *));

    for (unsigned int i = 0; i < n_utenti; i++) {
        probs_request[i] =
            (double *) malloc(m_files * sizeof(double));
    }

    readProbabilitiesCSV(
        PROBA_PATH_REQUEST,
        probs_request,
        n_utenti,
        m_files
    );

    srand(time(NULL));
    unsigned long int randSeed = rand();

    gsl_rng_env_setup();
    gsl_rng_default_seed = randSeed;

    gsl_rng *r = gsl_rng_alloc(gsl_rng_default);

    ofstream out("requests_com_0_15.csv");

    if (!out.is_open()) {
        cout << "Error: Cannot create requests_real_200.csv" << endl;
        exit(0);
    }

    out << "run";
    for (unsigned int i = 0; i < n_utenti; i++) {
        out << ",user_" << i;
    }
    out << endl;

    for (int run = 1; run <= N_RUNS; run++) {
        out << run;

        for (unsigned int i = 0; i < n_utenti; i++) {

            gsl_ran_discrete_t *rand_disc =gsl_ran_discrete_preproc(m_files,probs_request[i]);

            int idx_rand = gsl_ran_discrete(r, rand_disc);

            gsl_ran_discrete_free(rand_disc);

            out << "," << idx_rand;
        }

        out << endl;
    }

    out.close();

    gsl_rng_free(r);

    for (unsigned int i = 0; i < n_utenti; i++) {
        free(probs_request[i]);
    }

    free(probs_request);

    cout << "Created requests_real_200.csv" << endl;

    return 0;
}