#include "./matches.hpp"

double findCorrelation(int U1, int U2, int NUM_MOVIES, int *matrixUI) {
// Calculate the correlation between two Users. From the formula, we've deduced
// that it finds the covariance of the ratings two Users have given to the sa-
// me Movies and then divides by the product of both Users' standard deviation.
// The most famous example of correlation is the formula by Karl Pearson here:
//      https://en.wikipedia.org/wiki/Correlation_and_dependence
    double rAvg1, rAvg2;
    rAvg1 = 0.0;
    rAvg2 = 0.0;
    for(int j = 0; j < NUM_MOVIES; j++) {
    // Add to each User's rating average the rating it has given to all Movies.
        rAvg1 += matrixUI[(U1 * NUM_MOVIES) + j];
        rAvg2 += matrixUI[(U2 * NUM_MOVIES) + j];
    }
    rAvg1 /= NUM_MOVIES;
    rAvg2 /= NUM_MOVIES;
    // Now average this rating over the total amount of Movies, this will
    // give the mean rating for each Users' ratings.
    double coVar;
    coVar = 0.0;
    // This will go in the top of the formula. We identified the upper part to
    // be exactly like the upper part of the covariance formula, but didn't
    // know where the lower part (N - 1) went, which we later found out was
    // cancelled out with stuff on the divisor. The covariance measures how
    // much two random values change together, so its usefulness in finding
    // how its related finding two Users' ratings correlation is clear.
    //     https://en.wikipedia.org/wiki/Covariance
    for(int j = 0; j < NUM_MOVIES; j++) {
        double dist1 = matrixUI[(U1 * NUM_MOVIES) + j] - rAvg1;
        double dist2 = matrixUI[(U2 * NUM_MOVIES) + j] - rAvg2;
        // Distances from each value to the mean.
        coVar += (dist1 * dist2);
        // Sum of the Distances Products
    }
    double sqSum1, sqSum2;
    sqSum1 = 0.0;
    sqSum2 = 0.0;
    // Now, we'll proceed to sum the squares of the differences between each
    // Users' rating and that User's mean rating, as described here:
    //      https://en.wikipedia.org/wiki/Total_sum_of_squares.
    for(int j = 0; j < NUM_MOVIES; j++) {
        double dist1 = matrixUI[(U1 * NUM_MOVIES) + j] - rAvg1;
        double dist2 = matrixUI[(U2 * NUM_MOVIES) + j] - rAvg2;
        // Distances from each value to the mean.
        sqSum1 += (dist1 * dist1);
        sqSum2 += (dist2 * dist2);
        // Sum of Squared Distances
    }
    // Now we have the total distances from each point to the mean, lets find
    // the square root of each of these, which basically amounts to finding the
    // the upper term of the Standard Deviation, again, the lower term went mi-
    // missing. Info about the standard deviation can be found at:
    //      https://en.wikipedia.org/wiki/Standard_deviation
    double stdDev1, stdDev2;
    stdDev1 = sqrt(sqSum1);
    stdDev2 = sqrt(sqSum2);
    // The reason why the lower terms of the covariance and the standard dev-
    // iations producs, is because they cancel out! From the covariance (upper
    // formula) one can extract a 1 / (N - 1).  From the product of standard
    // deviations (lower formulas), one can extract two 1 / sqrt(N - 1), which
    // multiplied gives another 1 / (N - 1). So these cancel out and thats it!
    if(stdDev1 == 0 || stdDev2 == 0) {
    // This is a special and VERY ODD case. It means a User gave a rating to
    // ALL Movies and ALL ratings are the same (highly unlikely but...) or a
    // User simply hasn't watched ANY movie (rating of everything is 0).
        return 0.0;
    }
    double correlation = coVar / (stdDev1 * stdDev2);
    return correlation;
}

int* findBests(int NUM_USERS, int NUM_MOVIES, int NUM_BEST,
               int U1, int *matrixUI) {
// Find the B (NUM_BEST) amount of Users that have the best correlation with U1
    double *corrList;
    corrList = new double [NUM_USERS];
    // Will store the correlation values between a User and all other Users,
    // storing just info about a particular User one User at a time, done on a
    // row by row basis instead of storing ALL correlations between all Users.
    // ...
    // corrList[U2] == Level of preference likeness between U1 and U2.
    // The likeness is represented with a value between -1 and 1.
    //    -1: Nothing alike,
    //     1: Completely alike,
    //     0: Nothing can be deduced
    // Cases for 0: Covariance is 0, or the Standard Deviation of either person
    //              is 0, which is awful for any statistical analysis.
    // ...
    // U1: The current User we are comparing.
    // U2: All other Users to compare against.
    int *recList;
    recList = new int [NUM_BEST];
    // Will contain the top B Users U1 is the most correlated with.
    for(int U2 = 0; U2 < NUM_USERS; U2++) {
        corrList[U2] = (U1 != U2)?
            findCorrelation(U1, U2, NUM_MOVIES, matrixUI) : 1.0;
        // Check that the User isn't comparing with itself to find the cor-
        // relation value, if such is the case, give a value of 1.0 (MAX).
    }
    double bestCorr;
    int bestIndx;
    // bestCorr: Variable to go on storing the current best correlation.
    // bestUser: The index of the User with the current best correlation.
    for(int k = 0; k < NUM_BEST; k++) {
    // Now, lets find the B Users that have the best correlation with User
    // i. We'll test this constantly assuming 100 Users and 5 best matches.
    // We must be careful with this B, if its a number too close to the to-
    // tal amount of Users, searching for that amount of total matches will
    // be equal to getting all the elements sorted in descending order, and
    // that amounts to a O(N^2) sort, which is VERY SLOW. Meanwhile, doing
    // a complete search several times is O(kN) with a constant factor (the
    // amount of top matches we want to get). One could simply sort all the
    // correlations for a given User in O(N LOG N) time and access the top
    // matches in constant time, but a complete search will suffice in our
    // case where we want just a small number of matches. In complexity te-
    // rms, O(kN) < O(N LOG N) < O(N^2) and for a website with a HUGE amou-
    // nt of Users, it certainly makes a difference. Lets stick with O(kN).
        bestCorr = -2;
        // Suitable default value since correlations range from -1 to 1.
        bestIndx = -1;
        // Suitable default value since indexes go from 0 to NUM_USERS - 1.
        for(int U2 = 0; U2 < NUM_USERS; U2++) {
        // Now, as it was said previously, lets do the complete search for
        // the best correlation among this User's correlation list.
            if(U1 != U2 && corrList[U2] > bestCorr){
            // Lets store the current best correlation value (and User),
            // obviously, ignoring the case where its the same User :-).
                bestCorr = corrList[U2];
                bestIndx = U2;
            }
        }
        // After the complete search is done, lets store the definitive
        // best match for User i, at position k (which ranges from 0 to B).
        recList[k] = bestIndx;
        // Store the best User index at the rank k. 0 best ranked, up to
        // B - 1 which is the less best ranked amount the top B matches.
        corrList[bestIndx] = -7;
        // Suitable value because we want to ignore this User from now on.
        // Usually a -INF value would do it, but this one does it too!.
    }
    delete corrList;
    return recList;
}