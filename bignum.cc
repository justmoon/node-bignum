#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iostream>

#include <nan.h>
#include <openssl/bn.h>
#include <map>
#include <utility>

using namespace v8;
using namespace node;
using namespace std;

#define REQ_STR_ARG(I, VAR)                                   \
  if (info.Length()<= (I) || !info[I]->IsString()) {          \
    Nan::ThrowTypeError("Argument " #I " must be a string");    \
    return;                                     \
  }                                                           \
  Local<String> VAR = Local<String>::Cast(info[I]);

#define REQ_UTF8_ARG(I, VAR)                                  \
  if (info.Length() <= (I) || !info[I]->IsString()) {         \
    Nan::ThrowTypeError(                                        \
      "Argument " #I " must be a utf8 string");               \
    return;                                     \
  }                                                           \
  String::Utf8Value VAR(info[I]->ToString());

#define REQ_INT32_ARG(I, VAR)                                 \
  if (info.Length() <= (I) || !info[I]->IsInt32()) {          \
    Nan::ThrowTypeError("Argument " #I " must be an int32");    \
    return;                                     \
  }                                                           \
  int32_t VAR = info[I]->ToInt32()->Value();

#define REQ_UINT32_ARG(I, VAR)                                \
  if (info.Length() <= (I) || !info[I]->IsUint32()) {         \
    Nan::ThrowTypeError("Argument " #I " must be a uint32");    \
    return;                                     \
  }                                                           \
  uint32_t VAR = Nan::To<int32_t>(info[I]).FromJust();

#define REQ_INT64_ARG(I, VAR)                                 \
  if (info.Length() <= (I) || !info[I]->IsNumber()) {         \
    Nan::ThrowTypeError("Argument " #I " must be an int64");    \
    return;                                     \
  }                                                           \
  int64_t VAR = info[I]->ToInteger(info.GetIsolate()->GetCurrentContext()).ToLocalChecked()->Value();

#define REQ_UINT64_ARG(I, VAR)                                \
  if (info.Length() <= (I) || !info[I]->IsNumber()) {         \
    Nan::ThrowTypeError("Argument " #I " must be a uint64");    \
    return;                                     \
  }                                                           \
  uint64_t VAR = Nan::To<v8::Integer>(info[I]).ToLocalChecked()->Value();

#define REQ_BOOL_ARG(I, VAR)                                  \
  if (info.Length() <= (I) || !info[I]->IsBoolean()) {        \
    Nan::ThrowTypeError("Argument " #I " must be a boolean");   \
    return;                                     \
  }                                                           \
  bool VAR = Nan::To<v8::Boolean>(info[I]).ToLocalChecked()->Value();

#define WRAP_RESULT(RES, VAR)                                           \
  Local<Value> arg[1] = { Nan::New<External>(static_cast<BigNum*>(RES)) };  \
  Local<Object> VAR = Nan::NewInstance(  \
    Nan::New<FunctionTemplate>(constructor_template)->GetFunction(info.GetIsolate()->GetCurrentContext()).ToLocalChecked(), \
    1, \
    arg \
  ).ToLocalChecked();

class AutoBN_CTX
{
protected:
  BN_CTX* ctx;
  BN_CTX* operator=(BN_CTX* ctx_new) { return ctx = ctx_new; }

public:
  AutoBN_CTX()
  {
    ctx = BN_CTX_new();
    // TODO: Handle ctx == NULL
  }

  ~AutoBN_CTX()
  {
    if (ctx != NULL)
      BN_CTX_free(ctx);
  }

  operator BN_CTX*() { return ctx; }
  BN_CTX& operator*() { return *ctx; }
  BN_CTX** operator&() { return &ctx; }
  bool operator!() { return (ctx == NULL); }
};

/**
 * BN_jacobi_priv() computes the Jacobi symbol of A with respect to N.
 *
 * Hence, *jacobi = 1 when the jacobi symbol is unity and *jacobi = -1 when the
 * jacobi symbol is -1. N must be odd and >= 3. It is required that 0 <= A < N.
 *
 * When successful 0 is returned. -1 is returned on failure.
 *
 * This is an implementation of an iterative version of Algorithm 2.149 on page
 * 73 of the book "Handbook of Applied Cryptography" by Menezes, Oorshot,
 * Vanstone. Note that there is a typo in step 1. Step 1 should return the value
 * 1. The algorithm has a running time of O((lg N)^2) bit operations.
 *
 * @author Adam L. Young
 */
int BN_jacobi_priv(const BIGNUM *A,const BIGNUM *N,int *jacobi,
                   BN_CTX *ctx)
{
  int e,returnvalue=0,s,bit0,bit1,bit2,a1bit0,a1bit1;
  BIGNUM *zero,*a1,*n1,*three,*tmp;

  if (!jacobi)
    return -1;
  *jacobi = 1;
  if ((!A) || (!N) || (!ctx))
    return -1;
  if (!BN_is_odd(N))
    return -1; /* ERROR: BN_jacobi() given an even N */
  if (BN_cmp(A,N) >= 0)
    return -1;
  n1=BN_new();zero=BN_new();a1=BN_new();three=BN_new();tmp=BN_new();
  BN_set_word(zero,0);
  BN_set_word(three,3);
  if (BN_cmp(N,three) < 0)
	{ /* This function was written by Adam L. Young */
    returnvalue = -1;
    goto endBN_jacobi;
	}
  if (BN_cmp(zero,A) > 0)
	{
    returnvalue = -1;
    goto endBN_jacobi;
	}
  BN_copy(a1,A);
  BN_copy(n1,N);
startjacobistep1:
  if (BN_is_zero(a1)) /* step 1 */
    goto endBN_jacobi;  /* *jacobi = 1; */
  if (BN_is_one(a1)) /* step 2 */
    goto endBN_jacobi;  /* *jacobi = 1; */
  for (e=0;;e++) /*  step 3 */
    if (BN_is_odd(a1))
      break;
    else
      BN_rshift1(a1,a1);
  s = 1; /* step 4 */
  bit0 = BN_is_odd(n1);
  bit1 = BN_is_bit_set(n1,1);
  if (e % 2)
	{
    bit2 = BN_is_bit_set(n1,2);
    if ((!bit2) && (bit1) && (bit0))
      s = -1;
    if ((bit2) && (!bit1) && (bit0))
      s = -1;
	}
  a1bit0 = BN_is_odd(a1);  /* step 5 */
  a1bit1 = BN_is_bit_set(a1,1);
  if (((bit1) && (bit0)) && ((a1bit1) && (a1bit0)))
    s = -s;
  BN_mod(n1,n1,a1,ctx); /* step 6 */
  BN_copy(tmp,a1);
  BN_copy(a1,n1);
  BN_copy(n1,tmp);
  *jacobi *= s;  /*  step 7 */
  goto startjacobistep1;
endBN_jacobi:
  BN_clear_free(zero);
  BN_clear_free(tmp);BN_clear_free(a1);
  BN_clear_free(n1);BN_clear_free(three);
  return returnvalue;
}

class BigNum : public Nan::ObjectWrap {
public:
  static void Initialize(Local<Object> target);
  BIGNUM* bignum_;
  static Nan::Persistent<Function> js_conditioner;
  static void SetJSConditioner(Local<Function> constructor);

protected:
  static Nan::Persistent<FunctionTemplate> constructor_template;

  BigNum(const Nan::Utf8String& str, uint64_t base);
  BigNum(uint64_t num);
  BigNum(int64_t num);
  BigNum(BIGNUM *num);
  BigNum();
  ~BigNum();

  static NAN_METHOD(New);
  static NAN_METHOD(ToString);
  static NAN_METHOD(Badd);
  static NAN_METHOD(Bsub);
  static NAN_METHOD(Bmul);
  static NAN_METHOD(Bdiv);
  static NAN_METHOD(Uadd);
  static NAN_METHOD(Usub);
  static NAN_METHOD(Umul);
  static NAN_METHOD(Udiv);
  static NAN_METHOD(Umul_2exp);
  static NAN_METHOD(Udiv_2exp);
  static NAN_METHOD(Babs);
  static NAN_METHOD(Bneg);
  static NAN_METHOD(Bmod);
  static NAN_METHOD(Umod);
  static NAN_METHOD(Bpowm);
  static NAN_METHOD(Upowm);
  static NAN_METHOD(Upow);
  static NAN_METHOD(Uupow);
  static NAN_METHOD(Brand0);
  static NAN_METHOD(Uprime0);
  static NAN_METHOD(Probprime);
  static NAN_METHOD(Bcompare);
  static NAN_METHOD(Scompare);
  static NAN_METHOD(Ucompare);
  static NAN_METHOD(Band);
  static NAN_METHOD(Bor);
  static NAN_METHOD(Bxor);
  static NAN_METHOD(Binvertm);
  static NAN_METHOD(Bsqrt);
  static NAN_METHOD(Broot);
  static NAN_METHOD(BitLength);
  static NAN_METHOD(Bgcd);
  static NAN_METHOD(Bjacobi);
  static NAN_METHOD(Bsetcompact);
  static NAN_METHOD(IsBitSet);
  static Local<Value> Bop(Nan::NAN_METHOD_ARGS_TYPE info, int op);
};

Nan::Persistent<FunctionTemplate> BigNum::constructor_template;

Nan::Persistent<Function> BigNum::js_conditioner;

void BigNum::SetJSConditioner(Local<Function> constructor) {
  js_conditioner.Reset(constructor);
}

void BigNum::Initialize(v8::Local<v8::Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> tmpl = Nan::New<FunctionTemplate>(New);
  constructor_template.Reset(tmpl);

  tmpl->InstanceTemplate()->SetInternalFieldCount(1);
  tmpl->SetClassName(Nan::New("BigNum").ToLocalChecked());

  Nan::SetMethod(tmpl, "uprime0", Uprime0);

  Nan::SetPrototypeMethod(tmpl, "tostring", ToString);
  Nan::SetPrototypeMethod(tmpl, "badd", Badd);
  Nan::SetPrototypeMethod(tmpl, "bsub", Bsub);
  Nan::SetPrototypeMethod(tmpl, "bmul", Bmul);
  Nan::SetPrototypeMethod(tmpl, "bdiv", Bdiv);
  Nan::SetPrototypeMethod(tmpl, "uadd", Uadd);
  Nan::SetPrototypeMethod(tmpl, "usub", Usub);
  Nan::SetPrototypeMethod(tmpl, "umul", Umul);
  Nan::SetPrototypeMethod(tmpl, "udiv", Udiv);
  Nan::SetPrototypeMethod(tmpl, "umul2exp", Umul_2exp);
  Nan::SetPrototypeMethod(tmpl, "udiv2exp", Udiv_2exp);
  Nan::SetPrototypeMethod(tmpl, "babs", Babs);
  Nan::SetPrototypeMethod(tmpl, "bneg", Bneg);
  Nan::SetPrototypeMethod(tmpl, "bmod", Bmod);
  Nan::SetPrototypeMethod(tmpl, "umod", Umod);
  Nan::SetPrototypeMethod(tmpl, "bpowm", Bpowm);
  Nan::SetPrototypeMethod(tmpl, "upowm", Upowm);
  Nan::SetPrototypeMethod(tmpl, "upow", Upow);
  Nan::SetPrototypeMethod(tmpl, "brand0", Brand0);
  Nan::SetPrototypeMethod(tmpl, "probprime", Probprime);
  Nan::SetPrototypeMethod(tmpl, "bcompare", Bcompare);
  Nan::SetPrototypeMethod(tmpl, "scompare", Scompare);
  Nan::SetPrototypeMethod(tmpl, "ucompare", Ucompare);
  Nan::SetPrototypeMethod(tmpl, "band", Band);
  Nan::SetPrototypeMethod(tmpl, "bor", Bor);
  Nan::SetPrototypeMethod(tmpl, "bxor", Bxor);
  Nan::SetPrototypeMethod(tmpl, "binvertm", Binvertm);
  Nan::SetPrototypeMethod(tmpl, "bsqrt", Bsqrt);
  Nan::SetPrototypeMethod(tmpl, "broot", Broot);
  Nan::SetPrototypeMethod(tmpl, "bitLength", BitLength);
  Nan::SetPrototypeMethod(tmpl, "gcd", Bgcd);
  Nan::SetPrototypeMethod(tmpl, "jacobi", Bjacobi);
  Nan::SetPrototypeMethod(tmpl, "setCompact", Bsetcompact);
  Nan::SetPrototypeMethod(tmpl, "isbitset", IsBitSet);

  v8::Isolate *isolate = v8::Isolate::GetCurrent();
  Nan::Set(target, Nan::New("BigNum").ToLocalChecked(), tmpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
}

BigNum::BigNum(const Nan::Utf8String& str, uint64_t base) : Nan::ObjectWrap (),
    bignum_(BN_new())
{
  BN_zero(bignum_);

  BIGNUM *res = bignum_;

  const char *cstr = *str;
  switch (base) {
  case 2:
    for (int i = 0, l = str.length(); i < l; i++) {
      if (cstr[l-i-1] != '0') {
        BN_set_bit(bignum_, i);
      }
    }
    break;
  case 10:
    BN_dec2bn(&res, cstr);
    break;
  case 16:
    BN_hex2bn(&res, cstr);
    break;
  default:
    Nan::ThrowError("Invalid base, only 10 and 16 are supported");
    return;
  }
}

BigNum::BigNum(uint64_t num) : Nan::ObjectWrap (),
    bignum_(BN_new())
{
  if (sizeof(BN_ULONG) >= 8 || num <= 0xFFFFFFFFL) {
    BN_set_word(bignum_, num);
  } else {
    BN_set_word(bignum_, num >> 32);
    BN_lshift(bignum_, bignum_, 32);
    BN_add_word(bignum_, num & 0xFFFFFFFFL);
  }
}

BigNum::BigNum(int64_t num) : Nan::ObjectWrap (),
    bignum_(BN_new())
{
  bool neg = (num < 0);

  if (neg) {
    num = -num;
  }
  if (num < 0) { // num is -2^63
    BN_one(bignum_);
    BN_lshift(bignum_, bignum_, 63);
  } else if (sizeof(BN_ULONG) >= 8 || num <= 0xFFFFFFFFL) {
    BN_set_word(bignum_, num);
  } else {
    BN_set_word(bignum_, num >> 32);
    BN_lshift(bignum_, bignum_, 32);
    BN_add_word(bignum_, num & 0xFFFFFFFFL);
  }
  if (neg) {
    BN_set_negative(bignum_, 1);
  }
}

BigNum::BigNum(BIGNUM *num) : Nan::ObjectWrap (),
    bignum_(BN_new())
{
  BN_copy(bignum_, num);
}

BigNum::BigNum() : Nan::ObjectWrap (),
    bignum_(BN_new())
{
  BN_zero(bignum_);
}

BigNum::~BigNum()
{
  BN_clear_free(bignum_);
}

NAN_METHOD(BigNum::New)
{
  if (!info.IsConstructCall()) {
    int len = info.Length();
    Local<Value>* newArgs = new Local<Value>[len];
    for (int i = 0; i < len; i++) {
      newArgs[i] = info[i];
    }

    Nan::TryCatch tryCatch;
    Nan::MaybeLocal<Object> newInstMaybeLocal = Nan::NewInstance(
        Nan::New<FunctionTemplate>(constructor_template)->GetFunction(info.GetIsolate()->GetCurrentContext()).ToLocalChecked(), len, newArgs);
    if (tryCatch.HasCaught()) {
        tryCatch.ReThrow();
        return;
    }

    Local<Value> newInst = newInstMaybeLocal.ToLocalChecked();
    delete[] newArgs;
    info.GetReturnValue().Set(newInst);
    return;
  }

  BigNum *bignum;
  uint64_t base;

  Local<Context> currentContext = info.GetIsolate()->GetCurrentContext();

  if (info[0]->IsExternal()) {
    bignum = static_cast<BigNum*>(External::Cast(*(info[0]))->Value());
  } else {
    int len = info.Length();
    Local<Object> ctx = Nan::New<Object>();
    Local<Value>* newArgs = new Local<Value>[len];
    for (int i = 0; i < len; i++) {
      newArgs[i] = info[i];
    }
    Local<Value> obj;
    const int ok = Nan::New<Function>(js_conditioner)->
      Call(currentContext, ctx, info.Length(), newArgs).ToLocal(&obj);
    delete[] newArgs;

    if (!ok) {
      Nan::ThrowError("Invalid type passed to bignum constructor");
      return;
    }

    Nan::Utf8String str(Nan::Get(obj->ToObject(currentContext).ToLocalChecked(), Nan::New("num").ToLocalChecked()).ToLocalChecked()->ToString(currentContext).ToLocalChecked());
    base = Nan::To<int64_t>(Nan::Get(obj->ToObject(currentContext).ToLocalChecked(), Nan::New("base").ToLocalChecked()).ToLocalChecked()).FromJust();

    bignum = new BigNum(str, base);
  }

  bignum->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(BigNum::ToString)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  uint64_t base = 10;

  if (info.Length() > 0) {
    REQ_UINT64_ARG(0, tbase);
    base = tbase;
  }
  char *to = NULL;
  switch (base) {
  case 10:
    to = BN_bn2dec(bignum->bignum_);
    break;
  case 16:
    to = BN_bn2hex(bignum->bignum_);
    break;
  default:
    Nan::ThrowError("Invalid base, only 10 and 16 are supported");
    return;
  }

  Local<Value> result = Nan::New<String>(to).ToLocalChecked();
  OPENSSL_free(to);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Badd)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  BigNum *bn = Nan::ObjectWrap::Unwrap<BigNum>(info[0]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
  BigNum *res = new BigNum();

  BN_add(res->bignum_, bignum->bignum_, bn->bignum_);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Bsub)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  BigNum *bn = Nan::ObjectWrap::Unwrap<BigNum>(info[0]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
  BigNum *res = new BigNum();
  BN_sub(res->bignum_, bignum->bignum_, bn->bignum_);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Bmul)
{
  AutoBN_CTX ctx;
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  BigNum *bn = Nan::ObjectWrap::Unwrap<BigNum>(info[0]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
  BigNum *res = new BigNum();
  BN_mul(res->bignum_, bignum->bignum_, bn->bignum_, ctx);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Bdiv)
{
  AutoBN_CTX ctx;
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  BigNum *bi = Nan::ObjectWrap::Unwrap<BigNum>(info[0]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
  BigNum *res = new BigNum();
  BN_div(res->bignum_, NULL, bignum->bignum_, bi->bignum_, ctx);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Uadd)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  REQ_UINT64_ARG(0, x);
  BigNum *res = new BigNum(bignum->bignum_);
  if (sizeof(BN_ULONG) >= 8 || x <= 0xFFFFFFFFL) {
    BN_add_word(res->bignum_, x);
  } else {
    BigNum *bn = new BigNum(x);
    BN_add(res->bignum_, bignum->bignum_, bn->bignum_);
  }

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Usub)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  REQ_UINT64_ARG(0, x);
  BigNum *res = new BigNum(bignum->bignum_);
  if (sizeof(BN_ULONG) >= 8 || x <= 0xFFFFFFFFL) {
    BN_sub_word(res->bignum_, x);
  } else {
    BigNum *bn = new BigNum(x);
    BN_sub(res->bignum_, bignum->bignum_, bn->bignum_);
  }

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Umul)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  REQ_UINT64_ARG(0, x);
  BigNum *res = new BigNum(bignum->bignum_);
  if (sizeof(BN_ULONG) >= 8 || x <= 0xFFFFFFFFL) {
    BN_mul_word(res->bignum_, x);
  } else {
    AutoBN_CTX ctx;
    BigNum *bn = new BigNum(x);
    BN_mul(res->bignum_, bignum->bignum_, bn->bignum_, ctx);
  }

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Udiv)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  REQ_UINT64_ARG(0, x);
  BigNum *res = new BigNum(bignum->bignum_);
  if (sizeof(BN_ULONG) >= 8 || x <= 0xFFFFFFFFL) {
    BN_div_word(res->bignum_, x);
  } else {
    AutoBN_CTX ctx;
    BigNum *bn = new BigNum(x);
    BN_div(res->bignum_, NULL, bignum->bignum_, bn->bignum_, ctx);
  }

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Umul_2exp)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  REQ_UINT32_ARG(0, x);
  BigNum *res = new BigNum();
  BN_lshift(res->bignum_, bignum->bignum_, x);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Udiv_2exp)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  REQ_UINT32_ARG(0, x);
  BigNum *res = new BigNum();
  BN_rshift(res->bignum_, bignum->bignum_, x);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Babs)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  BigNum *res = new BigNum(bignum->bignum_);
  BN_set_negative(res->bignum_, 0);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Bneg)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  BigNum *res = new BigNum(bignum->bignum_);
  BN_set_negative(res->bignum_, !BN_is_negative(res->bignum_));

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Bmod)
{
  AutoBN_CTX ctx;
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  BigNum *bn = Nan::ObjectWrap::Unwrap<BigNum>(info[0]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
  BigNum *res = new BigNum();
  BN_div(NULL, res->bignum_, bignum->bignum_, bn->bignum_, ctx);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Umod)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  REQ_UINT64_ARG(0, x);
  BigNum *res = new BigNum();
  if (sizeof(BN_ULONG) >= 8 || x <= 0xFFFFFFFFL) {
    BN_set_word(res->bignum_, BN_mod_word(bignum->bignum_, x));
  } else {
    AutoBN_CTX ctx;
    BigNum *bn = new BigNum(x);
    BN_div(NULL, res->bignum_, bignum->bignum_, bn->bignum_, ctx);
  }

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Bpowm)
{
  AutoBN_CTX ctx;
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  BigNum *bn1 = Nan::ObjectWrap::Unwrap<BigNum>(info[0]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
  BigNum *bn2 = Nan::ObjectWrap::Unwrap<BigNum>(info[1]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
  BigNum *res = new BigNum();
  BN_mod_exp(res->bignum_, bignum->bignum_, bn1->bignum_, bn2->bignum_, ctx);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Upowm)
{
  AutoBN_CTX ctx;
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  REQ_UINT64_ARG(0, x);
  BigNum *bn = Nan::ObjectWrap::Unwrap<BigNum>(info[1]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
  BigNum *exp = new BigNum(x);

  BigNum *res = new BigNum();
  BN_mod_exp(res->bignum_, bignum->bignum_, exp->bignum_, bn->bignum_, ctx);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Upow)
{
  AutoBN_CTX ctx;
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  REQ_UINT64_ARG(0, x);
  BigNum *exp = new BigNum(x);

  BigNum *res = new BigNum();
  BN_exp(res->bignum_, bignum->bignum_, exp->bignum_, ctx);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Brand0)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  BigNum *res = new BigNum();

  BN_rand_range(res->bignum_, bignum->bignum_);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Uprime0)
{
  REQ_UINT32_ARG(0, x);
  REQ_BOOL_ARG(1, safe);

  BigNum *res = new BigNum();

  BN_generate_prime_ex(res->bignum_, x, safe, NULL, NULL, NULL);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Probprime)
{
  AutoBN_CTX ctx;
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  REQ_UINT32_ARG(0, reps);

  info.GetReturnValue().Set(Nan::New<Number>(BN_is_prime_ex(bignum->bignum_, reps, ctx, NULL)));
}

NAN_METHOD(BigNum::IsBitSet)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  REQ_UINT32_ARG(0, n);

  info.GetReturnValue().Set(Nan::New<Number>(BN_is_bit_set(bignum->bignum_, n)));
}

NAN_METHOD(BigNum::Bcompare)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  BigNum *bn = Nan::ObjectWrap::Unwrap<BigNum>(info[0]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());

  info.GetReturnValue().Set(Nan::New<Number>(BN_cmp(bignum->bignum_, bn->bignum_)));
}

NAN_METHOD(BigNum::Scompare)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  REQ_INT64_ARG(0, x);
  BigNum *bn = new BigNum(x);
  int res = BN_cmp(bignum->bignum_, bn->bignum_);

  info.GetReturnValue().Set(Nan::New<Number>(res));
}

NAN_METHOD(BigNum::Ucompare)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  REQ_UINT64_ARG(0, x);
  int res;
  if (sizeof(BN_ULONG) >= 8 || x <= 0xFFFFFFFFL) {
    BIGNUM* bn = BN_new();
    BN_set_word(bn, x);
    res = BN_cmp(bignum->bignum_, bn);
    BN_clear_free(bn);
  } else {
    BigNum *bn = new BigNum(x);
    res = BN_cmp(bignum->bignum_, bn->bignum_);
  }

  info.GetReturnValue().Set(Nan::New<Number>(res));
}

// Utility functions from converting OpenSSL's MPI
// format to two's complement, which is what bitwise
// operations are expected to work on
static void
mpi2twosComplement(uint8_t *bytes, size_t numBytes)
{
  int i;

  bytes[0] &= ~0x80;
  for (i = 0; i < (int) numBytes; i++) {
    bytes[i] = ~bytes[i];
  }
  for (i = numBytes - 1; i >= 0; i--) {
    if (bytes[i] == 0xff) {
      bytes[i] = 0;
    } else {
      bytes[i]++;
      break;
    }
  }
}

static void
twos_complement2mpi(uint8_t *bytes, size_t numBytes)
{
  int i;

  for (i = numBytes - 1; i >= 0; i--) {
    if (bytes[i] == 0) {
      bytes[i] = 0xff;
    } else {
      bytes[i]--;
      break;
    }
  }
  for (i = 0; i < (int) numBytes; i++) {
    bytes[i] = ~bytes[i];
  }
  bytes[0] |= 0x80;
}

const int BN_PAYLOAD_OFFSET = 4;

// Shifts things around in an OpenSSL MPI buffer so that
// the sizes of bignum operands match
static void
shiftSizeAndMSB(uint8_t *bytes, uint8_t *sizeBuffer, size_t offset)
{
  memset(bytes + offset, 0, BN_PAYLOAD_OFFSET);
  memcpy(bytes, sizeBuffer, BN_PAYLOAD_OFFSET);

  // We've copied the size header over; now we just need to move
  // the MSB signifying negativity if it's present
  if(bytes[BN_PAYLOAD_OFFSET + offset] & 0x80) {
    bytes[BN_PAYLOAD_OFFSET] |= 0x80;
    bytes[BN_PAYLOAD_OFFSET + offset] &= ~0x80;
  }
}

static bool
isMinimumNegativeNumber(uint8_t *bytes, size_t size)
{
  if (bytes[0] != 0x80) {
    return false;
  }

  for (size_t i = 1; i < size; i++) {
    if (bytes[i] != 0) {
      return false;
    }
  }

  return true;
}

static void
swapEndianness(uint8_t *bytes)
{
  uint8_t tmp;

  tmp = bytes[0];
  bytes[0] = bytes[3];
  bytes[3] = tmp;

  tmp = bytes[1];
  bytes[1] = bytes[2];
  bytes[2] = tmp;
}

Local<Value>
BigNum::Bop(Nan::NAN_METHOD_ARGS_TYPE info, int op)
{
  Nan::EscapableHandleScope scope;

  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());
  BigNum *bn = Nan::ObjectWrap::Unwrap<BigNum>(info[0]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());

  bool bignumNegative = BN_is_negative(bignum->bignum_);
  bool bnNegative = BN_is_negative(bn->bignum_);

  BigNum *res = new BigNum();

  // Modified from https://github.com/Worlize/WebSocket-Node/blob/master/src/xor.cpp
  // Portions Copyright (c) Agora S.A.
  // Licensed under the MIT License.

  int payloadSize = BN_bn2mpi(bignum->bignum_, NULL);
  int maskSize = BN_bn2mpi(bn->bignum_, NULL);

  uint32_t size = max(payloadSize, maskSize);
  int offset = abs(payloadSize - maskSize);

  int payloadOffset = 0;
  int maskOffset = 0;

  if (payloadSize < maskSize) {
    payloadOffset = offset;
  } else if (payloadSize > maskSize) {
    maskOffset = offset;
  }

  uint8_t* payload = (uint8_t*) calloc(size, sizeof(char));
  uint8_t* mask = (uint8_t*) calloc(size, sizeof(char));

  BN_bn2mpi(bignum->bignum_, payload + payloadOffset);
  BN_bn2mpi(bn->bignum_, mask + maskOffset);

  if (payloadSize < maskSize) {
    shiftSizeAndMSB(payload, mask, payloadOffset);
  } else {
    shiftSizeAndMSB(mask, payload, maskOffset);
  }

  payload += BN_PAYLOAD_OFFSET;
  mask += BN_PAYLOAD_OFFSET;
  size -= BN_PAYLOAD_OFFSET;

  if(bignumNegative) {
    mpi2twosComplement(payload, size);
  }
  if(bnNegative) {
    mpi2twosComplement(mask, size);
  }

  uint32_t* pos32 = (uint32_t*) payload;
  uint32_t* end32 = pos32 + (size / 4);
  uint32_t* mask32 = (uint32_t*) mask;

  switch (op) {
    case 0: while (pos32 < end32) *(pos32++) &= *(mask32++); break;
    case 1: while (pos32 < end32) *(pos32++) |= *(mask32++); break;
    case 2: while (pos32 < end32) *(pos32++) ^= *(mask32++); break;
  }

  uint8_t* pos8 = (uint8_t*) pos32;
  uint8_t* end8 = payload + size;
  uint8_t* mask8 = (uint8_t*) mask32;

  switch (op) {
    case 0: while (pos8 < end8) *(pos8++) &= *(mask8++); break;
    case 1: while (pos8 < end8) *(pos8++) |= *(mask8++); break;
    case 2: while (pos8 < end8) *(pos8++) ^= *(mask8++); break;
  }

  payload -= BN_PAYLOAD_OFFSET;
  mask -= BN_PAYLOAD_OFFSET;
  size += BN_PAYLOAD_OFFSET;

  // If the value is the largest negative number representible by
  // size bytes, we need to add another byte to the payload buffer,
  // otherwise OpenSSL's BN_mpi2bn will interpret the number as -0
  if (isMinimumNegativeNumber(payload + BN_PAYLOAD_OFFSET, size - BN_PAYLOAD_OFFSET)) {
    bool bigEndian = (size - BN_PAYLOAD_OFFSET) == *((uint32_t *) payload);

    uint8_t *newPayload = (uint8_t *) calloc(size + 1, 1);

    memcpy(newPayload + 5, payload + BN_PAYLOAD_OFFSET, size - BN_PAYLOAD_OFFSET);
    newPayload[BN_PAYLOAD_OFFSET] = 0x80;
    size++;

    size -= BN_PAYLOAD_OFFSET;
    memcpy(newPayload, &size, BN_PAYLOAD_OFFSET);
    size += BN_PAYLOAD_OFFSET;

    if (!bigEndian) {
      swapEndianness(newPayload);
    }

    free(payload);
    payload = newPayload;
  } else if(payload[BN_PAYLOAD_OFFSET] & 0x80) {
    twos_complement2mpi(payload + BN_PAYLOAD_OFFSET, size - BN_PAYLOAD_OFFSET);
  }

  BN_mpi2bn(payload, size, res->bignum_);

  WRAP_RESULT(res, result);

  free(payload);
  free(mask);

  return scope.Escape(result);
}

NAN_METHOD(BigNum::Band)
{
  info.GetReturnValue().Set(Bop(info, 0));
}

NAN_METHOD(BigNum::Bor)
{
  info.GetReturnValue().Set(Bop(info, 1));
}

NAN_METHOD(BigNum::Bxor)
{
  info.GetReturnValue().Set(Bop(info, 2));
}

NAN_METHOD(BigNum::Binvertm)
{
  AutoBN_CTX ctx;
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  BigNum *bn = Nan::ObjectWrap::Unwrap<BigNum>(info[0]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
  BigNum *res = new BigNum();
  BN_mod_inverse(res->bignum_, bignum->bignum_, bn->bignum_, ctx);

  WRAP_RESULT(res, result);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Bsqrt)
{
  Nan::ThrowError("sqrt is not supported by OpenSSL.");
}

NAN_METHOD(BigNum::Broot)
{
  Nan::ThrowError("root is not supported by OpenSSL.");
}

NAN_METHOD(BigNum::BitLength)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  int size = BN_num_bits(bignum->bignum_);
  Local<Value> result = Nan::New<Integer>(size);

  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Bgcd)
{
  AutoBN_CTX ctx;
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  BigNum *bi = Nan::ObjectWrap::Unwrap<BigNum>(info[0]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
  BigNum *res = new BigNum();

  BN_gcd(res->bignum_, bignum->bignum_, bi->bignum_, ctx);

  WRAP_RESULT(res, result);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(BigNum::Bjacobi)
{
  AutoBN_CTX ctx;
  BigNum *bn_a = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  BigNum *bn_n = Nan::ObjectWrap::Unwrap<BigNum>(info[0]->ToObject(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
  int res = 0;

  if (BN_jacobi_priv(bn_a->bignum_, bn_n->bignum_, &res, ctx) == -1) {
    Nan::ThrowError("Jacobi symbol calculation failed");
    return;
  }

  info.GetReturnValue().Set(Nan::New<Integer>(res));
}

NAN_METHOD(BigNum::Bsetcompact)
{
  BigNum *bignum = Nan::ObjectWrap::Unwrap<BigNum>(info.This());

  unsigned int nCompact = Nan::To<uint32_t>(info[0]).FromJust();
  unsigned int nSize = nCompact >> 24;
  bool fNegative     =(nCompact & 0x00800000) != 0;
  unsigned int nWord = nCompact & 0x007fffff;
  if (nSize <= 3)
  {
      nWord >>= 8*(3-nSize);
      BN_set_word(bignum->bignum_, nWord);
  }
  else
  {
      BN_set_word(bignum->bignum_, nWord);
      BN_lshift(bignum->bignum_, bignum->bignum_, 8*(nSize-3));
  }
  BN_set_negative(bignum->bignum_, fNegative);

  info.GetReturnValue().Set(info.This());
}

static NAN_METHOD(SetJSConditioner)
{
  Nan::HandleScope scope;

  BigNum::SetJSConditioner(Local<Function>::Cast(info[0]));

  return;
}

extern "C" void
init (Local<Object> target)
{
  Nan::HandleScope scope;

  BigNum::Initialize(target);
  Nan::SetMethod(target, "setJSConditioner", SetJSConditioner);
}

NODE_MODULE(bignum, init)
