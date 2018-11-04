#include <iostream>
#include <string>
#include <vector>

#include <dlib/clustering.h>
#include <dlib/rand.h>
#include <dlib/serialize.h>

#include "read.h"
#include "unifier.h"

using namespace dlib;

int
main ( int argc, char **argv )
{
    if ( argc != 3 )
    {
        std::cout << "Provide number of clusters and filename: ./rclst <num> <filename>\n";
        exit ( 1 );
    }

    unsigned short number_of_clusters = std::atoi ( argv[1] );
    if ( number_of_clusters == 0 )
    {
        std::cout << "Number of clusters should be a positive number\n";
    }
    std::string filename = argv[2];

    typedef matrix<double,7,1> sample_type;
    typedef radial_basis_kernel<sample_type> kernel_type;
    auto unifier = realty::SampleUnifier ( );

    kcentroid<kernel_type> kc ( kernel_type ( .1 ), .01, 1024 );
    kkmeans<kernel_type> test ( kc );

    std::vector<sample_type> samples;
    std::vector<sample_type> initial_centers;
    auto  __samples = realty::get_raw_data ( );
    auto __samples_real = __samples;

    for ( int i = 0; i < 7; ++i )
    {
        unifier.unify ( __samples, i );
    }
    unifier.save_to_file ( "unifier.dat" );
    unifier.restore_from_file ( "unifier.dat" );

    for ( int i = 0; i < 7; ++i )
    {
        unifier.unify ( __samples_real, i );
    }

    sample_type m;
    dlib::rand rnd;

    samples.reserve ( __samples.size ( ) );
    for ( auto& sample: __samples_real )
    {
        for ( int i = 0; i < 7; ++i )
        {
            m ( i ) = sample[i];
        }
        samples.push_back ( m );
    }

    test.set_number_of_centers ( number_of_clusters );
    pick_initial_centers ( number_of_clusters, initial_centers, samples, test.get_kernel ( ) );
    test.train ( samples, initial_centers );

    for (unsigned long i = 0; i < samples.size(); ++i)
    {
        auto x = samples[i];
        std::cout << test ( samples[i] ) + 1 << std::endl;
    }

    serialize ( filename + ".dat" ) << test;

    return 0;
}
