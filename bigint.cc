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
    mpz_init(*res);
    mpz_add(*res, *bigints[i], *bigints[j]);
    
    bigints[bigindex] = res;
    return bigindex++;
}

extern "C" uint32_t bsub (uint32_t i, uint32_t j) {
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_add(*res, *bigints[i], *bigints[j]);
    
    bigints[bigindex] = res;
    return bigindex++;
}

extern "C" uint32_t bmul (uint32_t i, uint32_t j) {
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_mul(*res, *bigints[i], *bigints[j]);
    
    bigints[bigindex] = res;
    return bigindex++;
}

extern "C" uint32_t bdiv (uint32_t i, uint32_t j) {
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_div(*res, *bigints[i], *bigints[j]);
    
    bigints[bigindex] = res;
    return bigindex++;
}

extern "C" uint32_t uadd (uint32_t i, uint64_t x) {
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_add_ui(*res, *bigints[i], (unsigned long int) x);
    
    bigints[bigindex] = res;
    return bigindex++;
}

extern "C" uint32_t usub (uint32_t i, uint64_t x) {
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_sub_ui(*res, *bigints[i], (unsigned long int) x);
    
    bigints[bigindex] = res;
    return bigindex++;
}

extern "C" uint32_t umul (uint32_t i, uint64_t x) {
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_mul_ui(*res, *bigints[i], (unsigned long int) x);
    
    bigints[bigindex] = res;
    return bigindex++;
}

extern "C" uint32_t udiv (uint32_t i, uint64_t x) {
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_div_ui(*res, *bigints[i], (unsigned long int) x);
    
    bigints[bigindex] = res;
    return bigindex++;
}

extern "C" uint32_t babs (uint32_t i) {
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_abs(*res, *bigints[i]);
    
    bigints[bigindex] = res;
    return bigindex++;
}

extern "C" uint32_t bneg (uint32_t i) {
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_neg(*res, *bigints[i]);
    
    bigints[bigindex] = res;
    return bigindex++;
}
