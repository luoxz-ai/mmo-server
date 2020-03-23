// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: config/Cfg_Suit.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fSuit_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fSuit_2eproto

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
#include "common/AttribChangeDataProto.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_config_2fCfg_5fSuit_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_config_2fCfg_5fSuit_2eproto {
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
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_config_2fCfg_5fSuit_2eproto;
class Cfg_Suit;
class Cfg_SuitDefaultTypeInternal;
extern Cfg_SuitDefaultTypeInternal _Cfg_Suit_default_instance_;
class Cfg_Suit_Row;
class Cfg_Suit_RowDefaultTypeInternal;
extern Cfg_Suit_RowDefaultTypeInternal _Cfg_Suit_Row_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::Cfg_Suit* Arena::CreateMaybeMessage<::Cfg_Suit>(Arena*);
template<> ::Cfg_Suit_Row* Arena::CreateMaybeMessage<::Cfg_Suit_Row>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class Cfg_Suit_Row :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Cfg_Suit.Row) */ {
 public:
  Cfg_Suit_Row();
  virtual ~Cfg_Suit_Row();

  Cfg_Suit_Row(const Cfg_Suit_Row& from);
  Cfg_Suit_Row(Cfg_Suit_Row&& from) noexcept
    : Cfg_Suit_Row() {
    *this = ::std::move(from);
  }

  inline Cfg_Suit_Row& operator=(const Cfg_Suit_Row& from) {
    CopyFrom(from);
    return *this;
  }
  inline Cfg_Suit_Row& operator=(Cfg_Suit_Row&& from) noexcept {
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
  static const Cfg_Suit_Row& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Cfg_Suit_Row* internal_default_instance() {
    return reinterpret_cast<const Cfg_Suit_Row*>(
               &_Cfg_Suit_Row_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Cfg_Suit_Row& a, Cfg_Suit_Row& b) {
    a.Swap(&b);
  }
  inline void Swap(Cfg_Suit_Row* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Cfg_Suit_Row* New() const final {
    return CreateMaybeMessage<Cfg_Suit_Row>(nullptr);
  }

  Cfg_Suit_Row* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Cfg_Suit_Row>(arena);
  }
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Cfg_Suit_Row* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Cfg_Suit.Row";
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
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_config_2fCfg_5fSuit_2eproto);
    return ::descriptor_table_config_2fCfg_5fSuit_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kAttribChangeListFieldNumber = 12,
    kIdFieldNumber = 1,
    kNumFieldNumber = 2,
  };
  // repeated .AttribChangeDataProto attrib_change_list = 12;
  int attrib_change_list_size() const;
  private:
  int _internal_attrib_change_list_size() const;
  public:
  void clear_attrib_change_list();
  ::AttribChangeDataProto* mutable_attrib_change_list(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::AttribChangeDataProto >*
      mutable_attrib_change_list();
  private:
  const ::AttribChangeDataProto& _internal_attrib_change_list(int index) const;
  ::AttribChangeDataProto* _internal_add_attrib_change_list();
  public:
  const ::AttribChangeDataProto& attrib_change_list(int index) const;
  ::AttribChangeDataProto* add_attrib_change_list();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::AttribChangeDataProto >&
      attrib_change_list() const;

  // uint32 id = 1;
  void clear_id();
  ::PROTOBUF_NAMESPACE_ID::uint32 id() const;
  void set_id(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_id() const;
  void _internal_set_id(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 num = 2;
  void clear_num();
  ::PROTOBUF_NAMESPACE_ID::uint32 num() const;
  void set_num(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_num() const;
  void _internal_set_num(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // @@protoc_insertion_point(class_scope:Cfg_Suit.Row)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::AttribChangeDataProto > attrib_change_list_;
  ::PROTOBUF_NAMESPACE_ID::uint32 id_;
  ::PROTOBUF_NAMESPACE_ID::uint32 num_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_config_2fCfg_5fSuit_2eproto;
};
// -------------------------------------------------------------------

class Cfg_Suit :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Cfg_Suit) */ {
 public:
  Cfg_Suit();
  virtual ~Cfg_Suit();

  Cfg_Suit(const Cfg_Suit& from);
  Cfg_Suit(Cfg_Suit&& from) noexcept
    : Cfg_Suit() {
    *this = ::std::move(from);
  }

  inline Cfg_Suit& operator=(const Cfg_Suit& from) {
    CopyFrom(from);
    return *this;
  }
  inline Cfg_Suit& operator=(Cfg_Suit&& from) noexcept {
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
  static const Cfg_Suit& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Cfg_Suit* internal_default_instance() {
    return reinterpret_cast<const Cfg_Suit*>(
               &_Cfg_Suit_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(Cfg_Suit& a, Cfg_Suit& b) {
    a.Swap(&b);
  }
  inline void Swap(Cfg_Suit* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Cfg_Suit* New() const final {
    return CreateMaybeMessage<Cfg_Suit>(nullptr);
  }

  Cfg_Suit* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Cfg_Suit>(arena);
  }
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Cfg_Suit* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Cfg_Suit";
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
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_config_2fCfg_5fSuit_2eproto);
    return ::descriptor_table_config_2fCfg_5fSuit_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  typedef Cfg_Suit_Row Row;

  // accessors -------------------------------------------------------

  enum : int {
    kRowsFieldNumber = 1,
  };
  // repeated .Cfg_Suit.Row rows = 1;
  int rows_size() const;
  private:
  int _internal_rows_size() const;
  public:
  void clear_rows();
  ::Cfg_Suit_Row* mutable_rows(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Suit_Row >*
      mutable_rows();
  private:
  const ::Cfg_Suit_Row& _internal_rows(int index) const;
  ::Cfg_Suit_Row* _internal_add_rows();
  public:
  const ::Cfg_Suit_Row& rows(int index) const;
  ::Cfg_Suit_Row* add_rows();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Suit_Row >&
      rows() const;

  // @@protoc_insertion_point(class_scope:Cfg_Suit)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Suit_Row > rows_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_config_2fCfg_5fSuit_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Cfg_Suit_Row

// uint32 id = 1;
inline void Cfg_Suit_Row::clear_id() {
  id_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Suit_Row::_internal_id() const {
  return id_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Suit_Row::id() const {
  // @@protoc_insertion_point(field_get:Cfg_Suit.Row.id)
  return _internal_id();
}
inline void Cfg_Suit_Row::_internal_set_id(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  id_ = value;
}
inline void Cfg_Suit_Row::set_id(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_id(value);
  // @@protoc_insertion_point(field_set:Cfg_Suit.Row.id)
}

// uint32 num = 2;
inline void Cfg_Suit_Row::clear_num() {
  num_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Suit_Row::_internal_num() const {
  return num_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Suit_Row::num() const {
  // @@protoc_insertion_point(field_get:Cfg_Suit.Row.num)
  return _internal_num();
}
inline void Cfg_Suit_Row::_internal_set_num(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  num_ = value;
}
inline void Cfg_Suit_Row::set_num(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_num(value);
  // @@protoc_insertion_point(field_set:Cfg_Suit.Row.num)
}

// repeated .AttribChangeDataProto attrib_change_list = 12;
inline int Cfg_Suit_Row::_internal_attrib_change_list_size() const {
  return attrib_change_list_.size();
}
inline int Cfg_Suit_Row::attrib_change_list_size() const {
  return _internal_attrib_change_list_size();
}
inline ::AttribChangeDataProto* Cfg_Suit_Row::mutable_attrib_change_list(int index) {
  // @@protoc_insertion_point(field_mutable:Cfg_Suit.Row.attrib_change_list)
  return attrib_change_list_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::AttribChangeDataProto >*
Cfg_Suit_Row::mutable_attrib_change_list() {
  // @@protoc_insertion_point(field_mutable_list:Cfg_Suit.Row.attrib_change_list)
  return &attrib_change_list_;
}
inline const ::AttribChangeDataProto& Cfg_Suit_Row::_internal_attrib_change_list(int index) const {
  return attrib_change_list_.Get(index);
}
inline const ::AttribChangeDataProto& Cfg_Suit_Row::attrib_change_list(int index) const {
  // @@protoc_insertion_point(field_get:Cfg_Suit.Row.attrib_change_list)
  return _internal_attrib_change_list(index);
}
inline ::AttribChangeDataProto* Cfg_Suit_Row::_internal_add_attrib_change_list() {
  return attrib_change_list_.Add();
}
inline ::AttribChangeDataProto* Cfg_Suit_Row::add_attrib_change_list() {
  // @@protoc_insertion_point(field_add:Cfg_Suit.Row.attrib_change_list)
  return _internal_add_attrib_change_list();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::AttribChangeDataProto >&
Cfg_Suit_Row::attrib_change_list() const {
  // @@protoc_insertion_point(field_list:Cfg_Suit.Row.attrib_change_list)
  return attrib_change_list_;
}

// -------------------------------------------------------------------

// Cfg_Suit

// repeated .Cfg_Suit.Row rows = 1;
inline int Cfg_Suit::_internal_rows_size() const {
  return rows_.size();
}
inline int Cfg_Suit::rows_size() const {
  return _internal_rows_size();
}
inline void Cfg_Suit::clear_rows() {
  rows_.Clear();
}
inline ::Cfg_Suit_Row* Cfg_Suit::mutable_rows(int index) {
  // @@protoc_insertion_point(field_mutable:Cfg_Suit.rows)
  return rows_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Suit_Row >*
Cfg_Suit::mutable_rows() {
  // @@protoc_insertion_point(field_mutable_list:Cfg_Suit.rows)
  return &rows_;
}
inline const ::Cfg_Suit_Row& Cfg_Suit::_internal_rows(int index) const {
  return rows_.Get(index);
}
inline const ::Cfg_Suit_Row& Cfg_Suit::rows(int index) const {
  // @@protoc_insertion_point(field_get:Cfg_Suit.rows)
  return _internal_rows(index);
}
inline ::Cfg_Suit_Row* Cfg_Suit::_internal_add_rows() {
  return rows_.Add();
}
inline ::Cfg_Suit_Row* Cfg_Suit::add_rows() {
  // @@protoc_insertion_point(field_add:Cfg_Suit.rows)
  return _internal_add_rows();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Suit_Row >&
Cfg_Suit::rows() const {
  // @@protoc_insertion_point(field_list:Cfg_Suit.rows)
  return rows_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fSuit_2eproto