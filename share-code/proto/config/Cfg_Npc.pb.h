// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: config/Cfg_Npc.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fNpc_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fNpc_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3011000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3011002 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_config_2fCfg_5fNpc_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_config_2fCfg_5fNpc_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[2]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_config_2fCfg_5fNpc_2eproto;
class Cfg_Npc;
class Cfg_NpcDefaultTypeInternal;
extern Cfg_NpcDefaultTypeInternal _Cfg_Npc_default_instance_;
class Cfg_Npc_Row;
class Cfg_Npc_RowDefaultTypeInternal;
extern Cfg_Npc_RowDefaultTypeInternal _Cfg_Npc_Row_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::Cfg_Npc* Arena::CreateMaybeMessage<::Cfg_Npc>(Arena*);
template<> ::Cfg_Npc_Row* Arena::CreateMaybeMessage<::Cfg_Npc_Row>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class Cfg_Npc_Row :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Cfg_Npc.Row) */ {
 public:
  Cfg_Npc_Row();
  virtual ~Cfg_Npc_Row();

  Cfg_Npc_Row(const Cfg_Npc_Row& from);
  Cfg_Npc_Row(Cfg_Npc_Row&& from) noexcept
    : Cfg_Npc_Row() {
    *this = ::std::move(from);
  }

  inline Cfg_Npc_Row& operator=(const Cfg_Npc_Row& from) {
    CopyFrom(from);
    return *this;
  }
  inline Cfg_Npc_Row& operator=(Cfg_Npc_Row&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const Cfg_Npc_Row& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Cfg_Npc_Row* internal_default_instance() {
    return reinterpret_cast<const Cfg_Npc_Row*>(
               &_Cfg_Npc_Row_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Cfg_Npc_Row& a, Cfg_Npc_Row& b) {
    a.Swap(&b);
  }
  inline void Swap(Cfg_Npc_Row* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Cfg_Npc_Row* New() const final {
    return CreateMaybeMessage<Cfg_Npc_Row>(nullptr);
  }

  Cfg_Npc_Row* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Cfg_Npc_Row>(arena);
  }
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Cfg_Npc_Row* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Cfg_Npc.Row";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_config_2fCfg_5fNpc_2eproto);
    return ::descriptor_table_config_2fCfg_5fNpc_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kNameFieldNumber = 3,
    kDialogTxtFieldNumber = 4,
    kShopLinknameFieldNumber = 7,
    kIdFieldNumber = 1,
    kLevelFieldNumber = 2,
    kTypeFlagFieldNumber = 5,
    kShopIdFieldNumber = 6,
    kScriptidFieldNumber = 9,
    kIdcampFieldNumber = 8,
    kMapidFieldNumber = 10,
    kPosxFieldNumber = 11,
    kPosyFieldNumber = 12,
    kFaceFieldNumber = 13,
    kHighFieldNumber = 14,
    kVolumeFieldNumber = 15,
  };
  // string name = 3;
  void clear_name();
  const std::string& name() const;
  void set_name(const std::string& value);
  void set_name(std::string&& value);
  void set_name(const char* value);
  void set_name(const char* value, size_t size);
  std::string* mutable_name();
  std::string* release_name();
  void set_allocated_name(std::string* name);
  private:
  const std::string& _internal_name() const;
  void _internal_set_name(const std::string& value);
  std::string* _internal_mutable_name();
  public:

  // string dialog_txt = 4;
  void clear_dialog_txt();
  const std::string& dialog_txt() const;
  void set_dialog_txt(const std::string& value);
  void set_dialog_txt(std::string&& value);
  void set_dialog_txt(const char* value);
  void set_dialog_txt(const char* value, size_t size);
  std::string* mutable_dialog_txt();
  std::string* release_dialog_txt();
  void set_allocated_dialog_txt(std::string* dialog_txt);
  private:
  const std::string& _internal_dialog_txt() const;
  void _internal_set_dialog_txt(const std::string& value);
  std::string* _internal_mutable_dialog_txt();
  public:

  // string shop_linkname = 7;
  void clear_shop_linkname();
  const std::string& shop_linkname() const;
  void set_shop_linkname(const std::string& value);
  void set_shop_linkname(std::string&& value);
  void set_shop_linkname(const char* value);
  void set_shop_linkname(const char* value, size_t size);
  std::string* mutable_shop_linkname();
  std::string* release_shop_linkname();
  void set_allocated_shop_linkname(std::string* shop_linkname);
  private:
  const std::string& _internal_shop_linkname() const;
  void _internal_set_shop_linkname(const std::string& value);
  std::string* _internal_mutable_shop_linkname();
  public:

  // uint32 id = 1;
  void clear_id();
  ::PROTOBUF_NAMESPACE_ID::uint32 id() const;
  void set_id(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_id() const;
  void _internal_set_id(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 level = 2;
  void clear_level();
  ::PROTOBUF_NAMESPACE_ID::uint32 level() const;
  void set_level(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_level() const;
  void _internal_set_level(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 type_flag = 5;
  void clear_type_flag();
  ::PROTOBUF_NAMESPACE_ID::uint32 type_flag() const;
  void set_type_flag(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_type_flag() const;
  void _internal_set_type_flag(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 shop_id = 6;
  void clear_shop_id();
  ::PROTOBUF_NAMESPACE_ID::uint32 shop_id() const;
  void set_shop_id(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_shop_id() const;
  void _internal_set_shop_id(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint64 scriptid = 9;
  void clear_scriptid();
  ::PROTOBUF_NAMESPACE_ID::uint64 scriptid() const;
  void set_scriptid(::PROTOBUF_NAMESPACE_ID::uint64 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint64 _internal_scriptid() const;
  void _internal_set_scriptid(::PROTOBUF_NAMESPACE_ID::uint64 value);
  public:

  // uint32 idcamp = 8;
  void clear_idcamp();
  ::PROTOBUF_NAMESPACE_ID::uint32 idcamp() const;
  void set_idcamp(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_idcamp() const;
  void _internal_set_idcamp(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 mapid = 10;
  void clear_mapid();
  ::PROTOBUF_NAMESPACE_ID::uint32 mapid() const;
  void set_mapid(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_mapid() const;
  void _internal_set_mapid(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // float posx = 11;
  void clear_posx();
  float posx() const;
  void set_posx(float value);
  private:
  float _internal_posx() const;
  void _internal_set_posx(float value);
  public:

  // float posy = 12;
  void clear_posy();
  float posy() const;
  void set_posy(float value);
  private:
  float _internal_posy() const;
  void _internal_set_posy(float value);
  public:

  // float face = 13;
  void clear_face();
  float face() const;
  void set_face(float value);
  private:
  float _internal_face() const;
  void _internal_set_face(float value);
  public:

  // float high = 14;
  void clear_high();
  float high() const;
  void set_high(float value);
  private:
  float _internal_high() const;
  void _internal_set_high(float value);
  public:

  // float volume = 15;
  void clear_volume();
  float volume() const;
  void set_volume(float value);
  private:
  float _internal_volume() const;
  void _internal_set_volume(float value);
  public:

  // @@protoc_insertion_point(class_scope:Cfg_Npc.Row)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr name_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr dialog_txt_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr shop_linkname_;
  ::PROTOBUF_NAMESPACE_ID::uint32 id_;
  ::PROTOBUF_NAMESPACE_ID::uint32 level_;
  ::PROTOBUF_NAMESPACE_ID::uint32 type_flag_;
  ::PROTOBUF_NAMESPACE_ID::uint32 shop_id_;
  ::PROTOBUF_NAMESPACE_ID::uint64 scriptid_;
  ::PROTOBUF_NAMESPACE_ID::uint32 idcamp_;
  ::PROTOBUF_NAMESPACE_ID::uint32 mapid_;
  float posx_;
  float posy_;
  float face_;
  float high_;
  float volume_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_config_2fCfg_5fNpc_2eproto;
};
// -------------------------------------------------------------------

class Cfg_Npc :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Cfg_Npc) */ {
 public:
  Cfg_Npc();
  virtual ~Cfg_Npc();

  Cfg_Npc(const Cfg_Npc& from);
  Cfg_Npc(Cfg_Npc&& from) noexcept
    : Cfg_Npc() {
    *this = ::std::move(from);
  }

  inline Cfg_Npc& operator=(const Cfg_Npc& from) {
    CopyFrom(from);
    return *this;
  }
  inline Cfg_Npc& operator=(Cfg_Npc&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const Cfg_Npc& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Cfg_Npc* internal_default_instance() {
    return reinterpret_cast<const Cfg_Npc*>(
               &_Cfg_Npc_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(Cfg_Npc& a, Cfg_Npc& b) {
    a.Swap(&b);
  }
  inline void Swap(Cfg_Npc* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Cfg_Npc* New() const final {
    return CreateMaybeMessage<Cfg_Npc>(nullptr);
  }

  Cfg_Npc* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Cfg_Npc>(arena);
  }
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Cfg_Npc* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Cfg_Npc";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_config_2fCfg_5fNpc_2eproto);
    return ::descriptor_table_config_2fCfg_5fNpc_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  typedef Cfg_Npc_Row Row;

  // accessors -------------------------------------------------------

  enum : int {
    kRowsFieldNumber = 1,
  };
  // repeated .Cfg_Npc.Row rows = 1;
  int rows_size() const;
  private:
  int _internal_rows_size() const;
  public:
  void clear_rows();
  ::Cfg_Npc_Row* mutable_rows(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Npc_Row >*
      mutable_rows();
  private:
  const ::Cfg_Npc_Row& _internal_rows(int index) const;
  ::Cfg_Npc_Row* _internal_add_rows();
  public:
  const ::Cfg_Npc_Row& rows(int index) const;
  ::Cfg_Npc_Row* add_rows();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Npc_Row >&
      rows() const;

  // @@protoc_insertion_point(class_scope:Cfg_Npc)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Npc_Row > rows_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_config_2fCfg_5fNpc_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Cfg_Npc_Row

// uint32 id = 1;
inline void Cfg_Npc_Row::clear_id() {
  id_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Npc_Row::_internal_id() const {
  return id_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Npc_Row::id() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.id)
  return _internal_id();
}
inline void Cfg_Npc_Row::_internal_set_id(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  id_ = value;
}
inline void Cfg_Npc_Row::set_id(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_id(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.id)
}

// uint32 level = 2;
inline void Cfg_Npc_Row::clear_level() {
  level_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Npc_Row::_internal_level() const {
  return level_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Npc_Row::level() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.level)
  return _internal_level();
}
inline void Cfg_Npc_Row::_internal_set_level(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  level_ = value;
}
inline void Cfg_Npc_Row::set_level(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_level(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.level)
}

// string name = 3;
inline void Cfg_Npc_Row::clear_name() {
  name_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline const std::string& Cfg_Npc_Row::name() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.name)
  return _internal_name();
}
inline void Cfg_Npc_Row::set_name(const std::string& value) {
  _internal_set_name(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.name)
}
inline std::string* Cfg_Npc_Row::mutable_name() {
  // @@protoc_insertion_point(field_mutable:Cfg_Npc.Row.name)
  return _internal_mutable_name();
}
inline const std::string& Cfg_Npc_Row::_internal_name() const {
  return name_.GetNoArena();
}
inline void Cfg_Npc_Row::_internal_set_name(const std::string& value) {
  
  name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
}
inline void Cfg_Npc_Row::set_name(std::string&& value) {
  
  name_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:Cfg_Npc.Row.name)
}
inline void Cfg_Npc_Row::set_name(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Cfg_Npc.Row.name)
}
inline void Cfg_Npc_Row::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Cfg_Npc.Row.name)
}
inline std::string* Cfg_Npc_Row::_internal_mutable_name() {
  
  return name_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* Cfg_Npc_Row::release_name() {
  // @@protoc_insertion_point(field_release:Cfg_Npc.Row.name)
  
  return name_.ReleaseNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void Cfg_Npc_Row::set_allocated_name(std::string* name) {
  if (name != nullptr) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:Cfg_Npc.Row.name)
}

// string dialog_txt = 4;
inline void Cfg_Npc_Row::clear_dialog_txt() {
  dialog_txt_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline const std::string& Cfg_Npc_Row::dialog_txt() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.dialog_txt)
  return _internal_dialog_txt();
}
inline void Cfg_Npc_Row::set_dialog_txt(const std::string& value) {
  _internal_set_dialog_txt(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.dialog_txt)
}
inline std::string* Cfg_Npc_Row::mutable_dialog_txt() {
  // @@protoc_insertion_point(field_mutable:Cfg_Npc.Row.dialog_txt)
  return _internal_mutable_dialog_txt();
}
inline const std::string& Cfg_Npc_Row::_internal_dialog_txt() const {
  return dialog_txt_.GetNoArena();
}
inline void Cfg_Npc_Row::_internal_set_dialog_txt(const std::string& value) {
  
  dialog_txt_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
}
inline void Cfg_Npc_Row::set_dialog_txt(std::string&& value) {
  
  dialog_txt_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:Cfg_Npc.Row.dialog_txt)
}
inline void Cfg_Npc_Row::set_dialog_txt(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  dialog_txt_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Cfg_Npc.Row.dialog_txt)
}
inline void Cfg_Npc_Row::set_dialog_txt(const char* value, size_t size) {
  
  dialog_txt_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Cfg_Npc.Row.dialog_txt)
}
inline std::string* Cfg_Npc_Row::_internal_mutable_dialog_txt() {
  
  return dialog_txt_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* Cfg_Npc_Row::release_dialog_txt() {
  // @@protoc_insertion_point(field_release:Cfg_Npc.Row.dialog_txt)
  
  return dialog_txt_.ReleaseNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void Cfg_Npc_Row::set_allocated_dialog_txt(std::string* dialog_txt) {
  if (dialog_txt != nullptr) {
    
  } else {
    
  }
  dialog_txt_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), dialog_txt);
  // @@protoc_insertion_point(field_set_allocated:Cfg_Npc.Row.dialog_txt)
}

// uint32 type_flag = 5;
inline void Cfg_Npc_Row::clear_type_flag() {
  type_flag_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Npc_Row::_internal_type_flag() const {
  return type_flag_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Npc_Row::type_flag() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.type_flag)
  return _internal_type_flag();
}
inline void Cfg_Npc_Row::_internal_set_type_flag(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  type_flag_ = value;
}
inline void Cfg_Npc_Row::set_type_flag(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_type_flag(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.type_flag)
}

// uint32 shop_id = 6;
inline void Cfg_Npc_Row::clear_shop_id() {
  shop_id_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Npc_Row::_internal_shop_id() const {
  return shop_id_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Npc_Row::shop_id() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.shop_id)
  return _internal_shop_id();
}
inline void Cfg_Npc_Row::_internal_set_shop_id(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  shop_id_ = value;
}
inline void Cfg_Npc_Row::set_shop_id(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_shop_id(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.shop_id)
}

// string shop_linkname = 7;
inline void Cfg_Npc_Row::clear_shop_linkname() {
  shop_linkname_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline const std::string& Cfg_Npc_Row::shop_linkname() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.shop_linkname)
  return _internal_shop_linkname();
}
inline void Cfg_Npc_Row::set_shop_linkname(const std::string& value) {
  _internal_set_shop_linkname(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.shop_linkname)
}
inline std::string* Cfg_Npc_Row::mutable_shop_linkname() {
  // @@protoc_insertion_point(field_mutable:Cfg_Npc.Row.shop_linkname)
  return _internal_mutable_shop_linkname();
}
inline const std::string& Cfg_Npc_Row::_internal_shop_linkname() const {
  return shop_linkname_.GetNoArena();
}
inline void Cfg_Npc_Row::_internal_set_shop_linkname(const std::string& value) {
  
  shop_linkname_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
}
inline void Cfg_Npc_Row::set_shop_linkname(std::string&& value) {
  
  shop_linkname_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:Cfg_Npc.Row.shop_linkname)
}
inline void Cfg_Npc_Row::set_shop_linkname(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  shop_linkname_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Cfg_Npc.Row.shop_linkname)
}
inline void Cfg_Npc_Row::set_shop_linkname(const char* value, size_t size) {
  
  shop_linkname_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Cfg_Npc.Row.shop_linkname)
}
inline std::string* Cfg_Npc_Row::_internal_mutable_shop_linkname() {
  
  return shop_linkname_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* Cfg_Npc_Row::release_shop_linkname() {
  // @@protoc_insertion_point(field_release:Cfg_Npc.Row.shop_linkname)
  
  return shop_linkname_.ReleaseNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void Cfg_Npc_Row::set_allocated_shop_linkname(std::string* shop_linkname) {
  if (shop_linkname != nullptr) {
    
  } else {
    
  }
  shop_linkname_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), shop_linkname);
  // @@protoc_insertion_point(field_set_allocated:Cfg_Npc.Row.shop_linkname)
}

// uint32 idcamp = 8;
inline void Cfg_Npc_Row::clear_idcamp() {
  idcamp_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Npc_Row::_internal_idcamp() const {
  return idcamp_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Npc_Row::idcamp() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.idcamp)
  return _internal_idcamp();
}
inline void Cfg_Npc_Row::_internal_set_idcamp(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  idcamp_ = value;
}
inline void Cfg_Npc_Row::set_idcamp(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_idcamp(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.idcamp)
}

// uint64 scriptid = 9;
inline void Cfg_Npc_Row::clear_scriptid() {
  scriptid_ = PROTOBUF_ULONGLONG(0);
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 Cfg_Npc_Row::_internal_scriptid() const {
  return scriptid_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint64 Cfg_Npc_Row::scriptid() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.scriptid)
  return _internal_scriptid();
}
inline void Cfg_Npc_Row::_internal_set_scriptid(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  
  scriptid_ = value;
}
inline void Cfg_Npc_Row::set_scriptid(::PROTOBUF_NAMESPACE_ID::uint64 value) {
  _internal_set_scriptid(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.scriptid)
}

// uint32 mapid = 10;
inline void Cfg_Npc_Row::clear_mapid() {
  mapid_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Npc_Row::_internal_mapid() const {
  return mapid_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Npc_Row::mapid() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.mapid)
  return _internal_mapid();
}
inline void Cfg_Npc_Row::_internal_set_mapid(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  mapid_ = value;
}
inline void Cfg_Npc_Row::set_mapid(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_mapid(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.mapid)
}

// float posx = 11;
inline void Cfg_Npc_Row::clear_posx() {
  posx_ = 0;
}
inline float Cfg_Npc_Row::_internal_posx() const {
  return posx_;
}
inline float Cfg_Npc_Row::posx() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.posx)
  return _internal_posx();
}
inline void Cfg_Npc_Row::_internal_set_posx(float value) {
  
  posx_ = value;
}
inline void Cfg_Npc_Row::set_posx(float value) {
  _internal_set_posx(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.posx)
}

// float posy = 12;
inline void Cfg_Npc_Row::clear_posy() {
  posy_ = 0;
}
inline float Cfg_Npc_Row::_internal_posy() const {
  return posy_;
}
inline float Cfg_Npc_Row::posy() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.posy)
  return _internal_posy();
}
inline void Cfg_Npc_Row::_internal_set_posy(float value) {
  
  posy_ = value;
}
inline void Cfg_Npc_Row::set_posy(float value) {
  _internal_set_posy(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.posy)
}

// float face = 13;
inline void Cfg_Npc_Row::clear_face() {
  face_ = 0;
}
inline float Cfg_Npc_Row::_internal_face() const {
  return face_;
}
inline float Cfg_Npc_Row::face() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.face)
  return _internal_face();
}
inline void Cfg_Npc_Row::_internal_set_face(float value) {
  
  face_ = value;
}
inline void Cfg_Npc_Row::set_face(float value) {
  _internal_set_face(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.face)
}

// float high = 14;
inline void Cfg_Npc_Row::clear_high() {
  high_ = 0;
}
inline float Cfg_Npc_Row::_internal_high() const {
  return high_;
}
inline float Cfg_Npc_Row::high() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.high)
  return _internal_high();
}
inline void Cfg_Npc_Row::_internal_set_high(float value) {
  
  high_ = value;
}
inline void Cfg_Npc_Row::set_high(float value) {
  _internal_set_high(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.high)
}

// float volume = 15;
inline void Cfg_Npc_Row::clear_volume() {
  volume_ = 0;
}
inline float Cfg_Npc_Row::_internal_volume() const {
  return volume_;
}
inline float Cfg_Npc_Row::volume() const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.Row.volume)
  return _internal_volume();
}
inline void Cfg_Npc_Row::_internal_set_volume(float value) {
  
  volume_ = value;
}
inline void Cfg_Npc_Row::set_volume(float value) {
  _internal_set_volume(value);
  // @@protoc_insertion_point(field_set:Cfg_Npc.Row.volume)
}

// -------------------------------------------------------------------

// Cfg_Npc

// repeated .Cfg_Npc.Row rows = 1;
inline int Cfg_Npc::_internal_rows_size() const {
  return rows_.size();
}
inline int Cfg_Npc::rows_size() const {
  return _internal_rows_size();
}
inline void Cfg_Npc::clear_rows() {
  rows_.Clear();
}
inline ::Cfg_Npc_Row* Cfg_Npc::mutable_rows(int index) {
  // @@protoc_insertion_point(field_mutable:Cfg_Npc.rows)
  return rows_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Npc_Row >*
Cfg_Npc::mutable_rows() {
  // @@protoc_insertion_point(field_mutable_list:Cfg_Npc.rows)
  return &rows_;
}
inline const ::Cfg_Npc_Row& Cfg_Npc::_internal_rows(int index) const {
  return rows_.Get(index);
}
inline const ::Cfg_Npc_Row& Cfg_Npc::rows(int index) const {
  // @@protoc_insertion_point(field_get:Cfg_Npc.rows)
  return _internal_rows(index);
}
inline ::Cfg_Npc_Row* Cfg_Npc::_internal_add_rows() {
  return rows_.Add();
}
inline ::Cfg_Npc_Row* Cfg_Npc::add_rows() {
  // @@protoc_insertion_point(field_add:Cfg_Npc.rows)
  return _internal_add_rows();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Npc_Row >&
Cfg_Npc::rows() const {
  // @@protoc_insertion_point(field_list:Cfg_Npc.rows)
  return rows_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fNpc_2eproto
