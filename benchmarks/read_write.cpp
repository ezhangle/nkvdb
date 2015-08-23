#include <ctime>
#include <iostream>
#include <cstdlib>
#include <storage/storage.h>
#include <utils/ProcessLogger.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

const std::string storage_path = "benchmarkStorage";

int meas2write = 10;
int pagesize = 1000000;
bool write_only = false;
bool verbose = false;
bool dont_remove = false;

void makeAndWrite(int mc, int ic) {
    logger << "makeAndWrite mc:" << mc << " ic:" << ic << endl;

    const uint64_t storage_size = sizeof(storage::Page::Header) + (sizeof(storage::Meas)*pagesize);

    storage::DataStorage::PDataStorage ds = storage::DataStorage::Create(storage_path, storage_size);
    clock_t write_t0 = clock();
    storage::Meas::PMeas meas = storage::Meas::empty();

    for (int i = 0; i < ic; ++i) {
        meas->value = i%mc;
        meas->id = i%mc;
        meas->source = meas->flag = i%mc;
        meas->time = i;

        ds->append(meas);
    }

    clock_t write_t1 = clock();
    logger << "write time: " << ((float)write_t1 - write_t0) / CLOCKS_PER_SEC << endl;
    delete meas;
    ds = nullptr;
    utils::rm(storage_path);
}

void readIntervalBench(storage::DataStorage::PDataStorage ds, storage::Time from, storage::Time to,std::string message){
    logger<<"readInterval "<<message<<std::endl;

    clock_t read_t0 = clock();
    auto meases = ds->readInterval(from, to);
    clock_t read_t1 = clock();

    logger << "time: " << ((float)read_t1 - read_t0) / CLOCKS_PER_SEC << endl;
}

int main(int argc, char*argv[]) {

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("mc", po::value<int>(&meas2write)->default_value(meas2write), "measurment count")
            ("write-only", "don`t run readInterval")
            ("verbose", "verbose ouput")
            ("dont-remove", "dont remove created storage")
            ;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    } catch (std::exception&ex) {
        logger << "Error: " << ex.what()<<endl;
        exit(1);
    }
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("write-only")) {
        write_only = true;
    }


    if (vm.count("verbose")) {
        verbose = true;
    }

    if (vm.count("dont-remove")) {
        dont_remove = true;
    }

    const uint64_t storage_size = sizeof(storage::Page::Header) + (sizeof(storage::Meas)*pagesize);

    makeAndWrite(meas2write, 1000000);
    makeAndWrite(meas2write, 2000000);
    makeAndWrite(meas2write, 3000000);

    if (!write_only) {
        logger<<"\n";

        storage::DataStorage::PDataStorage ds = storage::DataStorage::Create(storage_path, storage_size);
        storage::Meas::PMeas meas = storage::Meas::empty();

        logger<<"creating storage..."<<std::endl;
        logger<<"pages_size:"<<pagesize<<std::endl;

        for (int i = 0; i < pagesize*10 ; ++i) {
            clock_t verb_t0 = clock();

            meas->value = i;
            meas->id = i%meas2write;
            meas->source = meas->flag = i%meas2write;
            meas->time = i;

            ds->append(meas);
            clock_t verb_t1 = clock();
            if (verbose) {
                logger << "write[" << i << "]: " << ((float)verb_t1 - verb_t0) / CLOCKS_PER_SEC << endl;
            }

        }
        delete meas;

        logger<<"big readers"<<std::endl;
        readIntervalBench(ds,0,pagesize/2, "0-0.5");
        readIntervalBench(ds,3*pagesize+pagesize/2,3*pagesize*2, "3.5-6");
        readIntervalBench(ds,7*pagesize,8*pagesize+pagesize*1.5,"7-9.5");

        logger<<"\nsmall readers"<<std::endl;
        readIntervalBench(ds,5*pagesize+pagesize/3,6*pagesize,"5.3-6");
        readIntervalBench(ds,2*pagesize,2*pagesize+pagesize*1.5, "2-3.5");
        readIntervalBench(ds,6*pagesize*0.3,7*pagesize*0.7, "6.3-7.7");

        if (!dont_remove)
            utils::rm(storage_path);
    }
}
