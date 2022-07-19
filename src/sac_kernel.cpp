#include "sac_kernel.h"
#include "qmc_reader.h"
#include "freq_grids.h"
#include <iostream>

namespace SAC {

    // interface member functions
    int Kernel::time_size() const { return this->m_time_size; }
    int Kernel::freq_size() const { return this->m_freq_size; }
    const Eigen::MatrixXd& Kernel::kernel() const { return this->m_kernel; }
    
    // double Kernel::kernel( int t, int freq ) const 
    // {
    //     assert( t >= 0 && t < this->m_time_size );
    //     assert( freq >= 0 && freq < this->m_freq_size );
    //     return this->m_kernel(t, freq);
    // }


    void Kernel::set_kernel_params( int time_size, int freq_size, const std::string& kernel_type )
    {   
        if ( kernel_type != "fermion" && kernel_type != "boson" ) {
            std::cerr << "SAC::Kernel::set_kernel_params(): "
                      << "undefined kernel type \'" << kernel_type 
                      << "\' ( the default type of the kernel is \'fermion\' )." << std::endl;
            exit(1);
        }
        else { this->m_kernel_type = kernel_type; }
        
        this->m_time_size = time_size;
        this->m_freq_size = freq_size;
        this->m_kernel.resize(time_size, freq_size);
    }


    void Kernel::initial( const QmcReader& qmc_reader, const Grids::FreqGrids& grids ) 
    {
        assert( qmc_reader.tgrids_qmc().size() == this->m_time_size );
        assert( grids.FreqNum() == this->m_freq_size );

        // kernel for fermionic green's functions
        // e.g. fermionic green's function  ->  usual fermionic spectral function
        if ( this->m_kernel_type == "fermion" ) {
            for ( auto i = 0; i < grids.FreqNum(); ++i ) {
                const double freq = grids.FreqIndex2Freq(i);
                this->m_kernel.col(i) = ( -freq * qmc_reader.tgrids_qmc().array() ).exp() 
                                      / ( 1.0 + exp( -qmc_reader.beta() * freq ) );
            }
        }

        // kernel for bosonic green's functions
        // e.g. bosonic Matsubara correlation function ->  dynamic susceptibility
        if ( this->m_kernel_type == "boson" ) {
            for ( auto i = 0; i < grids.FreqNum(); ++i ) {
                const double freq = grids.FreqIndex2Freq(i);
                this->m_kernel.col(i) = ( ( -freq * qmc_reader.tgrids_qmc().array() ).exp() 
                                        + ( -freq * ( qmc_reader.beta() - qmc_reader.tgrids_qmc().array() ) ).exp() ) 
                                        / ( 1.0 + exp( -qmc_reader.beta() * freq ) );
            }
        }
    }


    void Kernel::rotate( const Eigen::MatrixXd& rotate_mat ) 
    {
        assert( rotate_mat.rows() == this->m_time_size );
        assert( rotate_mat.cols() == this->m_time_size );

        this->m_kernel = rotate_mat * this->m_kernel;
        // for ( int i = 0; i < this->m_time_size; ++i ) {
        //     this->m_kernel.col(i) = rotate_mat * this->m_kernel.col(i);
        // }
    }

} // namespace SAC
