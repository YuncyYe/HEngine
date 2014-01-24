/**
 * @author VaL Doroshchuk
 * @copyright Alex X. Liu, Ke Shen, Eric Torng, VaL Doroshchuk
 * @license GNU GPL v2
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/times.h>

#include "HEngine_sn.h"

using namespace hengine;

int main( int argc, char **argv )
{
    float  userTime;
    struct rusage startTime, stopTime;

    getrusage( RUSAGE_SELF, &startTime );

    if ( argc != 3 )
    {
        std::cout << "Usage: " << argv[0] << " <k> <data file>" << std::endl << std::endl;

        return 1;
    }

    unsigned k = atoi( argv[1] );
    std::string line;

    std::cout << "Reading the dataset ........ ";
    fflush( stdout );

    BinTable db;
    std::ifstream dict;
    dict.open( argv[2], std::ifstream::in );
    while ( getline( dict, line ) )
    {
        Number h;

        std::istringstream reader( line );
        reader >> h;
        db.push_back( HEngine::number2BinStr( h ) );
    }

    std::cout <<  "done. " << db.size() << " hashes" << std::endl;
    std::cout << "Building with " << k << " hamming distance bound ....... ";
    fflush( stdout );

    HEngine_sn e( db, k );

    std::cout << "done" << std::endl;

    getrusage( RUSAGE_SELF, &stopTime );
    userTime =
                ( (float) ( stopTime.ru_utime.tv_sec  - startTime.ru_utime.tv_sec ) ) +
                ( (float) ( stopTime.ru_utime.tv_usec - startTime.ru_utime.tv_usec ) ) * 1e-6;

    std::cout << std::endl;
    std::cout << "Building time: " << userTime << " seconds" << std::endl;

    Number h;
    while ( true )
    {
        std::cout << "query> ";
        std::cin >> h;
        getrusage( RUSAGE_SELF, &startTime );

        Matches res = e.query( h );

        std::cout << "Found " << res.size() << " matches" << std::endl;
        for ( auto &item: res )
        {
            std::cout << "[" << item.second << "] " << HEngine::binStr2Number( item.first ) << std::endl;
        }

        getrusage( RUSAGE_SELF, &stopTime );
        userTime =
                    ( (float) ( stopTime.ru_utime.tv_sec  - startTime.ru_utime.tv_sec ) ) +
                    ( (float) ( stopTime.ru_utime.tv_usec - startTime.ru_utime.tv_usec ) ) * 1e-6;

        std::cout << std::endl;
        std::cout << "HEngine query time: " << userTime << " seconds" << std::endl << std::endl;

        getrusage( RUSAGE_SELF, &startTime );
        std::cout << "linear scaninng ... " << std::endl;

        Matches nt;
        for ( auto &item: db )
        {
            unsigned d = HEngine::getHammingDistance( h, HEngine::binStr2Number( item ) );
            if ( d <= k )
            {
                nt[item] = d;
            }
        }

        std::cout << "Found " << nt.size() << " matches" << std::endl;
        for ( auto &item: nt )
        {
            std::cout << "[" << item.second << "] " << HEngine::binStr2Number( item.first ) << std::endl;
        }

        getrusage( RUSAGE_SELF, &stopTime );
        userTime =
                    ( (float) ( stopTime.ru_utime.tv_sec  - startTime.ru_utime.tv_sec ) ) +
                    ( (float) ( stopTime.ru_utime.tv_usec - startTime.ru_utime.tv_usec ) ) * 1e-6;

        std::cout << std::endl;
        std::cout << "Linear query time: " << userTime << " seconds" << std::endl << std::endl;
    }


}
