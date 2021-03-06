#include "node-xapian.h"

Persistent<FunctionTemplate> TermGenerator::constructor_template;

void TermGenerator::Init(Handle<Object> target) {
  constructor_template = Persistent<FunctionTemplate>::New(FunctionTemplate::New(New));
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("TermGenerator"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "set_database", SetDatabase);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "set_flags", SetFlags);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "set_stemmer", SetStemmer);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "set_document", SetDocument);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "get_document", GetDocument);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "get_description", GetDescription);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "index_text", IndexText);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "index_text_without_positions", IndexTextWithoutPositions);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "increase_termpos", IncreaseTermpos);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "get_termpos", GetTermpos);
  NODE_SET_PROTOTYPE_METHOD(constructor_template, "set_termpos", SetTermpos);


  target->Set(String::NewSymbol("TermGenerator"), constructor_template->GetFunction());

  Handle<Object> aO = constructor_template->GetFunction();
  aO->Set(String::NewSymbol("FLAG_SPELLING"), Integer::New(Xapian::TermGenerator::FLAG_SPELLING), ReadOnly);
}

Handle<Value> TermGenerator::New(const Arguments& args) {
  HandleScope scope;
  if (args.Length())
    return ThrowException(Exception::TypeError(String::New("arguments are ()")));
  TermGenerator* that = new TermGenerator;
  that->Wrap(args.This());
  return args.This();
}

void TermGenerator::SetGet_process(void* pData, void* pThat) {
  SetGet_data* data = (SetGet_data*) pData;
  TermGenerator* that = (TermGenerator *) pThat;

  switch (data->action) {
  case SetGet_data::eSetDatabase: that->mTg.set_database(data->db->getWdb());                  break;
  case SetGet_data::eSetStemmer:  that->mTg.set_stemmer(data->st->mStem);                      break;
  case SetGet_data::eSetDocument: that->mTg.set_document(*data->doc->getDoc());                break;
  case SetGet_data::eGetDocument: data->xdoc = new Xapian::Document(that->mTg.get_document()); break;
  default: assert(0);
  }
}

Handle<Value> TermGenerator::SetGet_convert(void* pData) {
  SetGet_data* data = (SetGet_data*) pData;
  Handle<Value> aResult;

  switch (data->action) {
  case SetGet_data::eSetDatabase:    
  case SetGet_data::eSetStemmer:
  case SetGet_data::eSetDocument:
    aResult = Undefined(); break;
  case SetGet_data::eGetDocument:
    Local<Value> aDoc[] = { External::New(data->xdoc) };
    aResult = Document::constructor_template->GetFunction()->NewInstance(1, aDoc);
    break;
  }

  delete data;
  return aResult;
}

static int kSetDatabase[] = { eObjectDatabase, -eFunction, eEnd };
Handle<Value> TermGenerator::SetDatabase(const Arguments& args) {
  HandleScope scope;
  int aOpt[1];
  WritableDatabase* aDb;
  if (!checkArguments(kSetDatabase, args, aOpt) || !(aDb = GetInstance<WritableDatabase>(args[0])))
    return throwSignatureErr(kSetDatabase);

  SetGet_data* aData = new SetGet_data(ObjectWrap::Unwrap<TermGenerator>(args.This()), aDb); //deleted by SetGet_convert on non error

  Handle<Value> aResult;
  try {
    aResult = invoke<TermGenerator>(aOpt[0] >= 0, args, (void*)aData, SetGet_process, SetGet_convert);
  } catch (Handle<Value> ex) {
    delete aData;
    return ThrowException(ex);
  }
  return scope.Close(aResult);
}

Handle<Value> TermGenerator::SetFlags(const Arguments& args) {
  HandleScope scope;
  if (args.Length() < 1 || !args[0]->IsInt32())
    return ThrowException(Exception::TypeError(String::New("arguments are (integer)")));

  TermGenerator* that = ObjectWrap::Unwrap<TermGenerator>(args.This());
  try {
    that->mTg.set_flags((Xapian::TermGenerator::flags)args[0]->Int32Value());
  } catch (const Xapian::Error& err) {
    return ThrowException(Exception::Error(String::New(err.get_msg().c_str())));
  }

  return Undefined();
}

static int kSetStemmer[] = { eObjectStem, -eFunction, eEnd };
Handle<Value> TermGenerator::SetStemmer(const Arguments& args) {
  HandleScope scope;
  int aOpt[1];
  Stem* aSt;
  if (!checkArguments(kSetStemmer, args, aOpt) || !(aSt = GetInstance<Stem>(args[0])))
    return throwSignatureErr(kSetStemmer);

  SetGet_data* aData = new SetGet_data(ObjectWrap::Unwrap<TermGenerator>(args.This()), aSt); //deleted by SetGet_convert on non error

  Handle<Value> aResult;
  try {
    aResult = invoke<TermGenerator>(aOpt[0] >= 0, args, (void*)aData, SetGet_process, SetGet_convert);
  } catch (Handle<Value> ex) {
    delete aData;
    return ThrowException(ex);
  }
  return scope.Close(aResult);
}

static int kSetDocument[] = { eObjectDocument, -eFunction, eEnd };
Handle<Value> TermGenerator::SetDocument(const Arguments& args) {
  HandleScope scope;
  int aOpt[1];
  Document* aDoc;
  if (!checkArguments(kSetDocument, args, aOpt) || !(aDoc = GetInstance<Document>(args[0])))
    return throwSignatureErr(kSetDocument);

  SetGet_data* aData = new SetGet_data(ObjectWrap::Unwrap<TermGenerator>(args.This()), aDoc); //deleted by SetGet_convert on non error

  Handle<Value> aResult;
  try {
    aResult = invoke<TermGenerator>(aOpt[0] >= 0, args, (void*)aData, SetGet_process, SetGet_convert);
  } catch (Handle<Value> ex) {
    delete aData;
    return ThrowException(ex);
  }
  return scope.Close(aResult);
}

static int kGetDocument[] = { -eFunction, eEnd };
Handle<Value> TermGenerator::GetDocument(const Arguments& args) {
  HandleScope scope;
  int aOpt[1];
  if (!checkArguments(kGetDocument, args, aOpt))
    return throwSignatureErr(kGetDocument);

  SetGet_data* aData = new SetGet_data(ObjectWrap::Unwrap<TermGenerator>(args.This())); //deleted by SetGet_convert on non error

  Handle<Value> aResult;
  try {
    aResult = invoke<Enquire>(aOpt[0] >= 0, args, (void*)aData, SetGet_process, SetGet_convert);
  } catch (Handle<Value> ex) {
    delete aData;
    return ThrowException(ex);
  }
  return scope.Close(aResult);
}

enum { 
  eGetDescription, eIndexText, eIndexTextWithoutPositions, eIncreaseTermpos, eGetTermpos, eSetTermpos
};

void TermGenerator::Generic_process(void* pData, void* pThat) {
  GenericData* data = (GenericData*) pData;
  TermGenerator* that = (TermGenerator *) pThat;

  switch (data->action) {
  case eGetDescription:            data->retVal.setString(that->mTg.get_description());                                                     break;
  case eIndexText:                 that->mTg.index_text(*data->val[0].string, data->val[1].uint32, *data->val[2].string);                   break;
  case eIndexTextWithoutPositions: that->mTg.index_text_without_positions(*data->val[0].string, data->val[1].uint32, *data->val[2].string); break;
  case eIncreaseTermpos:           that->mTg.increase_termpos(data->val[0].uint32);                                                         break;
  case eGetTermpos:                data->retVal.uint32 = that->mTg.get_termpos();                                                           break;
  case eSetTermpos:                that->mTg.set_termpos(data->val[0].uint32);                                                              break;
  default: assert(0);
  }
}

Handle<Value> TermGenerator::Generic_convert(void* pData) {
  GenericData* data = (GenericData*) pData;
  Handle<Value> aResult;

  switch (data->action) {
  case eGetDescription: 
    aResult = String::New(data->retVal.string->c_str()); break;
  case eIndexText:
  case eIndexTextWithoutPositions:
  case eIncreaseTermpos:
  case eSetTermpos:
    aResult = Undefined();                               break;
  case eGetTermpos:
    aResult = Uint32::New(data->retVal.uint32);          break;
  }

  delete data;
  return aResult;
}

static int kGetDescription[] = { -eFunction, eEnd };
Handle<Value> TermGenerator::GetDescription(const Arguments& args) { return generic_start<TermGenerator>(eGetDescription, args, kGetDescription); }

static int kIndexText[] = { eString, -eUint32, -eString, -eFunction, eEnd };
static GenericData::Item kIndexTextDefault[2] = { (uint32_t)1, "" };
Handle<Value> TermGenerator::IndexText(const Arguments& args) { return generic_start<TermGenerator>(eIndexText, args, kIndexText, kIndexTextDefault); }

static int kIndexTextWithoutPositions[] = { eString, -eUint32, -eString, -eFunction, eEnd };
static GenericData::Item kIndexTextWithoutPositionsDefault[2] = { (uint32_t)1, "" };
Handle<Value> TermGenerator::IndexTextWithoutPositions(const Arguments& args) { return generic_start<TermGenerator>(eIndexTextWithoutPositions, args, kIndexTextWithoutPositions, kIndexTextWithoutPositionsDefault); }

static int kIncreaseTermpos[] = { -eUint32, -eFunction, eEnd };
static GenericData::Item kIncreaseTermposDefault[1] = { (uint32_t)100 };
Handle<Value> TermGenerator::IncreaseTermpos(const Arguments& args) { return generic_start<TermGenerator>(eIncreaseTermpos, args, kIncreaseTermpos, kIncreaseTermposDefault); }

static int kGetTermpos[] = { -eFunction, eEnd };
Handle<Value> TermGenerator::GetTermpos(const Arguments& args) { return generic_start<TermGenerator>(eGetTermpos, args, kGetTermpos); }

static int kSetTermpos[] = { eUint32, -eFunction, eEnd };
Handle<Value> TermGenerator::SetTermpos(const Arguments& args) { return generic_start<TermGenerator>(eSetTermpos, args, kSetTermpos); }
