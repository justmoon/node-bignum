#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <gmp.h>
#include <map>
#include <utility>

static std::map<uint32_t, mpz_t *> bigints;
static uint32_t bigindex = 0;

extern "C" uint32_t create () {
    mpz_t *bigi = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*bigi);
    
    bigints[bigindex] = bigi;
    return bigindex ++;
}

extern "C" uint32_t fromString (char *str, uint32_t base) {
    mpz_t *bigi = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*bigi);
    mpz_set_str(*bigi, str, base);
    
    bigints[bigindex] = bigi;
    return bigindex ++;
}

extern "C" uint32_t destroy (uint32_t index) {
    mpz_clear(*bigints[index]);
    bigints.erase(index);
    return 0;
}

extern "C" char * toString (uint32_t index, uint32_t base) {
    return mpz_get_str(0, base, *bigints[index]);
}

extern "C" uint32_t badd (uint32_t i, uint32_t j) {
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_add(*res, *bigints[i], *bigints[j]);
    
    bigints[bigindex] = res;
    return bigindex++;
}

extern "C" uint32_t bsub (uint32_t i, uint32_t j) {
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_add(*res, *bigints[i], *bigints[j]);
    
    bigints[bigindex] = res;
    return bigindex++;
}

extern "C" uint32_t bmul (uint32_t i, uint32_t j) {
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_mul(*res, *bigints[i], *bigints[j]);
    
    bigints[bigindex] = res;
    return bigindex++;
}

extern "C" uint32_t bdiv (uint32_t i, uint32_t j) {
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_div(*res, *bigints[i], *bigints[j]);
    
    bigints[bigindex] = res;
    return bigindex++;
}
