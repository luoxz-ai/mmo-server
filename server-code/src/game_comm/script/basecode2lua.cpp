#include "BaseCode.h"
#include "BaseType.h"
#include "CallStackDumper.h"
#include "FileLock.h"
#include "GameMath.h"
#include "INILoader.h"
#include "LockfreeQueue.h"
#include "Math.h"
#include "MemoryHeap.h"
#include "MyTimer.h"
#include "ProtobuffParse.h"
#include "ProtobuffUtil.h"
#include "RandomGet.h"
#include "SettingMap.h"
#include "Singleton.h"
#include "SkipList.h"
#include "StringAlgo.h"
#include "T_GameDataMap.h"
#include "Thread.h"
#include "UIDFactory.h"
#include "Vector2.h"
#include "Vector3.h"
#include "ZSet.h"
#include "export_lua.h"
#include "get_opt.h"
#include "i386-signal.h"
#include "log4z.h"
#include "loging_manager.h"
#include "md5.h"
#include "segvcatch.h"
#include "type_traits_ext.h"
#include "x86_64-signal.h"
#include "lua_tinker.h"
void basecode2lua(lua_State* L)
{
lua_tinker::def(L, "CheckSameDay",&CheckSameDay);
lua_tinker::def(L, "ConvertEnc",&ConvertEnc);
lua_tinker::def(L, "DateDiffLocal",&DateDiffLocal);
lua_tinker::def(L, "FindNameError", lua_tinker::args_type_overload_functor(
	lua_tinker::make_functor_ptr((bool(*)(const std::string &))(&FindNameError)), 
	lua_tinker::make_functor_ptr((bool(*)(const std::wstring &))(&FindNameError))));
lua_tinker::def(L, "GetBasePath",&GetBasePath);
lua_tinker::def(L, "GetFileExt",&GetFileExt);
lua_tinker::def(L, "GetFileNameFromFullPath",&GetFileNameFromFullPath);
lua_tinker::def(L, "GetFileNameWithoutExt",&GetFileNameWithoutExt);
lua_tinker::def(L, "GetFullPath",&GetFullPath);
lua_tinker::def(L, "GetHighFromU64",&GetHighFromU64);
lua_tinker::def(L, "GetLowFromU64",&GetLowFromU64);
lua_tinker::def(L, "GetMD5Code",&GetMD5Code);
lua_tinker::def(L, "GetMD5CodeBig",&GetMD5CodeBig);
lua_tinker::def(L, "GetNextDayBeginTime",&GetNextDayBeginTime);
lua_tinker::def(L, "GetTimeFromString",&GetTimeFromString);
lua_tinker::def(L, "HasFlag",&HasFlag);
lua_tinker::def(L, "IsUTF8_NoneControl",&IsUTF8_NoneControl);
lua_tinker::def(L, "MAKE32",&MAKE32);
lua_tinker::def(L, "MAKE64",&MAKE64);
lua_tinker::def(L, "MakeINT64",&MakeINT64);
lua_tinker::def(L, "MakeUINT64",&MakeUINT64);
lua_tinker::def(L, "MulDiv",&MulDiv);
lua_tinker::def(L, "RegexStrCheck",&RegexStrCheck);
lua_tinker::def(L, "RegexStrReload",&RegexStrReload);
lua_tinker::def(L, "ReplaceIllegaWords", lua_tinker::args_type_overload_functor(
	lua_tinker::make_functor_ptr((bool(*)(std::string &))(&ReplaceIllegaWords)), 
	lua_tinker::make_functor_ptr((bool(*)(std::wstring &))(&ReplaceIllegaWords))));
lua_tinker::def(L, "ReplaceStr",&ReplaceStr);
lua_tinker::def(L, "TimeGetMillisecond",&TimeGetMillisecond);
lua_tinker::def(L, "TimeGetMonotonic",&TimeGetMonotonic);
lua_tinker::def(L, "TimeGetSecond",&TimeGetSecond);
lua_tinker::def(L, "TimeGetSecondLocal",&TimeGetSecondLocal);
lua_tinker::def(L, "TrimPath",&TrimPath);
lua_tinker::def(L, "URLDecode",&URLDecode);
lua_tinker::def(L, "URLEncode",&URLEncode);
lua_tinker::def(L, "_TimeGetMillisecond",&_TimeGetMillisecond);
lua_tinker::def(L, "_TimeGetMonotonic",&_TimeGetMonotonic);
lua_tinker::def(L, "_TimeGetSecond",&_TimeGetSecond);
lua_tinker::def(L, "_TimeGetSecondLocal",&_TimeGetSecondLocal);
lua_tinker::def(L, "bit_flip",&bit_flip);
lua_tinker::def(L, "bit_set",&bit_set);
lua_tinker::def(L, "bit_test",&bit_test);
lua_tinker::def(L, "gmt2local",&gmt2local);
lua_tinker::def(L, "hex_get",&hex_get);
lua_tinker::def(L, "hex_set",&hex_set);
lua_tinker::def(L, "isleap",&isleap);
lua_tinker::def(L, "local2gmt",&local2gmt);
lua_tinker::def(L, "ltrim",&ltrim);
lua_tinker::def(L, "md5",&md5);
lua_tinker::def(L, "random_float",&random_float, 0.0f, 1.0f);
lua_tinker::def(L, "random_hit",&random_hit);
lua_tinker::def(L, "random_uint32",&random_uint32);
lua_tinker::def(L, "random_uint32_range",&random_uint32_range);
lua_tinker::def(L, "rtrim",&rtrim);
lua_tinker::def(L, "scan_dir",&scan_dir);
lua_tinker::def(L, "skip_utf8_bom",&skip_utf8_bom);
lua_tinker::def(L, "split_string",&split_string);
lua_tinker::def(L, "toHex",&toHex);
lua_tinker::def(L, "trim",&trim);
lua_tinker::def(L, "utf8_length",&utf8_length, 0);
lua_tinker::set(L,"UPDATE_FALSE",UPDATE_FALSE);
lua_tinker::set(L,"UPDATE_TRUE",UPDATE_TRUE);
lua_tinker::set(L, "SYNC_ALL",SYNC_ALL);
lua_tinker::set(L, "SYNC_ALL_DELAY",SYNC_ALL_DELAY);
lua_tinker::set(L, "SYNC_FALSE",SYNC_FALSE);
lua_tinker::set(L, "SYNC_TRUE",SYNC_TRUE);
lua_tinker::class_add<CMyTimer>(L, "CMyTimer",true);
lua_tinker::class_def<CMyTimer>(L, "Clear",&CMyTimer::Clear);
lua_tinker::class_def<CMyTimer>(L, "DecInterval",&CMyTimer::DecInterval);
lua_tinker::class_def<CMyTimer>(L, "GetRemain",&CMyTimer::GetRemain);
lua_tinker::class_def<CMyTimer>(L, "GetTimeOut",&CMyTimer::GetTimeOut);
lua_tinker::class_def<CMyTimer>(L, "IncInterval",&CMyTimer::IncInterval);
lua_tinker::class_def<CMyTimer>(L, "IsActive",&CMyTimer::IsActive);
lua_tinker::class_def<CMyTimer>(L, "IsTimeOut", lua_tinker::args_type_overload_member_functor(
	lua_tinker::make_member_functor_ptr((bool(CMyTimer::*)())(&CMyTimer::IsTimeOut)), 
	lua_tinker::make_member_functor_ptr((bool(CMyTimer::*)(int))(&CMyTimer::IsTimeOut))));
lua_tinker::class_def<CMyTimer>(L, "SetInterval",&CMyTimer::SetInterval);
lua_tinker::class_def<CMyTimer>(L, "Startup",&CMyTimer::Startup);
lua_tinker::class_def<CMyTimer>(L, "TimeOver",&CMyTimer::TimeOver);
lua_tinker::class_def<CMyTimer>(L, "ToNextTick",&CMyTimer::ToNextTick);
lua_tinker::class_def<CMyTimer>(L, "ToNextTime", lua_tinker::args_type_overload_member_functor(
	lua_tinker::make_member_functor_ptr((bool(CMyTimer::*)())(&CMyTimer::ToNextTime)), 
	lua_tinker::make_member_functor_ptr((bool(CMyTimer::*)(int))(&CMyTimer::ToNextTime))));
lua_tinker::class_def<CMyTimer>(L, "Update",&CMyTimer::Update);
lua_tinker::class_con<CMyTimer>(L, lua_tinker::constructor<CMyTimer, int>::invoke, 0);
lua_tinker::class_add<FloatRect>(L, "FloatRect",true);
lua_tinker::class_def<FloatRect>(L, "height",&FloatRect::height);
lua_tinker::class_def<FloatRect>(L, "isIntersect",&FloatRect::isIntersect);
lua_tinker::class_def<FloatRect>(L, "width",&FloatRect::width);
lua_tinker::class_con<FloatRect>(L,lua_tinker::args_type_overload_constructor(
	new lua_tinker::constructor<FloatRect>(), 
	new lua_tinker::constructor<FloatRect, const FloatRect &>(), 
	new lua_tinker::constructor<FloatRect, const float &, const float &, const float &, const float &>()));
lua_tinker::class_mem<FloatRect>(L,"bottom",&FloatRect::bottom);
lua_tinker::class_mem<FloatRect>(L,"left",&FloatRect::left);
lua_tinker::class_mem<FloatRect>(L,"right",&FloatRect::right);
lua_tinker::class_mem<FloatRect>(L,"top",&FloatRect::top);
lua_tinker::namespace_add(L, "GameMath");
lua_tinker::namespace_def(L, "GameMath", "DDALineEx",&GameMath::DDALineEx);
lua_tinker::namespace_def(L, "GameMath", "DDALineForeach",&GameMath::DDALineForeach);
lua_tinker::namespace_def(L, "GameMath", "correctRect",&GameMath::correctRect);
lua_tinker::namespace_def(L, "GameMath", "distance",&GameMath::distance);
lua_tinker::namespace_def(L, "GameMath", "distance2D",&GameMath::distance2D);
lua_tinker::namespace_def(L, "GameMath", "getAngleFrom",&GameMath::getAngleFrom, GameMath::VECTOR2_NORTH);
lua_tinker::namespace_def(L, "GameMath", "getDirectAngleFrom",&GameMath::getDirectAngleFrom, GameMath::VECTOR2_NORTH, true);
lua_tinker::namespace_def(L, "GameMath", "getDirectRadianFrom",&GameMath::getDirectRadianFrom, GameMath::VECTOR2_NORTH, true);
lua_tinker::namespace_def(L, "GameMath", "getRadianFrom",&GameMath::getRadianFrom, GameMath::VECTOR2_NORTH);
lua_tinker::namespace_def(L, "GameMath", "isIntersect",&GameMath::isIntersect);
lua_tinker::namespace_def(L, "GameMath", "manhattanDistance",&GameMath::manhattanDistance);
lua_tinker::namespace_def(L, "GameMath", "random_vector2",&GameMath::random_vector2);
lua_tinker::namespace_def(L, "GameMath", "rotate",&GameMath::rotate);
lua_tinker::namespace_def(L, "GameMath", "rotateByRadian",&GameMath::rotateByRadian);
lua_tinker::namespace_def(L, "GameMath", "simpleDistance",&GameMath::simpleDistance);
lua_tinker::namespace_set(L,"GameMath","VECTOR2_EAST",GameMath::VECTOR2_EAST);
lua_tinker::namespace_set(L,"GameMath","VECTOR2_NORTH",GameMath::VECTOR2_NORTH);
lua_tinker::namespace_set(L,"GameMath","VECTOR2_SOUTH",GameMath::VECTOR2_SOUTH);
lua_tinker::namespace_set(L,"GameMath","VECTOR2_WEST",GameMath::VECTOR2_WEST);
lua_tinker::namespace_add(L, "GameMath::Intersection");
lua_tinker::namespace_def(L, "GameMath::Intersection", "LineIntersection2D",&GameMath::Intersection::LineIntersection2D);
lua_tinker::namespace_def(L, "GameMath::Intersection", "getNearestPoint",&GameMath::Intersection::getNearestPoint);
lua_tinker::namespace_def(L, "GameMath::Intersection", "isInABBox",&GameMath::Intersection::isInABBox);
lua_tinker::namespace_def(L, "GameMath::Intersection", "isInFOV",&GameMath::Intersection::isInFOV);
lua_tinker::namespace_def(L, "GameMath::Intersection", "lineIntersection2D",&GameMath::Intersection::lineIntersection2D);
lua_tinker::scope_inner(L, "GameMath", "Intersection", "GameMath::Intersection");
lua_tinker::class_add<Math>(L, "Math",true);
lua_tinker::class_def_static<Math>(L, "get2Pow",&Math::get2Pow);
lua_tinker::class_def_static<Math>(L, "is2Pow",&Math::is2Pow);
lua_tinker::class_def_static<Math>(L, "isNaN",&Math::isNaN);
lua_tinker::class_add<Rect>(L, "Rect",true);
lua_tinker::class_def<Rect>(L, "height",&Rect::height);
lua_tinker::class_def<Rect>(L, "isIntersect",&Rect::isIntersect);
lua_tinker::class_def<Rect>(L, "operator=",&Rect::operator=);
lua_tinker::class_def<Rect>(L, "width",&Rect::width);
lua_tinker::class_con<Rect>(L,lua_tinker::args_type_overload_constructor(
	new lua_tinker::constructor<Rect>(), 
	new lua_tinker::constructor<Rect, const Rect &>(), 
	new lua_tinker::constructor<Rect, const long &, const long &, const long &, const long &>()));
lua_tinker::class_mem<Rect>(L,"bottom",&Rect::bottom);
lua_tinker::class_mem<Rect>(L,"left",&Rect::left);
lua_tinker::class_mem<Rect>(L,"right",&Rect::right);
lua_tinker::class_mem<Rect>(L,"top",&Rect::top);
lua_tinker::class_add<Vector2>(L, "Vector2",true);
lua_tinker::class_def<Vector2>(L, "crossProduct",&Vector2::crossProduct);
lua_tinker::class_def<Vector2>(L, "distance",&Vector2::distance);
lua_tinker::class_def<Vector2>(L, "dotProduct",&Vector2::dotProduct);
lua_tinker::class_def<Vector2>(L, "isNaN",&Vector2::isNaN);
lua_tinker::class_def<Vector2>(L, "isZeroLength",&Vector2::isZeroLength);
lua_tinker::class_def<Vector2>(L, "length",&Vector2::length);
lua_tinker::class_def<Vector2>(L, "makeCeil",&Vector2::makeCeil);
lua_tinker::class_def<Vector2>(L, "makeFloor",&Vector2::makeFloor);
lua_tinker::class_def<Vector2>(L, "midPoint",&Vector2::midPoint);
lua_tinker::class_def<Vector2>(L, "normalise",&Vector2::normalise);
lua_tinker::class_def<Vector2>(L, "normalisedCopy",&Vector2::normalisedCopy);
lua_tinker::class_def<Vector2>(L, "operator!=",&Vector2::operator!=);
lua_tinker::class_def<Vector2>(L, "__lt",&Vector2::operator<);
lua_tinker::class_def<Vector2>(L, "__eq",&Vector2::operator==);
lua_tinker::class_def<Vector2>(L, "operator>",&Vector2::operator>);
lua_tinker::class_def<Vector2>(L, "perpendicular",&Vector2::perpendicular);
lua_tinker::class_def<Vector2>(L, "randomDeviant",&Vector2::randomDeviant);
lua_tinker::class_def<Vector2>(L, "reflect",&Vector2::reflect);
lua_tinker::class_def<Vector2>(L, "squaredDistance",&Vector2::squaredDistance);
lua_tinker::class_def<Vector2>(L, "squaredLength",&Vector2::squaredLength);
lua_tinker::class_def<Vector2>(L, "swap",&Vector2::swap);
lua_tinker::class_con<Vector2>(L,lua_tinker::args_type_overload_constructor(
	new lua_tinker::constructor<Vector2>(), 
	new lua_tinker::constructor<Vector2, const float>(), 
	new lua_tinker::constructor<Vector2, const float, const float>()));
lua_tinker::class_mem_static_readonly<Vector2>(L,"NEGATIVE_UNIT_X",&Vector2::NEGATIVE_UNIT_X);
lua_tinker::class_mem_static_readonly<Vector2>(L,"NEGATIVE_UNIT_Y",&Vector2::NEGATIVE_UNIT_Y);
lua_tinker::class_mem_static_readonly<Vector2>(L,"UNIT_SCALE",&Vector2::UNIT_SCALE);
lua_tinker::class_mem_static_readonly<Vector2>(L,"UNIT_X",&Vector2::UNIT_X);
lua_tinker::class_mem_static_readonly<Vector2>(L,"UNIT_Y",&Vector2::UNIT_Y);
lua_tinker::class_mem_static_readonly<Vector2>(L,"ZERO",&Vector2::ZERO);
lua_tinker::class_mem<Vector2>(L,"x",&Vector2::x);
lua_tinker::class_mem<Vector2>(L,"y",&Vector2::y);
lua_tinker::class_add<Vector3>(L, "Vector3",true);
lua_tinker::class_def<Vector3>(L, "absDotProduct",&Vector3::absDotProduct);
lua_tinker::class_def<Vector3>(L, "crossProduct",&Vector3::crossProduct);
lua_tinker::class_def<Vector3>(L, "distance",&Vector3::distance);
lua_tinker::class_def<Vector3>(L, "dotProduct",&Vector3::dotProduct);
lua_tinker::class_def<Vector3>(L, "isNaN",&Vector3::isNaN);
lua_tinker::class_def<Vector3>(L, "isZeroLength",&Vector3::isZeroLength);
lua_tinker::class_def<Vector3>(L, "length",&Vector3::length);
lua_tinker::class_def<Vector3>(L, "makeCeil",&Vector3::makeCeil);
lua_tinker::class_def<Vector3>(L, "makeFloor",&Vector3::makeFloor);
lua_tinker::class_def<Vector3>(L, "midPoint",&Vector3::midPoint);
lua_tinker::class_def<Vector3>(L, "normalise",&Vector3::normalise);
lua_tinker::class_def<Vector3>(L, "normalisedCopy",&Vector3::normalisedCopy);
lua_tinker::class_def<Vector3>(L, "operator!=",&Vector3::operator!=);
lua_tinker::class_def<Vector3>(L, "__lt",&Vector3::operator<);
lua_tinker::class_def<Vector3>(L, "__eq",&Vector3::operator==);
lua_tinker::class_def<Vector3>(L, "operator>",&Vector3::operator>);
lua_tinker::class_def<Vector3>(L, "perpendicular",&Vector3::perpendicular);
lua_tinker::class_def<Vector3>(L, "positionCloses",&Vector3::positionCloses, 1e-03f);
lua_tinker::class_def<Vector3>(L, "positionEquals",&Vector3::positionEquals, 1e-03);
lua_tinker::class_def<Vector3>(L, "primaryAxis",&Vector3::primaryAxis);
lua_tinker::class_def<Vector3>(L, "reflect",&Vector3::reflect);
lua_tinker::class_def<Vector3>(L, "squaredDistance",&Vector3::squaredDistance);
lua_tinker::class_def<Vector3>(L, "squaredLength",&Vector3::squaredLength);
lua_tinker::class_def<Vector3>(L, "swap",&Vector3::swap);
lua_tinker::class_con<Vector3>(L,lua_tinker::args_type_overload_constructor(
	new lua_tinker::constructor<Vector3>(), 
	new lua_tinker::constructor<Vector3, const float>(), 
	new lua_tinker::constructor<Vector3, const float, const float, const float>()));
lua_tinker::class_mem_static_readonly<Vector3>(L,"NEGATIVE_UNIT_X",&Vector3::NEGATIVE_UNIT_X);
lua_tinker::class_mem_static_readonly<Vector3>(L,"NEGATIVE_UNIT_Y",&Vector3::NEGATIVE_UNIT_Y);
lua_tinker::class_mem_static_readonly<Vector3>(L,"NEGATIVE_UNIT_Z",&Vector3::NEGATIVE_UNIT_Z);
lua_tinker::class_mem_static_readonly<Vector3>(L,"UNIT_SCALE",&Vector3::UNIT_SCALE);
lua_tinker::class_mem_static_readonly<Vector3>(L,"UNIT_X",&Vector3::UNIT_X);
lua_tinker::class_mem_static_readonly<Vector3>(L,"UNIT_Y",&Vector3::UNIT_Y);
lua_tinker::class_mem_static_readonly<Vector3>(L,"UNIT_Z",&Vector3::UNIT_Z);
lua_tinker::class_mem_static_readonly<Vector3>(L,"ZERO",&Vector3::ZERO);
lua_tinker::class_mem<Vector3>(L,"x",&Vector3::x);
lua_tinker::class_mem<Vector3>(L,"y",&Vector3::y);
lua_tinker::class_mem<Vector3>(L,"z",&Vector3::z);

}