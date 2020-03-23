// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: config/Cfg_Scene_EnterPoint.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fScene_5fEnterPoint_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fScene_5fEnterPoint_2eproto

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
#define PROTOBUF_INTERNAL_EXPORT_config_2fCfg_5fScene_5fEnterPoint_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_config_2fCfg_5fScene_5fEnterPoint_2eproto {
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
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto;
class Cfg_Scene_EnterPoint;
class Cfg_Scene_EnterPointDefaultTypeInternal;
extern Cfg_Scene_EnterPointDefaultTypeInternal _Cfg_Scene_EnterPoint_default_instance_;
class Cfg_Scene_EnterPoint_Row;
class Cfg_Scene_EnterPoint_RowDefaultTypeInternal;
extern Cfg_Scene_EnterPoint_RowDefaultTypeInternal _Cfg_Scene_EnterPoint_Row_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::Cfg_Scene_EnterPoint* Arena::CreateMaybeMessage<::Cfg_Scene_EnterPoint>(Arena*);
template<> ::Cfg_Scene_EnterPoint_Row* Arena::CreateMaybeMessage<::Cfg_Scene_EnterPoint_Row>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class Cfg_Scene_EnterPoint_Row :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Cfg_Scene_EnterPoint.Row) */ {
 public:
  Cfg_Scene_EnterPoint_Row();
  virtual ~Cfg_Scene_EnterPoint_Row();

  Cfg_Scene_EnterPoint_Row(const Cfg_Scene_EnterPoint_Row& from);
  Cfg_Scene_EnterPoint_Row(Cfg_Scene_EnterPoint_Row&& from) noexcept
    : Cfg_Scene_EnterPoint_Row() {
    *this = ::std::move(from);
  }

  inline Cfg_Scene_EnterPoint_Row& operator=(const Cfg_Scene_EnterPoint_Row& from) {
    CopyFrom(from);
    return *this;
  }
  inline Cfg_Scene_EnterPoint_Row& operator=(Cfg_Scene_EnterPoint_Row&& from) noexcept {
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
  static const Cfg_Scene_EnterPoint_Row& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Cfg_Scene_EnterPoint_Row* internal_default_instance() {
    return reinterpret_cast<const Cfg_Scene_EnterPoint_Row*>(
               &_Cfg_Scene_EnterPoint_Row_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Cfg_Scene_EnterPoint_Row& a, Cfg_Scene_EnterPoint_Row& b) {
    a.Swap(&b);
  }
  inline void Swap(Cfg_Scene_EnterPoint_Row* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Cfg_Scene_EnterPoint_Row* New() const final {
    return CreateMaybeMessage<Cfg_Scene_EnterPoint_Row>(nullptr);
  }

  Cfg_Scene_EnterPoint_Row* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Cfg_Scene_EnterPoint_Row>(arena);
  }
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Cfg_Scene_EnterPoint_Row* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Cfg_Scene_EnterPoint.Row";
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
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto);
    return ::descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kIdmapFieldNumber = 1,
    kIdxFieldNumber = 2,
    kXFieldNumber = 3,
    kYFieldNumber = 4,
    kRangeFieldNumber = 5,
    kFaceFieldNumber = 6,
    kTeamReqFieldNumber = 10,
    kGuildReqFieldNumber = 11,
    kVipLevReqFieldNumber = 14,
    kLevReqFieldNumber = 12,
    kProfReqFieldNumber = 13,
  };
  // uint32 idmap = 1;
  void clear_idmap();
  ::PROTOBUF_NAMESPACE_ID::uint32 idmap() const;
  void set_idmap(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_idmap() const;
  void _internal_set_idmap(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 idx = 2;
  void clear_idx();
  ::PROTOBUF_NAMESPACE_ID::uint32 idx() const;
  void set_idx(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_idx() const;
  void _internal_set_idx(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // float x = 3;
  void clear_x();
  float x() const;
  void set_x(float value);
  private:
  float _internal_x() const;
  void _internal_set_x(float value);
  public:

  // float y = 4;
  void clear_y();
  float y() const;
  void set_y(float value);
  private:
  float _internal_y() const;
  void _internal_set_y(float value);
  public:

  // float range = 5;
  void clear_range();
  float range() const;
  void set_range(float value);
  private:
  float _internal_range() const;
  void _internal_set_range(float value);
  public:

  // float face = 6;
  void clear_face();
  float face() const;
  void set_face(float value);
  private:
  float _internal_face() const;
  void _internal_set_face(float value);
  public:

  // uint32 team_req = 10;
  void clear_team_req();
  ::PROTOBUF_NAMESPACE_ID::uint32 team_req() const;
  void set_team_req(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_team_req() const;
  void _internal_set_team_req(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 guild_req = 11;
  void clear_guild_req();
  ::PROTOBUF_NAMESPACE_ID::uint32 guild_req() const;
  void set_guild_req(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_guild_req() const;
  void _internal_set_guild_req(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 vip_lev_req = 14;
  void clear_vip_lev_req();
  ::PROTOBUF_NAMESPACE_ID::uint32 vip_lev_req() const;
  void set_vip_lev_req(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_vip_lev_req() const;
  void _internal_set_vip_lev_req(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 lev_req = 12;
  void clear_lev_req();
  ::PROTOBUF_NAMESPACE_ID::uint32 lev_req() const;
  void set_lev_req(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_lev_req() const;
  void _internal_set_lev_req(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // uint32 prof_req = 13;
  void clear_prof_req();
  ::PROTOBUF_NAMESPACE_ID::uint32 prof_req() const;
  void set_prof_req(::PROTOBUF_NAMESPACE_ID::uint32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::uint32 _internal_prof_req() const;
  void _internal_set_prof_req(::PROTOBUF_NAMESPACE_ID::uint32 value);
  public:

  // @@protoc_insertion_point(class_scope:Cfg_Scene_EnterPoint.Row)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::uint32 idmap_;
  ::PROTOBUF_NAMESPACE_ID::uint32 idx_;
  float x_;
  float y_;
  float range_;
  float face_;
  ::PROTOBUF_NAMESPACE_ID::uint32 team_req_;
  ::PROTOBUF_NAMESPACE_ID::uint32 guild_req_;
  ::PROTOBUF_NAMESPACE_ID::uint32 vip_lev_req_;
  ::PROTOBUF_NAMESPACE_ID::uint32 lev_req_;
  ::PROTOBUF_NAMESPACE_ID::uint32 prof_req_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_config_2fCfg_5fScene_5fEnterPoint_2eproto;
};
// -------------------------------------------------------------------

class Cfg_Scene_EnterPoint :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Cfg_Scene_EnterPoint) */ {
 public:
  Cfg_Scene_EnterPoint();
  virtual ~Cfg_Scene_EnterPoint();

  Cfg_Scene_EnterPoint(const Cfg_Scene_EnterPoint& from);
  Cfg_Scene_EnterPoint(Cfg_Scene_EnterPoint&& from) noexcept
    : Cfg_Scene_EnterPoint() {
    *this = ::std::move(from);
  }

  inline Cfg_Scene_EnterPoint& operator=(const Cfg_Scene_EnterPoint& from) {
    CopyFrom(from);
    return *this;
  }
  inline Cfg_Scene_EnterPoint& operator=(Cfg_Scene_EnterPoint&& from) noexcept {
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
  static const Cfg_Scene_EnterPoint& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Cfg_Scene_EnterPoint* internal_default_instance() {
    return reinterpret_cast<const Cfg_Scene_EnterPoint*>(
               &_Cfg_Scene_EnterPoint_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(Cfg_Scene_EnterPoint& a, Cfg_Scene_EnterPoint& b) {
    a.Swap(&b);
  }
  inline void Swap(Cfg_Scene_EnterPoint* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Cfg_Scene_EnterPoint* New() const final {
    return CreateMaybeMessage<Cfg_Scene_EnterPoint>(nullptr);
  }

  Cfg_Scene_EnterPoint* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Cfg_Scene_EnterPoint>(arena);
  }
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Cfg_Scene_EnterPoint* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Cfg_Scene_EnterPoint";
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
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto);
    return ::descriptor_table_config_2fCfg_5fScene_5fEnterPoint_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  typedef Cfg_Scene_EnterPoint_Row Row;

  // accessors -------------------------------------------------------

  enum : int {
    kRowsFieldNumber = 1,
  };
  // repeated .Cfg_Scene_EnterPoint.Row rows = 1;
  int rows_size() const;
  private:
  int _internal_rows_size() const;
  public:
  void clear_rows();
  ::Cfg_Scene_EnterPoint_Row* mutable_rows(int index);
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Scene_EnterPoint_Row >*
      mutable_rows();
  private:
  const ::Cfg_Scene_EnterPoint_Row& _internal_rows(int index) const;
  ::Cfg_Scene_EnterPoint_Row* _internal_add_rows();
  public:
  const ::Cfg_Scene_EnterPoint_Row& rows(int index) const;
  ::Cfg_Scene_EnterPoint_Row* add_rows();
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Scene_EnterPoint_Row >&
      rows() const;

  // @@protoc_insertion_point(class_scope:Cfg_Scene_EnterPoint)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Scene_EnterPoint_Row > rows_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_config_2fCfg_5fScene_5fEnterPoint_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Cfg_Scene_EnterPoint_Row

// uint32 idmap = 1;
inline void Cfg_Scene_EnterPoint_Row::clear_idmap() {
  idmap_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::_internal_idmap() const {
  return idmap_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::idmap() const {
  // @@protoc_insertion_point(field_get:Cfg_Scene_EnterPoint.Row.idmap)
  return _internal_idmap();
}
inline void Cfg_Scene_EnterPoint_Row::_internal_set_idmap(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  idmap_ = value;
}
inline void Cfg_Scene_EnterPoint_Row::set_idmap(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_idmap(value);
  // @@protoc_insertion_point(field_set:Cfg_Scene_EnterPoint.Row.idmap)
}

// uint32 idx = 2;
inline void Cfg_Scene_EnterPoint_Row::clear_idx() {
  idx_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::_internal_idx() const {
  return idx_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::idx() const {
  // @@protoc_insertion_point(field_get:Cfg_Scene_EnterPoint.Row.idx)
  return _internal_idx();
}
inline void Cfg_Scene_EnterPoint_Row::_internal_set_idx(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  idx_ = value;
}
inline void Cfg_Scene_EnterPoint_Row::set_idx(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_idx(value);
  // @@protoc_insertion_point(field_set:Cfg_Scene_EnterPoint.Row.idx)
}

// float x = 3;
inline void Cfg_Scene_EnterPoint_Row::clear_x() {
  x_ = 0;
}
inline float Cfg_Scene_EnterPoint_Row::_internal_x() const {
  return x_;
}
inline float Cfg_Scene_EnterPoint_Row::x() const {
  // @@protoc_insertion_point(field_get:Cfg_Scene_EnterPoint.Row.x)
  return _internal_x();
}
inline void Cfg_Scene_EnterPoint_Row::_internal_set_x(float value) {
  
  x_ = value;
}
inline void Cfg_Scene_EnterPoint_Row::set_x(float value) {
  _internal_set_x(value);
  // @@protoc_insertion_point(field_set:Cfg_Scene_EnterPoint.Row.x)
}

// float y = 4;
inline void Cfg_Scene_EnterPoint_Row::clear_y() {
  y_ = 0;
}
inline float Cfg_Scene_EnterPoint_Row::_internal_y() const {
  return y_;
}
inline float Cfg_Scene_EnterPoint_Row::y() const {
  // @@protoc_insertion_point(field_get:Cfg_Scene_EnterPoint.Row.y)
  return _internal_y();
}
inline void Cfg_Scene_EnterPoint_Row::_internal_set_y(float value) {
  
  y_ = value;
}
inline void Cfg_Scene_EnterPoint_Row::set_y(float value) {
  _internal_set_y(value);
  // @@protoc_insertion_point(field_set:Cfg_Scene_EnterPoint.Row.y)
}

// float range = 5;
inline void Cfg_Scene_EnterPoint_Row::clear_range() {
  range_ = 0;
}
inline float Cfg_Scene_EnterPoint_Row::_internal_range() const {
  return range_;
}
inline float Cfg_Scene_EnterPoint_Row::range() const {
  // @@protoc_insertion_point(field_get:Cfg_Scene_EnterPoint.Row.range)
  return _internal_range();
}
inline void Cfg_Scene_EnterPoint_Row::_internal_set_range(float value) {
  
  range_ = value;
}
inline void Cfg_Scene_EnterPoint_Row::set_range(float value) {
  _internal_set_range(value);
  // @@protoc_insertion_point(field_set:Cfg_Scene_EnterPoint.Row.range)
}

// float face = 6;
inline void Cfg_Scene_EnterPoint_Row::clear_face() {
  face_ = 0;
}
inline float Cfg_Scene_EnterPoint_Row::_internal_face() const {
  return face_;
}
inline float Cfg_Scene_EnterPoint_Row::face() const {
  // @@protoc_insertion_point(field_get:Cfg_Scene_EnterPoint.Row.face)
  return _internal_face();
}
inline void Cfg_Scene_EnterPoint_Row::_internal_set_face(float value) {
  
  face_ = value;
}
inline void Cfg_Scene_EnterPoint_Row::set_face(float value) {
  _internal_set_face(value);
  // @@protoc_insertion_point(field_set:Cfg_Scene_EnterPoint.Row.face)
}

// uint32 team_req = 10;
inline void Cfg_Scene_EnterPoint_Row::clear_team_req() {
  team_req_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::_internal_team_req() const {
  return team_req_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::team_req() const {
  // @@protoc_insertion_point(field_get:Cfg_Scene_EnterPoint.Row.team_req)
  return _internal_team_req();
}
inline void Cfg_Scene_EnterPoint_Row::_internal_set_team_req(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  team_req_ = value;
}
inline void Cfg_Scene_EnterPoint_Row::set_team_req(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_team_req(value);
  // @@protoc_insertion_point(field_set:Cfg_Scene_EnterPoint.Row.team_req)
}

// uint32 guild_req = 11;
inline void Cfg_Scene_EnterPoint_Row::clear_guild_req() {
  guild_req_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::_internal_guild_req() const {
  return guild_req_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::guild_req() const {
  // @@protoc_insertion_point(field_get:Cfg_Scene_EnterPoint.Row.guild_req)
  return _internal_guild_req();
}
inline void Cfg_Scene_EnterPoint_Row::_internal_set_guild_req(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  guild_req_ = value;
}
inline void Cfg_Scene_EnterPoint_Row::set_guild_req(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_guild_req(value);
  // @@protoc_insertion_point(field_set:Cfg_Scene_EnterPoint.Row.guild_req)
}

// uint32 lev_req = 12;
inline void Cfg_Scene_EnterPoint_Row::clear_lev_req() {
  lev_req_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::_internal_lev_req() const {
  return lev_req_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::lev_req() const {
  // @@protoc_insertion_point(field_get:Cfg_Scene_EnterPoint.Row.lev_req)
  return _internal_lev_req();
}
inline void Cfg_Scene_EnterPoint_Row::_internal_set_lev_req(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  lev_req_ = value;
}
inline void Cfg_Scene_EnterPoint_Row::set_lev_req(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_lev_req(value);
  // @@protoc_insertion_point(field_set:Cfg_Scene_EnterPoint.Row.lev_req)
}

// uint32 prof_req = 13;
inline void Cfg_Scene_EnterPoint_Row::clear_prof_req() {
  prof_req_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::_internal_prof_req() const {
  return prof_req_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::prof_req() const {
  // @@protoc_insertion_point(field_get:Cfg_Scene_EnterPoint.Row.prof_req)
  return _internal_prof_req();
}
inline void Cfg_Scene_EnterPoint_Row::_internal_set_prof_req(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  prof_req_ = value;
}
inline void Cfg_Scene_EnterPoint_Row::set_prof_req(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_prof_req(value);
  // @@protoc_insertion_point(field_set:Cfg_Scene_EnterPoint.Row.prof_req)
}

// uint32 vip_lev_req = 14;
inline void Cfg_Scene_EnterPoint_Row::clear_vip_lev_req() {
  vip_lev_req_ = 0u;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::_internal_vip_lev_req() const {
  return vip_lev_req_;
}
inline ::PROTOBUF_NAMESPACE_ID::uint32 Cfg_Scene_EnterPoint_Row::vip_lev_req() const {
  // @@protoc_insertion_point(field_get:Cfg_Scene_EnterPoint.Row.vip_lev_req)
  return _internal_vip_lev_req();
}
inline void Cfg_Scene_EnterPoint_Row::_internal_set_vip_lev_req(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  
  vip_lev_req_ = value;
}
inline void Cfg_Scene_EnterPoint_Row::set_vip_lev_req(::PROTOBUF_NAMESPACE_ID::uint32 value) {
  _internal_set_vip_lev_req(value);
  // @@protoc_insertion_point(field_set:Cfg_Scene_EnterPoint.Row.vip_lev_req)
}

// -------------------------------------------------------------------

// Cfg_Scene_EnterPoint

// repeated .Cfg_Scene_EnterPoint.Row rows = 1;
inline int Cfg_Scene_EnterPoint::_internal_rows_size() const {
  return rows_.size();
}
inline int Cfg_Scene_EnterPoint::rows_size() const {
  return _internal_rows_size();
}
inline void Cfg_Scene_EnterPoint::clear_rows() {
  rows_.Clear();
}
inline ::Cfg_Scene_EnterPoint_Row* Cfg_Scene_EnterPoint::mutable_rows(int index) {
  // @@protoc_insertion_point(field_mutable:Cfg_Scene_EnterPoint.rows)
  return rows_.Mutable(index);
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Scene_EnterPoint_Row >*
Cfg_Scene_EnterPoint::mutable_rows() {
  // @@protoc_insertion_point(field_mutable_list:Cfg_Scene_EnterPoint.rows)
  return &rows_;
}
inline const ::Cfg_Scene_EnterPoint_Row& Cfg_Scene_EnterPoint::_internal_rows(int index) const {
  return rows_.Get(index);
}
inline const ::Cfg_Scene_EnterPoint_Row& Cfg_Scene_EnterPoint::rows(int index) const {
  // @@protoc_insertion_point(field_get:Cfg_Scene_EnterPoint.rows)
  return _internal_rows(index);
}
inline ::Cfg_Scene_EnterPoint_Row* Cfg_Scene_EnterPoint::_internal_add_rows() {
  return rows_.Add();
}
inline ::Cfg_Scene_EnterPoint_Row* Cfg_Scene_EnterPoint::add_rows() {
  // @@protoc_insertion_point(field_add:Cfg_Scene_EnterPoint.rows)
  return _internal_add_rows();
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField< ::Cfg_Scene_EnterPoint_Row >&
Cfg_Scene_EnterPoint::rows() const {
  // @@protoc_insertion_point(field_list:Cfg_Scene_EnterPoint.rows)
  return rows_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_config_2fCfg_5fScene_5fEnterPoint_2eproto