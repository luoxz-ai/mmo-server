// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: config/Cfg_Suit.proto

#include "config/Cfg_Suit.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
extern PROTOBUF_INTERNAL_EXPORT_common_2fAttribChangeDataProto_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_AttribChangeDataProto_common_2fAttribChangeDataProto_2eproto;
extern PROTOBUF_INTERNAL_EXPORT_config_2fCfg_5fSuit_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Cfg_Suit_Row_config_2fCfg_5fSuit_2eproto;
class Cfg_Suit_RowDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Cfg_Suit_Row> _instance;
} _Cfg_Suit_Row_default_instance_;
class Cfg_SuitDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Cfg_Suit> _instance;
} _Cfg_Suit_default_instance_;
static void InitDefaultsscc_info_Cfg_Suit_config_2fCfg_5fSuit_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Cfg_Suit_default_instance_;
    new (ptr) ::Cfg_Suit();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Cfg_Suit::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Cfg_Suit_config_2fCfg_5fSuit_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_Cfg_Suit_config_2fCfg_5fSuit_2eproto}, {
      &scc_info_Cfg_Suit_Row_config_2fCfg_5fSuit_2eproto.base,}};

static void InitDefaultsscc_info_Cfg_Suit_Row_config_2fCfg_5fSuit_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Cfg_Suit_Row_default_instance_;
    new (ptr) ::Cfg_Suit_Row();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Cfg_Suit_Row::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Cfg_Suit_Row_config_2fCfg_5fSuit_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_Cfg_Suit_Row_config_2fCfg_5fSuit_2eproto}, {
      &scc_info_AttribChangeDataProto_common_2fAttribChangeDataProto_2eproto.base,}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_config_2fCfg_5fSuit_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_config_2fCfg_5fSuit_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_config_2fCfg_5fSuit_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_config_2fCfg_5fSuit_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Cfg_Suit_Row, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Cfg_Suit_Row, id_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Suit_Row, num_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Suit_Row, attrib_change_list_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Cfg_Suit, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Cfg_Suit, rows_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Cfg_Suit_Row)},
  { 8, -1, sizeof(::Cfg_Suit)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Cfg_Suit_Row_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Cfg_Suit_default_instance_),
};

const char descriptor_table_protodef_config_2fCfg_5fSuit_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\025config/Cfg_Suit.proto\032\"common/AttribCh"
  "angeDataProto.proto\"{\n\010Cfg_Suit\022\033\n\004rows\030"
  "\001 \003(\0132\r.Cfg_Suit.Row\032R\n\003Row\022\n\n\002id\030\001 \001(\r\022"
  "\013\n\003num\030\002 \001(\r\0222\n\022attrib_change_list\030\014 \003(\013"
  "2\026.AttribChangeDataProtoB\002H\002b\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_config_2fCfg_5fSuit_2eproto_deps[1] = {
  &::descriptor_table_common_2fAttribChangeDataProto_2eproto,
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_config_2fCfg_5fSuit_2eproto_sccs[2] = {
  &scc_info_Cfg_Suit_config_2fCfg_5fSuit_2eproto.base,
  &scc_info_Cfg_Suit_Row_config_2fCfg_5fSuit_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_config_2fCfg_5fSuit_2eproto_once;
static bool descriptor_table_config_2fCfg_5fSuit_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_config_2fCfg_5fSuit_2eproto = {
  &descriptor_table_config_2fCfg_5fSuit_2eproto_initialized, descriptor_table_protodef_config_2fCfg_5fSuit_2eproto, "config/Cfg_Suit.proto", 196,
  &descriptor_table_config_2fCfg_5fSuit_2eproto_once, descriptor_table_config_2fCfg_5fSuit_2eproto_sccs, descriptor_table_config_2fCfg_5fSuit_2eproto_deps, 2, 1,
  schemas, file_default_instances, TableStruct_config_2fCfg_5fSuit_2eproto::offsets,
  file_level_metadata_config_2fCfg_5fSuit_2eproto, 2, file_level_enum_descriptors_config_2fCfg_5fSuit_2eproto, file_level_service_descriptors_config_2fCfg_5fSuit_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_config_2fCfg_5fSuit_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_config_2fCfg_5fSuit_2eproto), true);

// ===================================================================

void Cfg_Suit_Row::InitAsDefaultInstance() {
}
class Cfg_Suit_Row::_Internal {
 public:
};

void Cfg_Suit_Row::clear_attrib_change_list() {
  attrib_change_list_.Clear();
}
Cfg_Suit_Row::Cfg_Suit_Row()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Cfg_Suit.Row)
}
Cfg_Suit_Row::Cfg_Suit_Row(const Cfg_Suit_Row& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      attrib_change_list_(from.attrib_change_list_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::memcpy(&id_, &from.id_,
    static_cast<size_t>(reinterpret_cast<char*>(&num_) -
    reinterpret_cast<char*>(&id_)) + sizeof(num_));
  // @@protoc_insertion_point(copy_constructor:Cfg_Suit.Row)
}

void Cfg_Suit_Row::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Cfg_Suit_Row_config_2fCfg_5fSuit_2eproto.base);
  ::memset(&id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&num_) -
      reinterpret_cast<char*>(&id_)) + sizeof(num_));
}

Cfg_Suit_Row::~Cfg_Suit_Row() {
  // @@protoc_insertion_point(destructor:Cfg_Suit.Row)
  SharedDtor();
}

void Cfg_Suit_Row::SharedDtor() {
}

void Cfg_Suit_Row::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Cfg_Suit_Row& Cfg_Suit_Row::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Cfg_Suit_Row_config_2fCfg_5fSuit_2eproto.base);
  return *internal_default_instance();
}


void Cfg_Suit_Row::InternalSwap(Cfg_Suit_Row* other) {
  using std::swap;
  GetReflection()->Swap(this, other);}

::PROTOBUF_NAMESPACE_ID::Metadata Cfg_Suit_Row::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void Cfg_Suit::InitAsDefaultInstance() {
}
class Cfg_Suit::_Internal {
 public:
};

Cfg_Suit::Cfg_Suit()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Cfg_Suit)
}
Cfg_Suit::Cfg_Suit(const Cfg_Suit& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      rows_(from.rows_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:Cfg_Suit)
}

void Cfg_Suit::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Cfg_Suit_config_2fCfg_5fSuit_2eproto.base);
}

Cfg_Suit::~Cfg_Suit() {
  // @@protoc_insertion_point(destructor:Cfg_Suit)
  SharedDtor();
}

void Cfg_Suit::SharedDtor() {
}

void Cfg_Suit::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Cfg_Suit& Cfg_Suit::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Cfg_Suit_config_2fCfg_5fSuit_2eproto.base);
  return *internal_default_instance();
}


void Cfg_Suit::InternalSwap(Cfg_Suit* other) {
  using std::swap;
  GetReflection()->Swap(this, other);}

::PROTOBUF_NAMESPACE_ID::Metadata Cfg_Suit::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Cfg_Suit_Row* Arena::CreateMaybeMessage< ::Cfg_Suit_Row >(Arena* arena) {
  return Arena::CreateInternal< ::Cfg_Suit_Row >(arena);
}
template<> PROTOBUF_NOINLINE ::Cfg_Suit* Arena::CreateMaybeMessage< ::Cfg_Suit >(Arena* arena) {
  return Arena::CreateInternal< ::Cfg_Suit >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>