#include <iostream>
#include <string>
#include <omp.h>
#include <bits/stdc++.h>

using namespace std;

int MAX_THREAD;

struct matrix{
    int height, width;
    double **data;
};

void loadMatrix(matrix *m){
    cin >> m->height;
    cin >> m->width;
    m->data = new double*[m->height];
    for (int i = 0; i < m->height; i++){
        m->data[i] = new double[m->width];
        for (int j = 0; j < m->width; j++){
            cin >> m->data[i][j];
        }
    }
}

void freeMatrix(matrix *m){
	//return;
	//delete m->data;
    for (int i = 0; i < m->height; i++){
        delete[] m->data[i];
    }
	delete[] m->data;
}
void fillRandomMatrix(matrix *m, int h, int w){
    m->height = h;
    m->width = w;
    m->data = new double*[m->height];
    for (int i = 0; i < m->height; i++){
        m->data[i] = new double[m->width];
        for (int j = 0; j < m->width; j++){
            m->data[i][j] = rand();
        }
    }
    cout << m->height << " x " << m->width << " matrix" << endl;
}

void setMat(matrix *m, int h, int w){
    m->width = w;
    m->height = h;
    m->data = new double*[h];
    for (int i = 0; i < h; i++){
        m->data[i] = new double[w];
    }
}

void printMatrix(matrix *m){
    cout << m->height << " " << m->width << endl;
    for (int i = 0; i < m->height; i++){
        for (int j = 0; j < m->width; j++){
            cout << m->data[i][j] << " ";
        }
        cout << endl;
    }
}

void matrix_mult_serial(matrix *m1, matrix *m2, matrix *m3)
{
	//Dynamic Scheduler
	//memset(d,0,sizeof d);
	int i,j,k;
	double st=omp_get_wtime();
	for(i=0;i<m3->height;i++)
        for( j=0;j<m3->width;j++)
            for(k=0;k<m1->width;k++)
                m3->data[i][j]+=m1->data[i][k]*m2->data[k][j];
	double en=omp_get_wtime();
	printf("Serial ----------- %lfs\n",en-st);
	//check(n);
}

void matrix_mult_parallel1(matrix *m1, matrix *m2, matrix *m3)
{
	//Dynamic Scheduler
	//memset(d,0,sizeof d);
	int i,j,k;
	double st=omp_get_wtime();
	#pragma omp parallel for
	for(i=0;i<m3->height;i++)
        for( j=0;j<m3->width;j++)
            for(k=0;k<m1->width;k++)
                m3->data[i][j]+=m1->data[i][k]*m2->data[k][j];
	double en=omp_get_wtime();
	printf("Parallel (1)------ %lfs ",en-st);
	//check(n);
}

void matrix_mult_parallel2(matrix *m1, matrix *m2, matrix *m3)
{
	//Dynamic Scheduler
	//memset(d,0,sizeof d);
	int i,j,k;
	double st=omp_get_wtime();
	#pragma omp parallel for schedule(dynamic,50) collapse(2) private(i,j,k) shared(m1, m2, m3)
	for(i=0;i<m3->height;i++)
        for( j=0;j<m3->width;j++)
            for(k=0;k<m1->width;k++)
                m3->data[i][j]+=m1->data[i][k]*m2->data[k][j];
	double en=omp_get_wtime();
	printf("Parallel (2)------ %lfs ",en-st);
	//check(n);
}

//===============================================================================
matrix *matA;
matrix *matB;
matrix *matC;
int step_i = 0;
void* multi(void* arg)
{
    int core = step_i++;

    // Each thread computes 1/4th of matrix multiplication
    for (int i = core * matC->height / MAX_THREAD; i < (core + 1) * matC->height / MAX_THREAD; i++)
        for (int j = 0; j < matC->width; j++)
            for (int k = 0; k < matA->width; k++)
                matC->data[i][j] += matA->data[i][k] * matB->data[k][j];
}

void matrix_mult_thread(matrix *m1, matrix *m2, matrix *m3){
    // declaring four threads
    //MAX_THREAD = omp_get_max_threads();
    MAX_THREAD = thread::hardware_concurrency();
    pthread_t threads[MAX_THREAD];
    matA = m1;
    matB = m2;
    matC = m3;
    // Creating four threads, each evaluating its own part
    double st=omp_get_wtime();
    for (int i = 0; i < MAX_THREAD; i++) {
        int* p;
        pthread_create(&threads[i], NULL, multi, (void*)(p));
    }

    // joining and waiting for all threads to complete
    for (int i = 0; i < MAX_THREAD; i++)
        pthread_join(threads[i], NULL);

    double en=omp_get_wtime();
	printf("Threads  (3)------ %lfs ",en-st);

}

bool isMatrixCorrect(matrix *m, matrix *m2){
    for (int i = 0; i < m->height; i++){
        for (int j = 0; j < m->width; j++){
            if (m->data[i][j] != m2->data[i][j])
                return false;
        }
    }
    return true;
}
void help(){
	cout << "MATRIX MULTIPLICATION================================================================" << endl;
	cout << "This is a simple program for matrix multiplication." << endl;
	cout << endl;
	cout << "INPUT:" << endl;
	cout << "If you start this program, it will ask you to input two matrices in following format:" << endl;
	cout << "height width" << endl;
	cout << "1st row" << endl;
	cout << "......." << endl;
	cout << "nth row" << endl;
	cout << "example:" << endl;
	cout << "3 3" << endl;
	cout << "1 2 3" << endl;
	cout << "4 5 6" << endl;
	cout << "7 8 9" << endl;
	cout << "This programs main purpose is to show some parallel ways to multiplicate matrices." << endl;
	cout << endl;
	cout << "OUTPUT:" << endl;
	cout << "It shows time of each action and at the end lets you choose if you want the solution to be printed." << endl;
	cout << "First way is simply serial multiplication." << endl;
	cout << "Second one is just simple use of omp library:\n#omp parallel for" << endl;
	cout << "Third is optimized parallel multiplication using omp library:\n#pragma omp parallel for schedule(dynamic,50) collapse(2) private(i,j,k) shared(m1, m2, m3)" << endl;
	cout << "Fourt way uses only threads with no special libraries." << endl;
}
int main(int argc, char *argv[]){
	if (argc == 2){
		string h = argv[1];
		if (h.compare("--help") == 0){
			help();
			return 0;
		}
		cout << "error: wrong argument, type --help to learn how to use this program" << endl;
		return 1;
	}
	else if (argc > 2){
		cout << "error: too many arguments, type --help to learn how to use this program" << endl;
		return 2;
	}
    matrix *m1 = new matrix;
    matrix *m2 = new matrix;
	cout << "Enter first matrix:" << endl;
	loadMatrix(m1);
	cout << "Enter second matrix:" << endl;
    loadMatrix(m2);
    //fillRandomMatrix(m1, 10, 10);
    //fillRandomMatrix(m2, 10, 10);

    matrix *mCorrectAnswer = new matrix;
    setMat(mCorrectAnswer, m1->height, m2->width);
    matrix_mult_serial(m1, m2, mCorrectAnswer);
    //printMatrix(m3);

    matrix *m3 = new matrix;
    setMat(m3, m1->height, m2->width);
    matrix_mult_parallel1(m1, m2, m3);
    cout << (isMatrixCorrect(mCorrectAnswer, m3) ? "correct":"incorrect") << endl;
	freeMatrix(m3);

    setMat(m3, m1->height, m2->width);
    matrix_mult_parallel2(m1, m2, m3);
    cout << (isMatrixCorrect(mCorrectAnswer, m3) ? "correct":"incorrect") << endl;
	freeMatrix(m3);

    setMat(m3, m1->height, m2->width);
    matrix_mult_thread(m1, m2, m3);
    cout << (isMatrixCorrect(mCorrectAnswer, m3) ? "correct":"incorrect") << endl;
    //printMatrix(m3);
	cout << "Do you want to print solution? [y/n] ";
	char sol;
	cin >> sol;
	if (sol == 'y')
		printMatrix(m3);
	
	freeMatrix(m1);
	freeMatrix(m2);
	freeMatrix(m3);
	freeMatrix(mCorrectAnswer);
	return 0;
}
