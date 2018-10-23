#include "SzlFileLoader.h"
#include "IJKSubzoneInfo.h"
namespace tecplot { namespace ___3933 { ___372 IJKSubzoneInfo::getSubzonesOnIndexPlane( IJKPlanes_e                            whichPlane, ___81                             planeIndex, ___3269<___2090::SubzoneAddress>& szAddresses) const { REQUIRE(whichPlane == ___1867 || whichPlane == ___1872 || whichPlane == ___1874); REQUIRE(IMPLICATION(whichPlane == ___1867, planeIndex < m_zoneIJKDim.i()) && IMPLICATION(whichPlane == ___1872, planeIndex < m_zoneIJKDim.___2105()) && IMPLICATION(whichPlane == ___1874, planeIndex < m_zoneIJKDim.___2134())); REQUIRE(szAddresses.empty()); ___372 ___2039 = ___4226; try { switch (whichPlane) { case ___1867: { ___81 const iSzPlane = szIJKAtItemIJK(___1844(planeIndex,0,0)).i(); size_t const ___2812 = m_subzoneTilingIJKDim.___2105()*m_subzoneTilingIJKDim.___2134(); szAddresses.reserve(___2812); szAddresses.___3503(___2812); ___2090::SubzoneAddress* szArray = &szAddresses[0]; for ( ___81 kSz = 0; kSz < m_subzoneTilingIJKDim.___2134(); ++kSz ) { for ( ___81 jSz = 0; jSz < m_subzoneTilingIJKDim.___2105(); ++jSz ) { *szArray = szAddressAtSzIJK(___1844(iSzPlane,jSz,kSz)); ++szArray; } } ___478(static_cast<size_t>(szArray-&szAddresses[0])==szAddresses.size()); } break; case ___1872: { ___81 const jSzPlane = szIJKAtItemIJK(___1844(0,planeIndex,0)).___2105(); size_t const ___2812 = m_subzoneTilingIJKDim.i()*m_subzoneTilingIJKDim.___2134(); szAddresses.reserve(___2812); szAddresses.___3503(___2812); ___2090::SubzoneAddress* szArray = &szAddresses[0]; for ( ___81 kSz = 0; kSz < m_subzoneTilingIJKDim.___2134(); ++kSz ) { for ( ___81 iSz = 0; iSz < m_subzoneTilingIJKDim.i(); ++iSz ) { *szArray = szAddressAtSzIJK(___1844(iSz,jSzPlane,kSz)); ++szArray; } } ___478(static_cast<size_t>(szArray-&szAddresses[0])==szAddresses.size()); } break; case ___1874: { ___81 const kSzPlane = szIJKAtItemIJK(___1844(0,0,planeIndex)).___2134(); size_t const ___2812 = m_subzoneTilingIJKDim.i()*m_subzoneTilingIJKDim.___2105(); szAddresses.reserve(___2812); szAddresses.___3503(___2812); ___2090::SubzoneAddress* szArray = &szAddresses[0]; for ( ___81 jSz = 0; jSz < m_subzoneTilingIJKDim.___2105(); ++jSz ) { for ( ___81 iSz = 0; iSz < m_subzoneTilingIJKDim.i(); ++iSz ) { *szArray = szAddressAtSzIJK(___1844(iSz,jSz,kSzPlane)); ++szArray; } } ___478(static_cast<size_t>(szArray-&szAddresses[0])==szAddresses.size()); } break; default: ___478(___1305); break; } } catch (...) { ___2039 = ___1186("getSubzonesOnIndexPlane failed"); } ENSURE(IMPLICATION(___2039, !szAddresses.empty())); ENSURE(IMPLICATION(___2039, isSorted(szAddresses.begin(), szAddresses.end()))); ENSURE(IMPLICATION(___2039, szAddresses[szAddresses.size()-1].subzoneOffset() < getNumSzs())); return ___2039; } ___372 IJKSubzoneInfo::getSubzonesOnIndexLine( IJKLines_e                             whichLine, ___81                             mIndex, ___81                             nIndex, ___3269<___2090::SubzoneAddress>& szAddresses) const { REQUIRE(whichLine == ___1857 || whichLine == ___1859 || whichLine == ___1860); REQUIRE(IMPLICATION(whichLine == ___1857, mIndex < m_zoneIJKDim.___2105() && nIndex < m_zoneIJKDim.___2134()) && IMPLICATION(whichLine == ___1859, mIndex < m_zoneIJKDim.i() && nIndex < m_zoneIJKDim.___2134()) && IMPLICATION(whichLine == ___1860, mIndex < m_zoneIJKDim.i() && nIndex < m_zoneIJKDim.___2105())); REQUIRE(szAddresses.empty()); ___372 ___2039 = ___4226; try { switch (whichLine) { case ___1857: { ___1844 const ijkSzBase(szIJKAtItemIJK(___1844(0,mIndex,nIndex))); ___81 const jSzBase = ijkSzBase.___2105(); ___81 const kSzBase = ijkSzBase.___2134(); size_t const ___2812 = m_subzoneTilingIJKDim.i(); szAddresses.reserve(___2812); szAddresses.___3503(___2812); ___2090::SubzoneAddress* szArray = &szAddresses[0]; for ( ___81 iSz = 0; iSz < m_subzoneTilingIJKDim.i(); ++iSz ) { *szArray = szAddressAtSzIJK(___1844(iSz,jSzBase,kSzBase)); ++szArray; } ___478(static_cast<size_t>(szArray-&szAddresses[0])==szAddresses.size()); } break; case ___1859: { ___1844 const ijkSzBase(szIJKAtItemIJK(___1844(mIndex,0,nIndex))); ___81 const iSzBase = ijkSzBase.i(); ___81 const kSzBase = ijkSzBase.___2134(); size_t const ___2812 = m_subzoneTilingIJKDim.___2105(); szAddresses.reserve(___2812); szAddresses.___3503(___2812); ___2090::SubzoneAddress* szArray = &szAddresses[0]; for ( ___81 jSz = 0; jSz < m_subzoneTilingIJKDim.___2105(); ++jSz ) { *szArray = szAddressAtSzIJK(___1844(iSzBase,jSz,kSzBase)); ++szArray; } ___478(static_cast<size_t>(szArray-&szAddresses[0])==szAddresses.size()); } break; case ___1860: { ___1844 const ijkSzBase(szIJKAtItemIJK(___1844(mIndex,nIndex,0))); ___81 const iSzBase = ijkSzBase.i(); ___81 const jSzBase = ijkSzBase.___2105(); size_t const ___2812 = m_subzoneTilingIJKDim.___2134(); szAddresses.reserve(___2812); szAddresses.___3503(___2812); ___2090::SubzoneAddress* szArray = &szAddresses[0]; for ( ___81 kSz = 0; kSz < m_subzoneTilingIJKDim.___2134(); ++kSz ) { *szArray = szAddressAtSzIJK(___1844(iSzBase,jSzBase,kSz)); ++szArray; } ___478(static_cast<size_t>(szArray-&szAddresses[0])==szAddresses.size());
} break; default: ___478(___1305); break; } } catch (...) { ___2039 = ___1186("getSubzonesOnIndexPlane failed"); } ENSURE(IMPLICATION(___2039, !szAddresses.empty())); ENSURE(IMPLICATION(___2039, isSorted(szAddresses.begin(), szAddresses.end()))); ENSURE(IMPLICATION(___2039, szAddresses[szAddresses.size()-1].___2977()==m_partition)); ENSURE(IMPLICATION(___2039, szAddresses[szAddresses.size()-1].subzoneOffset() < getNumSzs())); return ___2039; } }}
