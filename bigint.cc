#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <v8.h>
#include <node.h>
#include <node_events.h>
#include <gmp.h>
#include <map>
#include <utility>

using namespace v8;
using namespace node;

#define REQ_STR_ARG(I, VAR)                            \
    if (args.Length()<= (I) || !args[I]->IsString())            \
        return ThrowException(Exception::TypeError(            \
            String::New("Argument " #I " must be a string")));    \
    Local<String> VAR = Local<String>::Cast(args[I]);

#define REQ_UTF8_ARG(I, VAR)                            \
    if (args.Length() <= (I) || !args[I]->IsString())            \
        return ThrowException(Exception::TypeError(                     \
            String::New("Argument " #I " must be a utf8 string"))); \
    String::Utf8Value VAR(args[I]->ToString());

#define REQ_INT32_ARG(I, VAR)                            \
    if (args.Length() <= (I) || !args[I]->IsInt32())            \
        return ThrowException(Exception::TypeError(                     \
            String::New("Argument " #I " must be an int32")));      \
    int32_t VAR = args[I]->ToInt32()->Value();

#define REQ_UINT32_ARG(I, VAR)                            \
    if (args.Length() <= (I) || !args[I]->IsUint32())            \
        return ThrowException(Exception::TypeError(                     \
            String::New("Argument " #I " must be a uint32")));      \
    uint32_t VAR = args[I]->ToUint32()->Value();

#define REQ_INT64_ARG(I, VAR)                            \
    if (args.Length() <= (I) || !args[I]->IsNumber())            \
        return ThrowException(Exception::TypeError(                     \
            String::New("Argument " #I " must be an int64")));      \
    int64_t VAR = args[I]->ToInteger()->Value();

#define REQ_UINT64_ARG(I, VAR)                            \
    if (args.Length() <= (I) || !args[I]->IsNumber())            \
        return ThrowException(Exception::TypeError(                     \
            String::New("Argument " #I " must be a uint64")));      \
    uint64_t VAR = args[I]->ToInteger()->Value();


static std::map<uint32_t, mpz_t *>    bigints;
static uint32_t                bigindex    = 0;
static gmp_randstate_t *        randstate    = NULL;

static Handle<Value>
FromString(const Arguments& args)
{
    HandleScope scope;

    REQ_UTF8_ARG(0, str);
    REQ_UINT32_ARG(1, base);

    mpz_t * bigi = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*bigi);

    mpz_set_str(*bigi, *str, base);

    bigints[bigindex] = bigi;
    return Number::New(bigindex++);
}

static Handle<Value>
Destroy(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, index);
    mpz_clear(*bigints[index]);
    bigints.erase(index);

    return Number::New(0);
}

static Handle<Value>
ToString(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, index);
    REQ_UINT32_ARG(1, base);

    return String::New(mpz_get_str(0, base, *bigints[index]));
}

static Handle<Value>
Badd(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT32_ARG(1, j);

    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);

    mpz_add(*res, *bigints[i], *bigints[j]);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Bsub(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT32_ARG(1, j);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_sub(*res, *bigints[i], *bigints[j]);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Bmul(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT32_ARG(1, j);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_mul(*res, *bigints[i], *bigints[j]);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Bdiv(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT32_ARG(1, j);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_div(*res, *bigints[i], *bigints[j]);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Uadd(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT64_ARG(1, x);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_add_ui(*res, *bigints[i], x);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Usub(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT64_ARG(1, x);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_sub_ui(*res, *bigints[i], x);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Umul(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT64_ARG(1, x);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_mul_ui(*res, *bigints[i], x);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Udiv(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT64_ARG(1, x);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_div_ui(*res, *bigints[i], x);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Babs(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_abs(*res, *bigints[i]);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Bneg(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_neg(*res, *bigints[i]);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Bmod(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT32_ARG(1, j);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_mod(*res, *bigints[i], *bigints[j]);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Umod(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT64_ARG(1, x);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_mod_ui(*res, *bigints[i], x);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Bpowm(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT32_ARG(1, j);
    REQ_UINT32_ARG(2, k);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_powm(*res, *bigints[i], *bigints[j], *bigints[k]);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Upowm(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT64_ARG(1, x);
    REQ_UINT32_ARG(2, k);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_powm_ui(*res, *bigints[i], x, *bigints[k]);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Upow(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT64_ARG(1, x);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_pow_ui(*res, *bigints[i], x);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Uupow(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT64_ARG(0, x);
    REQ_UINT64_ARG(1, y);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_ui_pow_ui(*res, x, y);
    
    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Brand0(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    
    if(randstate == NULL) {
        randstate = (gmp_randstate_t *) malloc(sizeof(gmp_randstate_t));
        gmp_randinit_default(*randstate);
    }
    
    mpz_urandomm(*res, *randstate, *bigints[i]);

    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Probprime(const Arguments& args)
{
    HandleScope scope;
    
    REQ_UINT32_ARG(0, i);
    REQ_UINT32_ARG(1, reps);

    return Number::New(mpz_probab_prime_p(*bigints[i], reps));
}

static Handle<Value>
Nextprime(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_nextprime(*res, *bigints[i]);

    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Bcompare(const Arguments& args)
{
    HandleScope scope;
    
    REQ_UINT32_ARG(0, i);
    REQ_UINT32_ARG(1, j);

    return Number::New(mpz_cmp(*bigints[i], *bigints[j]));
}

static Handle<Value>
Scompare(const Arguments& args)
{
    HandleScope scope;
    
    REQ_UINT32_ARG(0, i);
    REQ_INT64_ARG(1, x);
    
    return Number::New(mpz_cmp_si(*bigints[i], x));
}

static Handle<Value>
Ucompare(const Arguments& args)
{
    HandleScope scope;
    
    REQ_UINT32_ARG(0, i);
    REQ_UINT64_ARG(1, x);

    return Number::New(mpz_cmp_ui(*bigints[i], x));
}

static Handle<Value>
Band(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT32_ARG(1, j);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_and(*res, *bigints[i], *bigints[j]);

    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Bor(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT32_ARG(1, j);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_ior(*res, *bigints[i], *bigints[j]);

    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Bxor(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT32_ARG(1, j);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_xor(*res, *bigints[i], *bigints[j]);

    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

static Handle<Value>
Binvertm(const Arguments& args)
{
    HandleScope scope;

    REQ_UINT32_ARG(0, i);
    REQ_UINT32_ARG(1, j);
    mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
    mpz_init(*res);
    mpz_invert(*res, *bigints[i], *bigints[j]);

    bigints[bigindex] = res;
    
    return Number::New(bigindex++);
}

extern "C" void
init (Handle<Object> t)
{
    HandleScope scope;

    NODE_SET_METHOD(t, "fromString", FromString);
    NODE_SET_METHOD(t, "destroy", Destroy);
    NODE_SET_METHOD(t, "toString", ToString);
    NODE_SET_METHOD(t, "badd", Badd);
    NODE_SET_METHOD(t, "bsub", Bsub);
    NODE_SET_METHOD(t, "bmul", Bmul);
    NODE_SET_METHOD(t, "bdiv", Bdiv);
    NODE_SET_METHOD(t, "uadd", Uadd);
    NODE_SET_METHOD(t, "usub", Usub);
    NODE_SET_METHOD(t, "umul", Umul);
    NODE_SET_METHOD(t, "udiv", Udiv);
    NODE_SET_METHOD(t, "babs", Babs);
    NODE_SET_METHOD(t, "bneg", Bneg);
    NODE_SET_METHOD(t, "bmod", Bmod);
    NODE_SET_METHOD(t, "umod", Umod);
    NODE_SET_METHOD(t, "bpowm", Bpowm);
    NODE_SET_METHOD(t, "upowm", Upowm);
    NODE_SET_METHOD(t, "upow", Upow);
    NODE_SET_METHOD(t, "uupow", Uupow);
    NODE_SET_METHOD(t, "brand0", Brand0);
    NODE_SET_METHOD(t, "probprime", Probprime);
    NODE_SET_METHOD(t, "nextprime", Nextprime);
    NODE_SET_METHOD(t, "bcompare", Bcompare);
    NODE_SET_METHOD(t, "scompare", Scompare);
    NODE_SET_METHOD(t, "ucompare", Ucompare);
    NODE_SET_METHOD(t, "band", Band);
    NODE_SET_METHOD(t, "bor", Bor);
    NODE_SET_METHOD(t, "bxor", Bxor);
    NODE_SET_METHOD(t, "binvertm", Binvertm);
}
