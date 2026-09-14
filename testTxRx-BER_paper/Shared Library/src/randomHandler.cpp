#include "randomHandler.h"
#include "randomUniform.h"

namespace caching{

float randomNumber(float minNumber, float maxNumber){
    long seed = 1;
    float randNum = randomUniformFunction(&seed);

    float result = (randNum * (maxNumber - minNumber)) + minNumber;

    return result;
}

}//end namespace caching