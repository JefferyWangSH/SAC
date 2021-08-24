#include <iostream>
#include "Kernel.h"
#include "SAC.h"
#include "FrequencyGrid.h"

Kernel::Kernel::Kernel(int nt, int nfreq) {
    this->nt = nt;
    this->nfreq = nfreq;
    this->kernel.resize(nt, nfreq);
}

void Kernel::Kernel::init(const Simulation::SAC &sac, Grid::FrequencyGrid grid, const std::string &mode) {
    assert( sac.tau.size() == nt );
    assert( grid.upper() == nfreq );

    // kernel for fermion greens function
    if ( mode == "fermion" ) {
        for (int i = grid.lower(); i < grid.upper(); ++i) {
            const double freq = grid.index_to_freq(i);
            kernel.col(i) = (-freq * sac.tau.array()).exp() / ( 1.0 + exp(-sac.beta * freq) );
        }
    }

    // kernel for boson greens function
    if ( mode == "boson" ) {
        for (int i = grid.lower(); i < grid.upper(); ++i) {
            const double freq = grid.index_to_freq(i);
            kernel.col(i) = ( (-freq * sac.tau.array()).exp() + (-freq * (sac.beta - sac.tau.array())).exp() )
                          / ( 1.0 + exp(-sac.beta * freq) );
        }
    }
}

void Kernel::Kernel::rotate(const Eigen::MatrixXd &rotate_mat) {
    assert( rotate_mat.rows() == nt );
    assert( rotate_mat.cols() == nt );

    kernel = rotate_mat * kernel;
//    for (int i = 0; i < nt; ++i) {
//        kernel.col(i) = rotate_mat * kernel.col(i);
//    }
}

