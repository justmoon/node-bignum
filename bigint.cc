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
using namespace std;

#define REQ_STR_ARG(I, VAR)							\
	if (args.Length()<= (I) || !args[I]->IsString())			\
		return ThrowException(Exception::TypeError(			\
			String::New("Argument " #I " must be a string")));	\
	Local<String> VAR = Local<String>::Cast(args[I]);

#define REQ_UTF8_ARG(I, VAR)							\
	if (args.Length() <= (I) || !args[I]->IsString())			\
		return ThrowException(Exception::TypeError(                     \
			String::New("Argument " #I " must be a utf8 string"))); \
	String::Utf8Value VAR(args[I]->ToString());

#define REQ_INT32_ARG(I, VAR)							\
	if (args.Length() <= (I) || !args[I]->IsInt32())			\
		return ThrowException(Exception::TypeError(                     \
			String::New("Argument " #I " must be an int32")));      \
	int32_t VAR = args[I]->ToInt32()->Value();

#define REQ_UINT32_ARG(I, VAR)							\
	if (args.Length() <= (I) || !args[I]->IsUint32())			\
		return ThrowException(Exception::TypeError(                     \
			String::New("Argument " #I " must be a uint32")));      \
	uint32_t VAR = args[I]->ToUint32()->Value();

#define REQ_INT64_ARG(I, VAR)							\
	if (args.Length() <= (I) || !args[I]->IsNumber())			\
		return ThrowException(Exception::TypeError(                     \
			String::New("Argument " #I " must be an int64")));      \
	int64_t VAR = args[I]->ToInteger()->Value();

#define REQ_UINT64_ARG(I, VAR)							\
	if (args.Length() <= (I) || !args[I]->IsNumber())			\
		return ThrowException(Exception::TypeError(                     \
			String::New("Argument " #I " must be a uint64")));      \
	uint64_t VAR = args[I]->ToInteger()->Value();

#define WRAP_RESULT(RES, VAR)							\
	Handle<Value> arg[1] = { External::New(*RES) };				\
	Local<Object> VAR = constructor_template->GetFunction()->NewInstance(1, arg);

class BigInt : ObjectWrap {
	public:
		static void Initialize(Handle<Object> target);
		mpz_t *bigint_;
		static Persistent<Function> js_conditioner;
		static void SetJSConditioner(Persistent<Function> constructor);

	protected:
		static Persistent<FunctionTemplate> constructor_template;

		BigInt(const String::Utf8Value& str, uint64_t base);
		BigInt(uint64_t num);
		BigInt(int64_t num);
		BigInt(mpz_t *num);
		BigInt();
		~BigInt();

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

static gmp_randstate_t *		randstate	= NULL;

Persistent<FunctionTemplate> BigInt::constructor_template;

Persistent<Function> BigInt::js_conditioner;

void BigInt::SetJSConditioner(Persistent<Function> constructor) {
	js_conditioner = constructor;
}

void BigInt::Initialize(v8::Handle<v8::Object> target) {
	HandleScope scope;
	
	Local<FunctionTemplate> t = FunctionTemplate::New(New);
	constructor_template = Persistent<FunctionTemplate>::New(t);

	constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
	constructor_template->SetClassName(String::NewSymbol("BigInt"));

	NODE_SET_PROTOTYPE_METHOD(constructor_template, "toString", ToString);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "badd", Badd);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "bsub", Bsub);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "bmul", Bmul);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "bdiv", Bdiv);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "uadd", Uadd);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "usub", Usub);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "umul", Umul);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "udiv", Udiv);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "babs", Babs);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "bneg", Bneg);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "bmod", Bmod);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "umod", Umod);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "bpowm", Bpowm);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "upowm", Upowm);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "upow", Upow);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "uupow", Uupow);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "brand0", Brand0);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "probprime", Probprime);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "nextprime", Nextprime);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "bcompare", Bcompare);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "scompare", Scompare);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "ucompare", Ucompare);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "band", Band);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "bor", Bor);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "bxor", Bxor);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "binvertm", Binvertm);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "bsqrt", Bsqrt);
	NODE_SET_PROTOTYPE_METHOD(constructor_template, "broot", Broot);

	target->Set(String::NewSymbol("BigInt"), constructor_template->GetFunction());
}

BigInt::BigInt (const v8::String::Utf8Value& str, uint64_t base) : ObjectWrap ()
{
	bigint_ = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*bigint_);

	mpz_set_str(*bigint_, *str, base);
}

BigInt::BigInt (uint64_t num) : ObjectWrap ()
{
	bigint_ = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*bigint_);

	mpz_set_ui(*bigint_, num);
}

BigInt::BigInt (int64_t num) : ObjectWrap ()
{
	bigint_ = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*bigint_);

	mpz_set_si(*bigint_, num);
}

BigInt::BigInt (mpz_t *num) : ObjectWrap ()
{
	bigint_ = num;
}

BigInt::BigInt () : ObjectWrap ()
{
	bigint_ = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*bigint_);

	mpz_set_ui(*bigint_, 0);
}

BigInt::~BigInt ()
{
	mpz_clear(*bigint_);
	free(bigint_);
}

Handle<Value>
BigInt::New(const Arguments& args)
{
	if(!args.IsConstructCall()) {
		Handle<Value> newArgs[args.Length()];
		for(int i = 0; i < args.Length(); i++) {
			newArgs[i] = args[i];
		}
		return constructor_template->GetFunction()->NewInstance(args.Length(), newArgs);
	}
	HandleScope scope;
	BigInt *bigint;
	uint64_t base;

	if(args[0]->IsExternal()) {
		mpz_t *num = (mpz_t *) External::Unwrap(args[0]);
		bigint = new BigInt(num);
	} else {
		Local<Object> ctx = Local<Object>::New(Object::New());
		Handle<Value> newArgs[args.Length()];
		for(int i = 0; i < args.Length(); i++) {
			newArgs[i] = args[i];
		}
		Local<Value> obj = js_conditioner->Call(ctx, args.Length(), newArgs);

		String::Utf8Value str(obj->ToObject()->Get(String::NewSymbol("num"))->ToString());
		base = obj->ToObject()->Get(String::NewSymbol("base"))->ToNumber()->Value();

		bigint = new BigInt(str, base);
	}

	bigint->Wrap(args.This());

	return scope.Close(args.This());
}

Handle<Value>
BigInt::ToString(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	uint64_t base = 10;

	if(args.Length() > 0) {
		REQ_UINT64_ARG(0, tbase);
		base = tbase;
	}
	char *to = mpz_get_str(0, base, *bigint->bigint_);

	Handle<Value> result = String::New(to);
	free(to);

	return scope.Close(result);
}

Handle<Value>
BigInt::Badd(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	BigInt *bi = ObjectWrap::Unwrap<BigInt>(args[0]->ToObject());
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);

	mpz_add(*res, *bigint->bigint_, *bi->bigint_);

	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Bsub(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	BigInt *bi = ObjectWrap::Unwrap<BigInt>(args[0]->ToObject());
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_sub(*res, *bigint->bigint_, *bi->bigint_);

	WRAP_RESULT(res, result);
	
	return scope.Close(result);
}

Handle<Value>
BigInt::Bmul(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	BigInt *bi = ObjectWrap::Unwrap<BigInt>(args[0]->ToObject());
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_mul(*res, *bigint->bigint_, *bi->bigint_);
	
	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Bdiv(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	BigInt *bi = ObjectWrap::Unwrap<BigInt>(args[0]->ToObject());
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_div(*res, *bigint->bigint_, *bi->bigint_);
	
	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Uadd(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	REQ_UINT64_ARG(0, x);
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_add_ui(*res, *bigint->bigint_, x);
	
	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Usub(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	REQ_UINT64_ARG(0, x);
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_sub_ui(*res, *bigint->bigint_, x);
	
	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Umul(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	REQ_UINT64_ARG(0, x);
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_mul_ui(*res, *bigint->bigint_, x);
	
	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Udiv(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	REQ_UINT64_ARG(0, x);
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_div_ui(*res, *bigint->bigint_, x);
	
	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Babs(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_abs(*res, *bigint->bigint_);
	
	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Bneg(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_neg(*res, *bigint->bigint_);
	
	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Bmod(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	BigInt *bi = ObjectWrap::Unwrap<BigInt>(args[0]->ToObject());
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_mod(*res, *bigint->bigint_, *bi->bigint_);
	
	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Umod(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	REQ_UINT64_ARG(0, x);
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_mod_ui(*res, *bigint->bigint_, x);
	
	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Bpowm(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	BigInt *bi1 = ObjectWrap::Unwrap<BigInt>(args[0]->ToObject());
	BigInt *bi2 = ObjectWrap::Unwrap<BigInt>(args[1]->ToObject());
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_powm(*res, *bigint->bigint_, *bi1->bigint_, *bi2->bigint_);

	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Upowm(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	REQ_UINT64_ARG(0, x);
	BigInt *bi = ObjectWrap::Unwrap<BigInt>(args[1]->ToObject());
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_powm_ui(*res, *bigint->bigint_, x, *bi->bigint_);
	
	WRAP_RESULT(res, result);
	
	return scope.Close(result);	
}

Handle<Value>
BigInt::Upow(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	REQ_UINT64_ARG(0, x);
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_pow_ui(*res, *bigint->bigint_, x);
	
	WRAP_RESULT(res, result);

	return scope.Close(result);
}

/* 
 * This makes no sense?  It doesn't act on the object but is a
 * prototype method.
 */
Handle<Value>
BigInt::Uupow(const Arguments& args)
{
	HandleScope scope;

	REQ_UINT64_ARG(0, x);
	REQ_UINT64_ARG(1, y);
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_ui_pow_ui(*res, x, y);
	
	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Brand0(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	
	if(randstate == NULL) {
		randstate = (gmp_randstate_t *) malloc(sizeof(gmp_randstate_t));
		gmp_randinit_default(*randstate);
		unsigned long seed = rand() + (time(NULL) * 1000) + clock();
        	gmp_randseed_ui(*randstate, seed);
	}
	
	mpz_urandomm(*res, *randstate, *bigint->bigint_);

	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Probprime(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;
	
	REQ_UINT32_ARG(0, reps);

	return scope.Close(Number::New(mpz_probab_prime_p(*bigint->bigint_, reps)));
}

Handle<Value>
BigInt::Nextprime(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_nextprime(*res, *bigint->bigint_);

	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Bcompare(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;
	
	BigInt *bi = ObjectWrap::Unwrap<BigInt>(args[0]->ToObject());

	return scope.Close(Number::New(mpz_cmp(*bigint->bigint_, *bi->bigint_)));
}

Handle<Value>
BigInt::Scompare(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;
	
	REQ_INT64_ARG(0, x);
	
	return scope.Close(Number::New(mpz_cmp_si(*bigint->bigint_, x)));
}

Handle<Value>
BigInt::Ucompare(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;
	
	REQ_UINT64_ARG(0, x);

	return scope.Close(Number::New(mpz_cmp_ui(*bigint->bigint_, x)));
}

Handle<Value>
BigInt::Band(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	BigInt *bi = ObjectWrap::Unwrap<BigInt>(args[0]->ToObject());
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_and(*res, *bigint->bigint_, *bi->bigint_);

	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Bor(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	BigInt *bi = ObjectWrap::Unwrap<BigInt>(args[0]->ToObject());
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_ior(*res, *bigint->bigint_, *bi->bigint_);

	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Bxor(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	BigInt *bi = ObjectWrap::Unwrap<BigInt>(args[0]->ToObject());
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_xor(*res, *bigint->bigint_, *bi->bigint_);

	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Binvertm(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	BigInt *bi = ObjectWrap::Unwrap<BigInt>(args[0]->ToObject());
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_invert(*res, *bigint->bigint_, *bi->bigint_);

	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Bsqrt(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_sqrt(*res, *bigint->bigint_);

	WRAP_RESULT(res, result);

	return scope.Close(result);
}

Handle<Value>
BigInt::Broot(const Arguments& args)
{
	BigInt *bigint = ObjectWrap::Unwrap<BigInt>(args.This());
	HandleScope scope;

	REQ_UINT64_ARG(0, x);
	mpz_t *res = (mpz_t *) malloc(sizeof(mpz_t));
	mpz_init(*res);
	mpz_root(*res, *bigint->bigint_, x);

	WRAP_RESULT(res, result);

	return scope.Close(result);
}

static Handle<Value>
SetJSConditioner(const Arguments& args)
{
	HandleScope scope;

	BigInt::SetJSConditioner(Persistent<Function>::New(Local<Function>::Cast(args[0])));

	return Undefined();
}

extern "C" void
init (Handle<Object> target)
{
	HandleScope scope;

	BigInt::Initialize(target);
	NODE_SET_METHOD(target, "setJSConditioner", SetJSConditioner);
}
