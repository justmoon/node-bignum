#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <v8.h>
#include <node.h>
#include <openssl/bn.h>
#include <map>
#include <utility>

using namespace v8;
using namespace node;
using namespace std;

#define REQ_STR_ARG(I, VAR)                                   \
  if (args.Length()<= (I) || !args[I]->IsString())            \
    return ThrowException(Exception::TypeError(               \
      String::New("Argument " #I " must be a string")));      \
  Local<String> VAR = Local<String>::Cast(args[I]);

#define REQ_UTF8_ARG(I, VAR)                                  \
  if (args.Length() <= (I) || !args[I]->IsString())           \
    return ThrowException(Exception::TypeError(               \
      String::New("Argument " #I " must be a utf8 string"))); \
  String::Utf8Value VAR(args[I]->ToString());

#define REQ_INT32_ARG(I, VAR)                                 \
  if (args.Length() <= (I) || !args[I]->IsInt32())            \
    return ThrowException(Exception::TypeError(               \
      String::New("Argument " #I " must be an int32")));      \
  int32_t VAR = args[I]->ToInt32()->Value();

#define REQ_UINT32_ARG(I, VAR)                                \
  if (args.Length() <= (I) || !args[I]->IsUint32())           \
    return ThrowException(Exception::TypeError(               \
      String::New("Argument " #I " must be a uint32")));      \
  uint32_t VAR = args[I]->ToUint32()->Value();

#define REQ_INT64_ARG(I, VAR)                                 \
  if (args.Length() <= (I) || !args[I]->IsNumber())           \
    return ThrowException(Exception::TypeError(               \
      String::New("Argument " #I " must be an int64")));      \
  int64_t VAR = args[I]->ToInteger()->Value();

#define REQ_UINT64_ARG(I, VAR)                                \
  if (args.Length() <= (I) || !args[I]->IsNumber())           \
    return ThrowException(Exception::TypeError(               \
      String::New("Argument " #I " must be a uint64")));      \
  uint64_t VAR = args[I]->ToInteger()->Value();

#define WRAP_RESULT(RES, VAR)                                 \
  Handle<Value> arg[1] = { External::New(RES) };              \
  Local<Object> VAR = constructor_template->GetFunction()->NewInstance(1, arg);

class AutoBN_CTX
{
protected:
  BN_CTX* ctx;
  BN_CTX* operator=(BN_CTX* ctx_new) { return ctx = ctx_new; }

public:
  AutoBN_CTX()
  {
    ctx = BN_CTX_new();
    // TODO: Handle this error some way that Node.js can catch
    if (ctx == NULL)
      throw "AutoBN_CTX : BN_CTX_new() returned NULL";
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

class BigNum : ObjectWrap {
public:
  static void Initialize(Handle<Object> target);
  BIGNUM *bignum_;
  static Persistent<Function> js_conditioner;
  static void SetJSConditioner(Persistent<Function> constructor);

protected:
  static Persistent<FunctionTemplate> constructor_template;

  BigNum(const String::Utf8Value& str, uint64_t base);
  BigNum(uint64_t num);
  BigNum(int64_t num);
  BigNum(BIGNUM *num);
  BigNum();
  ~BigNum();

  static Handle<Value> New(const Arguments& args);
  static Handle<Value> ToString(const Arguments& args);
  static Handle<Value> Badd(const Arguments& args);
  static Handle<Value> Bsub(const Arguments& args);
  static Handle<Value> Bmul(const Arguments& args);
  static Handle<Value> Bdiv(const Arguments& args);
  static Handle<Value> Uadd(const Arguments& args);
  static Handle<Value> Usub(const Arguments& args);
  static Handle<Value> Umul(const Arguments& args);
  static Handle<Value> Udiv(const Arguments& args);
  static Handle<Value> Umul_2exp(const Arguments& args);
  static Handle<Value> Udiv_2exp(const Arguments& args);
  static Handle<Value> Babs(const Arguments& args);
  static Handle<Value> Bneg(const Arguments& args);
  static Handle<Value> Bmod(const Arguments& args);
  static Handle<Value> Umod(const Arguments& args);
  static Handle<Value> Bpowm(const Arguments& args);
  static Handle<Value> Upowm(const Arguments& args);
  static Handle<Value> Upow(const Arguments& args);
  static Handle<Value> Uupow(const Arguments& args);
  static Handle<Value> Brand0(const Arguments& args);
  static Handle<Value> Probprime(const Arguments& args);
  static Handle<Value> Nextprime(const Arguments& args);
  static Handle<Value> Bcompare(const Arguments& args);
  static Handle<Value> Scompare(const Arguments& args);
  static Handle<Value> Ucompare(const Arguments& args);
  static Handle<Value> Band(const Arguments& args);
  static Handle<Value> Bor(const Arguments& args);
  static Handle<Value> Bxor(const Arguments& args);
  static Handle<Value> Binvertm(const Arguments& args);
  static Handle<Value> Bsqrt(const Arguments& args);
  static Handle<Value> Broot(const Arguments& args);
};

Persistent<FunctionTemplate> BigNum::constructor_template;

Persistent<Function> BigNum::js_conditioner;

void BigNum::SetJSConditioner(Persistent<Function> constructor) {
  js_conditioner = constructor;
}

void BigNum::Initialize(v8::Handle<v8::Object> target) {
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);
  constructor_template = Persistent<FunctionTemplate>::New(t);

  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("BigNum"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "tostring", ToString);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "badd", Badd);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "bsub", Bsub);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "bmul", Bmul);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "bdiv", Bdiv);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "uadd", Uadd);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "usub", Usub);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "umul", Umul);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "udiv", Udiv);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "umul2exp", Umul_2exp);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "udiv2exp", Udiv_2exp);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "babs", Babs);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "bneg", Bneg);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "bmod", Bmod);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "umod", Umod);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "bpowm", Bpowm);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "upowm", Upowm);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "upow", Upow);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "brand0", Brand0);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "probprime", Probprime);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "bcompare", Bcompare);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "scompare", Scompare);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "ucompare", Ucompare);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "band", Band);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "bor", Bor);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "bxor", Bxor);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "binvertm", Binvertm);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "bsqrt", Bsqrt);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "broot", Broot);

  target->Set(String::NewSymbol("BigNum"), constructor_template->GetFunction());
}

BigNum::BigNum (const v8::String::Utf8Value& str, uint64_t base) : ObjectWrap ()
{
  bignum_ = NULL;

  const char *cstr = *str;
  switch (base) {
  case 2:
    bignum_ = BN_new();
    BN_init(bignum_);
    for (int i = 0, l = str.length(); i < l; i++) {
      if (cstr[l-i-1] != '0') {
        BN_set_bit(bignum_, i);
      }
    }
    break;
  case 10:
    BN_dec2bn(&bignum_, cstr);
    break;
  case 16:
    BN_hex2bn(&bignum_, cstr);
    break;
  default:
    ThrowException(Exception::Error(String::New("Invalid base, only 10 and 16 are supported")));
    return;
  }
}

BigNum::BigNum (uint64_t num) : ObjectWrap ()
{
  bignum_ = BN_new();
  BN_init(bignum_);

  BN_set_word(bignum_, num);
}

BigNum::BigNum (int64_t num) : ObjectWrap ()
{
  bignum_ = BN_new();
  BN_init(bignum_);

  if (num > 0) {
    BN_set_word(bignum_, num);
  } else {
    BN_set_word(bignum_, -num);
    BN_set_negative(bignum_, 1);
  }
}

BigNum::BigNum (BIGNUM *num) : ObjectWrap ()
{
  bignum_ = num;
}

BigNum::BigNum () : ObjectWrap ()
{
  bignum_ = BN_new();
  BN_init(bignum_);

  BN_zero(bignum_);
}

BigNum::~BigNum ()
{
  BN_clear_free(bignum_);
}

Handle<Value>
BigNum::New(const Arguments& args)
{
  if (!args.IsConstructCall()) {
    int len = args.Length();
    Handle<Value>* newArgs = new Handle<Value>[len];
    for (int i = 0; i < len; i++) {
      newArgs[i] = args[i];
    }
    Handle<Value> newInst = constructor_template->GetFunction()->NewInstance(len, newArgs);
    delete[] newArgs;
    return newInst;
  }
  HandleScope scope;
  BigNum *bignum;
  uint64_t base;

  if (args[0]->IsExternal()) {
    BIGNUM *num = (BIGNUM *) External::Unwrap(args[0]);
    bignum = new BigNum(num);
  } else {
    int len = args.Length();
    Local<Object> ctx = Local<Object>::New(Object::New());
    Handle<Value>* newArgs = new Handle<Value>[len];
    for (int i = 0; i < len; i++) {
      newArgs[i] = args[i];
    }
    Local<Value> obj = js_conditioner->Call(ctx, args.Length(), newArgs);

    if (!*obj) {
      return ThrowException(Exception::Error(String::New("Invalid type passed to bignum constructor")));
    }

    String::Utf8Value str(obj->ToObject()->Get(String::NewSymbol("num"))->ToString());
    base = obj->ToObject()->Get(String::NewSymbol("base"))->ToNumber()->Value();

    bignum = new BigNum(str, base);
    delete[] newArgs;
  }

  bignum->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value>
BigNum::ToString(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  uint64_t base = 10;

  if (args.Length() > 0) {
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
    return ThrowException(Exception::Error(String::New("Invalid base, only 10 and 16 are supported")));
  }

  Handle<Value> result = String::New(to);
  free(to);

  return scope.Close(result);
}

Handle<Value>
BigNum::Badd(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  BigNum *bn = ObjectWrap::Unwrap<BigNum>(args[0]->ToObject());
  BIGNUM *res = BN_new();
  BN_init(res);

  BN_add(res, bignum->bignum_, bn->bignum_);

  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Bsub(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  BigNum *bn = ObjectWrap::Unwrap<BigNum>(args[0]->ToObject());
  BIGNUM *res = BN_new();
  BN_init(res);
  BN_sub(res, bignum->bignum_, bn->bignum_);

  WRAP_RESULT(res, result);
  
  return scope.Close(result);
}

Handle<Value>
BigNum::Bmul(const Arguments& args)
{
  AutoBN_CTX ctx;
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  BigNum *bn = ObjectWrap::Unwrap<BigNum>(args[0]->ToObject());
  BIGNUM *res = BN_new();
  BN_init(res);
  BN_mul(res, bignum->bignum_, bn->bignum_, ctx);
  
  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Bdiv(const Arguments& args)
{
  AutoBN_CTX ctx;
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  BigNum *bi = ObjectWrap::Unwrap<BigNum>(args[0]->ToObject());
  BIGNUM *res = BN_new();
  BN_init(res);
  BN_div(res, NULL, bignum->bignum_, bi->bignum_, ctx);
  
  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Uadd(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  REQ_UINT64_ARG(0, x);
  BIGNUM *res = BN_dup(bignum->bignum_);
  BN_add_word(res, x);
  
  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Usub(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  REQ_UINT64_ARG(0, x);
  BIGNUM *res = BN_dup(bignum->bignum_);
  BN_sub_word(res, x);
  
  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Umul(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  REQ_UINT64_ARG(0, x);
  BIGNUM *res = BN_dup(bignum->bignum_);
  BN_mul_word(res, x);
  
  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Udiv(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  REQ_UINT64_ARG(0, x);
  BIGNUM *res = BN_dup(bignum->bignum_);
  BN_div_word(res, x);
  
  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Umul_2exp(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  REQ_UINT64_ARG(0, x);
  BIGNUM *res = BN_new();
  BN_init(res);
  BN_lshift(res, bignum->bignum_, x);
  
  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Udiv_2exp(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  REQ_UINT64_ARG(0, x);
  BIGNUM *res = BN_new();
  BN_init(res);
  BN_rshift(res, bignum->bignum_, x);
  
  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Babs(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  BIGNUM *res = BN_dup(bignum->bignum_);
  BN_set_negative(res, 0);
  
  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Bneg(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  BIGNUM *res = BN_dup(bignum->bignum_);
  BN_set_negative(res, !BN_is_negative(res));
  
  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Bmod(const Arguments& args)
{
  AutoBN_CTX ctx;
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  BigNum *bn = ObjectWrap::Unwrap<BigNum>(args[0]->ToObject());
  BIGNUM *res = BN_new();
  BN_init(res);
  BN_div(NULL, res, bignum->bignum_, bn->bignum_, ctx);
  
  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Umod(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  REQ_UINT64_ARG(0, x);
  BIGNUM *res = BN_new();
  BN_init(res);
  BN_set_word(res, BN_mod_word(bignum->bignum_, x));
  
  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Bpowm(const Arguments& args)
{
  AutoBN_CTX ctx;
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  BigNum *bn1 = ObjectWrap::Unwrap<BigNum>(args[0]->ToObject());
  BigNum *bn2 = ObjectWrap::Unwrap<BigNum>(args[1]->ToObject());
  BIGNUM *res = BN_new();
  BN_init(res);
  BN_mod_exp(res, bignum->bignum_, bn1->bignum_, bn2->bignum_, ctx);

  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Upowm(const Arguments& args)
{
  AutoBN_CTX ctx;
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  REQ_UINT64_ARG(0, x);
  BigNum *bn = ObjectWrap::Unwrap<BigNum>(args[1]->ToObject());
  BIGNUM *exp = BN_new();
  BN_init(exp);
  BN_set_word(exp, x);
  BIGNUM *res = BN_new();
  BN_init(res);
  BN_mod_exp(res, bignum->bignum_, exp, bn->bignum_, ctx);

  WRAP_RESULT(res, result);

  BN_free(exp);

  return scope.Close(result); 
}

Handle<Value>
BigNum::Upow(const Arguments& args)
{
  AutoBN_CTX ctx;
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  REQ_UINT64_ARG(0, x);
  BIGNUM *exp = BN_new();
  BN_init(exp);
  BN_set_word(exp, x);
  BIGNUM *res = BN_new();
  BN_init(res);
  
  BN_exp(res, bignum->bignum_, exp, ctx);
  
  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Brand0(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  BIGNUM *res = BN_new();
  BN_init(res);

  BN_rand_range(res, bignum->bignum_);

  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Probprime(const Arguments& args)
{
  AutoBN_CTX ctx;
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;
  
  REQ_UINT32_ARG(0, reps);

  return scope.Close(Number::New(BN_is_prime(bignum->bignum_, reps, NULL, ctx, NULL)));
}

Handle<Value>
BigNum::Bcompare(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;
  
  BigNum *bn = ObjectWrap::Unwrap<BigNum>(args[0]->ToObject());

  return scope.Close(Number::New(BN_cmp(bignum->bignum_, bn->bignum_)));
}

Handle<Value>
BigNum::Scompare(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  REQ_INT64_ARG(0, x);
  BIGNUM *bn = BN_new();
  if (x > 0) {
    BN_set_word(bn, x);
  } else {
    BN_set_word(bn, -x);
    BN_set_negative(bn, 1);
  }

  return scope.Close(Number::New(BN_cmp(bignum->bignum_, bn)));
}

Handle<Value>
BigNum::Ucompare(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;
  
  REQ_UINT64_ARG(0, x);
  BIGNUM *bn = BN_new();
  BN_init(bn);
  BN_set_word(bn, x);

  return scope.Close(Number::New(BN_cmp(bignum->bignum_, bn)));
}

Handle<Value>
BigNum::Band(const Arguments& args)
{
  HandleScope scope;

  return ThrowException(Exception::Error(String::New("Boolean operations not supported by OpenSSL")));
}

Handle<Value>
BigNum::Bor(const Arguments& args)
{
  HandleScope scope;

  return ThrowException(Exception::Error(String::New("Boolean operations not supported by OpenSSL")));
}

Handle<Value>
BigNum::Bxor(const Arguments& args)
{
  HandleScope scope;

  return ThrowException(Exception::Error(String::New("Boolean operations not supported by OpenSSL")));
}

Handle<Value>
BigNum::Binvertm(const Arguments& args)
{
  AutoBN_CTX ctx;
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  BigNum *bn = ObjectWrap::Unwrap<BigNum>(args[0]->ToObject());
  BIGNUM *res = BN_new();
  BN_init(res);
  BN_mod_inverse(res, bignum->bignum_, bn->bignum_, ctx);

  WRAP_RESULT(res, result);

  return scope.Close(result);
}

Handle<Value>
BigNum::Bsqrt(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  return ThrowException(Exception::Error(String::New("sqrt is not supported by OpenSSL.")));
}

Handle<Value>
BigNum::Broot(const Arguments& args)
{
  BigNum *bignum = ObjectWrap::Unwrap<BigNum>(args.This());
  HandleScope scope;

  return ThrowException(Exception::Error(String::New("root is not supported by OpenSSL.")));
}

static Handle<Value>
SetJSConditioner(const Arguments& args)
{
  HandleScope scope;

  BigNum::SetJSConditioner(Persistent<Function>::New(Local<Function>::Cast(args[0])));

  return Undefined();
}

extern "C" void
init (Handle<Object> target)
{
  HandleScope scope;

  BigNum::Initialize(target);
  NODE_SET_METHOD(target, "setJSConditioner", SetJSConditioner);
}
