// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: config/Cfg_Scene_EnterPoint.proto

#include "config/Cfg_Scene_EnterPoint.pb.h"

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
extern PROTOBUF_INTERNAL_EXPORT_config_2fCfg_5fScene_5fEnterPoint_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Cfg_Scene_EnterPoint_Row_config_2fCfg_5fScene_5fEnterPoint_2eproto;
class Cfg_Scene_EnterPoint_RowDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Cfg_Scene_EnterPoint_Row> _instance;
} _Cfg_Scene_EnterPoint_Row_default_instance_;
class Cfg_Scene_EnterPointDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<Cfg_Scene_EnterPoint> _instance;
} _Cfg_Scene_EnterPoint_default_instance_;
static void InitDefaultsscc_info_Cfg_Scene_EnterPoint_config_2fCfg_5fScene_5fEnterPoint_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Cfg_Scene_EnterPoint_default_instance_;
    new (ptr) ::Cfg_Scene_EnterPoint();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Cfg_Scene_EnterPoint::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_Cfg_Scene_EnterPoint_config_2fCfg_5fScene_5fEnterPoint_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_Cfg_Scene_EnterPoint_config_2fCfg_5fScene_5fEnterPoint_2eproto}, {
      &scc_info_Cfg_Scene_EnterPoint_Row_config_2fCfg_5fScene_5fEnterPoint_2eproto.base,}};

static void InitDefaultsscc_info_Cfg_Scene_EnterPoint_Row_config_2fCfg_5fScene_5fEnterPoint_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_Cfg_Scene_EnterPoint_Row_default_instance_;
    new (ptr) ::Cfg_Scene_EnterPoint_Row();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Cfg_Scene_EnterPoint_Row::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Cfg_Scene_EnterPoint_Row_config_2fCfg_5fScene_5fEnterPoint_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_Cfg_Scene_EnterPoint_Row_config_2fCfg_5fScene_5fEnterPoint_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_config_2fCfg_5fScene_5fEnterPoint_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_config_2fCfg_5fScene_5fEnterPoint_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_config_2fCfg_5fScene_5fEnterPoint_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_config_2fCfg_5fScene_5fEnterPoint_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint_Row, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint_Row, idmap_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint_Row, idx_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint_Row, x_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint_Row, y_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint_Row, range_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint_Row, face_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint_Row, team_req_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint_Row, guild_req_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint_Row, lev_req_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint_Row, prof_req_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint_Row, vip_lev_req_),
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint_Row, idphase_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Cfg_Scene_EnterPoint, rows_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Cfg_Scene_EnterPoint_Row)},
  { 17, -1, sizeof(::Cfg_Scene_EnterPoint)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Cfg_Scene_EnterPoint_Row_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_Cfg_Scene_EnterPoint_default_instance_),
};

const char descriptor_table_protodef_config_2fCfg_5fScene_5fEnterPoint_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n!config/Cfg_Scene_EnterPoint.proto\"\204\002\n\024"
  "Cfg_Scene_EnterPoint\022\'\n\004rows\030\001 \003(\0132\031.Cfg"
  "_Scene_EnterPoint.Row\032\302\001\n\003Row\022\r\n\005idmap\030\001"
  " \001(\r\022\013\n\003idx\030\002 \001(\r\022\t\n\001x\030\003 \001(\002\022\t\n\001y\030\004 \001(\002\022"
  "\r\n\005range\030\005 \001(\002\022\014\n\004face\030\006 \001(\002\022\020\n\010team_req"
  "\030\n \001(\r\022\021\n\tguild_req\030\013 \001(\r\022\017\n\007lev_req\030\014 \001"
  "(\r\022\020\n\010prof_req\030\r \001(\r\022\023\n\013vip_lev_req\030\016 \001("
  "\r\022\017\n\007idphase\030\017 \001(\005B\002H\002b\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto_sccs[2] = {
  &scc_info_Cfg_Scene_EnterPoint_config_2fCfg_5fScene_5fEnterPoint_2eproto.base,
  &scc_info_Cfg_Scene_EnterPoint_Row_config_2fCfg_5fScene_5fEnterPoint_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto_once;
static bool descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto = {
  &descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto_initialized, descriptor_table_protodef_config_2fCfg_5fScene_5fEnterPoint_2eproto, "config/Cfg_Scene_EnterPoint.proto", 310,
  &descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto_once, descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto_sccs, descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto_deps, 2, 0,
  schemas, file_default_instances, TableStruct_config_2fCfg_5fScene_5fEnterPoint_2eproto::offsets,
  file_level_metadata_config_2fCfg_5fScene_5fEnterPoint_2eproto, 2, file_level_enum_descriptors_config_2fCfg_5fScene_5fEnterPoint_2eproto, file_level_service_descriptors_config_2fCfg_5fScene_5fEnterPoint_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_config_2fCfg_5fScene_5fEnterPoint_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto), true);

// ===================================================================

void Cfg_Scene_EnterPoint_Row::InitAsDefaultInstance() {
}
class Cfg_Scene_EnterPoint_Row::_Internal {
 public:
};

Cfg_Scene_EnterPoint_Row::Cfg_Scene_EnterPoint_Row()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Cfg_Scene_EnterPoint.Row)
}
Cfg_Scene_EnterPoint_Row::Cfg_Scene_EnterPoint_Row(const Cfg_Scene_EnterPoint_Row& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::memcpy(&idmap_, &from.idmap_,
    static_cast<size_t>(reinterpret_cast<char*>(&idphase_) -
    reinterpret_cast<char*>(&idmap_)) + sizeof(idphase_));
  // @@protoc_insertion_point(copy_constructor:Cfg_Scene_EnterPoint.Row)
}

void Cfg_Scene_EnterPoint_Row::SharedCtor() {
  ::memset(&idmap_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&idphase_) -
      reinterpret_cast<char*>(&idmap_)) + sizeof(idphase_));
}

Cfg_Scene_EnterPoint_Row::~Cfg_Scene_EnterPoint_Row() {
  // @@protoc_insertion_point(destructor:Cfg_Scene_EnterPoint.Row)
  SharedDtor();
}

void Cfg_Scene_EnterPoint_Row::SharedDtor() {
}

void Cfg_Scene_EnterPoint_Row::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Cfg_Scene_EnterPoint_Row& Cfg_Scene_EnterPoint_Row::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Cfg_Scene_EnterPoint_Row_config_2fCfg_5fScene_5fEnterPoint_2eproto.base);
  return *internal_default_instance();
}


void Cfg_Scene_EnterPoint_Row::InternalSwap(Cfg_Scene_EnterPoint_Row* other) {
  using std::swap;
  GetReflection()->Swap(this, other);}

::PROTOBUF_NAMESPACE_ID::Metadata Cfg_Scene_EnterPoint_Row::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void Cfg_Scene_EnterPoint::InitAsDefaultInstance() {
}
class Cfg_Scene_EnterPoint::_Internal {
 public:
};

Cfg_Scene_EnterPoint::Cfg_Scene_EnterPoint()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Cfg_Scene_EnterPoint)
}
Cfg_Scene_EnterPoint::Cfg_Scene_EnterPoint(const Cfg_Scene_EnterPoint& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      rows_(from.rows_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:Cfg_Scene_EnterPoint)
}

void Cfg_Scene_EnterPoint::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_Cfg_Scene_EnterPoint_config_2fCfg_5fScene_5fEnterPoint_2eproto.base);
}

Cfg_Scene_EnterPoint::~Cfg_Scene_EnterPoint() {
  // @@protoc_insertion_point(destructor:Cfg_Scene_EnterPoint)
  SharedDtor();
}

void Cfg_Scene_EnterPoint::SharedDtor() {
}

void Cfg_Scene_EnterPoint::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Cfg_Scene_EnterPoint& Cfg_Scene_EnterPoint::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_Cfg_Scene_EnterPoint_config_2fCfg_5fScene_5fEnterPoint_2eproto.base);
  return *internal_default_instance();
}


void Cfg_Scene_EnterPoint::InternalSwap(Cfg_Scene_EnterPoint* other) {
  using std::swap;
  GetReflection()->Swap(this, other);}

::PROTOBUF_NAMESPACE_ID::Metadata Cfg_Scene_EnterPoint::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Cfg_Scene_EnterPoint_Row* Arena::CreateMaybeMessage< ::Cfg_Scene_EnterPoint_Row >(Arena* arena) {
  return Arena::CreateInternal< ::Cfg_Scene_EnterPoint_Row >(arena);
}
template<> PROTOBUF_NOINLINE ::Cfg_Scene_EnterPoint* Arena::CreateMaybeMessage< ::Cfg_Scene_EnterPoint >(Arena* arena) {
  return Arena::CreateInternal< ::Cfg_Scene_EnterPoint >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
