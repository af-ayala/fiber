/*
* ---------------
* heFFTe backend
* ---------------
*/
#ifndef FIBER_BACKEND_HEFFTE_H
#define FIBER_BACKEND_HEFFTE_H

#include <stdio.h>

//=====================  Complex-to-Complex transform =========================

#if defined(FIBER_ENABLE_HEFFTE)
#include "heffte.h"

void compute_z2z_heffte( int const inbox_low[3], int const inbox_high[3],
                  int const outbox_low[3], int const outbox_high[3], 
                  MPI_Comm const comm,
                  void const *in, void *out, int heffte_switch, double *timer)
{
    // Plan definition
    heffte_plan plan;

    MPI_Barrier(comm);
    timer[0] = -MPI_Wtime();
    // plan create
    int status = heffte_plan_create(Heffte_BACKEND_FFTW, inbox_low, inbox_high, NULL, outbox_low, outbox_high, NULL, comm, NULL, &plan);
    MPI_Barrier(comm);
    timer[0] = +MPI_Wtime();

    if (status != Heffte_SUCCESS){
        printf("Failed at heffte_plan_create() with error code: %d\n", status);
        MPI_Abort(comm, 1);
    }

    // FFT execution
    MPI_Barrier(comm);
    timer[1] = -MPI_Wtime();
    // compute
    heffte_forward_z2z(plan, in, out, heffte_switch);
    MPI_Barrier(comm);
    timer[1] = +MPI_Wtime();

    status = heffte_plan_destroy(plan);
    if (status != Heffte_SUCCESS){
        printf("Failed at heffte_plan_destroy() with error code: %d\n", status);
        MPI_Abort(comm, 1);
    }
}

//=====================  Real-to-Complex transform =========================


// void compute_d2z_heffte( int const * , int const *, int const * , int const * , MPI_Comm const , double const *, void *);
void compute_d2z_heffte( int const inbox_low[3], int const inbox_high[3],
                  int const outbox_low[3], int const outbox_high[3], 
                  MPI_Comm const comm,
                  double const *in, void *out, int heffte_switch, double *timer)
{

    // Plan definition
    heffte_plan plan;

    MPI_Barrier(comm);
    timer[0] = -MPI_Wtime();
    int status;
    if(heffte_switch == 1){
        printf("Calling GPU heffte \n");
        // status = heffte_plan_create_r2c(Heffte_BACKEND_CUFFT, inbox_low, inbox_high, NULL, outbox_low, outbox_high, NULL, 0, comm, NULL, &plan);
        status = heffte_plan_create(Heffte_BACKEND_CUFFT, inbox_low, inbox_high, NULL, outbox_low, outbox_high, NULL, comm, NULL, &plan);
    }        
    else{        
        printf("Calling CPU heffte \n");
        // status = heffte_plan_create_r2c(Heffte_BACKEND_FFTW, inbox_low, inbox_high, NULL, outbox_low, outbox_high, NULL, 0, comm, NULL, &plan);
        status = heffte_plan_create(Heffte_BACKEND_FFTW, inbox_low, inbox_high, NULL, outbox_low, outbox_high, NULL, comm, NULL, &plan);
    }        
    MPI_Barrier(comm);
    timer[0] = +MPI_Wtime();

    if (status != Heffte_SUCCESS){
        printf("Failed at heffte_plan_create() with error code: %d\n", status);
        MPI_Abort(comm, 1);
    }

    printf("size in = %d \n" , heffte_size_inbox(plan) );
    printf("size ou = %d \n" , heffte_size_outbox(plan) );
    printf("size wr = %d \n" , heffte_size_workspace(plan) );
    printf("backend = %d \n" , heffte_get_backend(plan)  );
    printf("r2c = %d \n" , heffte_is_r2c(plan)  );


    // FFT execution
    MPI_Barrier(comm);
    timer[1] = -MPI_Wtime();
    // compute
    heffte_forward_d2z(plan, in, out, Heffte_SCALE_NONE);
    MPI_Barrier(comm);
    timer[1] = +MPI_Wtime();

    status = heffte_plan_destroy(plan);
    if (status != Heffte_SUCCESS){
        printf("Failed at heffte_plan_destroy() with error code: %d\n", status);
        MPI_Abort(comm, 1);
    }
}

void compute_z2d_heffte( int const inbox_low[3], int const inbox_high[3],
                  int const outbox_low[3], int const outbox_high[3], 
                  MPI_Comm const comm,
                  void const *in, double *out, double *timer)
{
    // Plan definition
    heffte_plan plan;
    MPI_Barrier(comm);
    timer[0] = -MPI_Wtime();

    int status = heffte_plan_create(Heffte_BACKEND_FFTW, inbox_low, inbox_high, NULL, outbox_low, outbox_high, NULL, comm, NULL, &plan);
    MPI_Barrier(comm);
    timer[0] = +MPI_Wtime();

    if (status != Heffte_SUCCESS){
        printf("Failed at heffte_plan_create() with error code: %d\n", status);
        MPI_Abort(comm, 1);
    }

    // FFT execution
    MPI_Barrier(comm);
    timer[1] = -MPI_Wtime();
    // compute
    heffte_backward_z2d(plan, in, out, Heffte_SCALE_FULL);
    MPI_Barrier(comm);
    timer[1] = +MPI_Wtime();

    status = heffte_plan_destroy(plan);
    if (status != Heffte_SUCCESS){
        printf("Failed at heffte_plan_destroy() with error code: %d\n", status);
        MPI_Abort(comm, 1);
    }
}


// ========================================================================================

#else
void compute_z2z_heffte( int const inbox_low[3], int const inbox_high[3],
                  int const outbox_low[3], int const outbox_high[3], 
                  MPI_Comm const comm,
                  void const *in, void *out, int heffte_switch, double *timer)
{}

void compute_d2z_heffte( int const inbox_low[3], int const inbox_high[3],
                  int const outbox_low[3], int const outbox_high[3], 
                  MPI_Comm const comm,
                  double const *in, void *out, int heffte_switch, double *timer)
{}

void compute_z2d_heffte( int const inbox_low[3], int const inbox_high[3],
                  int const outbox_low[3], int const outbox_high[3], 
                  MPI_Comm const comm,
                  void const *in, double *out, double *timer)
{}
#endif

#endif  //! FIBER_BACKEND_HEFFTE_H