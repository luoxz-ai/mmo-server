#include "AxisAlignedBox.h"
#include "BaseCode.h"
#include "BaseType.h"
#include "CallStackDumper.h"
#include "CheckUtil.h"
#include "EnumUtil.h"
#include "FileLock.h"
#include "FileUtil.h"
#include "GameMath.h"
#include "IDGenPool.h"
#include "INILoader.h"
#include "IntUtil.h"
#include "LockfreeQueue.h"
#include "LoggingMgr.h"
#include "MD5.h"
#include "MathDef.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "MemoryHelp.h"
#include "Misc.h"
#include "MyTimer.h"
#include "Noncopyable.h"
#include "ObjectHeap.h"
#include "ProtobuffParse.h"
#include "ProtobuffUtil.h"
#include "Quaternion.h"
#include "RandomGet.h"
#include "Ray.h"
#include "SampleDynamicBitset.h"
#include "SegvCatch.h"
#include "SettingMap.h"
#include "Singleton.h"
#include "SkipList.h"
#include "StaticReflection.h"
#include "StaticTypeInfo.h"
#include "StringAlgo.h"
#include "T_GameDataMap.h"
#include "Thread.h"
#include "TimeUtil.h"
#include "UIDFactory.h"
#include "Vector2.h"
#include "Vector3.h"
#include "ZSet.h"
#include "dynamic_bitset.h"
#include "export_lua.h"
#include "get_opt.h"
#include "i386-signal.h"
#include "log4z.h"
#include "lua_tinker.h"
#include "magic_enum.h"
#include "nameof.h"
#include "type_traits_ext.h"
#include "x86_64-signal.h"
#include "xml2struct.h"
void basecode2lua(lua_State* L)
{
    lua_tinker::class_add<Angle>(L, "Angle", false);
    lua_tinker::class_add<AxisAlignedBox>(L, "AxisAlignedBox", false);
    lua_tinker::class_def_static<AxisAlignedBox>(L, "BOX_INFINITE", &AxisAlignedBox::BOX_INFINITE);
    lua_tinker::class_def_static<AxisAlignedBox>(L, "BOX_NULL", &AxisAlignedBox::BOX_NULL);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "contains",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((bool (AxisAlignedBox::*)(const AxisAlignedBox&) const)(&AxisAlignedBox::contains)),
            lua_tinker::make_member_functor_ptr((bool (AxisAlignedBox::*)(const Vector3&) const)(&AxisAlignedBox::contains))));
    lua_tinker::class_def<AxisAlignedBox>(L, "distance", &AxisAlignedBox::distance);
    lua_tinker::class_def<AxisAlignedBox>(L, "getAllCorners", &AxisAlignedBox::getAllCorners);
    lua_tinker::class_def<AxisAlignedBox>(L, "getCenter", &AxisAlignedBox::getCenter);
    lua_tinker::class_def<AxisAlignedBox>(L, "getCorner", &AxisAlignedBox::getCorner);
    lua_tinker::class_def<AxisAlignedBox>(L, "getHalfSize", &AxisAlignedBox::getHalfSize);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "getMaximum",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((Vector3 & (AxisAlignedBox::*)())(&AxisAlignedBox::getMaximum)),
            lua_tinker::make_member_functor_ptr((const Vector3& (AxisAlignedBox::*)() const)(&AxisAlignedBox::getMaximum))));
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "getMinimum",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((Vector3 & (AxisAlignedBox::*)())(&AxisAlignedBox::getMinimum)),
            lua_tinker::make_member_functor_ptr((const Vector3& (AxisAlignedBox::*)() const)(&AxisAlignedBox::getMinimum))));
    lua_tinker::class_def<AxisAlignedBox>(L, "getSize", &AxisAlignedBox::getSize);
    lua_tinker::class_def<AxisAlignedBox>(L, "intersection", &AxisAlignedBox::intersection);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "intersects",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((bool (AxisAlignedBox::*)(const AxisAlignedBox&) const)(&AxisAlignedBox::intersects)),
            lua_tinker::make_member_functor_ptr((bool (AxisAlignedBox::*)(const Vector3&) const)(&AxisAlignedBox::intersects))));
    lua_tinker::class_def<AxisAlignedBox>(L, "isFinite", &AxisAlignedBox::isFinite);
    lua_tinker::class_def<AxisAlignedBox>(L, "isInfinite", &AxisAlignedBox::isInfinite);
    lua_tinker::class_def<AxisAlignedBox>(L, "isNull", &AxisAlignedBox::isNull);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "merge",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const AxisAlignedBox&))(&AxisAlignedBox::merge)),
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const Vector3&))(&AxisAlignedBox::merge))));
    lua_tinker::class_def<AxisAlignedBox>(L, "operator!=", &AxisAlignedBox::operator!=);
    lua_tinker::class_def<AxisAlignedBox>(L, "__eq", &AxisAlignedBox::operator==);
    lua_tinker::class_def<AxisAlignedBox>(L, "scale", &AxisAlignedBox::scale);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "setExtents",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const Vector3&, const Vector3&))(&AxisAlignedBox::setExtents)),
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(float, float, float, float, float, float))(&AxisAlignedBox::setExtents))));
    lua_tinker::class_def<AxisAlignedBox>(L, "setInfinite", &AxisAlignedBox::setInfinite);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "setMaximum",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const Vector3&))(&AxisAlignedBox::setMaximum)),
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(float, float, float))(&AxisAlignedBox::setMaximum))));
    lua_tinker::class_def<AxisAlignedBox>(L, "setMaximumX", &AxisAlignedBox::setMaximumX);
    lua_tinker::class_def<AxisAlignedBox>(L, "setMaximumY", &AxisAlignedBox::setMaximumY);
    lua_tinker::class_def<AxisAlignedBox>(L, "setMaximumZ", &AxisAlignedBox::setMaximumZ);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "setMinimum",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const Vector3&))(&AxisAlignedBox::setMinimum)),
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(float, float, float))(&AxisAlignedBox::setMinimum))));
    lua_tinker::class_def<AxisAlignedBox>(L, "setMinimumX", &AxisAlignedBox::setMinimumX);
    lua_tinker::class_def<AxisAlignedBox>(L, "setMinimumY", &AxisAlignedBox::setMinimumY);
    lua_tinker::class_def<AxisAlignedBox>(L, "setMinimumZ", &AxisAlignedBox::setMinimumZ);
    lua_tinker::class_def<AxisAlignedBox>(L, "setNull", &AxisAlignedBox::setNull);
    lua_tinker::class_def<AxisAlignedBox>(L, "squaredDistance", &AxisAlignedBox::squaredDistance);
    lua_tinker::class_def<AxisAlignedBox>(
        L,
        "transform",
        lua_tinker::args_type_overload_member_functor(
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const Affine3&))(&AxisAlignedBox::transform)),
            lua_tinker::make_member_functor_ptr((void (AxisAlignedBox::*)(const Matrix4&))(&AxisAlignedBox::transform))));
    lua_tinker::class_def<AxisAlignedBox>(L, "volume", &AxisAlignedBox::volume);
    lua_tinker::class_con<AxisAlignedBox>(
        L,
        lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<AxisAlignedBox>(),
                                                   new lua_tinker::constructor<AxisAlignedBox, AxisAlignedBox::Extent>(),
                                                   new lua_tinker::constructor<AxisAlignedBox, const Vector3&, const Vector3&>(),
                                                   new lua_tinker::constructor<AxisAlignedBox, float, float, float, float, float, float>()));
    lua_tinker::class_var_static<AxisAlignedBox>(L, "EXTENT_FINITE", AxisAlignedBox::EXTENT_FINITE);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "EXTENT_INFINITE", AxisAlignedBox::EXTENT_INFINITE);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "EXTENT_NULL", AxisAlignedBox::EXTENT_NULL);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "FAR_LEFT_BOTTOM", AxisAlignedBox::FAR_LEFT_BOTTOM);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "FAR_LEFT_TOP", AxisAlignedBox::FAR_LEFT_TOP);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "FAR_RIGHT_BOTTOM", AxisAlignedBox::FAR_RIGHT_BOTTOM);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "FAR_RIGHT_TOP", AxisAlignedBox::FAR_RIGHT_TOP);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "NEAR_LEFT_BOTTOM", AxisAlignedBox::NEAR_LEFT_BOTTOM);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "NEAR_LEFT_TOP", AxisAlignedBox::NEAR_LEFT_TOP);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "NEAR_RIGHT_BOTTOM", AxisAlignedBox::NEAR_RIGHT_BOTTOM);
    lua_tinker::class_var_static<AxisAlignedBox>(L, "NEAR_RIGHT_TOP", AxisAlignedBox::NEAR_RIGHT_TOP);
    lua_tinker::class_add<CMyTimer>(L, "CMyTimer", false);
    lua_tinker::class_def<CMyTimer>(L, "Clear", &CMyTimer::Clear);
    lua_tinker::class_def<CMyTimer>(L, "DecInterval", &CMyTimer::DecInterval);
    lua_tinker::class_def<CMyTimer>(L, "GetRemain", &CMyTimer::GetRemain);
    lua_tinker::class_def<CMyTimer>(L, "GetTimeOut", &CMyTimer::GetTimeOut);
    lua_tinker::class_def<CMyTimer>(L, "IncInterval", &CMyTimer::IncInterval);
    lua_tinker::class_def<CMyTimer>(L, "IsActive", &CMyTimer::IsActive);
    lua_tinker::class_def<CMyTimer>(
        L,
        "IsTimeOut",
        lua_tinker::args_type_overload_member_functor(lua_tinker::make_member_functor_ptr((bool (CMyTimer::*)())(&CMyTimer::IsTimeOut)),
                                                      lua_tinker::make_member_functor_ptr((bool (CMyTimer::*)(int32_t))(&CMyTimer::IsTimeOut))));
    lua_tinker::class_def<CMyTimer>(L, "SetInterval", &CMyTimer::SetInterval);
    lua_tinker::class_def<CMyTimer>(L, "Startup", &CMyTimer::Startup);
    lua_tinker::class_def<CMyTimer>(L, "TimeOver", &CMyTimer::TimeOver);
    lua_tinker::class_def<CMyTimer>(L, "ToNextTick", &CMyTimer::ToNextTick);
    lua_tinker::class_def<CMyTimer>(
        L,
        "ToNextTime",
        lua_tinker::args_type_overload_member_functor(lua_tinker::make_member_functor_ptr((bool (CMyTimer::*)())(&CMyTimer::ToNextTime)),
                                                      lua_tinker::make_member_functor_ptr((bool (CMyTimer::*)(int32_t))(&CMyTimer::ToNextTime))));
    lua_tinker::class_def<CMyTimer>(L, "Update", &CMyTimer::Update);
    lua_tinker::class_con<CMyTimer>(L, lua_tinker::constructor<CMyTimer, int32_t>::invoke, 0);
    lua_tinker::class_add<Degree>(L, "Degree", false);
    lua_tinker::class_def<Degree>(L, "operator!=", &Degree::operator!=);
    lua_tinker::class_def<Degree>(L,
                                  "__mul",
                                  lua_tinker::args_type_overload_member_functor(
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)(const Degree&) const)(&Degree::operator*)),
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)(float) const)(&Degree::operator*))));
    lua_tinker::class_def<Degree>(L,
                                  "__add",
                                  lua_tinker::args_type_overload_member_functor(
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)(const Degree&) const)(&Degree::operator+)),
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)(const Radian&) const)(&Degree::operator+)),
                                      lua_tinker::make_member_functor_ptr((const Degree& (Degree::*)() const)(&Degree::operator+))));
    lua_tinker::class_def<Degree>(L,
                                  "__sub",
                                  lua_tinker::args_type_overload_member_functor(
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)() const)(&Degree::operator-)),
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)(const Degree&) const)(&Degree::operator-)),
                                      lua_tinker::make_member_functor_ptr((Degree(Degree::*)(const Radian&) const)(&Degree::operator-))));
    lua_tinker::class_def<Degree>(L, "__div", &Degree::operator/);
    lua_tinker::class_def<Degree>(L, "__lt", &Degree::operator<);
    lua_tinker::class_def<Degree>(L, "__le", &Degree::operator<=);
    lua_tinker::class_def<Degree>(
        L,
        "operator=",
        lua_tinker::args_type_overload_member_functor(lua_tinker::make_member_functor_ptr((Degree & (Degree::*)(const Degree&))(&Degree::operator=)),
                                                      lua_tinker::make_member_functor_ptr((Degree & (Degree::*)(const Radian&))(&Degree::operator=)),
                                                      lua_tinker::make_member_functor_ptr((Degree & (Degree::*)(const float&))(&Degree::operator=))));
    lua_tinker::class_def<Degree>(L, "__eq", &Degree::operator==);
    lua_tinker::class_def<Degree>(L, "operator>", &Degree::operator>);
    lua_tinker::class_def<Degree>(L, "operator>=", &Degree::operator>=);
    lua_tinker::class_def<Degree>(L, "valueAngleUnits", &Degree::valueAngleUnits);
    lua_tinker::class_def<Degree>(L, "valueDegrees", &Degree::valueDegrees);
    lua_tinker::class_def<Degree>(L, "valueRadians", &Degree::valueRadians);
    lua_tinker::class_con<Degree>(
        L,
        lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<Degree, const Degree&>(),
                                                   new lua_tinker::constructor<Degree, const Radian&>(),
                                                   new lua_tinker::constructor<Degree, float>(1 /*default_args_count*/, 1 /*default_args_start*/)),
        0);
    lua_tinker::class_add<FloatRect>(L, "FloatRect", false);
    lua_tinker::class_def<FloatRect>(L, "height", &FloatRect::height);
    lua_tinker::class_def<FloatRect>(L, "isIntersect", &FloatRect::isIntersect);
    lua_tinker::class_def<FloatRect>(L, "width", &FloatRect::width);
    lua_tinker::class_con<FloatRect>(
        L,
        lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<FloatRect>(),
                                                   new lua_tinker::constructor<FloatRect, const FloatRect&>(),
                                                   new lua_tinker::constructor<FloatRect, float, float, float, float>()));
    lua_tinker::class_mem<FloatRect>(L, "bottom", &FloatRect::bottom);
    lua_tinker::class_mem<FloatRect>(L, "left", &FloatRect::left);
    lua_tinker::class_mem<FloatRect>(L, "right", &FloatRect::right);
    lua_tinker::class_mem<FloatRect>(L, "top", &FloatRect::top);
    lua_tinker::class_add<Math>(L, "Math", false);
    lua_tinker::class_def_static<Math>(L, "ACos", &Math::ACos);
    lua_tinker::class_def_static<Math>(L, "ASin", &Math::ASin);
    lua_tinker::class_def_static<Math>(L, "ATan", &Math::ATan);
    lua_tinker::class_def_static<Math>(L, "ATan2", &Math::ATan2);
    lua_tinker::class_def_static<Math>(L,
                                       "Abs",
                                       lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((Degree(*)(const Degree&))(&Math::Abs)),
                                                                              lua_tinker::make_functor_ptr((Radian(*)(const Radian&))(&Math::Abs)),
                                                                              lua_tinker::make_functor_ptr((float (*)(float))(&Math::Abs))));
    lua_tinker::class_def_static<Math>(L, "AngleUnitsToDegrees", &Math::AngleUnitsToDegrees);
    lua_tinker::class_def_static<Math>(L, "AngleUnitsToRadians", &Math::AngleUnitsToRadians);
    lua_tinker::class_def_static<Math>(L, "Ceil", &Math::Ceil);
    lua_tinker::class_def_static<Math>(
        L,
        "Cos",
        lua_tinker::args_type_overload_functor(
            lua_tinker::make_functor_ptr((float (*)(const Radian&, bool))(&Math::Cos), 1 /*default_args_count*/, 1 /*default_args_start*/),
            lua_tinker::make_functor_ptr((float (*)(float, bool))(&Math::Cos), 1 /*default_args_count*/, 2 /*default_args_start*/)),
        false,
        false);
    lua_tinker::class_def_static<Math>(L, "DegreesToAngleUnits", &Math::DegreesToAngleUnits);
    lua_tinker::class_def_static<Math>(L, "DegreesToRadians", &Math::DegreesToRadians);
    lua_tinker::class_def_static<Math>(L, "Exp", &Math::Exp);
    lua_tinker::class_def_static<Math>(L, "FloatEqual", &Math::FloatEqual, std::numeric_limits<float>::epsilon());
    lua_tinker::class_def_static<Math>(L, "Floor", &Math::Floor);
    lua_tinker::class_def_static<Math>(L, "IAbs", &Math::IAbs);
    lua_tinker::class_def_static<Math>(L, "ICeil", &Math::ICeil);
    lua_tinker::class_def_static<Math>(L, "IFloor", &Math::IFloor);
    lua_tinker::class_def_static<Math>(L, "ISign", &Math::ISign);
    lua_tinker::class_def_static<Math>(L, "InvSqrt", &Math::InvSqrt);
    lua_tinker::class_def_static<Math>(L, "Log", &Math::Log);
    lua_tinker::class_def_static<Math>(L, "Log2", &Math::Log2);
    lua_tinker::class_def_static<Math>(L, "LogN", &Math::LogN);
    lua_tinker::class_def_static<Math>(L, "Pow", &Math::Pow);
    lua_tinker::class_def_static<Math>(L, "RadiansToAngleUnits", &Math::RadiansToAngleUnits);
    lua_tinker::class_def_static<Math>(L, "RadiansToDegrees", &Math::RadiansToDegrees);
    lua_tinker::class_def_static<Math>(L,
                                       "Sign",
                                       lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((Degree(*)(const Degree&))(&Math::Sign)),
                                                                              lua_tinker::make_functor_ptr((Radian(*)(const Radian&))(&Math::Sign)),
                                                                              lua_tinker::make_functor_ptr((float (*)(float))(&Math::Sign))));
    lua_tinker::class_def_static<Math>(
        L,
        "Sin",
        lua_tinker::args_type_overload_functor(
            lua_tinker::make_functor_ptr((float (*)(const Radian&, bool))(&Math::Sin), 1 /*default_args_count*/, 1 /*default_args_start*/),
            lua_tinker::make_functor_ptr((float (*)(float, bool))(&Math::Sin), 1 /*default_args_count*/, 2 /*default_args_start*/)),
        false,
        false);
    lua_tinker::class_def_static<Math>(L, "Sqr", &Math::Sqr);
    lua_tinker::class_def_static<Math>(L,
                                       "Sqrt",
                                       lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((Degree(*)(const Degree&))(&Math::Sqrt)),
                                                                              lua_tinker::make_functor_ptr((Radian(*)(const Radian&))(&Math::Sqrt)),
                                                                              lua_tinker::make_functor_ptr((float (*)(float))(&Math::Sqrt))));
    lua_tinker::class_def_static<Math>(
        L,
        "Tan",
        lua_tinker::args_type_overload_functor(
            lua_tinker::make_functor_ptr((float (*)(const Radian&, bool))(&Math::Tan), 1 /*default_args_count*/, 1 /*default_args_start*/),
            lua_tinker::make_functor_ptr((float (*)(float, bool))(&Math::Tan), 1 /*default_args_count*/, 2 /*default_args_start*/)),
        false,
        false);
    lua_tinker::class_def_static<Math>(L, "get2Pow", &Math::get2Pow);
    lua_tinker::class_def_static<Math>(L, "getAngleUnit", &Math::getAngleUnit);
    lua_tinker::class_def_static<Math>(L, "ilog2", &Math::ilog2);
    lua_tinker::class_def_static<Math>(L, "is2Pow", &Math::is2Pow);
    lua_tinker::class_def_static<Math>(L, "isNaN", &Math::isNaN);
    lua_tinker::class_def_static<Math>(L, "nextPow2", &Math::nextPow2);
    lua_tinker::class_def_static<Math>(L,
                                       "saturate",
                                       lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((double (*)(double))(&Math::saturate)),
                                                                              lua_tinker::make_functor_ptr((float (*)(float))(&Math::saturate))));
    lua_tinker::class_mem_static_readonly<Math>(L, "HALF_PI", &Math::HALF_PI);
    lua_tinker::class_mem_static_readonly<Math>(L, "PI", &Math::PI);
    lua_tinker::class_mem_static_readonly<Math>(L, "TWO_PI", &Math::TWO_PI);
    lua_tinker::class_add<Ray>(L, "Ray", false);
    lua_tinker::class_def<Ray>(L, "getDirection", &Ray::getDirection);
    lua_tinker::class_def<Ray>(L, "getOrigin", &Ray::getOrigin);
    lua_tinker::class_def<Ray>(L, "getPoint", &Ray::getPoint);
    lua_tinker::class_def<Ray>(L, "__mul", &Ray::operator*);
    lua_tinker::class_def<Ray>(L, "setDirection", &Ray::setDirection);
    lua_tinker::class_def<Ray>(L, "setOrigin", &Ray::setOrigin);
    lua_tinker::class_con<Ray>(L,
                               lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<Ray>(),
                                                                          new lua_tinker::constructor<Ray, const Vector3&, const Vector3&>()));
    lua_tinker::class_add<Rect>(L, "Rect", false);
    lua_tinker::class_def<Rect>(L, "height", &Rect::height);
    lua_tinker::class_def<Rect>(L, "isIntersect", &Rect::isIntersect);
    lua_tinker::class_def<Rect>(L, "operator=", &Rect::operator=);
    lua_tinker::class_def<Rect>(L, "width", &Rect::width);
    lua_tinker::class_con<Rect>(L,
                                lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<Rect>(),
                                                                           new lua_tinker::constructor<Rect, const Rect&>(),
                                                                           new lua_tinker::constructor<Rect, long, long, long, long>()));
    lua_tinker::class_mem<Rect>(L, "bottom", &Rect::bottom);
    lua_tinker::class_mem<Rect>(L, "left", &Rect::left);
    lua_tinker::class_mem<Rect>(L, "right", &Rect::right);
    lua_tinker::class_mem<Rect>(L, "top", &Rect::top);
    lua_tinker::class_add<Vector2>(L, "Vector2", false);
    lua_tinker::class_def_static<Vector2>(L, "NEGATIVE_UNIT_X", &Vector2::NEGATIVE_UNIT_X);
    lua_tinker::class_def_static<Vector2>(L, "NEGATIVE_UNIT_Y", &Vector2::NEGATIVE_UNIT_Y);
    lua_tinker::class_def_static<Vector2>(L, "UNIT_SCALE", &Vector2::UNIT_SCALE);
    lua_tinker::class_def_static<Vector2>(L, "UNIT_X", &Vector2::UNIT_X);
    lua_tinker::class_def_static<Vector2>(L, "UNIT_Y", &Vector2::UNIT_Y);
    lua_tinker::class_def_static<Vector2>(L, "ZERO", &Vector2::ZERO);
    lua_tinker::class_def<Vector2>(L, "crossProduct", &Vector2::crossProduct);
    lua_tinker::class_def<Vector2>(L, "distance", &Vector2::distance);
    lua_tinker::class_def<Vector2>(L, "dotProduct", &Vector2::dotProduct);
    lua_tinker::class_def<Vector2>(L, "isNaN", &Vector2::isNaN);
    lua_tinker::class_def<Vector2>(L, "isZeroLength", &Vector2::isZeroLength);
    lua_tinker::class_def<Vector2>(L, "length", &Vector2::length);
    lua_tinker::class_def<Vector2>(L, "makeCeil", &Vector2::makeCeil);
    lua_tinker::class_def<Vector2>(L, "makeFloor", &Vector2::makeFloor);
    lua_tinker::class_def<Vector2>(L, "midPoint", &Vector2::midPoint);
    lua_tinker::class_def<Vector2>(L, "normalise", &Vector2::normalise);
    lua_tinker::class_def<Vector2>(L, "normalisedCopy", &Vector2::normalisedCopy);
    lua_tinker::class_def<Vector2>(L, "operator!=", &Vector2::operator!=);
    lua_tinker::class_def<Vector2>(L, "__lt", &Vector2::operator<);
    lua_tinker::class_def<Vector2>(L, "__eq", &Vector2::operator==);
    lua_tinker::class_def<Vector2>(L, "perpendicular", &Vector2::perpendicular);
    lua_tinker::class_def<Vector2>(L, "randomDeviant", &Vector2::randomDeviant);
    lua_tinker::class_def<Vector2>(L, "reflect", &Vector2::reflect);
    lua_tinker::class_def<Vector2>(L, "squaredDistance", &Vector2::squaredDistance);
    lua_tinker::class_def<Vector2>(L, "squaredLength", &Vector2::squaredLength);
    lua_tinker::class_def<Vector2>(L, "swap", &Vector2::swap);
    lua_tinker::class_con<Vector2>(L,
                                   lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<Vector2>(),
                                                                              new lua_tinker::constructor<Vector2, const float>(),
                                                                              new lua_tinker::constructor<Vector2, const float, const float>()));
    lua_tinker::class_mem<Vector2>(L, "x", &Vector2::x);
    lua_tinker::class_mem<Vector2>(L, "y", &Vector2::y);
    lua_tinker::class_add<Vector3>(L, "Vector3", false);
    lua_tinker::class_def_static<Vector3>(L, "NEGATIVE_UNIT_X", &Vector3::NEGATIVE_UNIT_X);
    lua_tinker::class_def_static<Vector3>(L, "NEGATIVE_UNIT_Y", &Vector3::NEGATIVE_UNIT_Y);
    lua_tinker::class_def_static<Vector3>(L, "NEGATIVE_UNIT_Z", &Vector3::NEGATIVE_UNIT_Z);
    lua_tinker::class_def_static<Vector3>(L, "UNIT_SCALE", &Vector3::UNIT_SCALE);
    lua_tinker::class_def_static<Vector3>(L, "UNIT_X", &Vector3::UNIT_X);
    lua_tinker::class_def_static<Vector3>(L, "UNIT_Y", &Vector3::UNIT_Y);
    lua_tinker::class_def_static<Vector3>(L, "UNIT_Z", &Vector3::UNIT_Z);
    lua_tinker::class_def_static<Vector3>(L, "ZERO", &Vector3::ZERO);
    lua_tinker::class_def<Vector3>(L, "absDotProduct", &Vector3::absDotProduct);
    lua_tinker::class_def<Vector3>(L, "crossProduct", &Vector3::crossProduct);
    lua_tinker::class_def<Vector3>(L, "distance", &Vector3::distance);
    lua_tinker::class_def<Vector3>(L, "dotProduct", &Vector3::dotProduct);
    lua_tinker::class_def<Vector3>(L, "isNaN", &Vector3::isNaN);
    lua_tinker::class_def<Vector3>(L, "isZeroLength", &Vector3::isZeroLength);
    lua_tinker::class_def<Vector3>(L, "length", &Vector3::length);
    lua_tinker::class_def<Vector3>(L, "makeCeil", &Vector3::makeCeil);
    lua_tinker::class_def<Vector3>(L, "makeFloor", &Vector3::makeFloor);
    lua_tinker::class_def<Vector3>(L, "midPoint", &Vector3::midPoint);
    lua_tinker::class_def<Vector3>(L, "normalise", &Vector3::normalise);
    lua_tinker::class_def<Vector3>(L, "normalisedCopy", &Vector3::normalisedCopy);
    lua_tinker::class_def<Vector3>(L, "operator!=", &Vector3::operator!=);
    lua_tinker::class_def<Vector3>(L, "__lt", &Vector3::operator<);
    lua_tinker::class_def<Vector3>(L, "__eq", &Vector3::operator==);
    lua_tinker::class_def<Vector3>(L, "operator>", &Vector3::operator>);
    lua_tinker::class_def<Vector3>(L, "perpendicular", &Vector3::perpendicular);
    lua_tinker::class_def<Vector3>(L, "positionCloses", &Vector3::positionCloses, 1e-03f);
    lua_tinker::class_def<Vector3>(L, "positionEquals", &Vector3::positionEquals, 1e-03);
    lua_tinker::class_def<Vector3>(L, "primaryAxis", &Vector3::primaryAxis);
    lua_tinker::class_def<Vector3>(L, "reflect", &Vector3::reflect);
    lua_tinker::class_def<Vector3>(L, "squaredDistance", &Vector3::squaredDistance);
    lua_tinker::class_def<Vector3>(L, "squaredLength", &Vector3::squaredLength);
    lua_tinker::class_def<Vector3>(L, "swap", &Vector3::swap);
    lua_tinker::class_con<Vector3>(
        L,
        lua_tinker::args_type_overload_constructor(new lua_tinker::constructor<Vector3>(),
                                                   new lua_tinker::constructor<Vector3, const float>(),
                                                   new lua_tinker::constructor<Vector3, const float, const float, const float>()));
    lua_tinker::class_mem<Vector3>(L, "x", &Vector3::x);
    lua_tinker::class_mem<Vector3>(L, "y", &Vector3::y);
    lua_tinker::class_mem<Vector3>(L, "z", &Vector3::z);
    lua_tinker::namespace_add(L, "GameMath");
    lua_tinker::namespace_def(L, "GameMath", "DDALineEx", &GameMath::DDALineEx);
    lua_tinker::namespace_def(L, "GameMath", "DDALineForeach", &GameMath::DDALineForeach);
    lua_tinker::namespace_def(L, "GameMath", "correctRect", &GameMath::correctRect);
    lua_tinker::namespace_def(L, "GameMath", "distance", &GameMath::distance);
    lua_tinker::namespace_def(L, "GameMath", "distance2D", &GameMath::distance2D);
    lua_tinker::namespace_def(L, "GameMath", "getAngleFrom", &GameMath::getAngleFrom, GameMath::VECTOR2_NORTH);
    lua_tinker::namespace_def(L, "GameMath", "getDirectAngleFrom", &GameMath::getDirectAngleFrom, GameMath::VECTOR2_NORTH, true);
    lua_tinker::namespace_def(L, "GameMath", "getDirectRadianFrom", &GameMath::getDirectRadianFrom, GameMath::VECTOR2_NORTH, true);
    lua_tinker::namespace_def(L, "GameMath", "getRadianFrom", &GameMath::getRadianFrom, GameMath::VECTOR2_NORTH);
    lua_tinker::namespace_def(L, "GameMath", "isIntersect", &GameMath::isIntersect);
    lua_tinker::namespace_def(L, "GameMath", "manhattanDistance", &GameMath::manhattanDistance);
    lua_tinker::namespace_def(L, "GameMath", "random_vector2", &GameMath::random_vector2);
    lua_tinker::namespace_def(L, "GameMath", "rotate", &GameMath::rotate);
    lua_tinker::namespace_def(L, "GameMath", "rotateByRadian", &GameMath::rotateByRadian);
    lua_tinker::namespace_def(L, "GameMath", "simpleDistance", &GameMath::simpleDistance);
    lua_tinker::namespace_set(L, "GameMath", "VECTOR2_EAST", GameMath::VECTOR2_EAST);
    lua_tinker::namespace_set(L, "GameMath", "VECTOR2_NORTH", GameMath::VECTOR2_NORTH);
    lua_tinker::namespace_set(L, "GameMath", "VECTOR2_SOUTH", GameMath::VECTOR2_SOUTH);
    lua_tinker::namespace_set(L, "GameMath", "VECTOR2_WEST", GameMath::VECTOR2_WEST);
    lua_tinker::namespace_add(L, "GameMath::Intersection");
    lua_tinker::scope_inner(L, "GameMath", "Intersection", "GameMath::Intersection");
    lua_tinker::namespace_add(L, "GameMath::Intersection2D");
    lua_tinker::namespace_def(L, "GameMath::Intersection2D", "LineIntersection2D", &GameMath::Intersection2D::LineIntersection2D);
    lua_tinker::namespace_def(L, "GameMath::Intersection2D", "getNearestPoint", &GameMath::Intersection2D::getNearestPoint);
    lua_tinker::namespace_def(L, "GameMath::Intersection2D", "isInABBox", &GameMath::Intersection2D::isInABBox);
    lua_tinker::namespace_def(L, "GameMath::Intersection2D", "isInFOV", &GameMath::Intersection2D::isInFOV);
    lua_tinker::namespace_def(L, "GameMath::Intersection2D", "lineIntersection2D", &GameMath::Intersection2D::lineIntersection2D);
    lua_tinker::scope_inner(L, "GameMath", "Intersection2D", "GameMath::Intersection2D");
    lua_tinker::def(L, "CheckSameDay", &CheckSameDay);
    lua_tinker::def(L, "ConvertEnc", &ConvertEnc);
    lua_tinker::def(L, "DayDiffLocal", &DayDiffLocal);
    lua_tinker::def(L,
                    "FindNameError",
                    lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((bool (*)(const std::string&))(&FindNameError)),
                                                           lua_tinker::make_functor_ptr((bool (*)(const std::wstring&))(&FindNameError))));
    lua_tinker::def(L, "GetBasePath", &GetBasePath);
    lua_tinker::def(L, "GetFileExt", &GetFileExt);
    lua_tinker::def(L, "GetFileNameFromFullPath", &GetFileNameFromFullPath);
    lua_tinker::def(L, "GetFileNameWithoutExt", &GetFileNameWithoutExt);
    lua_tinker::def(L, "GetFullPath", &GetFullPath);
    lua_tinker::def(L, "GetHighFromU64", &GetHighFromU64);
    lua_tinker::def(L, "GetLowFromU64", &GetLowFromU64);
    lua_tinker::def(L, "GetMD5Code", &GetMD5Code);
    lua_tinker::def(L, "GetMD5CodeBig", &GetMD5CodeBig);
    lua_tinker::def(L, "GetNextDayBeginTime", &GetNextDayBeginTime);
    lua_tinker::def(L, "GetTimeFromString", &GetTimeFromString);
    lua_tinker::def(L, "HasFlag", &HasFlag);
    lua_tinker::def(L, "IsUTF8_NoneControl", &IsUTF8_NoneControl);
    lua_tinker::def(L, "MAKE32", &MAKE32);
    lua_tinker::def(L, "MAKE64", &MAKE64);
    lua_tinker::def(L, "MakeINT64", &MakeINT64);
    lua_tinker::def(L, "MakeUINT64", &MakeUINT64);
    lua_tinker::def(L, "MonthDiffLocal", &MonthDiffLocal);
    lua_tinker::def(L, "MulDiv", &MulDiv);
    lua_tinker::def(L, "MulDivSign", &MulDivSign);
    lua_tinker::def(L, "NextDayBeginTimeStamp", &NextDayBeginTimeStamp);
    lua_tinker::def(L, "NextMonthBeginTimeStamp", &NextMonthBeginTimeStamp);
    lua_tinker::def(L, "NextWeekBeginTimeStamp", &NextWeekBeginTimeStamp);
    lua_tinker::def(L, "RegexStrCheck", &RegexStrCheck);
    lua_tinker::def(L, "RegexStrReload", &RegexStrReload);
    lua_tinker::def(L,
                    "ReplaceIllegaWords",
                    lua_tinker::args_type_overload_functor(lua_tinker::make_functor_ptr((bool (*)(std::string&))(&ReplaceIllegaWords)),
                                                           lua_tinker::make_functor_ptr((bool (*)(std::wstring&))(&ReplaceIllegaWords))));
    lua_tinker::def(L, "ReplaceStr", &ReplaceStr);
    lua_tinker::def(L, "TimeGetMillisecond", &TimeGetMillisecond);
    lua_tinker::def(L, "TimeGetMonotonic", &TimeGetMonotonic);
    lua_tinker::def(L, "TimeGetSecond", &TimeGetSecond);
    lua_tinker::def(L, "TimeGetSecondLocal", &TimeGetSecondLocal);
    lua_tinker::def(L, "TrimPath", &TrimPath);
    lua_tinker::def(L, "URLDecode", &URLDecode);
    lua_tinker::def(L, "URLEncode", &URLEncode);
    lua_tinker::def(L, "WeekDiffLocal", &WeekDiffLocal);
    lua_tinker::def(L, "_TimeGetMillisecond", &_TimeGetMillisecond);
    lua_tinker::def(L, "_TimeGetMonotonic", &_TimeGetMonotonic);
    lua_tinker::def(L, "_TimeGetSecond", &_TimeGetSecond);
    lua_tinker::def(L, "_TimeGetSecondLocal", &_TimeGetSecondLocal);
    lua_tinker::def(L, "bit_flip", &bit_flip);
    lua_tinker::def(L, "bit_set", &bit_set);
    lua_tinker::def(L, "bit_test", &bit_test);
    lua_tinker::def(L, "gmt2local", &gmt2local);
    lua_tinker::def(L, "hex_get", &hex_get);
    lua_tinker::def(L, "hex_set", &hex_set);
    lua_tinker::def(L, "isleap", &isleap);
    lua_tinker::def(L, "local2gmt", &local2gmt);
    lua_tinker::def(L, "lower_cast", &lower_cast);
    lua_tinker::def(L, "lower_cast_copy", &lower_cast_copy);
    lua_tinker::def(L, "ltrim", &ltrim);
    lua_tinker::def(L, "ltrim_copy", &ltrim_copy);
    lua_tinker::def(L, "md5", &md5);
    lua_tinker::def(L, "now", &now);
    lua_tinker::def(L, "random_float", &random_float, 0.0f, 1.0f);
    lua_tinker::def(L, "random_hit", &random_hit);
    lua_tinker::def(L, "random_uint32", &random_uint32);
    lua_tinker::def(L, "random_uint32_range", &random_uint32_range);
    lua_tinker::def(L, "rtrim", &rtrim);
    lua_tinker::def(L, "rtrim_copy", &rtrim_copy);
    lua_tinker::def(L, "scan_dir", &scan_dir);
    lua_tinker::def(L, "skip_utf8_bom", &skip_utf8_bom);
    lua_tinker::def(L, "split_string", &split_string);
    lua_tinker::def(L, "split_string_view", &split_string_view);
    lua_tinker::def(L, "timeToLocalTime", &timeToLocalTime);
    lua_tinker::def(L, "toHex", &toHex);
    lua_tinker::def(L, "trim", &trim);
    lua_tinker::def(L, "trim_copy", &trim_copy);
    lua_tinker::def(L, "upper_cast", &upper_cast);
    lua_tinker::def(L, "upper_cast_copy", &upper_cast_copy);
    lua_tinker::def(L, "utf8_length", &utf8_length, 0);
    lua_tinker::set(L, "UPDATE_FALSE", UPDATE_FALSE);
    lua_tinker::set(L, "UPDATE_TRUE", UPDATE_TRUE);
    lua_tinker::set(L, "SYNC_ALL", SYNC_ALL);
    lua_tinker::set(L, "SYNC_ALL_DELAY", SYNC_ALL_DELAY);
    lua_tinker::set(L, "SYNC_FALSE", SYNC_FALSE);
    lua_tinker::set(L, "SYNC_TRUE", SYNC_TRUE);
    lua_tinker::class_alias<Vector2, CPos2D>(L, "CPos2D");
    lua_tinker::class_alias<Vector3, CPos3D>(L, "CPos3D");
}