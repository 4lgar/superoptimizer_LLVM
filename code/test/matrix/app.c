#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define KB (1 << 10)
#define MB (1 << 20)

/* Customizable */
#define RANDOM_DOUBLE_MAX 10.0

#define MATRIX_DIM 1000

#define RUNS 5
#define ITERATIONS 10


#define DATA_FILE "/dev/urandom"

struct matrix {
    size_t row;
    size_t col;

    double **data;
};


/* returns the transpose matrix of m1 in m2.
 * returns false if dimensions incorrect */
static bool matrix_transpose(const struct matrix *m1, struct matrix *m2)
{
    if (m1->col != m2->row)
        return false;
    if (m1->row != m2->col)
        return false;

    for (size_t i = 0; i < m1->row; i++)
        for (size_t j = 0; j < m1->col; j++)
            m2->data[j][i] = m1->data[i][j];

    return true;
}

/* returns true if matrices dimensions correct for "m_res = m1 * m2", false otherwise */
static bool matrix_mult_check(const struct matrix *m1, const struct matrix *m2, const struct matrix *m_res)
{
    /* checking m1 & m2 dimensions */
    if (m1->col != m2->row)
        return false;
    /* checking m_res dimensions */
    if (m_res->row != m1->row)
        return false;
    if (m_res->col != m2->col)
        return false;

    return true;
}

/* performs matrix multiplication: m_res = m1 * m2
 * result stored in m_res (must be previously allocated)
 * returns false if matrices don't have correct dimensions */
static bool matrix_mult(const struct matrix *m1, const struct matrix *m2, struct matrix *m_res)
{
    if (!matrix_mult_check(m1, m2, m_res))
        return false;

    for (size_t i = 0; i < m_res->row; i++)
        for (size_t j = 0; j < m_res->col; j++)
            m_res->data[i][j] = 0.0;

    for (size_t i = 0; i < m1->row; i++)
        for (size_t k = 0; k < m1->col; k++)
            for (size_t j = 0; j < m2->col; j++)
                m_res->data[i][j] += m1->data[i][k] * m2->data[k][j];

    return true;
}

/* allocates matrix memory */
struct matrix *matrix_init(const size_t row, const size_t col)
{
    struct matrix *m;

    /* matrix struct allocation */
    m = malloc(sizeof(struct matrix));
    if (!m) {
        fprintf(stderr, "[%s] malloc error\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    m->row = row;
    m->col = col;

    /* matrix rows allocation */
    m->data = (double **)malloc(sizeof(double *) * m->row);
    if (!m->data) {
        fprintf(stderr, "[%s] malloc error\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    /* matrix columns allocation */
    for (size_t i = 0; i < m->row; i++) {
        m->data[i] = (double *)malloc(sizeof(double) * m->col);

        if (!m->data[i]) {
            fprintf(stderr, "[%s] malloc error\n", __func__);
            perror(__func__);
            exit(EXIT_FAILURE);
        }
    }

    return m;
}

/* initializes matrix with random values in range [0 .. RANDOM_DOUBLE_MAX] */
static void matrix_data_init(struct matrix *m)
{
    FILE *fdata;

    fdata = fopen(DATA_FILE, "rb");
    if (!fdata) {
        fprintf(stderr, "[%s] fopen error\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    /* matrix data init */
    for (size_t i = 0; i < m->row; i++) {
        for (size_t j = 0; j < m->col; j++) {
            /* fill m->data with random values */
            uint64_t rand;

            if (fread(&rand , sizeof (uint64_t), 1, fdata) != 1) {
                fprintf(stderr, "[%s] fread error\n", __func__);
                exit(EXIT_FAILURE);
            }

            /* restrict them to range [0 .. RANDOM_DOUBLE_MAX] */
            m->data[i][j] = (double)rand / (double)UINTMAX_MAX * RANDOM_DOUBLE_MAX; 
        }
    }
}

/* frees matrix memory */
void matrix_clear(struct matrix *m)
{
    for (size_t i = 0; i < m->row; i++)
        free(m->data[i]);

    free(m->data);
    free(m);
}

/* stores a non-singular square matrix in m2 created from m1.
 * m2 must be previously allocated and of dimensions m1->row x m1->row. 
 * returns false if incorrect matrix dimensions. */
static bool matrix_make_invertible(const struct matrix *m_src, struct matrix *m_dst)
{
    struct matrix *m1_transposed;

    if (m_dst->row != m_dst->col)
        return false;
    if (m_dst->row != m_src->row)
        return false;

    m1_transposed = matrix_init(m_src->col, m_src->row);

    if (!matrix_transpose(m_src, m1_transposed))
        return false;

    /* m_dst = m_src*m_src' and is guaranteed to be invertible */
    if (!matrix_mult(m_src, m1_transposed, m_dst))
        return false;

    matrix_clear(m1_transposed);
    return true;
}

/* returns a pointer to an allocated invertible square matrix
 * dim: square matrix dimensions */
struct matrix *matrix_init_invertible(const size_t dim)
{
    struct matrix *tmp, *invertible;

    tmp = matrix_init(dim, dim);
    invertible = matrix_init(dim, dim);

    matrix_data_init(tmp);
    matrix_make_invertible(tmp, invertible);

    matrix_clear(tmp);
    return invertible;
}


void matrix_swipe_lines(struct matrix *m, size_t index1, size_t index2)
{
    m->data[index2]     = (double*)((unsigned long)m->data[index2] ^ (unsigned long)m->data[index1]);
    m->data[index1]     = (double*)((unsigned long)m->data[index2] ^ (unsigned long)m->data[index1]);
    m->data[index2]     = (double*)((unsigned long)m->data[index2] ^ (unsigned long)m->data[index1]);
}

/*
* Reduce a line to have a 1 in the [line][cell]
*/
double matrix_reduce_line(struct matrix *m, size_t line, size_t cell){
    size_t i;
    double toReturn;

    toReturn = m->data[line][cell];

    for(i = 0; i < MATRIX_DIM; i++){
        m->data[line][i] /= toReturn;
    }

    return toReturn;
}

/*
* Divide a line by coef
*/
void matrix_divide_line_by_coef(struct matrix *m, size_t line, double coef){
    size_t i;

    for(i = 0; i < MATRIX_DIM; i++){
        m->data[line][i] /= coef;
    }  
}

/*
* Do a line1 = line1 - line2 on m matrix
*/
void matrix_substracte_lines(struct matrix *m, size_t line1, size_t line2){
    size_t i;

    for(i = 0; i < MATRIX_DIM; i++){
        m->data[line1][i] -= m->data[line2][i];
    }
}


/* performs matrix inversion.
 * result stored in m2 (must be previously allocated)
 * returns false if matrice isn't invertible */
bool matrix_inv(const struct matrix *m_src, struct matrix *m_dst)
{
    /*Usefull variables*/
    int i = 0;
    int j = 0;
    int k = 0;
    int matrixSize = MATRIX_DIM;
    int *magicalTrick = &i;
    double coef;

    struct matrix *m_work = matrix_init(MATRIX_DIM, MATRIX_DIM);

    /*Init m_dst like an unitary matrix and copy m_src*/
    for(; i < MATRIX_DIM; i++){
        for(j = 0; j < MATRIX_DIM; ++j){
            m_work->data[i][j]  = m_src->data[i][j];
            m_dst->data[i][j]   = 0;
        }
        m_dst->data[i][i] = 1.0;
    }

    /*Swap lines that have 0 in diag*/
    for(i = 0; i < MATRIX_DIM; i++){
        /*TODO: Seems to be an unlikely condition*/
        if(m_work->data[i][i] == 0){
            /*Search a line that could swap with this one*/
            /*
                Critera: the next line should not have 0 in the same cell OR
                the line to swap should not have 0 in the cell that corresponding to
                the diag of the target line
                TODO: Manage if the line or col is full of 0 or if the last one is a 0 or something like that...
            */
            j = i + 1;
            while((m_work->data[j][i] == 0 || m_work->data[i][j] == 0) && j < MATRIX_DIM){j++;}

            /*Swap lines*/
            matrix_swipe_lines(m_work, i, j);
            matrix_swipe_lines(m_dst, i, j);

            /*Update magical variable*/
            magicalTrick = &matrixSize;
        }
    }

    //printf("addr i : 0x%X \naddr matrixSize : 0x%X \nadd magicalTrick : 0x%X\n", &i, &matrixSize, magicalTrick);

    /*Upper triangle*/
    /*Set first cell to 1 by dividing the whole line by itself*/
    for(i = 0; i < MATRIX_DIM - 1; i++){
        coef = m_work->data[i][i];

        for(k = i; k < MATRIX_DIM; ++k){
            m_work->data[i][k] /= coef;
        }

        for(k = 0; k <= *magicalTrick; ++k){
            m_dst->data[i][k] /= coef;
        }

        for(j = i + 1; j < MATRIX_DIM; ++j){
            coef = m_work->data[j][i];

            for(k = i; k < MATRIX_DIM; ++k){
                m_work->data[j][k] -= m_work->data[i][k] * coef;
            }

            for(k = 0; k <= *magicalTrick; ++k){
                m_dst->data[j][k] -= m_dst->data[i][k] * coef;
            }
        }
    }

    /*From now, we make operation only on m_dst*/

    /*Correct the [MATRIX_DIM][MATRIX_DIM] coef*/
    coef = m_work->data[i][i];
    
    for(k = 0; k < MATRIX_DIM; ++k){
        m_dst->data[i][k] /= coef;
    }


    /*Same for lower triangle*/
    for(; i > 0; --i){
        for(j = i - 1; j >= 0; --j){
            coef = m_work->data[j][i];
            for(k = 0; k < MATRIX_DIM; ++k){
                m_dst->data[j][k] -= m_dst->data[i][k] * coef;
            }
        }
    }

    return true;
}

/* prints matrix "m" prefixed with "name" */
void matrix_print(const struct matrix *m, const char *name)
{
    for (size_t i = 0; i < m->row; i++) {
        fprintf(stderr, "\n[%s - %s] ", __func__, name);

        for (size_t j = 0; j < m->col; j++)
            fprintf(stderr, "%f ", m->data[i][j]);
    }

    fprintf(stderr, "\n");
}

/*
*   Print m in wolfram alpha style ({{0,-1},{1,0}})
*/
void matrix_print_wolfram(const struct matrix *m, const char *name){
    size_t i, j;

    printf("\n%s\n", name);

    printf("{");

    for(i = 0; i < MATRIX_DIM; i++){
        printf("{");
        for(j = 0; j < MATRIX_DIM; ++j){
            printf("%.2f", m->data[i][j]);
            if(j != MATRIX_DIM - 1){
                printf(",");
            }       
        }
        if(i == MATRIX_DIM - 1){
            printf("}");
        }else{
            printf("},");
        }
    }

    printf("}");

    printf("\n");
}

int main(void){
	struct matrix *m, *m_inverted;

	m = matrix_init_invertible(MATRIX_DIM);
	m_inverted = matrix_init(MATRIX_DIM, MATRIX_DIM);

	printf("Begin matrix_inv.\n");
	//matrix_print_wolfram(m, "SRC");

	matrix_inv(m, m_inverted);

	printf("End matrix_inv\n");
	//matrix_print(m_inverted, "DST");

	return 0;
}