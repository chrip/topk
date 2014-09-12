#include "vector_space.h"
#include "query_vector.h"
#include "top_k_queue_nrd.h"

#include <sys/time.h>
#include <fstream>

#include "gtest/gtest.h"

void stringToVec(const std::string& line, vec& vector)
{
    std::size_t begin = line.find("[")+1;
    std::size_t end = line.find("]");
    std::stringstream ss(line.substr(begin, end-begin));

    int i = 0;
    for( std::string val; std::getline(ss, val, ','); i++) {
        vector.push_back(std::stof(val));
        //if(i==9999) break;
    }
}

int getId(const std::string& line)
{
    std::size_t begin = line.find("\"id\":")+5;
    std::size_t end = line.find(",", begin);
    return std::stoi(line.substr(begin, end-begin));
}

static double getTimestampInMilliseconds()
{
    struct timeval now;
    gettimeofday (&now, NULL);
    return now.tv_usec/1000.0 + (unsigned long long)now.tv_sec * 1000;
}

TEST(tkqNrdTest, tryOutParameters) {


    size_t topK = 3;

    TopKQueueNRD tkqNRD = TopKQueueNRD();

    //std::ifstream input0( "/home/chrisschaefer/workspace/topk/job.json" );
    std::ifstream input0( "test_data/vector_space.json" );
    int i = 0;
    for( std::string line; getline( input0, line ); i++){
        vec vector = vec();
        stringToVec(line,vector);
        tkqNRD.addVector(getId(line),vector);
    }

    std::cout << tkqNRD.getVectorSpaceSize() << " vectors of length " << tkqNRD.getVectorSpace().getVectorSpacePyramide()[0][0].size() << std::endl;

    std::ifstream input1( "test_data/query_vector.json" );
    for( std::string line; getline( input1, line );)
    {

        vec vector = vec();
        stringToVec(line, vector);

        size_t compressionLevels = 0;
        for(;compressionLevels < 4; compressionLevels++){
            size_t compressionBlockRows = 1;
            for(;compressionBlockRows < 43; compressionBlockRows++){
                size_t compressionBlockCols = 1;
                for(;compressionBlockCols < 3; compressionBlockCols++){

                    double start, end;
                    tkqNRD.buildIndex(compressionBlockRows, compressionBlockCols, compressionLevels);

                    start = getTimestampInMilliseconds();
                    QueryVector qv = QueryVector(vector, compressionBlockRows, compressionLevels);
                    tkqNRD.executeTopK(qv, topK);

                    std::cout << "compressionLevels: " << compressionLevels
                              << ", compressionBlockCols: " << compressionBlockCols
                              << ", compressionBlockRows: " << compressionBlockRows;


                    ASSERT_EQ(36100, getId(line));
                    ASSERT_EQ("[[2572694,0.1704],[2570094,0.1659],[2558895,0.1657]]", tkqNRD.toString())
                    //ASSERT_EQ("[[2548890,0.3311],[2553413,0.2964],[2571922,0.2955]]", tkqNRD.toString())
                            << " level: " << compressionLevels
                            << " compressionBlockCols: " << compressionBlockCols
                            << " compressionBlockRows: " << compressionBlockRows;
                    end = getTimestampInMilliseconds();
                    std::cout  << ", time: "  <<  (end - start) << " ms" << std::endl;
                }
            }
        }
    }

}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

